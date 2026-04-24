/**
 ******************************************************************************
 * @file    main.c
 * @author  GPM Application Team + Claritone modification
 * @brief   Headless custom-PCB bring-up target for Claritone.
 ******************************************************************************
 */

#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "app_camerapipeline.h"
#include "app_config.h"
#include "app_postprocess.h"
#include "claritone_custom_board.h"
#include "crop_img.h"
#include "main.h"
#include "stai.h"
#include "stai_network.h"
#include "stm32n6xx_nucleo.h"

CLASSES_TABLE;

#ifndef APP_GIT_SHA1_STRING
#define APP_GIT_SHA1_STRING "dev"
#endif
#ifndef APP_VERSION_STRING
#define APP_VERSION_STRING "unversioned"
#endif

typedef enum
{
  ZONE_LEFT = 0,
  ZONE_CENTER,
  ZONE_RIGHT
} claritone_zone_t;

typedef struct
{
  bool valid;
  int class_id;
  float conf;
  float x_center;
  float y_center;
  float width;
  float height;
  claritone_zone_t zone;
} claritone_target_t;

#if POSTPROCESS_TYPE == POSTPROCESS_OD_YOLO_V2_UI
  od_yolov2_pp_static_param_t pp_params;
#elif POSTPROCESS_TYPE == POSTPROCESS_OD_YOLO_V5_UU
  od_yolov5_pp_static_param_t pp_params;
#elif POSTPROCESS_TYPE == POSTPROCESS_OD_YOLO_V8_UI
  od_yolov8_pp_static_param_t pp_params;
#elif POSTPROCESS_TYPE == POSTPROCESS_OD_ST_YOLOX_UI
  od_st_yolox_pp_static_param_t pp_params;
#elif POSTPROCESS_TYPE == POSTPROCESS_OD_SSD_UI
  od_ssd_pp_static_param_t pp_params;
#elif POSTPROCESS_TYPE == POSTPROCESS_OD_ST_YOLOD_UI
  od_yolo_d_pp_static_param_t pp_params;
#elif POSTPROCESS_TYPE == POSTPROCESS_OD_BLAZEFACE_UI
  od_blazeface_pp_static_param_t pp_params;
#else
  #error "PostProcessing type not supported"
#endif

UART_HandleTypeDef huart_console;
volatile int32_t cameraFrameReceived;
volatile uint32_t claritone_boot_stage;
volatile uint32_t claritone_frame_callback_count;
stai_ptr nn_in;
od_pp_out_t pp_output;

#define ALIGN_TO_16(value) (((value) + 15) & ~15)

#if (STAI_NETWORK_IN_1_WIDTH * STAI_NETWORK_IN_1_CHANNEL) != ALIGN_TO_16(STAI_NETWORK_IN_1_WIDTH * STAI_NETWORK_IN_1_CHANNEL)
#define DCMIPP_NN_NEEDS_CROP 1
#define DCMIPP_OUT_NN_LEN      (ALIGN_TO_16(STAI_NETWORK_IN_1_WIDTH * STAI_NETWORK_IN_1_CHANNEL) * STAI_NETWORK_IN_1_HEIGHT)
#define DCMIPP_OUT_NN_BUFF_LEN (DCMIPP_OUT_NN_LEN + 32 - DCMIPP_OUT_NN_LEN % 32)

__attribute__((aligned(32)))
static uint8_t dcmipp_out_nn[DCMIPP_OUT_NN_BUFF_LEN];
#else
#define DCMIPP_NN_NEEDS_CROP 0
#endif

STAI_NETWORK_CONTEXT_DECLARE(network_context, STAI_NETWORK_CONTEXT_SIZE)

static void SystemClock_Config(void);
static void CONSOLE_Config(void);
static void NPURam_enable(void);
static void NPUCache_config(void);
static void Security_Config(void);
static void set_clk_sleep_mode(void);
static void IAC_Config(void);
static void Hardware_init(void);
static void NeuralNetwork_init(uint32_t *nn_in_length, stai_ptr *nn_out, stai_size *number_output, int32_t nn_out_len[]);
static void Claritone_CameraLedDisable(void);
static void Claritone_BootLog(const char *message);

static const char *Claritone_ZoneToString(claritone_zone_t zone);
static claritone_zone_t Claritone_GetZone(float x_center);
static bool Claritone_IsPersonClass(int class_id);
static claritone_target_t Claritone_SelectBestPerson(const od_pp_out_t *p_postprocess);
static void Claritone_LogDetections(const od_pp_out_t *p_postprocess);
static void Claritone_HandleFrame(const od_pp_out_t *p_postprocess);

