/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-23     zm       the first version
 */
#include "i2c.h"

void I2C_UserConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructrue;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

    GPIO_InitStructrue.GPIO_Pin = SDA|SCL;
    GPIO_InitStructrue.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(I2C_Port,&GPIO_InitStructrue);
}
void I2C_SDA_Mode(u8 addr)
{
    GPIO_InitTypeDef GPIO_InitStructrue;
    if(addr)//1输出
    {
    GPIO_InitStructrue.GPIO_Pin = SDA;
        GPIO_InitStructrue.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(I2C_Port,&GPIO_InitStructrue);
    }
    else//0输入
    {
        GPIO_InitStructrue.GPIO_Pin = SDA;
        GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(I2C_Port,&GPIO_InitStructrue);
    }
}
void I2C_Start(void)
{
    I2C_SDA_Mode(I2C_OUT);
    SCL_HIGH;
    rt_hw_us_delay(5);
    SDA_LOW;
    rt_hw_us_delay(5);
    SCL_LOW;
}
void I2C_Stop(void)
{
    I2C_SDA_Mode(I2C_OUT);
    SDA_LOW;
    rt_hw_us_delay(5);
    SCL_HIGH;
    rt_hw_us_delay(5);
    SDA_HIGH;
}
u8 I2C_Write_Ack(void)
{
    u8 timeack=0;
    I2C_SDA_Mode(I2C_IN);
    SCL_HIGH;
    rt_hw_us_delay(4);
    while(GPIO_ReadInputDataBit(I2C_Port,SDA)){
    if(++timeack > 200)
        {
            I2C_Stop();
            return RT_ERROR;
        }
    }
    SCL_LOW;
    rt_hw_us_delay(4);

    return RT_EOK;
}
void I2C_Write_Byte(u8 data)
{
    SCL_LOW;
    rt_hw_us_delay(4);
    for(u8 i = 0;i < 8;i++)
        {
                I2C_SDA_Mode(I2C_OUT);//**
            if((data << i) & 0x80) SDA_HIGH;
            else   SDA_LOW;

            SCL_HIGH;
                rt_hw_us_delay(4);
            SCL_LOW;
                rt_hw_us_delay(4);
        }
}
u8 I2C_Read_Data(void)
{
    u8 data=0;
    for(u8 i = 0;i < 8;i++)
    {
        I2C_SDA_Mode(I2C_IN);//**
        SCL_HIGH;
        rt_hw_us_delay(4);

        data <<=1;

        if(GPIO_ReadInputDataBit(I2C_Port,SDA) == 1)
        {
        data |= 0x01;
        }
        SCL_LOW;
        rt_hw_us_delay(4);
    }
    return data;
}
