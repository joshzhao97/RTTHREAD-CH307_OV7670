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
#include "ili9431.h"

const u8 ov7670_init_reg_tbl[][2]=
{
        /*以下为OV7670 QVGA RGB565参数  */
            {0x3a, 0x04},//dummy
            {0x40, 0xd0},//565
            {0x12, 0x14},//QVGA,RGB输出

            //输出窗口设置
            {0x32, 0x80},//HREF control bit[2:0] HREF start 3 LSB    bit[5:3] HSTOP HREF end 3LSB
            {0x17, 0x16},//HSTART start high 8-bit MSB
            {0x18, 0x04},//5 HSTOP end high 8-bit
            {0x19, 0x02},
            {0x1a, 0x7b},//0x7a,
            {0x03, 0x06},//0x0a,帧竖直方向控制

            {0x0c, 0x00},
            {0x15, 0x00},//0x00
            {0x3e, 0x00},//10
            {0x70, 0x3a},
            {0x71, 0x35},
            {0x72, 0x11},
            {0x73, 0x00},//

            {0xa2, 0x02},//15
            {0x11, 0x81},//时钟分频设置,0,不分频.
            {0x7a, 0x20},
            {0x7b, 0x1c},
            {0x7c, 0x28},

            {0x7d, 0x3c},//20
            {0x7e, 0x55},
            {0x7f, 0x68},
            {0x80, 0x76},
            {0x81, 0x80},

            {0x82, 0x88},
            {0x83, 0x8f},
            {0x84, 0x96},
            {0x85, 0xa3},
            {0x86, 0xaf},

            {0x87, 0xc4},//30
            {0x88, 0xd7},
            {0x89, 0xe8},
            {0x13, 0xe0},
    {0x00, 0x00},//AGC

    {0x10, 0x00},
    {0x0d, 0x00},//全窗口， 位[5:4]: 01 半窗口，10 1/4窗口，11 1/4窗口
    {0x14, 0x28},//0x38, limit the max gain
    {0xa5, 0x05},
    {0xab, 0x07},

    {0x24, 0x75},
    {0x25, 0x63},
    {0x26, 0xA5},
    {0x9f, 0x78},
    {0xa0, 0x68},

    {0xa1, 0x03},//0x0b,
    {0xa6, 0xdf},//0xd8,
    {0xa7, 0xdf},//0xd8,
    {0xa8, 0xf0},
    {0xa9, 0x90},

    {0xaa, 0x94},
    {0x13, 0xe5},
    {0x0e, 0x61},
    {0x0f, 0x4b},
    {0x16, 0x02},

    {0x1e, 0x27},//图像输出镜像控制.0x07(改)
    {0x21, 0x02},
    {0x22, 0x91},
    {0x29, 0x07},
    {0x33, 0x0b},

    {0x35, 0x0b},
    {0x37, 0x1d},
    {0x38, 0x71},
    {0x39, 0x2a},
    {0x3c, 0x78},

    {0x4d, 0x40},
    {0x4e, 0x20},
    {0x69, 0x55},//固定增益值(改)
    {0x6b, 0x80},//PLL 重要参数 （改）PLL*6=48Mhz
    {0x74, 0x19},
    {0x8d, 0x4f},

    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0x00},
    {0x91, 0x00},
    {0x92, 0x00},//0x19,//0x66

    {0x96, 0x00},
    {0x9a, 0x80},
    {0xb0, 0x84},
    {0xb1, 0x0c},
    {0xb2, 0x0e},

    {0xb3, 0x82},
    {0xb8, 0x0a},
    {0x43, 0x14},
    {0x44, 0xf0},
    {0x45, 0x34},

    {0x46, 0x58},
    {0x47, 0x28},
    {0x48, 0x3a},
    {0x59, 0x88},
    {0x5a, 0x88},

    {0x5b, 0x44},
    {0x5c, 0x67},
    {0x5d, 0x49},
    {0x5e, 0x0e},
    {0x64, 0x04},
    {0x65, 0x20},

    {0x66, 0x05},
    {0x94, 0x04},
    {0x95, 0x08},
    {0x6c, 0x0a},
    {0x6d, 0x55},

    {0x4f, 0x80},
    {0x50, 0x80},
    {0x51, 0x00},
    {0x52, 0x22},
    {0x53, 0x5e},
    {0x54, 0x80},

    {0x6e, 0x11},//
    {0x6f, 0x9f},//0x9e for advance AWB

    {0x6a, 0x40},
    {0x01, 0x40},
    {0x02, 0x40},
    {0x13, 0xe7},
    {0x15, 0x00}, //重要参数(改)HSYNC\VSYNC\HREF电平控制


    {0x55, 0x0A},//亮度（改）
    {0x56, 0x4f},//对比度 （改）
    {0x58, 0x9e},
    {0x41, 0x08},
    {0x3f, 0x05},//边缘增强调整
    {0x75, 0x05},
    {0x76, 0xe1},
    {0x4c, 0x0F},//噪声抑制强度
    {0x77, 0x0a},
    {0x3d, 0xc2},//0xc0,
    {0x4b, 0x09},
    {0xc9, 0x60},
    {0x41, 0x38},
    {0x34, 0x11},
    {0x3b, 0x02},//0x00,//0x02,
    {0xa4, 0x89},//0x88,
    {0x96, 0x00},
    {0x97, 0x30},
    {0x98, 0x20},
    {0x99, 0x30},
    {0x9a, 0x84},
    {0x9b, 0x29},
    {0x9c, 0x03},
    {0x9d, 0x4c},
    {0x9e, 0x3f},
    {0x78, 0x04},
    {0x79, 0x01},
    {0xc8, 0xf0},
    {0x79, 0x0f},
    {0xc8, 0x00},
    {0x79, 0x10},
    {0xc8, 0x7e},
    {0x79, 0x0a},
    {0xc8, 0x80},
    {0x79, 0x0b},
    {0xc8, 0x01},
    {0x79, 0x0c},
    {0xc8, 0x0f},
    {0x79, 0x0d},
    {0xc8, 0x20},
    {0x79, 0x09},
    {0xc8, 0x80},
    {0x79, 0x02},
    {0xc8, 0xc0},
    {0x79, 0x03},
    {0xc8, 0x40},
    {0x79, 0x05},
    {0xc8, 0x30},
    {0x79, 0x26},
    {0x09, 0x02},
    {0x3b, 0x42},//0x82,//0xc0,//0xc2,  //night mode

};
const u8 ov7670_init_reg_tb2[][2]=
{
        /*以下为OV7670 QVGA RGB565参数  */
        {0x3a, 0x0C},//
        {0x40, 0xd0},//565
        {0x12, 0x14},//QVGA,RGB输出

        //输出窗口设置
        {0x32, 0x80},//HREF control bit[2:0] HREF start 3 LSB    bit[5:3] HSTOP HREF end 3LSB
        {0x17, 0x16},//HSTART start high 8-bit MSB
        {0x18, 0x04},//HSTOP end high 8-bit
        {0x19, 0x02},//VSTART start high 8-bit MSB
        {0x1a, 0x7A},//VSTOP end high 8-bit
        {0x03, 0x05},//帧竖直方向控制VRFE

        {0x0c, 0x00},
        {0x3e, 0x00},//
        {0x70, 0x3A},
        {0x71, 0x35},
        {0x72, 0x11},
        {0x73, 0x00},//

        {0xa2, 0x02},
        {0x11, 0x81},//时钟分频设置,0,不分频.
        {0x7a, 0x20},
        {0x7b, 0x1c},
        {0x7c, 0x28},

        {0x7d, 0x3c},
        {0x7e, 0x55},
        {0x7f, 0x68},
        {0x80, 0x76},
        {0x81, 0x80},

        {0x82, 0x88},
        {0x83, 0x8f},
        {0x84, 0x96},
        {0x85, 0xa3},
        {0x86, 0xaf},

        {0x87, 0xc4},
        {0x88, 0xd7},
        {0x89, 0xe8},
        {0x13, 0xe0},
        {0x00, 0x00},//AGC

        {0x10, 0x00},
        {0x0d, 0x00},//全窗口， 位[5:4]: 01 半窗口，10 1/4窗口，11 1/4窗口
        {0x14, 0x28},//0x38, limit the max gain
        {0xa5, 0x05},
        {0xab, 0x07},

        {0x24, 0x75},
        {0x25, 0x63},
        {0x26, 0xA5},
        {0x9f, 0x78},
        {0xa0, 0x68},

        {0xa1, 0x03},//0x0b,
        {0xa6, 0xdf},//0xd8,
        {0xa7, 0xdf},//0xd8,
        {0xa8, 0xf0},
        {0xa9, 0x90},

        {0xaa, 0x94},
        {0x13, 0xe5},
        {0x0e, 0x61},
        {0x0f, 0x4b},
        {0x16, 0x02},

        {0x1e, 0x27},//图像输出镜像控制.0x07(改)
        {0x21, 0x02},
        {0x22, 0x91},
        {0x29, 0x07},
        {0x33, 0x0b},

        {0x35, 0x0b},
        {0x37, 0x1d},
        {0x38, 0x71},
        {0x39, 0x2a},
        {0x3c, 0x78},

        {0x4d, 0x40},
        {0x4e, 0x20},
        {0x69, 0x55},//固定增益值(改)
        {0x6b, 0xC0},//PLL 重要参数 （改）PLL*4=48Mhz
        {0x74, 0x19},
        {0x8d, 0x4f},

        {0x8e, 0x00},
        {0x8f, 0x00},
        {0x90, 0x00},
        {0x91, 0x00},
        {0x92, 0x00},//0x19,//0x66

        {0x96, 0x00},
        {0x9a, 0x80},
        {0xb0, 0x84},
        {0xb1, 0x0c},
        {0xb2, 0x0e},

        {0xb3, 0x82},
        {0xb8, 0x0a},
        {0x43, 0x14},
        {0x44, 0xf0},
        {0x45, 0x34},

        {0x46, 0x58},
        {0x47, 0x28},
        {0x48, 0x3a},
        {0x59, 0x88},
        {0x5a, 0x88},

        {0x5b, 0x44},
        {0x5c, 0x67},
        {0x5d, 0x49},
        {0x5e, 0x0e},
        {0x64, 0x04},
        {0x65, 0x20},

        {0x66, 0x05},
        {0x94, 0x04},
        {0x95, 0x08},
        {0x6c, 0x0a},
        {0x6d, 0x55},

        {0x4f, 0x80},
        {0x50, 0x80},
        {0x51, 0x00},
        {0x52, 0x22},
        {0x53, 0x5e},
        {0x54, 0x80},

        {0x6e, 0x11},//
        {0x6f, 0x9f},//0x9e for advance AWB

        {0x6a, 0x40},
        {0x01, 0x40},
        {0x02, 0x40},
        {0x13, 0xe7},
        {0x15, 0x00}, //重要参数(改)HSYNC\VSYNC\HREF电平控制


        {0x55, 0x0A},//亮度（改）
        {0x56, 0x4f},//对比度 （改）
        {0x58, 0x9e},
        {0x41, 0x08},
        {0x3f, 0x05},//边缘增强调整
        {0x75, 0x05},
        {0x76, 0xe1},
        {0x4c, 0x0F},//噪声抑制强度
        {0x77, 0x0a},
        {0x3d, 0xc2},//0xc0,
        {0x4b, 0x09},
        {0xc9, 0x60},
        {0x41, 0x38},
        {0x34, 0x11},
        {0x3b, 0x02},//0x00,//0x02,
        {0xa4, 0x89},//0x88,
        {0x96, 0x00},
        {0x97, 0x30},
        {0x98, 0x20},
        {0x99, 0x30},
        {0x9a, 0x84},
        {0x9b, 0x29},
        {0x9c, 0x03},
        {0x9d, 0x4c},
        {0x9e, 0x3f},
        {0x78, 0x04},
        {0x79, 0x01},
        {0xc8, 0xf0},
        {0x79, 0x0f},
        {0xc8, 0x00},
        {0x79, 0x10},
        {0xc8, 0x7e},
        {0x79, 0x0a},
        {0xc8, 0x80},
        {0x79, 0x0b},
        {0xc8, 0x01},
        {0x79, 0x0c},
        {0xc8, 0x0f},
        {0x79, 0x0d},
        {0xc8, 0x20},
        {0x79, 0x09},
        {0xc8, 0x80},
        {0x79, 0x02},
        {0xc8, 0xc0},
        {0x79, 0x03},
        {0xc8, 0x40},
        {0x79, 0x05},
        {0xc8, 0x30},
        {0x79, 0x26},
        {0x09, 0x02},
        {0x3b, 0x42},//0x82,//0xc0,//0xc2,  //night mode
};