int main(void)
{
  claritone_boot_stage = 0x10;
  Hardware_init();
  Claritone_BootLog("BOOT 10: hardware init complete\r\n");

  uint32_t nn_in_len = 0;
  stai_size number_output = 0;
  stai_ptr nn_out[STAI_NETWORK_OUT_NUM] = {0};
  int32_t nn_out_len[STAI_NETWORK_OUT_NUM] = {0};
  uint32_t pitch_nn = 0;
  stai_network_info info;
  int ret;

  claritone_boot_stage = 0x20;
  Claritone_BootLog("BOOT 20: neural network init start\r\n");
  NeuralNetwork_init(&nn_in_len, nn_out, &number_output, nn_out_len);
  claritone_boot_stage = 0x21;
  Claritone_BootLog("BOOT 21: neural network init done\r\n");

  ret = stai_network_get_info(network_context, &info);
  assert(ret == STAI_SUCCESS);
  app_postprocess_init(&pp_params, &info);
  claritone_boot_stage = 0x22;
  Claritone_BootLog("BOOT 22: postprocess init done\r\n");

  claritone_boot_stage = 0x30;
  Claritone_BootLog("BOOT 30: camera pipeline init start\r\n");
  CameraPipeline_Init(&pitch_nn);
  claritone_boot_stage = 0x31;
  Claritone_BootLog("BOOT 31: camera pipeline init done\r\n");

  printf("========================================\r\n");
  printf("Claritone Custom PCB Bring-Up %s (%s)\r\n", APP_VERSION_STRING, APP_GIT_SHA1_STRING);
  printf("Build date & time: %s %s\r\n", __DATE__, __TIME__);
#if defined(__GNUC__)
  printf("Compiler: GCC %d.%d.%d\r\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(__ICCARM__)
  printf("Compiler: IAR EWARM %d.%d.%d\r\n", __VER__ / 1000000, (__VER__ / 1000) % 1000, __VER__ % 1000);
#else
  printf("Compiler: Unknown\r\n");
#endif
  printf("HAL: %lu.%lu.%lu\r\n", __STM32N6xx_HAL_VERSION_MAIN, __STM32N6xx_HAL_VERSION_SUB1, __STM32N6xx_HAL_VERSION_SUB2);
  printf("STEdgeAI Tools: %d.%d.%d\r\n", STAI_TOOLS_VERSION_MAJOR, STAI_TOOLS_VERSION_MINOR, STAI_TOOLS_VERSION_MICRO);
  printf("NN model: %s\r\n", STAI_NETWORK_ORIGIN_MODEL_NAME);
  printf("Selected sensor: VD55G1\r\n");
  printf("Mode: headless UART detections\r\n");
  printf("========================================\r\n");

  while (1)
  {
    CameraPipeline_IspUpdate();
    Claritone_BootLog("BOOT 40: arming NN pipe\r\n");

#if DCMIPP_NN_NEEDS_CROP
    CameraPipeline_NNPipe_Start(dcmipp_out_nn, CMW_MODE_SNAPSHOT);
#else
    CameraPipeline_NNPipe_Start(nn_in, CMW_MODE_SNAPSHOT);
#endif
    Claritone_BootLog("BOOT 41: NN pipe armed\r\n");

    uint32_t wait_start = HAL_GetTick();
    while (cameraFrameReceived == 0)
    {
      if ((HAL_GetTick() - wait_start) > 2000U)
      {
        printf("FRAME TIMEOUT: callbacks=%lu DCMIPP_P2SR=0x%08lX DCMIPP_P2IER=0x%08lX CSI_SR0=0x%08lX CSI_SR1=0x%08lX\r\n",
               (unsigned long)claritone_frame_callback_count,
               (unsigned long)CMW_CAMERA_GetDCMIPPHandle()->Instance->P2SR,
               (unsigned long)CMW_CAMERA_GetDCMIPPHandle()->Instance->P2IER,
               (unsigned long)CSI->SR0,
               (unsigned long)CSI->SR1);
        wait_start = HAL_GetTick();
      }
    }
    Claritone_BootLog("BOOT 42: frame received\r\n");
    cameraFrameReceived = 0;

    uint32_t ts[2] = {0};

#if DCMIPP_NN_NEEDS_CROP
    SCB_InvalidateDCache_by_Addr(dcmipp_out_nn, sizeof(dcmipp_out_nn));
    img_crop(dcmipp_out_nn, nn_in, pitch_nn, STAI_NETWORK_IN_1_WIDTH, STAI_NETWORK_IN_1_HEIGHT, STAI_NETWORK_IN_1_CHANNEL);
    SCB_CleanInvalidateDCache_by_Addr(nn_in, nn_in_len);
#endif

    claritone_boot_stage = 0x50;
    Claritone_BootLog("BOOT 50: stai_network_run start\r\n");
    ts[0] = HAL_GetTick();
    ret = stai_network_run(network_context, STAI_MODE_SYNC);
    ts[1] = HAL_GetTick();
    printf("BOOT 51: stai_network_run ret=%ld\r\n", (long)ret);
    assert(ret == 0);

    claritone_boot_stage = 0x52;
    Claritone_BootLog("BOOT 52: postprocess start\r\n");
    ret = app_postprocess_run((void **)nn_out, number_output, &pp_output, &pp_params);
    printf("BOOT 53: postprocess ret=%ld\r\n", (long)ret);
    assert(ret == 0);
    claritone_boot_stage = 0x54;

    Claritone_HandleFrame(&pp_output);
    printf("INFERENCE: %lums objects=%lu\r\n",
           (unsigned long)(ts[1] - ts[0]),
           (unsigned long)pp_output.nb_detect);

    for (int i = 0; i < number_output; i++)
    {
      void *tmp = nn_out[i];
      SCB_InvalidateDCache_by_Addr(tmp, nn_out_len[i]);
    }
  }
}

static const char *Claritone_ZoneToString(claritone_zone_t zone)
{
  switch (zone)
  {
    case ZONE_LEFT:
      return "LEFT";
    case ZONE_CENTER:
      return "CENTER";
    case ZONE_RIGHT:
      return "RIGHT";
    default:
      return "UNKNOWN";
  }
}

static claritone_zone_t Claritone_GetZone(float x_center)
{
  if (x_center < 0.33f)
  {
    return ZONE_LEFT;
  }
  else if (x_center < 0.66f)
  {
    return ZONE_CENTER;
  }
  else
  {
    return ZONE_RIGHT;
  }
}

static bool Claritone_IsPersonClass(int class_id)
{
  if ((class_id < 0) || (class_id >= NB_CLASSES))
  {
    return false;
  }

  return (classes_table[class_id] != NULL) && (strcmp(classes_table[class_id], "person") == 0);
}

static void Claritone_LogDetections(const od_pp_out_t *p_postprocess)
{
  od_pp_outBuffer_t *rois = p_postprocess->pOutBuff;
  uint32_t nb_rois = p_postprocess->nb_detect;

  for (uint32_t i = 0; i < nb_rois; i++)
  {
    const char *label = ((rois[i].class_index >= 0) && (rois[i].class_index < NB_CLASSES)) ? classes_table[rois[i].class_index] : "unknown";
    claritone_zone_t zone = Claritone_GetZone(rois[i].x_center);

    printf("DET %lu: class=%ld label=%s conf=%.0f%% x=%.2f y=%.2f w=%.2f h=%.2f zone=%s\r\n",
           (unsigned long)i,
           (long)rois[i].class_index,
           label,
           rois[i].conf * 100.0f,
           rois[i].x_center,
           rois[i].y_center,
           rois[i].width,
           rois[i].height,
           Claritone_ZoneToString(zone));
  }
}

static claritone_target_t Claritone_SelectBestPerson(const od_pp_out_t *p_postprocess)
{
  claritone_target_t best = {0};
  od_pp_outBuffer_t *rois = p_postprocess->pOutBuff;
  uint32_t nb_rois = p_postprocess->nb_detect;

  for (uint32_t i = 0; i < nb_rois; i++)
  {
    if (!Claritone_IsPersonClass(rois[i].class_index))
    {
      continue;
    }

    if (!best.valid || (rois[i].conf > best.conf))
    {
      best.valid = true;
      best.class_id = rois[i].class_index;
      best.conf = rois[i].conf;
      best.x_center = rois[i].x_center;
      best.y_center = rois[i].y_center;
      best.width = rois[i].width;
      best.height = rois[i].height;
      best.zone = Claritone_GetZone(rois[i].x_center);
    }
  }

  return best;
}

static void Claritone_HandleFrame(const od_pp_out_t *p_postprocess)
{
  static uint32_t frame_counter = 0;
  claritone_target_t best_person;

  frame_counter++;
  if ((frame_counter % 10U) == 0U)
  {
    Claritone_LogDetections(p_postprocess);
  }

  best_person = Claritone_SelectBestPerson(p_postprocess);
  if (best_person.valid)
  {
    printf("PERSON: zone=%s conf=%.0f%% x=%.2f y=%.2f\r\n",
           Claritone_ZoneToString(best_person.zone),
           best_person.conf * 100.0f,
           best_person.x_center,
           best_person.y_center);
  }
}

HAL_StatusTypeDef MX_DCMIPP_ClockConfig(DCMIPP_HandleTypeDef *hdcmipp)
{
  RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct = {0};
  HAL_StatusTypeDef ret = HAL_OK;

  UNUSED(hdcmipp);

  RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_DCMIPP;
  RCC_PeriphCLKInitStruct.DcmippClockSelection = RCC_DCMIPPCLKSOURCE_IC17;
  RCC_PeriphCLKInitStruct.ICSelection[RCC_IC17].ClockSelection = RCC_ICCLKSOURCE_PLL2;
  RCC_PeriphCLKInitStruct.ICSelection[RCC_IC17].ClockDivider = 3;
  ret = HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);
  if (ret != HAL_OK)
  {
    return ret;
  }

  RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CSI;
  RCC_PeriphCLKInitStruct.ICSelection[RCC_IC18].ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_PeriphCLKInitStruct.ICSelection[RCC_IC18].ClockDivider = 40;
  ret = HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);
  return ret;
}

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  BSP_SMPS_Init(SMPS_VOLTAGE_OVERDRIVE);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;

  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL1.PLLM = 2;
  RCC_OscInitStruct.PLL1.PLLN = 25;
  RCC_OscInitStruct.PLL1.PLLFractional = 0;
  RCC_OscInitStruct.PLL1.PLLP1 = 1;
  RCC_OscInitStruct.PLL1.PLLP2 = 1;

  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL2.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL2.PLLM = 8;
  RCC_OscInitStruct.PLL2.PLLFractional = 0;
  RCC_OscInitStruct.PLL2.PLLN = 125;
  RCC_OscInitStruct.PLL2.PLLP1 = 1;
  RCC_OscInitStruct.PLL2.PLLP2 = 1;

  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL3.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL3.PLLM = 8;
  RCC_OscInitStruct.PLL3.PLLN = 225;
  RCC_OscInitStruct.PLL3.PLLFractional = 0;
  RCC_OscInitStruct.PLL3.PLLP1 = 1;
  RCC_OscInitStruct.PLL3.PLLP2 = 2;

  RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL4.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL4.PLLM = 8;
  RCC_OscInitStruct.PLL4.PLLFractional = 0;
  RCC_OscInitStruct.PLL4.PLLN = 225;
  RCC_OscInitStruct.PLL4.PLLP1 = 6;
  RCC_OscInitStruct.PLL4.PLLP2 = 6;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while (1)
    {
    }
  }

  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_SYSCLK |
                                 RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 |
                                 RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK4 |
                                 RCC_CLOCKTYPE_PCLK5);

  RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_IC1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_IC2_IC6_IC11;
  RCC_ClkInitStruct.IC1Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC1Selection.ClockDivider = 1;
  RCC_ClkInitStruct.IC2Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC2Selection.ClockDivider = 2;
  RCC_ClkInitStruct.IC6Selection.ClockSelection = RCC_ICCLKSOURCE_PLL2;
  RCC_ClkInitStruct.IC6Selection.ClockDivider = 1;
  RCC_ClkInitStruct.IC11Selection.ClockSelection = RCC_ICCLKSOURCE_PLL3;
  RCC_ClkInitStruct.IC11Selection.ClockDivider = 1;

  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
  RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    while (1)
    {
    }
  }
}

