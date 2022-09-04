/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-23     zm       the first version
 */
#ifndef APPLICATIONS_I2C_H_
#define APPLICATIONS_I2C_H_


#include "ch32v30x.h"
#include <rtthread.h>
#include <rthw.h>
#include "drivers/pin.h"
#include <board.h>
#include <rtdbg.h>
#include <rtdevice.h>

#include <rtdbg.h>
#define SDA GPIO_Pin_0
#define SCL GPIO_Pin_1
#define I2C_Port GPIOB

#define SCL_HIGH GPIO_SetBits(I2C_Port,SCL)
#define SCL_LOW GPIO_ResetBits(I2C_Port,SCL)

#define SDA_HIGH GPIO_SetBits(I2C_Port,SDA)
#define SDA_LOW GPIO_ResetBits(I2C_Port,SDA)

#define I2C_OUT 1
#define I2C_IN  0

void I2C_UserConfig(void);
void I2C_SDA_Mode(u8 addr);
void I2C_Start(void);
void I2C_Stop(void);
u8 I2C_Write_Ack(void);
void I2C_Write_Byte(u8 data);
u8 I2C_Read_Data(void);



#endif /* APPLICATIONS_I2C_H_ */
