/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-01     zm       the first version
 */
#ifndef APPLICATIONS_OV7670_C_
#define APPLICATIONS_OV7670_C_
#include "ov7670.h"

static struct rt_i2c_bus_device *i2c_bus = RT_NULL;
static rt_bool_t initialized = RT_FALSE;

void CLK_init_ON(void)//输入时钟线
{
    GPIO_InitTypeDef GPIO_InitStructrue;
    RCC_APB2PeriphClockCmd(OV7670_MCLK_RCC, ENABLE);
    GPIO_InitStructrue.GPIO_Pin = OV7670_MCLK_PIN;
    GPIO_InitStructrue.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(OV7670_MCLK_GPIO, &GPIO_InitStructrue);
    RCC_MCOConfig(RCC_MCO_HSE);
}
void CLK_init_OFF(void)
{
    GPIO_InitTypeDef GPIO_InitStructrue;
    RCC_APB2PeriphClockCmd(OV7670_MCLK_RCC, ENABLE);
    GPIO_InitStructrue.GPIO_Pin = OV7670_MCLK_PIN;
    GPIO_InitStructrue.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(OV7670_MCLK_GPIO, &GPIO_InitStructrue);
}

void ov7670_GPIO_Init(void)//信号线
{
    GPIO_InitTypeDef GPIO_InitStructrue;
    RCC_APB2PeriphClockCmd(OV7670_D0_RCC | OV7670_D2_RCC | OV7670_D5_RCC, ENABLE);
    GPIO_InitStructrue.GPIO_Pin = OV7670_D0_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(OV7670_D0_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D1_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(OV7670_D1_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D2_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(OV7670_D2_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D3_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(OV7670_D3_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D4_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(OV7670_D4_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D5_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(OV7670_D5_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D6_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(OV7670_D6_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D7_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(OV7670_D7_GPIO, &GPIO_InitStructrue);
}

void OV7670_GPIO_CONTRL_CONFIG(void)//控制线
{
    GPIO_InitTypeDef GPIO_InitStructrue;
    RCC_APB2PeriphClockCmd(OV7670_VSYNC_RCC | OV7670_HSYNC_RCC | OV7670_PIXCLK_RCC, ENABLE);
    GPIO_InitStructrue.GPIO_Pin = OV7670_VSYNC_PIN | OV7670_HSYNC_PIN | OV7670_PIXCLK_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(OV7670_PIXCLK_GPIO, &GPIO_InitStructrue);
}

void OV7670_RST_PW_Init(void)//复位控制
{
    GPIO_InitTypeDef GPIO_InitStructrue;
    RCC_APB2PeriphClockCmd(OV7670_RST_PW_RCC, ENABLE);
    GPIO_InitStructrue.GPIO_Pin = OV7670_RST_PW_Pin;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructrue.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OV7670_RST_PW_GPIO, &GPIO_InitStructrue);
}

//接下来就是用i2c控制寄存器了

void OV7670_config_window(unsigned int startx,unsigned int starty,unsigned int width, unsigned int height)
{
    rt_uint16_t endx;
    rt_uint16_t endy;
    rt_uint8_t temp_reg1,temp_reg2;
    rt_uint8_t temp = 0;

    endx = (startx + width);
    endy = (starty + height + height);
    rt_i2c_master_recv(i2c_bus,REG_OV7670_VREF,RT_I2C_RD,&temp_reg1,1);
    temp_reg1 &= 0xf0;
    rt_i2c_master_recv(i2c_bus,REG_OV7670_HREF,RT_I2C_RD,&temp_reg2,1);
    temp_reg2 &= 0xc0;

    //horizontal
    temp = temp_reg2 | ((endx & 0x7) << 3) | (startx & 0x7);
    rt_i2c_master_send(i2c_bus, REG_OV7670_HREF, RT_I2C_WR, &temp, 1);
    temp = (startx & 0x7f8) >> 3;
    rt_i2c_master_send(i2c_bus,REG_OV7670_HSTART,RT_I2C_WR,&temp,1);
    temp = (endx & 0x7f8) >> 3;
    rt_i2c_master_send(i2c_bus,REG_OV7670_HSTOP,RT_I2C_WR,&temp,1);

    //vertical
    temp = temp_reg1 | ((endy & 0x3) << 2) | (starty & 0x3);
    rt_i2c_master_send(i2c_bus,REG_OV7670_VREF,RT_I2C_WR,&temp,1);
    temp = starty >> 2;
    rt_i2c_master_send(i2c_bus,REG_OV7670_VSTART,RT_I2C_WR,&temp,1);
    temp = endy >> 2;
    rt_i2c_master_send(i2c_bus,REG_OV7670_VSTOP,RT_I2C_WR,&temp,1);
}

void ov7670_setreg(void)
{
    rt_uint8_t ov7670_reg_data[200]= { 0x00
            , 0x04
            , 0xd0
            , 0x00
            , 0x14
            , 0x80
            , 0x16
            , 0x04
            , 0x02
            , 0x7b
            , 0x06
            , 0x04
            , 0x00
            , 0x3a
            , 0x35
            , 0x11
            , 0x00
            , 0x00
            , 0xff
            , 0x20
            , 0x1c
            , 0x28
            , 0x3c
            , 0x55
            , 0x68
            , 0x76
            , 0x80
            , 0x88
            , 0x8f
            , 0x96
            , 0xa3
            , 0xaf
            , 0xc4
            , 0xd7
            , 0xe8
            , 0xb6
            , 0xff
            , 0x00
            , 0x00
            , 0x00
            , 0x4e
            , 0x05
            , 0x07
            , 0x75
            , 0x63
            , 0xA5
            , 0x78
            , 0x68
            , 0xdf
            , 0xdf
            , 0xf0
            , 0x90
            , 0x94
            , 0x61
            , 0x43
            , 0x02
            , 0x37
            , 0x02
            , 0x91
            , 0x07
            , 0x0b
            , 0x0b
            , 0x3f
            , 0x01
            , 0x00
            , 0x78
            , 0x40
            , 0x20
            , 0x00
            , 0x4a
            , 0x19
            , 0x4f
            , 0x00
            , 0x00
            , 0x00
            , 0x00
            , 0x00
            , 0x00
            , 0x80
            , 0x84
            , 0x0c
            , 0x0e
            , 0x82
            , 0x0a
            , 0x14
            , 0xf0
            , 0x34
            , 0x58
            , 0x28
            , 0x3a
            , 0x88
            , 0x88
            , 0x44
            , 0x67
            , 0x49
            , 0x0e
            , 0x04
            , 0x20
            , 0x05
            , 0x04
            , 0x08
            , 0x0a
            , 0x55
            , 0x11
            , 0x9f
            , 0x40
            , 0x40
            , 0x40
            , 0x00
            , 0x80
            , 0x80
            , 0x00
            , 0x22
            , 0x5e
            , 0x80
            , 0x9e
            , 0x08
            , 0x00
            , 0x05
            , 0xe1
            , 0x00
            , 0x01
            , 0xc1
            , 0x09
            , 0x60
            , 0x40
            , 0x11
            , 0x02
            , 0x89
            , 0x00
            , 0x30
            , 0x20
            , 0x30
            , 0x84
            , 0x29
            , 0x03
            , 0x4c
            , 0x3f
            , 0x00
            , 0xc2
};

        rt_i2c_master_send(i2c_bus,0x8c,RT_I2C_WR, &ov7670_reg_data[0],1);
        rt_i2c_master_send(i2c_bus,0x3a,RT_I2C_WR, &ov7670_reg_data[1],1);
        rt_i2c_master_send(i2c_bus,0x40,RT_I2C_WR, &ov7670_reg_data[2],1);
        rt_i2c_master_send(i2c_bus,0x8c,RT_I2C_WR, &ov7670_reg_data[3],1);
        rt_i2c_master_send(i2c_bus,0x12,RT_I2C_WR, &ov7670_reg_data[4],1);
        rt_i2c_master_send(i2c_bus,0x32,RT_I2C_WR, &ov7670_reg_data[5],1);
        rt_i2c_master_send(i2c_bus,0x17,RT_I2C_WR, &ov7670_reg_data[6],1);
        rt_i2c_master_send(i2c_bus,0x18,RT_I2C_WR, &ov7670_reg_data[7],1);
        rt_i2c_master_send(i2c_bus,0x19,RT_I2C_WR, &ov7670_reg_data[8],1);
        rt_i2c_master_send(i2c_bus,0x1a,RT_I2C_WR, &ov7670_reg_data[9],1);
        rt_i2c_master_send(i2c_bus,0x03,RT_I2C_WR, &ov7670_reg_data[10],1);
        rt_i2c_master_send(i2c_bus,0x0c,RT_I2C_WR, &ov7670_reg_data[11],1);
        rt_i2c_master_send(i2c_bus,0x3e,RT_I2C_WR, &ov7670_reg_data[12],1);
        rt_i2c_master_send(i2c_bus,0x70,RT_I2C_WR, &ov7670_reg_data[13],1);
        rt_i2c_master_send(i2c_bus,0x71,RT_I2C_WR, &ov7670_reg_data[14],1);
        rt_i2c_master_send(i2c_bus,0x72,RT_I2C_WR, &ov7670_reg_data[15],1);
        rt_i2c_master_send(i2c_bus,0x73,RT_I2C_WR, &ov7670_reg_data[16],1);
        rt_i2c_master_send(i2c_bus,0xa2,RT_I2C_WR, &ov7670_reg_data[17],1);
        rt_i2c_master_send(i2c_bus,0x11,RT_I2C_WR, &ov7670_reg_data[18],1);
        //rt_i2c_master_seni2c_bus,d(0x1RT_I2C_WR,5 , 0x31);          ,1
        rt_i2c_master_send(i2c_bus,0x7a,RT_I2C_WR, &ov7670_reg_data[19],1);
        rt_i2c_master_send(i2c_bus,0x7b,RT_I2C_WR, &ov7670_reg_data[20],1);
        rt_i2c_master_send(i2c_bus,0x7c,RT_I2C_WR, &ov7670_reg_data[21],1);
        rt_i2c_master_send(i2c_bus,0x7d,RT_I2C_WR, &ov7670_reg_data[22],1);
        rt_i2c_master_send(i2c_bus,0x7e,RT_I2C_WR, &ov7670_reg_data[23],1);
        rt_i2c_master_send(i2c_bus,0x7f,RT_I2C_WR, &ov7670_reg_data[24],1);
        rt_i2c_master_send(i2c_bus,0x80,RT_I2C_WR, &ov7670_reg_data[25],1);
        rt_i2c_master_send(i2c_bus,0x81,RT_I2C_WR, &ov7670_reg_data[26],1);
        rt_i2c_master_send(i2c_bus,0x82,RT_I2C_WR, &ov7670_reg_data[27],1);
        rt_i2c_master_send(i2c_bus,0x83,RT_I2C_WR, &ov7670_reg_data[28],1);
        rt_i2c_master_send(i2c_bus,0x84,RT_I2C_WR, &ov7670_reg_data[29],1);
        rt_i2c_master_send(i2c_bus,0x85,RT_I2C_WR, &ov7670_reg_data[30],1);
        rt_i2c_master_send(i2c_bus,0x86,RT_I2C_WR, &ov7670_reg_data[31],1);
        rt_i2c_master_send(i2c_bus,0x87,RT_I2C_WR, &ov7670_reg_data[32],1);
        rt_i2c_master_send(i2c_bus,0x88,RT_I2C_WR, &ov7670_reg_data[33],1);
        rt_i2c_master_send(i2c_bus,0x89,RT_I2C_WR, &ov7670_reg_data[34],1);

        rt_i2c_master_send(i2c_bus,0x32,RT_I2C_WR, &ov7670_reg_data[35],1);

        rt_i2c_master_send(i2c_bus,0x13,RT_I2C_WR, &ov7670_reg_data[36],1);
        rt_i2c_master_send(i2c_bus,0x00,RT_I2C_WR, &ov7670_reg_data[37],1);
        rt_i2c_master_send(i2c_bus,0x10,RT_I2C_WR, &ov7670_reg_data[38],1);
        rt_i2c_master_send(i2c_bus,0x0d,RT_I2C_WR, &ov7670_reg_data[39],1);
        rt_i2c_master_send(i2c_bus,0x14,RT_I2C_WR, &ov7670_reg_data[40],1);
        rt_i2c_master_send(i2c_bus,0xa5,RT_I2C_WR, &ov7670_reg_data[41],1);
        rt_i2c_master_send(i2c_bus,0xab,RT_I2C_WR, &ov7670_reg_data[42],1);
        rt_i2c_master_send(i2c_bus,0x24,RT_I2C_WR, &ov7670_reg_data[43],1);
        rt_i2c_master_send(i2c_bus,0x25,RT_I2C_WR, &ov7670_reg_data[44],1);
        rt_i2c_master_send(i2c_bus,0x26,RT_I2C_WR, &ov7670_reg_data[45],1);
        rt_i2c_master_send(i2c_bus,0x9f,RT_I2C_WR, &ov7670_reg_data[46],1);
        rt_i2c_master_send(i2c_bus,0xa0,RT_I2C_WR, &ov7670_reg_data[47],1);
    //  rt_i2c_master_send(i2c_bus,0xa1,RT_I2C_WR, 0x03);//0x0b,      ,1
        rt_i2c_master_send(i2c_bus,0xa6,RT_I2C_WR, &ov7670_reg_data[48],1);
        rt_i2c_master_send(i2c_bus,0xa7,RT_I2C_WR, &ov7670_reg_data[49],1);
        rt_i2c_master_send(i2c_bus,0xa8,RT_I2C_WR, &ov7670_reg_data[50],1);
        rt_i2c_master_send(i2c_bus,0xa9,RT_I2C_WR, &ov7670_reg_data[51],1);
        rt_i2c_master_send(i2c_bus,0xaa,RT_I2C_WR, &ov7670_reg_data[52],1);
        //rt_i2c_master_seni2c_bus,d(0x1RT_I2C_WR,3, 0xe5);           ,1
        rt_i2c_master_send(i2c_bus,0x0e,RT_I2C_WR, &ov7670_reg_data[53],1);
        rt_i2c_master_send(i2c_bus,0x0f,RT_I2C_WR, &ov7670_reg_data[54],1);
        rt_i2c_master_send(i2c_bus,0x16,RT_I2C_WR, &ov7670_reg_data[55],1);
        rt_i2c_master_send(i2c_bus,0x1e,RT_I2C_WR, &ov7670_reg_data[56],1);
        rt_i2c_master_send(i2c_bus,0x21,RT_I2C_WR, &ov7670_reg_data[57],1);
        rt_i2c_master_send(i2c_bus,0x22,RT_I2C_WR, &ov7670_reg_data[58],1);
        rt_i2c_master_send(i2c_bus,0x29,RT_I2C_WR, &ov7670_reg_data[59],1);
        rt_i2c_master_send(i2c_bus,0x33,RT_I2C_WR, &ov7670_reg_data[60],1);
        rt_i2c_master_send(i2c_bus,0x35,RT_I2C_WR, &ov7670_reg_data[61],1);
        rt_i2c_master_send(i2c_bus,0x37,RT_I2C_WR, &ov7670_reg_data[62],1);
        rt_i2c_master_send(i2c_bus,0x38,RT_I2C_WR, &ov7670_reg_data[63],1);
        rt_i2c_master_send(i2c_bus,0x39,RT_I2C_WR, &ov7670_reg_data[64],1);
        rt_i2c_master_send(i2c_bus,0x3c,RT_I2C_WR, &ov7670_reg_data[65],1);
        rt_i2c_master_send(i2c_bus,0x4d,RT_I2C_WR, &ov7670_reg_data[66],1);
        rt_i2c_master_send(i2c_bus,0x4e,RT_I2C_WR, &ov7670_reg_data[67],1);
        rt_i2c_master_send(i2c_bus,0x69,RT_I2C_WR, &ov7670_reg_data[68],1);
        rt_i2c_master_send(i2c_bus,0x6b,RT_I2C_WR, &ov7670_reg_data[69],1);
        rt_i2c_master_send(i2c_bus,0x74,RT_I2C_WR, &ov7670_reg_data[70],1);
        rt_i2c_master_send(i2c_bus,0x8d,RT_I2C_WR, &ov7670_reg_data[71],1);
        rt_i2c_master_send(i2c_bus,0x8e,RT_I2C_WR, &ov7670_reg_data[72],1);
        rt_i2c_master_send(i2c_bus,0x8f,RT_I2C_WR, &ov7670_reg_data[73],1);
        rt_i2c_master_send(i2c_bus,0x90,RT_I2C_WR, &ov7670_reg_data[74],1);
        rt_i2c_master_send(i2c_bus,0x91,RT_I2C_WR, &ov7670_reg_data[75],1);
        rt_i2c_master_send(i2c_bus,0x92,RT_I2C_WR, &ov7670_reg_data[76],1);
        rt_i2c_master_send(i2c_bus,0x96,RT_I2C_WR, &ov7670_reg_data[77],1);
        rt_i2c_master_send(i2c_bus,0x9a,RT_I2C_WR, &ov7670_reg_data[78],1);
        rt_i2c_master_send(i2c_bus,0xb0,RT_I2C_WR, &ov7670_reg_data[79],1);
        rt_i2c_master_send(i2c_bus,0xb1,RT_I2C_WR, &ov7670_reg_data[80],1);
        rt_i2c_master_send(i2c_bus,0xb2,RT_I2C_WR, &ov7670_reg_data[81],1);
        rt_i2c_master_send(i2c_bus,0xb3,RT_I2C_WR, &ov7670_reg_data[82],1);
        rt_i2c_master_send(i2c_bus,0xb8,RT_I2C_WR, &ov7670_reg_data[83],1);
        rt_i2c_master_send(i2c_bus,0x43,RT_I2C_WR, &ov7670_reg_data[84],1);
        rt_i2c_master_send(i2c_bus,0x44,RT_I2C_WR, &ov7670_reg_data[85],1);
        rt_i2c_master_send(i2c_bus,0x45,RT_I2C_WR, &ov7670_reg_data[86],1);
        rt_i2c_master_send(i2c_bus,0x46,RT_I2C_WR, &ov7670_reg_data[87],1);
        rt_i2c_master_send(i2c_bus,0x47,RT_I2C_WR, &ov7670_reg_data[88],1);
        rt_i2c_master_send(i2c_bus,0x48,RT_I2C_WR, &ov7670_reg_data[89],1);

        rt_i2c_master_send(i2c_bus,0x59,RT_I2C_WR, &ov7670_reg_data[90],1);
        rt_i2c_master_send(i2c_bus,0x5a,RT_I2C_WR, &ov7670_reg_data[91],1);
        rt_i2c_master_send(i2c_bus,0x5b,RT_I2C_WR, &ov7670_reg_data[92],1);
        rt_i2c_master_send(i2c_bus,0x5c,RT_I2C_WR, &ov7670_reg_data[93],1);
        rt_i2c_master_send(i2c_bus,0x5d,RT_I2C_WR, &ov7670_reg_data[94],1);
        rt_i2c_master_send(i2c_bus,0x5e,RT_I2C_WR, &ov7670_reg_data[95],1);

        rt_i2c_master_send(i2c_bus,0x64,RT_I2C_WR, &ov7670_reg_data[96],1);
        rt_i2c_master_send(i2c_bus,0x65,RT_I2C_WR, &ov7670_reg_data[97],1);
        rt_i2c_master_send(i2c_bus,0x66,RT_I2C_WR, &ov7670_reg_data[98],1);

        rt_i2c_master_send(i2c_bus,0x94,RT_I2C_WR, &ov7670_reg_data[99],1);
        rt_i2c_master_send(i2c_bus,0x95,RT_I2C_WR, &ov7670_reg_data[100],1);

        rt_i2c_master_send(i2c_bus,0x6c,RT_I2C_WR, &ov7670_reg_data[101],1);
        rt_i2c_master_send(i2c_bus,0x6d,RT_I2C_WR, &ov7670_reg_data[102],1);
        rt_i2c_master_send(i2c_bus,0x6e,RT_I2C_WR, &ov7670_reg_data[103],1);
        rt_i2c_master_send(i2c_bus,0x6f,RT_I2C_WR, &ov7670_reg_data[104],1);

        rt_i2c_master_send(i2c_bus,0x6a,RT_I2C_WR, &ov7670_reg_data[105],1);
        rt_i2c_master_send(i2c_bus,0x01,RT_I2C_WR, &ov7670_reg_data[106],1);
        rt_i2c_master_send(i2c_bus,0x02,RT_I2C_WR, &ov7670_reg_data[107],1);

        //rt_i2c_master_seni2c_bus,d(0x1RT_I2C_WR,3, 0xe7);           ,1
        rt_i2c_master_send(i2c_bus,0x15,RT_I2C_WR, &ov7670_reg_data[108],1);
        rt_i2c_master_send(i2c_bus,0x4f,RT_I2C_WR, &ov7670_reg_data[109],1);
        rt_i2c_master_send(i2c_bus,0x50,RT_I2C_WR, &ov7670_reg_data[110],1);
        rt_i2c_master_send(i2c_bus,0x51,RT_I2C_WR, &ov7670_reg_data[111],1);
        rt_i2c_master_send(i2c_bus,0x52,RT_I2C_WR, &ov7670_reg_data[112],1);
        rt_i2c_master_send(i2c_bus,0x53,RT_I2C_WR, &ov7670_reg_data[113],1);
        rt_i2c_master_send(i2c_bus,0x54,RT_I2C_WR, &ov7670_reg_data[114],1);
        rt_i2c_master_send(i2c_bus,0x58,RT_I2C_WR, &ov7670_reg_data[115],1);

        rt_i2c_master_send(i2c_bus,0x41,RT_I2C_WR, &ov7670_reg_data[116],1);
        rt_i2c_master_send(i2c_bus,0x3f,RT_I2C_WR, &ov7670_reg_data[117],1);
        rt_i2c_master_send(i2c_bus,0x75,RT_I2C_WR, &ov7670_reg_data[118],1);
        rt_i2c_master_send(i2c_bus,0x76,RT_I2C_WR, &ov7670_reg_data[119],1);

        rt_i2c_master_send(i2c_bus,0x4c,RT_I2C_WR, &ov7670_reg_data[120],1);
        rt_i2c_master_send(i2c_bus,0x77,RT_I2C_WR, &ov7670_reg_data[121],1);

        rt_i2c_master_send(i2c_bus,0x3d,RT_I2C_WR, &ov7670_reg_data[122],1);
        rt_i2c_master_send(i2c_bus,0x4b,RT_I2C_WR, &ov7670_reg_data[123],1);
        rt_i2c_master_send(i2c_bus,0xc9,RT_I2C_WR, &ov7670_reg_data[124],1);
        //rt_i2c_master_seni2c_bus,d(0x4RT_I2C_WR,1, 0x38);           ,1
        rt_i2c_master_send(i2c_bus,0x56,RT_I2C_WR, &ov7670_reg_data[125],1);
        rt_i2c_master_send(i2c_bus,0x34,RT_I2C_WR, &ov7670_reg_data[126],1);
        rt_i2c_master_send(i2c_bus,0x3b,RT_I2C_WR, &ov7670_reg_data[127],1);
        rt_i2c_master_send(i2c_bus,0xa4,RT_I2C_WR, &ov7670_reg_data[128],1);

        rt_i2c_master_send(i2c_bus,0x96,RT_I2C_WR, &ov7670_reg_data[129],1);
        rt_i2c_master_send(i2c_bus,0x97,RT_I2C_WR, &ov7670_reg_data[130],1);
        rt_i2c_master_send(i2c_bus,0x98,RT_I2C_WR, &ov7670_reg_data[131],1);
        rt_i2c_master_send(i2c_bus,0x99,RT_I2C_WR, &ov7670_reg_data[132],1);
        rt_i2c_master_send(i2c_bus,0x9a,RT_I2C_WR, &ov7670_reg_data[133],1);
        rt_i2c_master_send(i2c_bus,0x9b,RT_I2C_WR, &ov7670_reg_data[134],1);
        rt_i2c_master_send(i2c_bus,0x9c,RT_I2C_WR, &ov7670_reg_data[135],1);
        rt_i2c_master_send(i2c_bus,0x9d,RT_I2C_WR, &ov7670_reg_data[136],1);
        rt_i2c_master_send(i2c_bus,0x9e,RT_I2C_WR, &ov7670_reg_data[137],1);

        rt_i2c_master_send(i2c_bus,0x09,RT_I2C_WR, &ov7670_reg_data[138],1);
        rt_i2c_master_send(i2c_bus,0x3b,RT_I2C_WR, &ov7670_reg_data[139],1);


}

u8 OV7670_Init(void)
{
    uint16_t i = 0;
    uint8_t regL;
    uint8_t regH;
    uint16_t reg;
    rt_uint8_t buffer = 0x80;
    i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(OV7670_I2C_BUS_NAME);

    if(i2c_bus == RT_NULL)
    {
        rt_kprintf("cant find %s bus\r\n",OV7670_I2C_BUS_NAME);
    }
    else {
        initialized = RT_TRUE;
    }

    ov7670_GPIO_Init();
    OV7670_GPIO_CONTRL_CONFIG();
    CLK_init_ON();
    OV7670_RST_PW_Init();

    OV7670_PWDN_H;
    rt_thread_mdelay(10);
    OV7670_RST_H;
    rt_thread_mdelay(10);
    OV7670_PWDN_L;
    OV7670_RST_L;

    rt_i2c_master_send(i2c_bus, REG_OV7670_COM7, RT_I2C_WR, &buffer, 1);
    rt_thread_mdelay(500);

   rt_i2c_master_recv(i2c_bus, REG_OV7670_MIDH, RT_I2C_RD, &regH, 1);
   rt_kprintf("MID:%08x\r\n",regH);
   rt_i2c_master_recv(i2c_bus, REG_OV7670_MIDL, RT_I2C_RD, &regL, 1);
   rt_kprintf("MID:%08x\r\n",regL);
   reg = regL  | (regH << 8);
   if (reg != OV7670_MID)
   {
       rt_kprintf("MID:%08x\r\n",reg);
       return RT_ERROR;
   }
   else {
    rt_kprintf("read MID suceess\r\n");
}

   rt_i2c_master_recv(i2c_bus, REG_OV7670_PID, RT_I2C_RD, &regH, 1);
   rt_i2c_master_recv(i2c_bus, REG_OV7670_VER, RT_I2C_RD, &regL, 1);
   reg = regL  | (regH << 8);
   if (reg != OV7670_PID)
   {
       rt_kprintf("PID:%08x\r\n",reg);
       return RT_ERROR;
   }
   else {
    rt_kprintf("read pid suceess\r\n");
}

}

MSH_CMD_EXPORT(OV7670_Init,ov7670_test);


#endif /* APPLICATIONS_OV7670_C_ */