static void CONSOLE_Config(void)
{
  RCC_PeriphCLKInitTypeDef periph_clk = {0};
  GPIO_InitTypeDef gpio_init = {0};

  periph_clk.PeriphClockSelection = RCC_PERIPHCLK_UART4;
  periph_clk.Uart4ClockSelection = RCC_UART4CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&periph_clk) != HAL_OK)
  {
    while (1)
    {
    }
  }

  CLARITONE_CONSOLE_UART_CLK_ENABLE();
  CLARITONE_CONSOLE_GPIO_CLK_ENABLE();
  __HAL_RCC_UART4_FORCE_RESET();
  __HAL_RCC_UART4_RELEASE_RESET();

  gpio_init.Mode = GPIO_MODE_AF_PP;
  gpio_init.Pull = GPIO_PULLUP;
  gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio_init.Alternate = CLARITONE_CONSOLE_TX_AF;
  gpio_init.Pin = CLARITONE_CONSOLE_TX_PIN;
  HAL_GPIO_Init(CLARITONE_CONSOLE_TX_PORT, &gpio_init);

  gpio_init.Alternate = CLARITONE_CONSOLE_RX_AF;
  gpio_init.Pin = CLARITONE_CONSOLE_RX_PIN;
  HAL_GPIO_Init(CLARITONE_CONSOLE_RX_PORT, &gpio_init);

  huart_console.Instance = CLARITONE_CONSOLE_UART;
  huart_console.Init.BaudRate = 115200;
  huart_console.Init.Mode = UART_MODE_TX_RX;
  huart_console.Init.Parity = UART_PARITY_NONE;
  huart_console.Init.WordLength = UART_WORDLENGTH_8B;
  huart_console.Init.StopBits = UART_STOPBITS_1;
  huart_console.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart_console.Init.OverSampling = UART_OVERSAMPLING_8;

  if (HAL_UART_Init(&huart_console) != HAL_OK)
  {
    while (1)
    {
    }
  }

  claritone_boot_stage = 0x04;
  Claritone_BootLog("\r\nBOOT 04: console ready on UART4 PA0/PA1\r\n");
}

