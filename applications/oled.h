/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-23     zm       the first version
 */
#ifndef APPLICATIONS_OLED_H_
#define APPLICATIONS_OLED_H_
#include "ch32v30x.h"
#include <rtthread.h>
#include <rthw.h>
#include "drivers/pin.h"
#include <board.h>
#include <rtdbg.h>
#include <rtdevice.h>

#include <rtdbg.h>
#include "i2c.h"
#define OLED_ADDRESS 0x76//0x78?

void OLED_Wite_CMD(u8 cmd);
void OLED_Wite_Data(u8 data);
void OLED_Write_On(void);
void OLED_Write_Off(void);
void OLED_Write_Clear(void);
void OLED_Write_Init(void);
void OLED_Write_Display(u8 x,u8 y,u8 hight,u8 weight,u8 *data);
void OLED_DISPLAY_8x16(u8 x, //显示汉字的页坐标（从0到7）（此处不可修改）
                        u8 y, //显示汉字的列坐标（从0到128）
                        u16 w);
void OLED_DISPLAY_8x16_BUFFER(u8 row,u8 *str,u8 i);
void OLED_printf(u8 row,char *fmt, ...);
void OLED_Write_Num(u8 x,u8 y,u8 num);

#endif /* APPLICATIONS_OLED_H_ */
