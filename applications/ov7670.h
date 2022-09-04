/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-01     zm       the first version
 */
#ifndef APPLICATIONS_OV7670_H_
#define APPLICATIONS_OV7670_H_
#include "ch32v30x.h"
#include <rtthread.h>
#include <rthw.h>
#include "drivers/pin.h"
#include <board.h>
#include <rtdbg.h>
#include <rtdevice.h>

#include "SCCB.h"
#include <rtdbg.h>

#define PIC_START_X 0
#define PIC_START_Y 0
#define PIC_WIDTH 320
#define PIC_HEIGHT 200

extern uint16_t camera_buffer[PIC_WIDTH * PIC_HEIGHT + 27];

#define OV7670_MID 0X7FA2
#define OV7670_PID 0X7673

#define OV7670_RST_PW_RCC RCC_APB2Periph_GPIOB
#define OV7670_RST_PW_Pin (GPIO_Pin_0 | GPIO_Pin_1)
#define OV7670_RST_PW_GPIO GPIOB
//I2C
#define OV7670_I2C_SCL_PIN GPIO_Pin_10
#define OV7670_I2C_SDA_PIN GPIO_Pin_11
#define OV7670_I2C_GPIO GPIOB
//RST PWDN
#define OV7670_PWDN_H GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define OV7670_PWDN_L GPIO_ResetBits(GPIOB,GPIO_Pin_1)

#define OV7670_RST_H GPIO_SetBits(GPIOB,GPIO_Pin_0)
#define OV7670_RST_L GPIO_ResetBits(GPIOB,GPIO_Pin_0)

//VS
#define OV7670_VSYNC_GPIO GPIOA
#define OV7670_VSYNC_RCC RCC_APB2Periph_GPIOA
#define OV7670_VSYNC_PIN GPIO_Pin_5
#define OV7670_VSYNC_PINSOURCE GPIO_PinSource5

//HS
#define OV7670_HSYNC_GPIO GPIOA
#define OV7670_HSYNC_RCC RCC_APB2Periph_GPIOA
#define OV7670_HSYNC_PIN GPIO_Pin_4
#define OV7670_HSYNC_PINSOURCE GPIO_PinSource4

//PIXCLK
#define OV7670_PIXCLK_GPIO GPIOA
#define OV7670_PIXCLK_RCC RCC_APB2Periph_GPIOA
#define OV7670_PIXCLK_PIN GPIO_Pin_6
#define OV7670_PIXCLK_PINSOURCE GPIO_PinSource6

//MCLK
#define OV7670_MCLK_GPIO GPIOA
#define OV7670_MCLK_RCC RCC_APB2Periph_GPIOA
#define OV7670_MCLK_PIN GPIO_Pin_8
#define OV7670_MCLK_PINSOURCE GPIO_PinSource8

//D0-D7
#define OV7670_D0_GPIO GPIOA
#define OV7670_D0_RCC RCC_APB2Periph_GPIOA
#define OV7670_D0_PIN GPIO_Pin_9
#define OV7670_D0_PINSOURCE GPIO_PinSource9

#define OV7670_D1_GPIO GPIOA
#define OV7670_D1_RCC RCC_APB2Periph_GPIOA
#define OV7670_D1_PIN GPIO_Pin_10
#define OV7670_D1_PINSOURCE GPIO_PinSource10

#define OV7670_D2_GPIO GPIOC
#define OV7670_D2_RCC RCC_APB2Periph_GPIOC
#define OV7670_D2_PIN GPIO_Pin_8
#define OV7670_D2_PINSOURCE GPIO_PinSource8

#define OV7670_D3_GPIO GPIOC
#define OV7670_D3_RCC RCC_APB2Periph_GPIOC
#define OV7670_D3_PIN GPIO_Pin_9
#define OV7670_D3_PINSOURCE GPIO_PinSource9

#define OV7670_D4_GPIO GPIOC
#define OV7670_D4_RCC RCC_APB2Periph_GPIOC
#define OV7670_D4_PIN GPIO_Pin_11
#define OV7670_D4_PINSOURCE GPIO_PinSource11

#define OV7670_D5_GPIO GPIOB
#define OV7670_D5_RCC RCC_APB2Periph_GPIOB
#define OV7670_D5_PIN GPIO_Pin_6
#define OV7670_D5_PINSOURCE GPIO_PinSource6

#define OV7670_D6_GPIO GPIOB
#define OV7670_D6_RCC RCC_APB2Periph_GPIOB
#define OV7670_D6_PIN GPIO_Pin_8
#define OV7670_D6_PINSOURCE GPIO_PinSource8

#define OV7670_D7_GPIO GPIOB
#define OV7670_D7_RCC RCC_APB2Periph_GPIOB
#define OV7670_D7_PIN GPIO_Pin_9
#define OV7670_D7_PINSOURCE GPIO_PinSource9


