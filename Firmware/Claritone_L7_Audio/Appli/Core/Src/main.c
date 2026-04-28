/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Claritone L7 Audio + Front ToF + Sensitivity button
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "gpdma.h"
#include "sai.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "tof_front.h"
/* USER CODE END Includes */

extern DMA_HandleTypeDef handle_GPDMA1_Channel0;

/* USER CODE BEGIN PV */
typedef enum {
    SENS_CLOSE = 0,
    SENS_MEDIUM,
    SENS_FAR,
    SENS_COUNT
} sensitivity_mode_t;

static const uint16_t sens_max_mm[SENS_COUNT] = { 300, 800, 1500 };
static const char    *sens_name[SENS_COUNT]   = { "CLOSE", "MEDIUM", "FAR" };

static volatile sensitivity_mode_t g_sens_mode = SENS_CLOSE;

#define HALF_FRAMES   128
#define TOTAL_FRAMES  (2 * HALF_FRAMES)
static int32_t audioBuffer[2 * TOTAL_FRAMES];

volatile int16_t g_volume_q15 = 0;
/* USER CODE END PV */

static void SystemIsolation_Config(void);

/* USER CODE BEGIN PFP */
static void sine_lut_init(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
static uint8_t btn_sens_check_press(void)
{
    static uint8_t debounce_count = 0;
    static uint8_t armed = 1;

    uint8_t raw = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6) == GPIO_PIN_RESET) ? 0 : 1;

    if (raw == 0) {
        if (debounce_count < 3) debounce_count++;
    } else {
        debounce_count = 0;
        armed = 1;
    }

    if (debounce_count >= 3 && armed) {
        armed = 0;
        return 1;
    }
    return 0;
}
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();

  /* USER CODE BEGIN SysInit */
  SCB_DisableDCache();
  /* USER CODE END SysInit */

  MX_GPIO_Init();
  MX_GPDMA1_Init();
  MX_UART4_Init();
  MX_SAI1_Init();
  SystemIsolation_Config();

  /* USER CODE BEGIN 2 */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitTypeDef g = {0};
  g.Pin = GPIO_PIN_1;
  g.Mode = GPIO_MODE_OUTPUT_PP;
  g.Pull = GPIO_NOPULL;
  g.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &g);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

  sine_lut_init();

  memset(audioBuffer, 0, sizeof(audioBuffer));
  if (HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t*)audioBuffer, 2*TOTAL_FRAMES) != HAL_OK)
  {
      Error_Handler();
  }

  HAL_UART_Transmit(&huart4,
      (const uint8_t *)"\r\nClaritone Demo - initializing front ToF...\r\n", 47, HAL_MAX_DELAY);

  uint8_t tof_status = ToF_Front_Init();
  if (tof_status != 0) {
      char msg[48];
      int len = snprintf(msg, sizeof(msg), "ToF init FAILED (code=%u)\r\n", tof_status);
      HAL_UART_Transmit(&huart4, (const uint8_t *)msg, len, HAL_MAX_DELAY);
  } else {
      HAL_UART_Transmit(&huart4,
          (const uint8_t *)"ToF ready\r\n\r\n", 13, HAL_MAX_DELAY);
  }
  /* USER CODE END 2 */

  while (1)
  {
    /* USER CODE BEGIN 3 */
    if (btn_sens_check_press()) {
        g_sens_mode = (g_sens_mode + 1) % SENS_COUNT;
        char msg[64];
        int len = snprintf(msg, sizeof(msg),
            ">>> Sensitivity: %s (max %u mm)\r\n",
            sens_name[g_sens_mode], sens_max_mm[g_sens_mode]);
        HAL_UART_Transmit(&huart4, (const uint8_t *)msg, len, 10);
    }

    tof_front_state_t st;
    if (ToF_Front_Poll(&st)) {
        if (st.valid) {
            int32_t d = st.distance_mm;
            int32_t max_mm = sens_max_mm[g_sens_mode];
            int32_t vol;
            if (d < 50)            vol = 32767;
            else if (d > max_mm)   vol = 0;
            else                   vol = (int32_t)(32767L * (max_mm - d) / (max_mm - 50));
            g_volume_q15 = (int16_t)vol;

            char msg[64];
            int len = snprintf(msg, sizeof(msg),
                "ToF: %u mm  vol=%d  [%s]\r\n",
                st.distance_mm, (int)g_volume_q15, sens_name[g_sens_mode]);
            HAL_UART_Transmit(&huart4, (const uint8_t *)msg, len, 10);
        } else {
            g_volume_q15 = 0;
        }
    }
    HAL_Delay(10);
    /* USER CODE END 3 */
  }
}

static void SystemIsolation_Config(void)
{
  __HAL_RCC_RIFSC_CLK_ENABLE();

  if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel0,
        DMA_CHANNEL_SEC|DMA_CHANNEL_PRIV|DMA_CHANNEL_SRC_SEC|DMA_CHANNEL_DEST_SEC) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_0,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_0,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_2,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_6,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_8,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_9,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOC,GPIO_PIN_4,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOC,GPIO_PIN_6,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOD,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOD,GPIO_PIN_8,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOE,GPIO_PIN_0,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOE,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
}

/* USER CODE BEGIN 4 */
volatile uint32_t halfCount = 0;
volatile uint32_t fullCount = 0;
volatile uint32_t sai_err_code = 0;
volatile uint32_t sai_err_count = 0;

#define SINE_LUT_SIZE   256u
static int32_t sine_lut[SINE_LUT_SIZE];
static uint32_t s_phase_q = 0;

static void sine_lut_init(void)
{
    const float twoPi = 2.0f * 3.1415926535f;
    for (uint32_t i = 0; i < SINE_LUT_SIZE; ++i) {
        float s = sinf(twoPi * (float)i / (float)SINE_LUT_SIZE);
        sine_lut[i] = (int32_t)(0x7FFFFFFF * 0.9f * s);
    }
    s_phase_q = 0;
}

static void fill_frames(uint32_t word_offset)
{
    uint32_t phase = s_phase_q;
    const uint32_t mask = (SINE_LUT_SIZE - 1u) << 16;
    int32_t vol = g_volume_q15;

    if (vol < 0) vol = 0;
    if (vol > 32767) vol = 32767;

    /* Pitch follows volume: 220 Hz quiet -> 1320 Hz close */
    uint32_t freq_hz = 220u + ((uint32_t)vol * 1100u) / 32767u;
    uint32_t inc = (uint32_t)(((uint64_t)freq_hz * SINE_LUT_SIZE * 65536u) / 48000u);

    for (int i = 0; i < HALF_FRAMES; ++i) {
        uint32_t idx = (phase & mask) >> 16;
        int32_t s = sine_lut[idx];
        int32_t out = (int32_t)(((int64_t)s * vol) >> 15);
        audioBuffer[word_offset + 2*i + 0] = out;
        audioBuffer[word_offset + 2*i + 1] = out;
        phase += inc;
    }

    s_phase_q = phase;
}

void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    (void)hsai;
    halfCount++;
    fill_frames(0);
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
    (void)hsai;
    fullCount++;
    fill_frames(2 * HALF_FRAMES);
}

void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    (void)hsai;
    sai_err_code = HAL_SAI_GetError(hsai);
    sai_err_count++;
}
/* USER CODE END 4 */

void Error_Handler(void)
{
  __disable_irq();
  while (1) { }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file; (void)line;
}
#endif
