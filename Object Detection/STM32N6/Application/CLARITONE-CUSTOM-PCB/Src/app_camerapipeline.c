/**
 ******************************************************************************
 * @file    app_camerapipeline.c
 * @brief   Headless camera pipeline for the Claritone custom PCB target.
 ******************************************************************************
 */

#include <assert.h>
#include <stdio.h>

#include "cmw_camera.h"
#include "cmw_io.h"
#include "app_camerapipeline.h"
#include "app_config.h"
#include "claritone_custom_board.h"
#include "stai_network.h"

/* Leave the driver use the default sensor resolution. */
#define CAMERA_WIDTH  0
#define CAMERA_HEIGHT 0

extern int32_t cameraFrameReceived;

static void CameraPipeline_Fail(const char *message, int code)
{
  printf("CAMERA ERROR: %s (code=%d)\r\n", message, code);
  __BKPT(0);
  while (1)
  {
  }
}

static void CameraPipeline_DebugPreflight(void)
{
  GPIO_InitTypeDef gpio_init = {0};
  uint8_t model_id[4] = {0};
  int ret;

  printf("CAM PREFLIGHT: start\r\n");

#if CMW_CAMERA_USE_ENABLE_GPIO
  EN_CAM_GPIO_ENABLE_VDDIO();
  EN_CAM_GPIO_CLK_ENABLE();
#endif
  NRST_CAM_GPIO_ENABLE_VDDIO();
  NRST_CAM_GPIO_CLK_ENABLE();

  gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;

#if CMW_CAMERA_USE_ENABLE_GPIO
  gpio_init.Pin = EN_CAM_PIN;
  HAL_GPIO_Init(EN_CAM_PORT, &gpio_init);
#else
  printf("CAM PREFLIGHT: enable gpio unused\r\n");
#endif
  gpio_init.Pin = NRST_CAM_PIN;
  HAL_GPIO_Init(NRST_CAM_PORT, &gpio_init);

#if CMW_CAMERA_USE_ENABLE_GPIO
  HAL_GPIO_WritePin(EN_CAM_PORT, EN_CAM_PIN, GPIO_PIN_RESET);
#endif
  HAL_GPIO_WritePin(NRST_CAM_PORT, NRST_CAM_PIN, GPIO_PIN_RESET);
  HAL_Delay(20);
#if CMW_CAMERA_USE_ENABLE_GPIO
  HAL_GPIO_WritePin(EN_CAM_PORT, EN_CAM_PIN, GPIO_PIN_SET);
#endif
  HAL_GPIO_WritePin(NRST_CAM_PORT, NRST_CAM_PIN, GPIO_PIN_SET);
  HAL_Delay(20);

  printf("CAM PREFLIGHT: control lines released\r\n");

  ret = BSP_I2C2_Init();
  printf("CAM PREFLIGHT: BSP_I2C2_Init=%d\r\n", ret);

  ret = BSP_I2C2_IsReady(CAMERA_VD55G1_ADDRESS, 3);
  printf("CAM PREFLIGHT: BSP_I2C2_IsReady(0x%02X)=%d\r\n", CAMERA_VD55G1_ADDRESS, ret);

  if (ret == 0)
  {
    ret = BSP_I2C2_ReadReg16(CAMERA_VD55G1_ADDRESS, 0x0000U, model_id, sizeof(model_id));
    printf("CAM PREFLIGHT: model_id read=%d bytes=%02X %02X %02X %02X\r\n",
           ret, model_id[0], model_id[1], model_id[2], model_id[3]);
  }
}

static void DCMIPP_PipeInitNn(uint32_t *pitch)
{
  CMW_Aspect_Ratio_Mode_t aspect_ratio;
  CMW_DCMIPP_Conf_t dcmipp_conf = {0};
  int ret;

  if (ASPECT_RATIO_MODE == ASPECT_RATIO_CROP)
  {
    aspect_ratio = CMW_Aspect_ratio_crop;
  }
  else if (ASPECT_RATIO_MODE == ASPECT_RATIO_FIT)
  {
    aspect_ratio = CMW_Aspect_ratio_fit;
  }
  else
  {
    aspect_ratio = CMW_Aspect_ratio_fit;
  }

  dcmipp_conf.output_width = STAI_NETWORK_IN_1_WIDTH;
  dcmipp_conf.output_height = STAI_NETWORK_IN_1_HEIGHT;
  dcmipp_conf.output_format = DCMIPP_PIXEL_PACKER_FORMAT_RGB888_YUV444_1;
  dcmipp_conf.output_bpp = STAI_NETWORK_IN_1_CHANNEL;
  dcmipp_conf.mode = aspect_ratio;
  dcmipp_conf.enable_swap = COLOR_MODE;
  dcmipp_conf.enable_gamma_conversion = 0;

  ret = CMW_CAMERA_SetPipeConfig(DCMIPP_PIPE2, &dcmipp_conf, pitch);
  if (ret != HAL_OK)
  {
    CameraPipeline_Fail("NN pipe configuration failed", ret);
  }
}

void CameraPipeline_Init(uint32_t *pitch_nn)
{
  int ret;
  CMW_CameraInit_t cam_conf = {0};
  CMW_Advanced_Config_t advanced_config = {0};

  CameraPipeline_DebugPreflight();

  cam_conf.width = CAMERA_WIDTH;
  cam_conf.height = CAMERA_HEIGHT;
  cam_conf.fps = CAMERA_FPS;
  cam_conf.mirror_flip = CAMERA_FLIP;

  advanced_config.selected_sensor = CLARITONE_CAMERA_SELECTED_SENSOR;
  printf("CAMERA: loading default sensor values\r\n");
  ret = CMW_CAMERA_SetDefaultSensorValues(&advanced_config);
  if (ret != CMW_ERROR_NONE)
  {
    CameraPipeline_Fail("failed to load default VD55G1 sensor configuration", ret);
  }

  printf("CAMERA: calling CMW_CAMERA_Init\r\n");
  ret = CMW_CAMERA_Init(&cam_conf, &advanced_config);
  printf("CAMERA: CMW_CAMERA_Init returned %d\r\n", ret);
  if (ret != CMW_ERROR_NONE)
  {
    CameraPipeline_Fail("camera init/probe failed", ret);
  }

  printf("CAMERA: configuring DCMIPP NN pipe\r\n");
  DCMIPP_PipeInitNn(pitch_nn);
  printf("CAMERA: DCMIPP NN pipe configured\r\n");
}

void CameraPipeline_DeInit(void)
{
  int ret = CMW_CAMERA_DeInit();
  assert(ret == CMW_ERROR_NONE);
}

void CameraPipeline_NNPipe_Start(uint8_t *nn_pipe_dst, uint32_t cam_mode)
{
  int ret = CMW_CAMERA_Start(DCMIPP_PIPE2, nn_pipe_dst, cam_mode);
  assert(ret == CMW_ERROR_NONE);
}

void CameraPipeline_IspUpdate(void)
{
  int ret = CMW_CAMERA_Run();
  assert(ret == CMW_ERROR_NONE);
}

int CMW_CAMERA_PIPE_FrameEventCallback(uint32_t pipe)
{
  if (pipe == DCMIPP_PIPE2)
  {
    cameraFrameReceived++;
  }

  return 0;
}

void CMW_CAMERA_PIPE_ErrorCallback(uint32_t pipe)
{
  printf("CAMERA PIPE ERROR: pipe=%lu\r\n", (unsigned long)pipe);
  __BKPT(0);
  while (1)
  {
  }
}
