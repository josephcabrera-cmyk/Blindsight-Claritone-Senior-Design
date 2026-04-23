 /**
 ******************************************************************************
 * @file    stm32_lcd_ex.c
 * @author  GPM Application Team
 *
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32_lcd_ex.h"

/* Functions Definition ------------------------------------------------------*/
void UTIL_LCDEx_PrintfAtLine(uint16_t line, const char * format, ...)
{
  (void)line;
  (void)format;
}

void UTIL_LCDEx_PrintfAt(uint32_t x_pos, uint32_t y_pos, Text_AlignModeTypdef mode, const char * format, ...)
{
  (void)x_pos;
  (void)y_pos;
  (void)mode;
  (void)format;
}