//OV7670的寄存器地址
#define REG_OV7670_MIDH 0X1C//厂商识别字节-高
#define REG_OV7670_MIDL 0X1D//厂商识别字节-低
#define REG_OV7670_PID 0X0A//产品高位识别号
#define REG_OV7670_VER 0X0B//产品低位识别号

#define REG_OV7670_HREF 0X32//HREF控制
#define REG_OV7670_HSTART 0X17//输出格式-行频开始高八位（低三位在HREF[2：0]）
#define REG_OV7670_HSTOP 0X18//输出格式-行频结束高八位（低三位在HREF[5：3]）
#define REG_OV7670_VSTART 0X19//输出格式-场频开始高八位（低二位在VREF[1：0]）
#define REG_OV7670_VSTOP 0X1A//输出格式-场频结束高八位（低二位在VREF[3：2]）
#define REG_OV7670_VREF 0X03//帧竖直方向控制

#define REG_OV7670_RGB444 0X8C//RGB444
#define REG_OV7670_TSLB 0x3a// 行缓冲测试选项
#define REG_OV7670_COM15 0x40//通用控制15
#define REG_OV7670_COM7 0x12//通用控制7
#define REG_OV7670_COM3 0x0c//通用控制3
#define REG_OV7670_COM14 0x3e//普通控制14
#define REG_OV7670_SCALING_XSC 0x70
#define REG_OV7670_SCALING_YSC 0x71
#define REG_OV7670_SCALING_DCWCTR 0x72
#define REG_OV7670_SCALING_PC 0x73
#define REG_OV7670_SCALING_PCLK_DELAY 0xa2
#define REG_OV7670_CLKRC 0x11//内部时钟
#define REG_OV7670_SLOP 0x7a
#define REG_OV7670_GAM1 0x7b//伽马曲线1
#define REG_OV7670_GAM2 0x7c
#define REG_OV7670_GAM3 0x7d
#define REG_OV7670_GAM4 0x7e
#define REG_OV7670_GAM5 0x7f
#define REG_OV7670_GAM6 0x80
#define REG_OV7670_GAM7 0x81
#define REG_OV7670_GAM8 0x82
#define REG_OV7670_GAM9 0x83
#define REG_OV7670_GAM10 0x84
#define REG_OV7670_GAM11 0x85
#define REG_OV7670_GAM12 0x86
#define REG_OV7670_GAM13 0x87
#define REG_OV7670_GAM14 0x88
#define REG_OV7670_GAM15 0x89

#define REG_OV7670_COM8 0x13//普通控制14
#define REG_OV7670_AGC 0x00//AGC-自动增益控制，增益设置
#define REG_OV7670_AECH 0x10// 曝光值
#define REG_OV7670_COM4 0x0d//通用控制4
#define REG_OV7670_COM9 0x14//通用控制9
#define REG_OV7670_BD50MAX 0xa5// 50Hz条纹滤波器步长限制
#define REG_OV7670_BD60MAX 0xab// 60Hz条纹滤波器步长限制
#define REG_OV7670_AEW 0x24//AGC/AEC-稳定运行区域（上限
#define REG_OV7670_AEB 0x25//AGC/AEC-稳定运行区域（下限
#define REG_OV7670_VPT 0x26//AGC/AEC快速运行区域
#define REG_OV7670_HAECC1 0x9f//基于直方图的AEC/AGC的控制1
#define REG_OV7670_HAECC2 0xa0//基于直方图的AEC/AGC的控制2
#define REG_OV7670_HAECC3 0xa6//基于直方图的AEC/AGC的控制3
#define REG_OV7670_HAECC4 0xa7//基于直方图的AEC/AGC的控制4
#define REG_OV7670_HAECC5 0xa8//基于直方图的AEC/AGC的控制5
#define REG_OV7670_HAECC6 0xa9//基于直方图的AEC/AGC的控制6
#define REG_OV7670_HAECC7 0xaa//基于直方图的AEC/AGC的控制7



void CLK_init_ON(void);
void CLK_init_OFF(void);

void OV7670_config_window(unsigned int startx,unsigned int starty,unsigned int width, unsigned int height);
unsigned char OV7670_init(void);
void ov7670_GPIO_Init(void);
void ov7670_setreg(void);
u8 OV7670_Init(void);
void OV7670_Light_Mode(u8 mode);
void OV7670_Color_Saturation(u8 sta);
void OV7670_Brightness(u8 bright);
void OV7670_Contrast(u8 contrast);
void OV7670_Special_Effect(u8 eff);
void set_cif(void);



#endif /* APPLICATIONS_OV7670_H_ */
