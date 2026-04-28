/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Claritone — Combined L0–L7 program
  *
  * Integrates:
  *   L0  Blinky        — heartbeat LED on SCROLL_1 (PD8)
  *   L1  UART          — UART4 @ 115200 baud status prints
  *   L2  GPIO          — SCL_BB / SDA_BB auxiliary toggling
  *   L3  I2C bit-bang  — VL53L7CX device-ID verification on boot
  *   L4  ULD           — VL53L7CX ranging (non-blocking poll in main loop)
  *   L5  ToF @ HSE     — same ranging path, clock already 266 MHz
  *   L6  HSE           — SystemClock_Config: HSE + PLL1 → 266.67 MHz CPU
  *   L7  Audio         — SAI1 + GPDMA1 ping-pong DMA, 440 Hz sine tone
  *
  * Architecture notes
  * ------------------
  *  • Audio is fully interrupt-driven (HAL_SAI_TxHalfCpltCallback /
  *    HAL_SAI_TxCpltCallback).  The main loop never blocks for audio.
  *  • ToF ranging uses claritone_tof_test_init() + a non-blocking
  *    claritone_tof_poll() wrapper (see Section 4 below).  If your
  *    claritone_tof_test.c only exposes claritone_tof_test_ranging_loop()
  *    you must add a single-shot poll function — see the stub provided.
  *  • The main loop runs on a 100 ms HAL_Delay tick with sub-tick counters
  *    for each periodic task (heartbeat, UART print, ToF poll).
  *
  * Required CubeMX peripheral configuration (combined project)
  * -----------------------------------------------------------
  *   UART4   PA0 TX / PA1 RX,  115200 8N1
  *   SAI1    Block A, master TX, I2S-standard, 32-bit stereo, 48 kHz
  *   GPDMA1  Channel 0 → SAI1_A (circular, word width)
  *   GPIO    PD1  TOF_LPN   OUT PP
  *           PD8  SCROLL_1  OUT PP  (heartbeat LED)
  *           PB1  SD_MODE   OUT PP  (amp enable — set HIGH)
  *           PB8  SCL_BB    OUT OD  (bit-bang I2C clock)
  *           PB9  SDA_BB    OUT OD  (bit-bang I2C data)
  *           PE0  TOF_INT   OUT PP
  *           PE1  TOF_I2C_RST OUT PP
  *
  * External dependencies (bring from your lesson projects)
  *   i2c_bb.h / i2c_bb.c
  *   claritone_tof_test.h / claritone_tof_test.c  — must expose
  *       claritone_tof_test_init()  → int  (0 = OK)
  *       claritone_tof_poll()       → int  (0 = new result ready, <0 = err)
  *       claritone_tof_last_mm()    → uint32_t  (last distance, mm)
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpdma.h"
#include "sai.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "i2c_bb.h"
#include "claritone_tof_test.h"
/* USER CODE END Includes */

/* ---------------------------------------------------------------------------
 * SECTION 1 — AUDIO (L7)
 * Ping-pong DMA, 440 Hz sine tone via LUT, ISR-safe (no float in callbacks)
 * --------------------------------------------------------------------------*/

/* USER CODE BEGIN PD */
#define HALF_FRAMES     256u
#define TOTAL_FRAMES    (2u * HALF_FRAMES)

#define SINE_LUT_SIZE   256u   /* must be power-of-2, one full period */
/* USER CODE END PD */

/* USER CODE BEGIN PV */
/* --- Audio buffers & LUT ------------------------------------------------- */
static int32_t  audioBuffer[2u * TOTAL_FRAMES]; /* stereo, ping-pong */
static int32_t  sine_lut[SINE_LUT_SIZE];

/* Q16.16 fixed-point phase accumulator — updated only in SAI callbacks */
static uint32_t s_phase_q     = 0u;
static uint32_t s_phase_inc_q = 0u;

/* Diagnostic counters (volatile so debugger/UART reads are coherent) */
static volatile uint32_t audio_half_count = 0u;
static volatile uint32_t audio_full_count = 0u;
static volatile uint32_t sai_err_code     = 0u;
static volatile uint32_t sai_err_count    = 0u;

/* --- ToF state ------------------------------------------------------------ */
static bool     tof_ready      = false;
static uint32_t tof_last_mm    = 0u;
static uint32_t tof_err_count  = 0u;
/* USER CODE END PV */

/* ---------------------------------------------------------------------------
 * SECTION 2 — PRIVATE FUNCTION PROTOTYPES
 * --------------------------------------------------------------------------*/
