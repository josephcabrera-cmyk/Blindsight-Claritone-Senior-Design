/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */
extern DMA_HandleTypeDef handle_GPDMA1_Channel0 ;

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define HALF_FRAMES   128
#define TOTAL_FRAMES  (2 * HALF_FRAMES)
static int32_t audioBuffer[2 * TOTAL_FRAMES];  /* stereo int32 ping-pong: 4*HALF_FRAMES words */

/* Sine oscillator state persists across calls so phase is continuous */
static float s_phase = 0.0f;
static const float s_phase_inc = 2.0f * 3.1415926535f * 440.0f / 48000.0f;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SystemIsolation_Config(void);
/* USER CODE BEGIN PFP */
static void sine_lut_init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */
  SCB_DisableDCache();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_GPDMA1_Init();
  MX_UART4_Init();
  MX_SAI1_Init();
  SystemIsolation_Config();
  /* USER CODE BEGIN 2 */

  /* Drive SD_MODE (PB1) high to enable amps */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef g = {0};
    g.Pin = GPIO_PIN_1;
    g.Mode = GPIO_MODE_OUTPUT_PP;
    g.Pull = GPIO_NOPULL;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &g);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

    /* Build sine LUT (calls sinf() in main context only, never in ISR) */
    sine_lut_init();

    /* Prime both halves with silence; callbacks will fill thereafter */
    memset(audioBuffer, 0, sizeof(audioBuffer));

    /* Start DMA */
    if (HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t*)audioBuffer, 2*TOTAL_FRAMES) != HAL_OK)
    {
        Error_Handler();
    }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief RIF Initialization Function
  * @param None
  * @retval None
  */
  static void SystemIsolation_Config(void)
{

/* USER CODE BEGIN RIF_Init 0 */

/* USER CODE END RIF_Init 0 */

  /* set all required IPs as secure privileged */
  __HAL_RCC_RIFSC_CLK_ENABLE();

  /* RIF-Aware IPs Config */

  /* set up GPDMA configuration */
  /* set GPDMA1 channel 0 used by SAI1 */
  if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel0,DMA_CHANNEL_SEC|DMA_CHANNEL_PRIV|DMA_CHANNEL_SRC_SEC|DMA_CHANNEL_DEST_SEC)!= HAL_OK )
  {
    Error_Handler();
  }

  /* set up GPIO configuration */
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_0,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOA,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_0,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_2,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_6,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_8,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOB,GPIO_PIN_9,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOD,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOD,GPIO_PIN_8,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOE,GPIO_PIN_0,GPIO_PIN_SEC|GPIO_PIN_NPRIV);
  HAL_GPIO_ConfigPinAttributes(GPIOE,GPIO_PIN_1,GPIO_PIN_SEC|GPIO_PIN_NPRIV);

/* USER CODE BEGIN RIF_Init 1 */

/* USER CODE END RIF_Init 1 */
/* USER CODE BEGIN RIF_Init 2 */

/* USER CODE END RIF_Init 2 */

}

/* USER CODE BEGIN 4 */

  volatile uint32_t halfCount = 0;
  volatile uint32_t fullCount = 0;
  volatile uint32_t sai_err_code = 0;
  volatile uint32_t sai_err_count = 0;

  /* Fill HALF_FRAMES stereo frames starting at word offset.
   * Writes Left then Right, both with the same sine sample. */
  /* Sine LUT approach -- no floating point in ISR */
  #define SINE_LUT_SIZE   256u      /* power of 2, one full period */
  static int32_t sine_lut[SINE_LUT_SIZE];

  /* Phase in LUT indices, Q16.16 fixed-point.
   * High 16 bits = integer index, low 16 bits = fractional. */
  static uint32_t s_phase_q = 0;
  static uint32_t s_phase_inc_q = 0;

  static void sine_lut_init(void)
  {
      const float twoPi = 2.0f * 3.1415926535f;
      for (uint32_t i = 0; i < SINE_LUT_SIZE; ++i) {
          float s = sinf(twoPi * (float)i / (float)SINE_LUT_SIZE);
          sine_lut[i] = (int32_t)(0x7FFFFFFF * 0.2f * s);
      }
      /* Phase increment per sample = (freq / sample_rate) * LUT_SIZE * 65536
       * For 440 Hz: (440/48000) * 256 * 65536 = 153834 (approx) */
      s_phase_inc_q = (uint32_t)(((uint64_t)440 * SINE_LUT_SIZE * 65536u) / 48000u);
      s_phase_q = 0;
  }

  static void fill_frames(uint32_t word_offset)
  {
      uint32_t phase = s_phase_q;
      const uint32_t inc = s_phase_inc_q;
      const uint32_t mask = (SINE_LUT_SIZE - 1u) << 16;  /* masks integer part */

      for (int i = 0; i < HALF_FRAMES; ++i) {
          uint32_t idx = (phase & mask) >> 16;
          int32_t s = sine_lut[idx];
          audioBuffer[word_offset + 2*i + 0] = s;
          audioBuffer[word_offset + 2*i + 1] = s;
          phase += inc;
      }

      s_phase_q = phase;
  }

  void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
  {
      (void)hsai;
      halfCount++;
      fill_frames(0);                    /* fill first half */
  }

  void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
  {
      (void)hsai;
      fullCount++;
      fill_frames(2 * HALF_FRAMES);      /* fill second half (word offset) */
  }

  void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
  {
      sai_err_code = HAL_SAI_GetError(hsai);
      sai_err_count++;
      __asm volatile("BKPT #0");
  }

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
