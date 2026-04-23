/**
 ******************************************************************************
 * @file    cmw_io.h
 * @brief   Custom Claritone board camera IO mapping.
 ******************************************************************************
 */

#ifndef CMW_IO_H
#define CMW_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32n6xx_hal.h"
#include "stm32n6xx_nucleo_bus.h"

/* Sensors parameters */
#define CAMERA_VD55G1_ADDRESS          0x20U
#define CAMERA_VD55G1_FREQ_IN_HZ       12000000U
#define CAMERA_VD65G4_ADDRESS          0x20U
#define CAMERA_VD65G4_FREQ_IN_HZ       12000000U
#define CAMERA_IMX335_ADDRESS          0x34U
#define CAMERA_OV5640_ADDRESS          0x78U
#define CAMERA_VD66GY_ADDRESS          0x20U
#define CAMERA_VD66GY_FREQ_IN_HZ       12000000U
#define CAMERA_VD56G3_ADDRESS          0x20U
#define CAMERA_VD56G3_FREQ_IN_HZ       12000000U
#define CAMERA_VD1943_ADDRESS          0x20U

/*
 * The Claritone camera header exposes two discrete control nets:
 *   PG2 -> POWER-EN
 *   PG3 -> LED-EN
 *
 * The VD55G1 path only requires the shutdown/power control to work, so we
 * keep NRST/Shutdown on PG2. The generic middleware still expects a second
 * enable GPIO, so we map that to PG3 instead of aliasing both roles to PG2.
 */
#define NRST_CAM_PIN                    GPIO_PIN_2
#define NRST_CAM_PORT                   GPIOG
#define NRST_CAM_GPIO_ENABLE_VDDIO()
#define NRST_CAM_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOG_CLK_ENABLE()

#define EN_CAM_PIN                      GPIO_PIN_3
#define EN_CAM_PORT                     GPIOG
#define EN_CAM_GPIO_ENABLE_VDDIO()
#define EN_CAM_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOG_CLK_ENABLE()

/*
 * The custom board does not expose a separate camera enable line that the
 * VD55G1 stack needs for bring-up. Keep the generic middleware from touching
 * a second GPIO and use PG2 shutdown/power only.
 */
#define CMW_CAMERA_USE_ENABLE_GPIO      0U

#define CMW_I2C_INIT                    BSP_I2C2_Init
#define CMW_I2C_DEINIT                  BSP_I2C2_DeInit
#define CMW_I2C_READREG16               BSP_I2C2_ReadReg16
#define CMW_I2C_WRITEREG16              BSP_I2C2_WriteReg16

#define CSI2_CLK_ENABLE()               __HAL_RCC_CSI_CLK_ENABLE()
#define CSI2_CLK_SLEEP_DISABLE()        __HAL_RCC_CSI_CLK_SLEEP_DISABLE()
#define CSI2_CLK_FORCE_RESET()          __HAL_RCC_CSI_FORCE_RESET()
#define CSI2_CLK_RELEASE_RESET()        __HAL_RCC_CSI_RELEASE_RESET()

#ifdef __cplusplus
}
#endif

#endif /* CMW_IO_H */
