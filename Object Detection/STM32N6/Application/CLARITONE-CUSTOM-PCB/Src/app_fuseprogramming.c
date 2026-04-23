 /**
 ******************************************************************************
 * @file    app_fuseprogramming.c
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

#include "app_fuseprogramming.h"

/*
 * Fuse programming is intentionally disabled for the custom PCB bring-up
 * target. The first milestone runs in dev mode over ST-LINK and should not
 * modify one-time-programmable state as a side effect of a stale build entry.
 */
void Fuse_Programming(void)
{
}