int _write(int file, char *ptr, int len)
{
  HAL_StatusTypeDef status;

  if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
  {
    errno = EBADF;
    return -1;
  }

  status = HAL_UART_Transmit(&huart_console, (uint8_t *)ptr, len, HAL_MAX_DELAY);
  return (status == HAL_OK ? len : 0);
}

void __assert_func(const char *filename, int line, const char *assert_func, const char *expr)
{
  char buffer[256];
  int len;

  len = snprintf(buffer, sizeof(buffer),
                 "\r\nASSERT: boot=0x%02lX file=%s line=%d func=%s expr=%s\r\n",
                 (unsigned long)claritone_boot_stage,
                 (filename != NULL) ? filename : "?",
                 line,
                 (assert_func != NULL) ? assert_func : "?",
                 (expr != NULL) ? expr : "?");

  if ((len > 0) && (huart_console.Instance != NULL))
  {
    uint16_t tx_len = (uint16_t)((len < (int)sizeof(buffer)) ? len : ((int)sizeof(buffer) - 1));
    (void)HAL_UART_Transmit(&huart_console, (uint8_t *)buffer, tx_len, 1000U);
  }

  __BKPT(0);
  while (1)
  {
  }
}

static void Hardware_init(void)
{
  claritone_boot_stage = 0x01;
  MEMSYSCTL->MSCR |= MEMSYSCTL_MSCR_ICACTIVE_Msk;

  __HAL_RCC_CPUCLK_CONFIG(RCC_CPUCLKSOURCE_HSI);
  __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_HSI);

  HAL_Init();
  SCB_EnableICache();

