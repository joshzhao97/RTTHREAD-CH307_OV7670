/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-04     zm       the first version
 */
#ifndef APPLICATIONS_SCCB_H_
#define APPLICATIONS_SCCB_H_
#include "ch32v30x.h"
#include <rtthread.h>
#include <rthw.h>
#include "drivers/pin.h"
#include <board.h>
#include <rtdbg.h>
#include <rtdevice.h>

#include <rtdbg.h>


#define SCCB_GPIO GPIOB
#define SCCB_RCC RCC_APB2Periph_GPIOB
#define SCCB_SIC_BIT     GPIO_Pin_10 //端口第10位
#define SCCB_SID_BIT     GPIO_Pin_11 //端口11位

#define SCCB_SIC_H()      GPIOB->BSHR = GPIO_Pin_10; //对端口A特定位的设置  ,这里设置高
#define SCCB_SIC_L()      GPIOB->BCR = GPIO_Pin_10;//这里设置低

#define SCCB_SID_H()      GPIOB->BSHR = GPIO_Pin_11;//同上
#define SCCB_SID_L()      GPIOB->BCR = GPIO_Pin_11;
/**/
#define SCCB_SID_IN      SCCB_SID_GPIO_INPUT();
#define SCCB_SID_OUT     SCCB_SID_GPIO_OUTPUT();


#define SCCB_SID_STATE   GPIO_ReadInputDataBit(SCCB_GPIO,SCCB_SID_BIT)


///////////////////////////////////////////
void SCCB_GPIO_Config(void);
void SCCB_SID_GPIO_OUTPUT(void);
void SCCB_SID_GPIO_INPUT(void);
void startSCCB(void);
void stopSCCB(void);
void noAck(void);
unsigned char SCCBwriteByte(unsigned char m_data);
unsigned char SCCBreadByte(void);


#endif /* APPLICATIONS_SCCB_H_ */