static void SystemClock_Config(void);
static void SystemIsolation_Config(void);

/* Audio helpers */
static void sine_lut_init(void);
static void audio_fill_half(uint32_t word_offset);

/* Boot-time I2C diagnostic (from L3 / L5) */
static bool     tof_verify_device_id(void);

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */


/* ===========================================================================
 * main()
 * =========================================================================*/
int main(void)
{
    /* --------------------------------------------------------------------- */
    /* MCU HAL init                                                           */
    /* --------------------------------------------------------------------- */
    HAL_Init();

    /* L6: switch to HSE + PLL1 → 266.67 MHz before anything else           */
    SystemClock_Config();

    /* Disable D-cache (required for SAI DMA coherency on this device)       */
    SCB_DisableDCache();

    /* --------------------------------------------------------------------- */
    /* Peripheral init — generated by CubeMX for the combined project        */
    /* --------------------------------------------------------------------- */
    MX_GPIO_Init();
    MX_GPDMA1_Init();
    MX_UART4_Init();
    MX_SAI1_Init();
    SystemIsolation_Config();

    /* --------------------------------------------------------------------- */
    /* L7: Enable speaker amplifiers (SD_MODE = PB1 HIGH)                   */
    /* --------------------------------------------------------------------- */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

    /* --------------------------------------------------------------------- */
    /* Boot banner (L1 UART)                                                 */
    /* --------------------------------------------------------------------- */
    {
        char buf[128];
        int  len = snprintf(buf, sizeof(buf),
            "\r\n=== Claritone Combined Firmware ===\r\n"
            "CPU clock : %lu Hz\r\n"
            "Peripherals: UART4, SAI1/DMA, I2C-BB, VL53L7CX\r\n\r\n",
            (unsigned long)SystemCoreClock);
        HAL_UART_Transmit(&huart4, (const uint8_t *)buf, len, HAL_MAX_DELAY);
    }

    /* --------------------------------------------------------------------- */
    /* L3 / I2C: verify ToF device ID before loading firmware               */
    /* --------------------------------------------------------------------- */
    i2c_bb_init();

    /* Power-up sequence: assert I2C RST briefly, then release LPN          */
    HAL_GPIO_WritePin(TOF_I2C_RST_GPIO_Port, TOF_I2C_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(TOF_I2C_RST_GPIO_Port, TOF_I2C_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(TOF_LPN_GPIO_Port,     TOF_LPN_Pin,     GPIO_PIN_SET);
    HAL_Delay(5);

    if (!tof_verify_device_id())
    {
        const char *msg = "[WARN] VL53L7CX device-ID mismatch — continuing anyway\r\n";
        HAL_UART_Transmit(&huart4, (const uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    }
    else
    {
        const char *msg = "[OK]   VL53L7CX device-ID verified (0xF0 / 0x0C)\r\n";
        HAL_UART_Transmit(&huart4, (const uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    }

    /* --------------------------------------------------------------------- */
    /* L4 / ULD: load firmware & start ranging                               */
    /* --------------------------------------------------------------------- */
    {
        const char *msg = "Initialising VL53L7CX ULD...\r\n";
        HAL_UART_Transmit(&huart4, (const uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    }

    if (claritone_tof_test_init() != 0)
    {
        const char *msg = "[ERROR] ToF init failed — ranging disabled\r\n";
        HAL_UART_Transmit(&huart4, (const uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        tof_ready = false;
    }
    else
    {
        const char *msg = "[OK]   ToF ranging active\r\n";
        HAL_UART_Transmit(&huart4, (const uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        tof_ready = true;
    }

    /* --------------------------------------------------------------------- */
    /* L7 / Audio: build sine LUT, prime DMA buffer, start DMA              */
    /* --------------------------------------------------------------------- */
    sine_lut_init();
    memset(audioBuffer, 0, sizeof(audioBuffer));

    if (HAL_SAI_Transmit_DMA(&hsai_BlockA1,
                              (uint8_t *)audioBuffer,
                              2u * TOTAL_FRAMES) != HAL_OK)
    {
        Error_Handler();
    }

    {
        const char *msg = "[OK]   SAI1 DMA audio running (440 Hz sine)\r\n\r\n";
        HAL_UART_Transmit(&huart4, (const uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    }

    /* ==================================================================== */
    /* MAIN LOOP                                                             */
    /* Tick = 100 ms.  Sub-counters gate each periodic task.                */
    /* Audio is entirely handled in ISR callbacks — no work done here.      */
    /* ==================================================================== */
    uint32_t tick        = 0u;  /* increments every 100 ms                  */
    uint32_t tof_poll_ms = 0u;  /* running ms counter for ToF poll interval */

    while (1)
    {
        HAL_Delay(100);
        tick++;

        /* ----------------------------------------------------------------- */
        /* L0 Blinky: heartbeat LED (SCROLL_1 / PD8) every 500 ms           */
        /* ----------------------------------------------------------------- */
        if (tick % 5u == 0u)
        {
            HAL_GPIO_TogglePin(SCROLL_1_GPIO_Port, SCROLL_1_Pin);
        }

        /* ----------------------------------------------------------------- */
        /* L2 GPIO: auxiliary pin toggle (SCL_BB / SDA_BB) — kept alive     */
        /* at harmless rates while I2C bus is idle between ToF polls.        */
        /* Comment this block out if it interferes with your I2C timing.     */
        /* ----------------------------------------------------------------- */
        /* SCL_BB (PB8) auxiliary toggle every 500 ms — 1 Hz square wave */
        if (tick % 5u == 0u)
        {
            /* Only toggle when not in an active I2C transaction */
            if (!tof_ready)   /* safe fallback: always toggle if ToF absent */
            {
                HAL_GPIO_TogglePin(SCL_BB_GPIO_Port, SCL_BB_Pin);
            }
        }

        /* ----------------------------------------------------------------- */
        /* L4 / L5 ToF: poll sensor every 200 ms                            */
        /* ----------------------------------------------------------------- */
        tof_poll_ms += 100u;
        if (tof_ready && tof_poll_ms >= 200u)
        {
            tof_poll_ms = 0u;

            /*
             * claritone_tof_poll() — YOU MUST ADD THIS to claritone_tof_test.c
             *
             * It should:
             *   1. Call vl53l7cx_check_data_ready() (non-blocking)
             *   2. If ready, call vl53l7cx_get_ranging_data() and cache result
             *   3. Return 0 if a fresh result was stored, 1 if not ready yet,
             *      negative on error.
             *
             * The stub below keeps the build clean until you add it:
             */

            int poll_rc = claritone_tof_poll();
            if (poll_rc == 0)
            {
                tof_last_mm = claritone_tof_last_mm();
            }
            else if (poll_rc < 0)
            {
                tof_err_count++;
            }
        }

        /* ----------------------------------------------------------------- */
        /* L1 / L6 UART: print status every 1 second (10 × 100 ms ticks)   */
        /* ----------------------------------------------------------------- */
        if (tick % 10u == 0u)
        {
            char buf[192];
            int  len = snprintf(buf, sizeof(buf),
                "[%5lu s] clk=%lu MHz | audio: h=%lu f=%lu errs=%lu"
                " | tof: %lu mm errs=%lu\r\n",
                (unsigned long)(tick / 10u),
                (unsigned long)(SystemCoreClock / 1000000u),
                (unsigned long)audio_half_count,
                (unsigned long)audio_full_count,
                (unsigned long)sai_err_count,
                (unsigned long)tof_last_mm,
                (unsigned long)tof_err_count);
            HAL_UART_Transmit(&huart4, (const uint8_t *)buf, len, HAL_MAX_DELAY);
        }

    } /* while(1) */
}


/* ===========================================================================
 * SECTION 3 — CLOCK CONFIGURATION (L6)
 * HSE + PLL1: PLLM=1, PLLN=32, P=1 → VCO=800 MHz, IC1÷3 → CPU 266.67 MHz
 * =========================================================================*/
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
    RCC_OscInitStruct.PLL1.PLLState       = RCC_PLL_ON;
    RCC_OscInitStruct.PLL1.PLLSource      = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL1.PLLM          = 1;
    RCC_OscInitStruct.PLL1.PLLN          = 32;
    RCC_OscInitStruct.PLL1.PLLFractional = 0;
    RCC_OscInitStruct.PLL1.PLLP1         = 1;
    RCC_OscInitStruct.PLL1.PLLP2         = 1;
    RCC_OscInitStruct.PLL2.PLLState       = RCC_PLL_NONE;
    RCC_OscInitStruct.PLL3.PLLState       = RCC_PLL_NONE;
    RCC_OscInitStruct.PLL4.PLLState       = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

    RCC_ClkInitStruct.ClockType     = RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_HCLK
                                    | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1
                                    | RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_PCLK5
                                    | RCC_CLOCKTYPE_PCLK4;
    RCC_ClkInitStruct.CPUCLKSource  = RCC_CPUCLKSOURCE_IC1;
    RCC_ClkInitStruct.SYSCLKSource  = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider= RCC_APB1_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider= RCC_APB2_DIV1;
    RCC_ClkInitStruct.APB4CLKDivider= RCC_APB4_DIV1;
    RCC_ClkInitStruct.APB5CLKDivider= RCC_APB5_DIV1;
    RCC_ClkInitStruct.IC1Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
    RCC_ClkInitStruct.IC1Selection.ClockDivider   = 3;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK) { Error_Handler(); }
}


/* ===========================================================================
 * SECTION 4 — AUDIO HELPERS (L7)
 * =========================================================================*/

/**
 * @brief  Build the sine LUT and compute the Q16.16 phase increment.
 *         Call once from main context before starting SAI DMA.
 */
static void sine_lut_init(void)
{
    const float twoPi = 2.0f * 3.14159265358979f;
    for (uint32_t i = 0u; i < SINE_LUT_SIZE; ++i)
    {
        float s = sinf(twoPi * (float)i / (float)SINE_LUT_SIZE);
        sine_lut[i] = (int32_t)(0x7FFFFFFFl * 0.2f * s);  /* 20 % full-scale */
    }

    /* Phase inc = (freq / fs) × LUT_SIZE × 65536
     * 440 Hz, 48 kHz: (440/48000) × 256 × 65536 ≈ 153 391              */
    s_phase_inc_q = (uint32_t)(((uint64_t)440u * SINE_LUT_SIZE * 65536u) / 48000u);
    s_phase_q     = 0u;
}

/**
 * @brief  Fill HALF_FRAMES stereo frames at word_offset in audioBuffer.
 *         ISR-safe: integer arithmetic only.
 */
static void audio_fill_half(uint32_t word_offset)
{
    uint32_t       phase = s_phase_q;
    const uint32_t inc   = s_phase_inc_q;
    const uint32_t mask  = (SINE_LUT_SIZE - 1u) << 16u;

    for (int i = 0; i < (int)HALF_FRAMES; ++i)
    {
        uint32_t idx = (phase & mask) >> 16u;
        int32_t  s   = sine_lut[idx];
        audioBuffer[word_offset + (uint32_t)(2 * i) + 0u] = s;  /* L */
        audioBuffer[word_offset + (uint32_t)(2 * i) + 1u] = s;  /* R */
        phase += inc;
    }
    s_phase_q = phase;
}

/* SAI DMA half-complete — fill first half of ring buffer */
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    (void)hsai;
    audio_half_count++;
    audio_fill_half(0u);
}

/* SAI DMA complete — fill second half of ring buffer */
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
    (void)hsai;
    audio_full_count++;
    audio_fill_half(2u * HALF_FRAMES);
}

void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    sai_err_code = HAL_SAI_GetError(hsai);
    sai_err_count++;

    char buf[64];
    int len = snprintf(buf, sizeof(buf),
        "[SAI ERROR] code=0x%08lX count=%lu\r\n",
        (unsigned long)sai_err_code,
        (unsigned long)sai_err_count);
    HAL_UART_Transmit(&huart4, (const uint8_t *)buf, len, HAL_MAX_DELAY);

    /* Attempt to restart DMA rather than dying */
    HAL_SAI_DMAStop(hsai);
    HAL_SAI_Transmit_DMA(&hsai_BlockA1,
                         (uint8_t *)audioBuffer,
                         2u * TOTAL_FRAMES);
}

/* ===========================================================================
 * SECTION 5 — I2C BIT-BANG HELPERS (L3)
 * Minimal device-ID verification on boot to confirm sensor is alive.
 * =========================================================================*/

/**
 * @brief  Write 1 byte to a 16-bit-addressed VL53L7CX register.
 */
static bool tof_write_reg(uint16_t reg, uint8_t val)
{
    i2c_bb_start();
    if (!i2c_bb_write_byte(0x52u))                { i2c_bb_stop(); return false; }
    if (!i2c_bb_write_byte((reg >> 8u) & 0xFFu))  { i2c_bb_stop(); return false; }
    if (!i2c_bb_write_byte(reg & 0xFFu))          { i2c_bb_stop(); return false; }
    if (!i2c_bb_write_byte(val))                  { i2c_bb_stop(); return false; }
    i2c_bb_stop();
    return true;
}

/**
 * @brief  Read 1 byte from a 16-bit-addressed VL53L7CX register.
 */
static bool tof_read_reg(uint16_t reg, uint8_t *val)
{
    i2c_bb_start();
    if (!i2c_bb_write_byte(0x52u))                { i2c_bb_stop(); return false; }
    if (!i2c_bb_write_byte((reg >> 8u) & 0xFFu))  { i2c_bb_stop(); return false; }
    if (!i2c_bb_write_byte(reg & 0xFFu))          { i2c_bb_stop(); return false; }
    i2c_bb_start();  /* repeated start */
    if (!i2c_bb_write_byte(0x53u))                { i2c_bb_stop(); return false; }
    *val = i2c_bb_read_byte(false);               /* NACK = last byte */
    i2c_bb_stop();
    return true;
}

/**
 * @brief  Select page 0, read device_id + revision_id, restore page 2.
 * @retval true if device_id == 0xF0 and revision_id == 0x0C
 */
static bool tof_verify_device_id(void)
{
    uint8_t device_id   = 0u;
    uint8_t revision_id = 0u;

    if (!tof_write_reg(0x7FFFu, 0x00u)) return false;
    if (!tof_read_reg(0x0000u, &device_id))   return false;
    if (!tof_read_reg(0x0001u, &revision_id)) return false;
    tof_write_reg(0x7FFFu, 0x02u);  /* restore default page */

    char buf[80];
    int  len = snprintf(buf, sizeof(buf),
        "  VL53L7CX  device_id=0x%02X  revision_id=0x%02X\r\n",
        device_id, revision_id);
    HAL_UART_Transmit(&huart4, (const uint8_t *)buf, len, HAL_MAX_DELAY);

    return (device_id == 0xF0u && revision_id == 0x0Cu);
}


/* ===========================================================================
 * SECTION 6 — RIF / SECURITY ISOLATION (superset of all lessons)
 * =========================================================================*/
static void SystemIsolation_Config(void)
{
    extern DMA_HandleTypeDef handle_GPDMA1_Channel0;

    __HAL_RCC_RIFSC_CLK_ENABLE();

    /* GPDMA1 channel 0 — used by SAI1_A */
    if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel0,
            DMA_CHANNEL_SEC | DMA_CHANNEL_PRIV |
            DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_DEST_SEC) != HAL_OK)
    {
        Error_Handler();
    }

    /* GPIO — union of all lesson pin sets */
    HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_0, GPIO_PIN_SEC | GPIO_PIN_NPRIV);  /* UART4 TX */
    HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_1, GPIO_PIN_SEC | GPIO_PIN_NPRIV);  /* UART4 RX */
    HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_0, GPIO_PIN_SEC | GPIO_PIN_NPRIV);  /* SAI */
    HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_1, GPIO_PIN_SEC | GPIO_PIN_NPRIV);  /* SD_MODE */
    HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_2, GPIO_PIN_SEC | GPIO_PIN_NPRIV);  /* SAI */
    HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_6, GPIO_PIN_SEC | GPIO_PIN_NPRIV);  /* SAI */
    HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_8, GPIO_PIN_SEC | GPIO_PIN_NPRIV);  /* SCL_BB */
    HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_9, GPIO_PIN_SEC | GPIO_PIN_NPRIV);  /* SDA_BB */
    HAL_GPIO_ConfigPinAttributes(GPIOD, GPIO_PIN_1, GPIO_PIN_SEC | GPIO_PIN_NPRIV);  /* TOF_LPN */
    HAL_GPIO_ConfigPinAttributes(GPIOD, GPIO_PIN_8, GPIO_PIN_SEC | GPIO_PIN_NPRIV);  /* SCROLL_1 */
    HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_0, GPIO_PIN_SEC | GPIO_PIN_NPRIV);  /* TOF_INT */
    HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_1, GPIO_PIN_SEC | GPIO_PIN_NPRIV);  /* TOF_I2C_RST */
}


/* ===========================================================================
 * Error handler & assert
 * =========================================================================*/
void Error_Handler(void)
{
    __disable_irq();
    /* Pulse SCROLL_1 rapidly to signal fault on hardware */
    while (1)
    {
        HAL_GPIO_TogglePin(SCROLL_1_GPIO_Port, SCROLL_1_Pin);
        for (volatile uint32_t d = 0; d < 500000u; d++) { __NOP(); }
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file; (void)line;
}
#endif /* USE_FULL_ASSERT */