#if defined(USE_DCACHE)
  MEMSYSCTL->MSCR |= MEMSYSCTL_MSCR_DCACTIVE_Msk;
  SCB_EnableDCache();
#endif

  claritone_boot_stage = 0x02;
  SystemClock_Config();
  claritone_boot_stage = 0x03;
  CONSOLE_Config();
  claritone_boot_stage = 0x05;
  Claritone_CameraLedDisable();
  claritone_boot_stage = 0x06;
  NPURam_enable();
  claritone_boot_stage = 0x07;
  NPUCache_config();
  claritone_boot_stage = 0x08;
  Security_Config();
  claritone_boot_stage = 0x09;
  IAC_Config();
  claritone_boot_stage = 0x0A;
  set_clk_sleep_mode();
}

static void Claritone_BootLog(const char *message)
{
  if ((huart_console.Instance == NULL) || (message == NULL))
  {
    return;
  }

  (void)HAL_UART_Transmit(&huart_console, (uint8_t *)message, (uint16_t)strlen(message), 100U);
}

static void Claritone_CameraLedDisable(void)
{
  GPIO_InitTypeDef gpio_init = {0};

  CLARITONE_CAMERA_LED_GPIO_CLK_ENABLE();
  gpio_init.Pin = CLARITONE_CAMERA_LED_PIN;
  gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CLARITONE_CAMERA_LED_PORT, &gpio_init);
  HAL_GPIO_WritePin(CLARITONE_CAMERA_LED_PORT, CLARITONE_CAMERA_LED_PIN, GPIO_PIN_RESET);
}

