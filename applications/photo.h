/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-11-10     zm       the first version
 */
#ifndef APPLICATIONS_PHOTO_H_
#define APPLICATIONS_PHOTO_H_


//#include "ili9431_font.h"
//#include "ov7670.h"
#include "bmp.h"

#define GPIO_KEY_PIN GPIO_Pin_4
#define GPIO_KEY GPIOE

#define Read_KEY GPIO_ReadInputDataBit(GPIO_KEY,GPIO_KEY_PIN)

rt_uint8_t KEY_Scan(void);
void GPIO_KEY_Init(void);

#endif /* APPLICATIONS_PHOTO_H_ */