u8 flag1=0;
u8 flag2=0;
u8 flag3=0;

rt_uint32_t JPEG_DVPDMAaddr0 = 0x20005000;
rt_uint32_t JPEG_DVPDMAaddr1 = 0x20005000 + 320 * 2;

rt_uint32_t DVP_ROW_cnt = 0;
rt_uint32_t frame_cnt = 0;
 rt_uint32_t addr_cnt = 0;
volatile rt_uint32_t href_cnt = 0;
void DVP_IRQHandler (void) __attribute__((interrupt("WCH-Interrupt-fast")));

void uart2_send_data(rt_uint16_t t)
{
    while( USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    USART_SendData(USART2, t);
    while( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void CLK_init_ON(void)//输入时钟线
{
    GPIO_InitTypeDef GPIO_InitStructrue;
    RCC_APB2PeriphClockCmd(OV7670_MCLK_RCC, ENABLE);
    GPIO_InitStructrue.GPIO_Pin = OV7670_MCLK_PIN;
    GPIO_InitStructrue.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(OV7670_MCLK_GPIO, &GPIO_InitStructrue);
    RCC_MCOConfig(RCC_MCO_HSE);//8MHZ
}
void CLK_init_OFF(void)
{
    GPIO_InitTypeDef GPIO_InitStructrue;
    RCC_APB2PeriphClockCmd(OV7670_MCLK_RCC, ENABLE);
    GPIO_InitStructrue.GPIO_Pin = OV7670_MCLK_PIN;
    GPIO_InitStructrue.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(OV7670_MCLK_GPIO, &GPIO_InitStructrue);
}

void ov7670_GPIO_Init(void)//信号线
{
    GPIO_InitTypeDef GPIO_InitStructrue;
    RCC_APB2PeriphClockCmd(OV7670_D0_RCC | OV7670_D2_RCC | OV7670_D5_RCC, ENABLE);
    GPIO_InitStructrue.GPIO_Pin = OV7670_D0_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(OV7670_D0_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D1_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(OV7670_D1_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D2_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(OV7670_D2_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D3_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(OV7670_D3_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D4_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(OV7670_D4_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D5_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(OV7670_D5_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D6_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(OV7670_D6_GPIO, &GPIO_InitStructrue);

    GPIO_InitStructrue.GPIO_Pin = OV7670_D7_PIN;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(OV7670_D7_GPIO, &GPIO_InitStructrue);
}

void OV7670_GPIO_CONTRL_CONFIG(void)//控制线
{
    GPIO_InitTypeDef GPIO_InitStructrue;
    RCC_APB2PeriphClockCmd(OV7670_VSYNC_RCC | OV7670_HSYNC_RCC | OV7670_PIXCLK_RCC, ENABLE);
    GPIO_InitStructrue.GPIO_Pin = OV7670_VSYNC_PIN | OV7670_HSYNC_PIN | OV7670_PIXCLK_PIN;

    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(OV7670_HSYNC_GPIO, &GPIO_InitStructrue);
    //RCC_MCOConfig(RCC_MCO_HSE);
}

void OV7670_RST_PW_Init(void)//复位控制
{
    GPIO_InitTypeDef GPIO_InitStructrue;
    RCC_APB2PeriphClockCmd(OV7670_RST_PW_RCC, ENABLE);
    GPIO_InitStructrue.GPIO_Pin = OV7670_RST_PW_Pin ;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructrue.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OV7670_RST_PW_GPIO, &GPIO_InitStructrue);
}

//接下来就是用i2c控制寄存器了

////////////////////////////
//功能：写OV7670寄存器
//返回：1-成功   0-失败
unsigned char wrOV7670Reg(unsigned char regID, unsigned char regDat)
{
    startSCCB();
    if(0==SCCBwriteByte(0x42))
    {
        stopSCCB();
        flag1 = 1;
        return(0);
    }
    rt_hw_us_delay(100);
    if(0==SCCBwriteByte(regID))
    {
        stopSCCB();
        flag2 = 1;
        return(0);
    }
    rt_hw_us_delay(100);
    if(0==SCCBwriteByte(regDat))
    {
        stopSCCB();
        flag3 = 3;
        return(0);
    }
    stopSCCB();

    return(1);
}

////////////////////////////
//功能：读OV7670寄存器
//返回：1-成功   0-失败
unsigned char rdOV7670Reg(unsigned char regID, unsigned char *regDat)
{
    //通过写操作设置寄存器地址
    startSCCB();
    if(0==SCCBwriteByte(0x42))
    {
        stopSCCB();
        flag1 = 1;
        //rt_kprintf("read2 ov7670 add failed\r\n");
        return(0);
    }
    rt_hw_us_delay(100);
    if(0==SCCBwriteByte(regID))
    {
        stopSCCB();
        flag2 = 2;
        //rt_kprintf("write2 ov7670 reg failed\r\n");
        return(0);
    }
    rt_hw_us_delay(100);
    stopSCCB();

    rt_hw_us_delay(100);

    //设置寄存器地址后，才是读
    startSCCB();
    if(0==SCCBwriteByte(0x42 | 0x01))
    {
        stopSCCB();
        flag3 = 3;
        //rt_kprintf("read2 ov7670 add failed\r\n");
        return(0);
    }
    rt_hw_us_delay(100);
    *regDat=SCCBreadByte();
    noAck();
    stopSCCB();
    return(1);
}


void OV7670_config_window(unsigned int startx,unsigned int starty,unsigned int width, unsigned int height)
{
        unsigned int endx;
        unsigned int endy;// "v*2"必须
        unsigned char temp_reg1, temp_reg2;
        unsigned char temp=0;

        endx=(startx+width*2)%784;
        endy=(starty+height+height);// "v*2"必须
            rdOV7670Reg(0x03, &temp_reg1 );
        temp_reg1 &= 0xf0;//Y
        rdOV7670Reg(0x32, &temp_reg2 );
        temp_reg2 &= 0xc0;//X

        // Horizontal
        temp = temp_reg2|((endx&0x7)<<3)|(startx&0x7);
        wrOV7670Reg(0x32, temp );
        temp = (startx&0x7F8)>>3;
        wrOV7670Reg(0x17, temp );
        temp = (endx&0x7F8)>>3;
        wrOV7670Reg(0x18, temp );

        // Vertical
        temp =temp_reg1|((endy&0x3)<<2)|(starty&0x3);
        wrOV7670Reg(0x03, temp );
        temp = (starty & 0X3FC)>>2;
        wrOV7670Reg(0x19, temp );
        temp = (endy & 0X3FC)>>2;
        wrOV7670Reg(0x1A, temp );
}

void ov7670_setreg(void)
{
    rt_uint32_t i = 0;
    rt_uint8_t flag = 0;
     /*rt_uint8_t ov7670_reg_data[200]= { 0x00
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

        wrOV7670Reg(0x8c,ov7670_reg_data[0]);
        wrOV7670Reg(0x3a,ov7670_reg_data[1]);
        wrOV7670Reg(0x40,ov7670_reg_data[2]);
        wrOV7670Reg(0x8c,ov7670_reg_data[3]);
        wrOV7670Reg(0x12,ov7670_reg_data[4]);
        wrOV7670Reg(0x32,ov7670_reg_data[5]);
        wrOV7670Reg(0x17,ov7670_reg_data[6]);
        wrOV7670Reg(0x18,ov7670_reg_data[7]);
        wrOV7670Reg(0x19,ov7670_reg_data[8]);
        wrOV7670Reg(0x1a,ov7670_reg_data[9]);
        wrOV7670Reg(0x03,ov7670_reg_data[10]);
        wrOV7670Reg(0x0c,ov7670_reg_data[11]);
        wrOV7670Reg(0x3e,ov7670_reg_data[12]);
        wrOV7670Reg(0x70,ov7670_reg_data[13]);
        wrOV7670Reg(0x71,ov7670_reg_data[14]);
        wrOV7670Reg(0x72,ov7670_reg_data[15]);
        wrOV7670Reg(0x73,ov7670_reg_data[16]);
        wrOV7670Reg(0xa2,ov7670_reg_data[17]);
        wrOV7670Reg(0x11,ov7670_reg_data[18]);
        //rt_i2c_master_send(0x15 , 0x31);
        wrOV7670Reg(0x7a,ov7670_reg_data[19]);
        wrOV7670Reg(0x7b,ov7670_reg_data[20]);
        wrOV7670Reg(0x7c,ov7670_reg_data[21]);
        wrOV7670Reg(0x7d,ov7670_reg_data[22]);
        wrOV7670Reg(0x7e,ov7670_reg_data[23]);
        wrOV7670Reg(0x7f,ov7670_reg_data[24]);
        wrOV7670Reg(0x80,ov7670_reg_data[25]);
        wrOV7670Reg(0x81,ov7670_reg_data[26]);
        wrOV7670Reg(0x82,ov7670_reg_data[27]);
        wrOV7670Reg(0x83,ov7670_reg_data[28]);
        wrOV7670Reg(0x84,ov7670_reg_data[29]);
        wrOV7670Reg(0x85,ov7670_reg_data[30]);
        wrOV7670Reg(0x86,ov7670_reg_data[31]);
        wrOV7670Reg(0x87,ov7670_reg_data[32]);
        wrOV7670Reg(0x88,ov7670_reg_data[33]);
        wrOV7670Reg(0x89,ov7670_reg_data[34]);

        wrOV7670Reg(0x32,ov7670_reg_data[35]);

        wrOV7670Reg(0x13,ov7670_reg_data[36]);
        wrOV7670Reg(0x00,ov7670_reg_data[37]);
        wrOV7670Reg(0x10,ov7670_reg_data[38]);
        wrOV7670Reg(0x0d,ov7670_reg_data[39]);
        wrOV7670Reg(0x14,ov7670_reg_data[40]);
        wrOV7670Reg(0xa5,ov7670_reg_data[41]);
        wrOV7670Reg(0xab,ov7670_reg_data[42]);
        wrOV7670Reg(0x24,ov7670_reg_data[43]);
        wrOV7670Reg(0x25,ov7670_reg_data[44]);
        wrOV7670Reg(0x26,ov7670_reg_data[45]);
        wrOV7670Reg(0x9f,ov7670_reg_data[46]);
        wrOV7670Reg(0xa0,ov7670_reg_data[47]);
    //  wrOV7670Reg(0xa1,0x03);//0x0b,
        wrOV7670Reg(0xa6,ov7670_reg_data[48]);
        wrOV7670Reg(0xa7,ov7670_reg_data[49]);
        wrOV7670Reg(0xa8,ov7670_reg_data[50]);
        wrOV7670Reg(0xa9,ov7670_reg_data[51]);
        wrOV7670Reg(0xaa,ov7670_reg_data[52]);
        //rt_i2c_master_send(0x13, 0xe5);
        wrOV7670Reg(0x0e,ov7670_reg_data[53]);
        wrOV7670Reg(0x0f,ov7670_reg_data[54]);
        wrOV7670Reg(0x16,ov7670_reg_data[55]);
        wrOV7670Reg(0x1e,ov7670_reg_data[56]);
        wrOV7670Reg(0x21,ov7670_reg_data[57]);
        wrOV7670Reg(0x22,ov7670_reg_data[58]);
        wrOV7670Reg(0x29,ov7670_reg_data[59]);
        wrOV7670Reg(0x33,ov7670_reg_data[60]);
        wrOV7670Reg(0x35,ov7670_reg_data[61]);
        wrOV7670Reg(0x37,ov7670_reg_data[62]);
        wrOV7670Reg(0x38,ov7670_reg_data[63]);
        wrOV7670Reg(0x39,ov7670_reg_data[64]);
        wrOV7670Reg(0x3c,ov7670_reg_data[65]);
        wrOV7670Reg(0x4d,ov7670_reg_data[66]);
        wrOV7670Reg(0x4e,ov7670_reg_data[67]);
        wrOV7670Reg(0x69,ov7670_reg_data[68]);
        wrOV7670Reg(0x6b,ov7670_reg_data[69]);
        wrOV7670Reg(0x74,ov7670_reg_data[70]);
        wrOV7670Reg(0x8d,ov7670_reg_data[71]);
        wrOV7670Reg(0x8e,ov7670_reg_data[72]);
        wrOV7670Reg(0x8f,ov7670_reg_data[73]);
        wrOV7670Reg(0x90,ov7670_reg_data[74]);
        wrOV7670Reg(0x91,ov7670_reg_data[75]);
        wrOV7670Reg(0x92,ov7670_reg_data[76]);
        wrOV7670Reg(0x96,ov7670_reg_data[77]);
        wrOV7670Reg(0x9a,ov7670_reg_data[78]);
        wrOV7670Reg(0xb0,ov7670_reg_data[79]);
        wrOV7670Reg(0xb1,ov7670_reg_data[80]);
        wrOV7670Reg(0xb2,ov7670_reg_data[81]);
        wrOV7670Reg(0xb3,ov7670_reg_data[82]);
        wrOV7670Reg(0xb8,ov7670_reg_data[83]);
        wrOV7670Reg(0x43,ov7670_reg_data[84]);
        wrOV7670Reg(0x44,ov7670_reg_data[85]);
        wrOV7670Reg(0x45,ov7670_reg_data[86]);
        wrOV7670Reg(0x46,ov7670_reg_data[87]);
        wrOV7670Reg(0x47,ov7670_reg_data[88]);
        wrOV7670Reg(0x48,ov7670_reg_data[89]);

        wrOV7670Reg(0x59,ov7670_reg_data[90]);
        wrOV7670Reg(0x5a,ov7670_reg_data[91]);
        wrOV7670Reg(0x5b,ov7670_reg_data[92]);
        wrOV7670Reg(0x5c,ov7670_reg_data[93]);
        wrOV7670Reg(0x5d,ov7670_reg_data[94]);
        wrOV7670Reg(0x5e,ov7670_reg_data[95]);

        wrOV7670Reg(0x64,ov7670_reg_data[96]);
        wrOV7670Reg(0x65,ov7670_reg_data[97]);
        wrOV7670Reg(0x66,ov7670_reg_data[98]);

        wrOV7670Reg(0x94,ov7670_reg_data[99]);
        wrOV7670Reg(0x95,ov7670_reg_data[100]);

        wrOV7670Reg(0x6c,ov7670_reg_data[101]);
        wrOV7670Reg(0x6d,ov7670_reg_data[102]);
        wrOV7670Reg(0x6e,ov7670_reg_data[103]);
        wrOV7670Reg(0x6f,ov7670_reg_data[104]);

        wrOV7670Reg(0x6a,ov7670_reg_data[105]);
        wrOV7670Reg(0x01,ov7670_reg_data[106]);
        wrOV7670Reg(0x02,ov7670_reg_data[107]);

        //rt_i2c_master_send(0x13, 0xe7);
        wrOV7670Reg(0x15,ov7670_reg_data[108]);
        wrOV7670Reg(0x4f,ov7670_reg_data[109]);
        wrOV7670Reg(0x50,ov7670_reg_data[110]);
        wrOV7670Reg(0x51,ov7670_reg_data[111]);
        wrOV7670Reg(0x52,ov7670_reg_data[112]);
        wrOV7670Reg(0x53,ov7670_reg_data[113]);
        wrOV7670Reg(0x54,ov7670_reg_data[114]);
        wrOV7670Reg(0x58,ov7670_reg_data[115]);

        wrOV7670Reg(0x41,ov7670_reg_data[116]);
        wrOV7670Reg(0x3f,ov7670_reg_data[117]);
        wrOV7670Reg(0x75,ov7670_reg_data[118]);
        wrOV7670Reg(0x76,ov7670_reg_data[119]);

        wrOV7670Reg(0x4c,ov7670_reg_data[120]);
        wrOV7670Reg(0x77,ov7670_reg_data[121]);

        wrOV7670Reg(0x3d,ov7670_reg_data[122]);
        wrOV7670Reg(0x4b,ov7670_reg_data[123]);
        wrOV7670Reg(0xc9,ov7670_reg_data[124]);
        //rt_i2c_master_send(0x40x38);
        wrOV7670Reg(0x56,ov7670_reg_data[125]);
        wrOV7670Reg(0x34,ov7670_reg_data[126]);
        wrOV7670Reg(0x3b,ov7670_reg_data[127]);
        wrOV7670Reg(0xa4,ov7670_reg_data[128]);

        wrOV7670Reg(0x96,ov7670_reg_data[129]);
        wrOV7670Reg(0x97,ov7670_reg_data[130]);
        wrOV7670Reg(0x98,ov7670_reg_data[131]);
        wrOV7670Reg(0x99,ov7670_reg_data[132]);
        wrOV7670Reg(0x9a,ov7670_reg_data[133]);
        wrOV7670Reg(0x9b,ov7670_reg_data[134]);
        wrOV7670Reg(0x9c,ov7670_reg_data[135]);
        wrOV7670Reg(0x9d,ov7670_reg_data[136]);
        wrOV7670Reg(0x9e,ov7670_reg_data[137]);

        wrOV7670Reg(0x09,ov7670_reg_data[138]);
        wrOV7670Reg(0x3b,ov7670_reg_data[139]);*/

    for(i = 0;i < sizeof(ov7670_init_reg_tb2) / sizeof(ov7670_init_reg_tb2[0]);i++)
    {
       flag = wrOV7670Reg(ov7670_init_reg_tb2[i][0], ov7670_init_reg_tb2[i][1]);
       if(flag == 1)
       {
           rt_kprintf("set reg suceess\r\n");
       }
       else {
           rt_kprintf("set reg failed\r\n");
       }
    }
    rt_kprintf("flag1 = %d flag2=%d flag3 = %d",flag1,flag2,flag3);

}

void DVP_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStruct = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DVP, ENABLE);

    DVP->CR0 &= ~RB_DVP_MSK_DAT_MOD;

#if  (DVP_Work_Mode == RGB565_MODE)

    DVP->CR0 |= RB_DVP_D8_MOD;//V_POLR?
    DVP->CR1 &= ~((RB_DVP_ALL_CLR) | RB_DVP_RCV_CLR);
    DVP->COL_NUM = 640;
    DVP->ROW_NUM = 240;

    DVP->DMA_BUF0 = RGB565_DVPDMAaddr0;
    DVP->DMA_BUF1 = RGB565_DVPDMAaddr1;
#endif
    DVP->CR1 &= ~RB_DVP_FCRC;
    DVP->CR1 |= DVP_RATE_100P;//25%

    DVP->IER |= RB_DVP_IE_STP_FRM;
    DVP->IER |= RB_DVP_IE_FIFO_OV;
    DVP->IER |= RB_DVP_IE_FRM_DONE;
    DVP->IER |= RB_DVP_IE_ROW_DONE;
    DVP->IER |= RB_DVP_IE_STR_FRM;

    NVIC_InitStruct.NVIC_IRQChannel = DVP_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    DVP->CR1 |= RB_DVP_DMA_EN;
    DVP->CR0 |= RB_DVP_ENABLE;

}

////////////////////////////////////////////////////////////////////////////
//OV7670功能设置
//白平衡设置
//0:自动
//1:太阳sunny
//2,阴天cloudy
//3,办公室office
//4,家里home
void OV7670_Light_Mode(u8 mode)
{
    u8 reg13val=0XE7;//默认就是设置为自动白平衡
    u8 reg01val=0;
    u8 reg02val=0;
    switch(mode)
    {
        case 1://sunny
            reg13val=0XE5;
            reg01val=0X5A;
            reg02val=0X5C;
            break;
        case 2://cloudy
            reg13val=0XE5;
            reg01val=0X58;
            reg02val=0X60;
            break;
        case 3://office
            reg13val=0XE5;
            reg01val=0X84;
            reg02val=0X4c;
            break;
        case 4://home
            reg13val=0XE5;
            reg01val=0X96;
            reg02val=0X40;
            break;
    }
    wrOV7670Reg(0X13,reg13val);//COM8设置
    wrOV7670Reg(0X01,reg01val);//AWB蓝色通道增益
    wrOV7670Reg(0X02,reg02val);//AWB红色通道增益
}


//色度设置
//0:-2
//1:-1
//2,0
//3,1
//4,2
void OV7670_Color_Saturation(u8 sat)
{
    u8 reg4f5054val=0X80;//默认就是sat=2,即不调节色度的设置
    u8 reg52val=0X22;
    u8 reg53val=0X5E;
    switch(sat)
    {
        case 0://-2
            reg4f5054val=0X40;
            reg52val=0X11;
            reg53val=0X2F;
            break;
        case 1://-1
            reg4f5054val=0X66;
            reg52val=0X1B;
            reg53val=0X4B;
            break;
        case 3://1
            reg4f5054val=0X99;
            reg52val=0X28;
            reg53val=0X71;
            break;
        case 4://2
            reg4f5054val=0XC0;
            reg52val=0X33;
            reg53val=0X8D;
            break;
    }
    wrOV7670Reg(0X4F,reg4f5054val); //色彩矩阵系数1
    wrOV7670Reg(0X50,reg4f5054val); //色彩矩阵系数2
    wrOV7670Reg(0X51,0X00);         //色彩矩阵系数3
    wrOV7670Reg(0X52,reg52val);     //色彩矩阵系数4
    wrOV7670Reg(0X53,reg53val);     //色彩矩阵系数5
    wrOV7670Reg(0X54,reg4f5054val); //色彩矩阵系数6
    wrOV7670Reg(0X58,0X9E);         //MTXS
}


//亮度设置
//0:-2
//1:-1
//2,0
//3,1
//4,2
void OV7670_Brightness(u8 bright)
{
    u8 reg55val=0X00;//默认就是bright=2
    switch(bright)
    {
        case 0://-2
            reg55val=0XB0;
            break;
        case 1://-1
            reg55val=0X98;
            break;
        case 3://1
            reg55val=0X18;
            break;
        case 4://2
            reg55val=0X30;
            break;
    }
    wrOV7670Reg(0X55,reg55val); //亮度调节
}


//对比度设置
//0:-2
//1:-1
//2,0
//3,1
//4,2
void OV7670_Contrast(u8 contrast)
{
    u8 reg56val=0X40;//默认就是contrast=2
    switch(contrast)
    {
        case 0://-2
            reg56val=0X30;
            break;
        case 1://-1
            reg56val=0X38;
            break;
        case 3://1
            reg56val=0X50;
            break;
        case 4://2
            reg56val=0X60;
            break;
    }
    wrOV7670Reg(0X56,reg56val); //对比度调节
}


//特效设置
//0:普通模式
//1,负片
//2,黑白
//3,偏红色
//4,偏绿色
//5,偏蓝色
//6,复古
void OV7670_Special_Effects(u8 eft)
{
    u8 reg3aval=0X04;//默认为普通模式
    u8 reg67val=0XC0;
    u8 reg68val=0X80;
    switch(eft)
    {
        case 1://负片
            reg3aval=0X24;
            reg67val=0X80;
            reg68val=0X80;
            break;
        case 2://黑白
            reg3aval=0X14;
            reg67val=0X80;
            reg68val=0X80;
            break;
        case 3://偏红色
            reg3aval=0X14;
            reg67val=0Xc0;
            reg68val=0X80;
            break;
        case 4://偏绿色
            reg3aval=0X14;
            reg67val=0X40;
            reg68val=0X40;
            break;
        case 5://偏蓝色
            reg3aval=0X14;
            reg67val=0X80;
            reg68val=0XC0;
            break;
        case 6://复古
            reg3aval=0X14;
            reg67val=0XA0;
            reg68val=0X40;
            break;
    }
    wrOV7670Reg(0X3A,reg3aval);//TSLB设置
    wrOV7670Reg(0X68,reg67val);//MANU,手动U值
    wrOV7670Reg(0X67,reg68val);//MANV,手动V值
}

void DVP_IRQHandler(void)
{
    /*//rt_kprintf("ENTER DVP IRQ\r\n");

    if(DVP->IFR & RB_DVP_IF_ROW_DONE)
    {
        DVP->IFR &= ~RB_DVP_IF_ROW_DONE;
        href_cnt++;

        if(addr_cnt % 2)
        {

            addr_cnt++;
            DVP->DMA_BUF1 += PIC_WIDTH  * 4;

        }
        else
        {

            addr_cnt++;
            DVP->DMA_BUF0 += PIC_WIDTH * 4;
        }
    }

    if(DVP->IFR & RB_DVP_IF_FRM_DONE)
    {

        DVP->IFR &= ~RB_DVP_IF_FRM_DONE;
#if (DVP_Work_Mode == RGB565_MODE)
        DVP->CR0 &= ~RB_DVP_ENABLE;
        //USING UART2 SEND DATA
        {
            rt_uint32_t i;
            rt_uint8_t val;
            rt_uint16_t color;
            //rt_kprintf("DVP FRAM DONE href——cnt%d\r\n",href_cnt);
            //rt_kprintf("DVP FRAM DONE ADDR_cnt%d\r\n",addr_cnt);
            href_cnt = href_cnt * PIC_WIDTH;

            for(i = 0;i < href_cnt  * 2 ;i = i + 1)
            {
                color = (*(rt_uint8_t*)(0x20005000 + i)) ;



                uart2_send_data(color);

            }
        }

        DVP->CR0 |= RB_DVP_ENABLE;
        DVP->DMA_BUF0 = JPEG_DVPDMAaddr0;
        DVP->DMA_BUF1 = JPEG_DVPDMAaddr1;
        addr_cnt = 0;
        href_cnt = 0;

#endif
    }
    if(DVP->IFR & RB_DVP_IF_STR_FRM)
    {
        //rt_kprintf("str_frm\r\n");
        DVP->IFR &= ~RB_DVP_IF_STR_FRM;
        //rt_kprintf("DVP START FRAM frame_cnt%d\r\n",frame_cnt);
        frame_cnt++;
    }
    if(DVP->IFR & RB_DVP_IF_STP_FRM)
    {
        //rt_kprintf("stp frm\r\n");
        DVP->IFR &= ~RB_DVP_IF_STP_FRM;
    }
    if(DVP->IFR & RB_DVP_IF_FIFO_OV)
    {
        //rt_kprintf("fifo done\r\n");
        DVP->IFR &= ~RB_DVP_IF_FIFO_OV;
    }*/
    //行中断
    if (DVP->IFR & RB_DVP_IF_ROW_DONE)
    {
        /* Write 0 clear 0 */
        DVP->IFR &= ~RB_DVP_IF_ROW_DONE;  //clear Interrupt

        #if (DVP_Work_Mode == RGB565_MODE)
                if (addr_cnt%2)     //buf1 done
                {
                    addr_cnt++;
                    //Send DVP data to LCD
                    DMA_Cmd(DMA2_Channel5, DISABLE );
                    DMA_SetCurrDataCounter(DMA2_Channel5,lcddev.width);
                    DMA2_Channel5->PADDR = RGB565_DVPDMAaddr0;
                    DMA_Cmd(DMA2_Channel5, ENABLE);

                }
                else                //buf0 done
                {
                    addr_cnt++;
                    //Send DVP data to LCD
                    DMA_Cmd(DMA2_Channel5, DISABLE );
                    DMA_SetCurrDataCounter(DMA2_Channel5,lcddev.width);
                    DMA2_Channel5->PADDR = RGB565_DVPDMAaddr1;
                    DMA_Cmd(DMA2_Channel5, ENABLE);
                }

                //href_cnt++;
                //rt_kprintf("cnt:%d",addr_cnt);

        #endif

            }

            if (DVP->IFR & RB_DVP_IF_FRM_DONE)//帧中断
            {
                DVP->IFR &= ~RB_DVP_IF_FRM_DONE;  //clear Interrupt
                //rt_kprintf("cnt:%d",addr_cnt);
                //rt_kprintf("fram_cnt:%d\r\n",frame_cnt);
                addr_cnt = 0;
                //href_cnt = 0;

            }
    if (DVP->IFR & RB_DVP_IF_STR_FRM)
         {
          DVP->IFR &= ~RB_DVP_IF_STR_FRM;

          frame_cnt++;

         }

    if (DVP->IFR & RB_DVP_IF_STP_FRM)
        {
           DVP->IFR &= ~RB_DVP_IF_STP_FRM;

        }

    if (DVP->IFR & RB_DVP_IF_FIFO_OV)
        {
          DVP->IFR &= ~RB_DVP_IF_FIFO_OV;
        }
}

u8 OV7670_Init(void)
{
    //uint16_t i = 0;
    uint8_t regL;
    uint8_t regH;
    uint16_t reg;
    rt_uint8_t buffer = 0x80;


    ov7670_GPIO_Init();
    OV7670_GPIO_CONTRL_CONFIG();


    OV7670_RST_PW_Init();

    OV7670_PWDN_L;
    rt_thread_mdelay(10);
    OV7670_RST_L;
    rt_thread_mdelay(10);
    OV7670_RST_H;

    SCCB_GPIO_Config();
    CLK_init_ON();

    rt_thread_mdelay(100);
    wrOV7670Reg( REG_OV7670_COM7, buffer);
    rt_thread_mdelay(100);

   rdOV7670Reg(REG_OV7670_MIDH, &regH);
   rt_kprintf("flag1:%dflag2:%dflag3:%d\r\n",flag1,flag2,flag3);
   rt_kprintf("MID:%08x\r\n",regH);
   rdOV7670Reg( REG_OV7670_MIDL, &regL);
   rt_kprintf("flag1:%dflag2:%dflag3:%d\r\n",flag1,flag2,flag3);
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

   rdOV7670Reg( REG_OV7670_PID, &regH);
   rdOV7670Reg( REG_OV7670_VER, &regL);
   reg = regL  | (regH << 8);
   if (reg != OV7670_PID)
   {
       rt_kprintf("PID:%08x\r\n",reg);
       return RT_ERROR;
   }
   else {
    rt_kprintf("read pid suceess\r\n");
}
   rt_thread_mdelay(100);
   ov7670_setreg();
   OV7670_config_window(184, 10, 320, 240);//
   return RT_EOK;

}

void OV7670_CreatColor()
{
    uint16_t i,j,k = 0;
    uint16_t color = 0;
    //列缓存区
    uint16_t buff[320];

    while(1)
    {
        //数据开始(从上往下，从左往右)
        //printf("data:\n");

        for(i=0;i<240;i++)
        {
            //printf("L");//列有效
            for(j=0;j < 320;j++)//一列
            {
                //生成彩条  9E F7,8D EF ,9E 3F,83 1F,FF F0,43 D9,95 28,82 10
                if(i < 30)
                {
                    color = 0x9EF7;
                }else if(i < 60)
                {
                    color = 0x8DEF;
                }else if(i < 90)
                {
                    color = 0x9E3F;
                }else if(i < 120)
                {
                    color = 0x831F;
                }else if(i < 150)
                {
                    color = 0xFFF0;
                }else if(i < 180)
                {
                    color = 0x43D9;
                }else if(i < 210)
                {
                    color = 0x9528;
                }else
                {
                    color = 0x8210;
                }
                buff[j] = color;
            }
            //准备好一列数据
            for(k=0;k<320;k++)
            {
                uart2_send_data(buff[k]);//打印色条
            }
            //printf("\n");
        }
    }
}


//MSH_CMD_EXPORT(OV7670_Init,ov7670_test);
MSH_CMD_EXPORT(DVP_Init,dvp_sendto_usart1_test);

MSH_CMD_EXPORT(OV7670_CreatColor,ov760_creat_color);
#endif /* APPLICATIONS_OV7670_C_ */