static void NeuralNetwork_init(uint32_t *nn_in_length, stai_ptr *nn_out, stai_size *number_output, int32_t nn_out_len[])
{
  stai_network_info info;
  int ret;

  ret = stai_runtime_init();
  assert(ret == STAI_SUCCESS);

  ret = stai_network_init(network_context);
  assert(ret == STAI_SUCCESS);

  ret = stai_network_get_info(network_context, &info);
  assert(ret == STAI_SUCCESS);
  assert(info.n_inputs == 1);

  *number_output = STAI_NETWORK_OUT_NUM;
  *nn_in_length = info.inputs[0].size_bytes;

  ret = stai_network_get_inputs(network_context, &nn_in, (stai_size *)&info.n_inputs);
  assert(ret == STAI_SUCCESS);

  ret = stai_network_get_outputs(network_context, nn_out, number_output);
  assert(ret == STAI_SUCCESS);

  for (int i = 0; i < *number_output; i++)
  {
    nn_out_len[i] = info.outputs[i].size_bytes;
  }
}

static void NPURam_enable(void)
{
  RAMCFG_HandleTypeDef hramcfg = {0};

  __HAL_RCC_NPU_CLK_ENABLE();
  __HAL_RCC_NPU_FORCE_RESET();
  __HAL_RCC_NPU_RELEASE_RESET();

  __HAL_RCC_AXISRAM3_MEM_CLK_ENABLE();
  __HAL_RCC_AXISRAM4_MEM_CLK_ENABLE();
  __HAL_RCC_AXISRAM5_MEM_CLK_ENABLE();
  __HAL_RCC_AXISRAM6_MEM_CLK_ENABLE();
  __HAL_RCC_RAMCFG_CLK_ENABLE();

  hramcfg.Instance = RAMCFG_SRAM3_AXI;
  HAL_RAMCFG_EnableAXISRAM(&hramcfg);
  hramcfg.Instance = RAMCFG_SRAM4_AXI;
  HAL_RAMCFG_EnableAXISRAM(&hramcfg);
  hramcfg.Instance = RAMCFG_SRAM5_AXI;
  HAL_RAMCFG_EnableAXISRAM(&hramcfg);
  hramcfg.Instance = RAMCFG_SRAM6_AXI;
  HAL_RAMCFG_EnableAXISRAM(&hramcfg);
}

static void set_clk_sleep_mode(void)
{
  __HAL_RCC_NPU_CLK_SLEEP_ENABLE();
  __HAL_RCC_CACHEAXI_CLK_SLEEP_ENABLE();
  __HAL_RCC_DCMIPP_CLK_SLEEP_ENABLE();
  __HAL_RCC_CSI_CLK_SLEEP_ENABLE();

  __HAL_RCC_FLEXRAM_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM1_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM2_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM3_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM4_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM5_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM6_MEM_CLK_SLEEP_ENABLE();
}

static void NPUCache_config(void)
{
  npu_cache_enable();
}

static void Security_Config(void)
{
  RIMC_MasterConfig_t RIMC_master = {0};

  __HAL_RCC_RIFSC_CLK_ENABLE();

  RIMC_master.MasterCID = RIF_CID_1;
  RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;

  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_NPU, &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DCMIPP, &RIMC_master);

  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_NPU, RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_CSI, RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DCMIPP, RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
}

static void IAC_Config(void)
{
  __HAL_RCC_IAC_CLK_ENABLE();
  __HAL_RCC_IAC_FORCE_RESET();
  __HAL_RCC_IAC_RELEASE_RESET();
}

void IAC_IRQHandler(void)
{
  while (1)
  {
  }
}

void npu_cache_enable_clocks_and_reset(void)
{
  __HAL_RCC_CACHEAXIRAM_MEM_CLK_ENABLE();
  __HAL_RCC_CACHEAXI_CLK_ENABLE();
  __HAL_RCC_CACHEAXI_FORCE_RESET();
  __HAL_RCC_CACHEAXI_RELEASE_RESET();
}

void npu_cache_disable_clocks_and_reset(void)
{
  __HAL_RCC_CACHEAXIRAM_MEM_CLK_DISABLE();
  __HAL_RCC_CACHEAXI_CLK_DISABLE();
  __HAL_RCC_CACHEAXI_FORCE_RESET();
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  UNUSED(file);
  UNUSED(line);
  __BKPT(0);
  while (1)
  {
  }
}
#endif
