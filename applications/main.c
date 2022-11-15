/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
/*  SCL-PB10,SDA-PB11,VS-pa5,HS-PA4,PCLK-PA6,MCLK-PA8,D7-PB9,D6-PB8,D5-PB6,D4-PC11,D3-PC9,D2-PC8,D1-PA10,D0-PA9,RST-PB0（or gnd?）,PWDN-PB1(or gnd?)*/
//由于杜邦线信号干扰图像不稳定帧率40FPS
//使用spi2挂载sd卡文件系统：cs gpiob_12,, spi2_clk gpiob_13,,,  spi_miso gpiob_14,,,  spi_mosi gpiob_15,,,
//目前挂载文件系统成功，函数接口实现，保存文件有时成功
//key:PE4按下拍照
/*
 *@Note
   TFTLCD箭殻�
  LCD！！PIN�
    PD11！！FSMC_A16
    PD12！！FSMC_A17
    PD5 ！！FSMC_NEW
    PD4 ！！FSMC_NOE
    PA15！！LCDRST#
    PD14！！FSMC_D0
    PD15！！FSMC_D1
    PD0 ！！FSMC_D2
    PD1 ！！FSMC_D3
    PE7 ！！FSMC_D4
    PE8 ！！FSMC_D5
    PE9 ！！FSMC_D6
    PE10！！FSMC_D7
    PE11！！FSMC_D8
    PE12！！FSMC_D9
    PE13！！FSMC_D10
    PE14！！FSMC_D11
    PE15！！FSMC_D12
    PD8 ！！FSMC_D13
    PD9 ！！FSMC_D14
    PD10！！FSMC_D15
    PB14！！IO_BLCTR//换成pb4
    PA8 ！！IO_MISO_NC
    PB3 ！！IO_MOSI_SDA
    PB15！！IO_TKINT
    PC13！！IO_BUSY_NC
    PC0 ！！IO_TKRST#
    PB4 ！！IO_CLK
*/
#include "ch32v30x.h"
#include <rtthread.h>
#include <rthw.h>
#include "drivers/pin.h"
#include <board.h>
#include <rtdbg.h>
#include <rtdevice.h>
#include <drv_spi.h>
#include <dfs_elm.h>
#include <dfs_fs.h>
#include <dfs_posix.h>
#include "drv_spi.h"
#include "spi_msd.h"
#include <rtdbg.h>
#include "ili9431.h"
#include "ov7670.h"
//#include <ff.h>


/* Global typedef */


/* Global define */


#define RGB565_ROW_NUM   240
#define RGB565_COL_NUM   640   //Col * 2
/* Global Variable */
rt_device_t dev;
extern rt_uint32_t frame_cnt;

rt_sem_t sd_sem = RT_NULL;

rt_uint32_t  RGB565_DVPDMAaddr0 = 0x2000A000;
rt_uint32_t  RGB565_DVPDMAaddr1 = 0x2000A000 + RGB565_COL_NUM;


/*********************************************************************
 * @fn      LCD_Reset_GPIO_Init
 *
 * @brief   Init LCD reset GPIO.
 *
 * @return  none
 */
void LCD_Reset_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_15);
}


/*********************************************************************
 * @fn      DMA_SRAMLCD_Init
 *
 * @brief   Init SRAMLCD DMA
 *
 * @param   ddr: DVP data memory base addr.
 *
 * @return  none
 */
void DMA_SRAMLCD_Init(u32 ddr)
{
    DMA_InitTypeDef DMA_InitStructure={0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    DMA_DeInit(DMA2_Channel5);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ddr;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&LCD->LCD_RAM;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA2_Channel5, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      DMA_SRAMLCD_Enable
 *
 * @brief   Enable SRAMLCD DMA transmission
 *
 * @return  none
 */
void DMA_SRAMLCD_Enable(void)
{
    DMA_Cmd(DMA2_Channel5, DISABLE );
    DMA_SetCurrDataCounter(DMA2_Channel5,lcddev.width);
    DMA_Cmd(DMA2_Channel5, ENABLE);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */

int main(void)
{
    rt_uint32_t FPS = 0;
    rt_uint32_t TEMP = 0;
    //int ret;
    //u8 lightmode=0,saturation=2,brightness=2,contrast=2,effect=0;
    rt_kprintf("MCU: CH32V307\n");
	rt_kprintf("SysClk: %dHz\n",SystemCoreClock);
    rt_kprintf("www.wch.cn\n");
    rt_kprintf("per seconds = %d",RT_TICK_PER_SECOND);
	//LED1_BLINK_INIT();


	//rt_err_t ret = 0;
	//rt_size_t oled;
	rt_uint32_t i = 0;
	rt_thread_mdelay(2000);
    rt_kprintf("test\r\n");
	for(i = 0;i < 1000; i++)
	{
	  	rt_hw_us_delay(1000);
	  	//rt_kprintf("reload = %d",SysTick->CMP);
	}

    rt_kprintf("start count\r\n");

    sd_sem = rt_sem_create("sd_sem", 1, RT_IPC_FLAG_FIFO);
    if(sd_sem == RT_NULL)
        rt_kprintf("create sd_sem failed\r\n");
    rt_kprintf("create sd_sem success\r\n");

    /* LCD reset */
    LCD_Reset_GPIO_Init();
    GPIO_ResetBits(GPIOA,GPIO_Pin_15);
    rt_thread_mdelay(100);
    GPIO_SetBits(GPIOA,GPIO_Pin_15);
    rt_kprintf("reset lcd done\r\n");
    LCD_Init();

    rt_kprintf("width:%02d height:%02d \n", lcddev.width, lcddev.height);
    DMA_SRAMLCD_Init((u32)RGB565_DVPDMAaddr0);  //DMA2
    while(OV7670_Init() != RT_EOK);
    rt_kprintf("ov7670_init sucees\r\n");
    rt_thread_mdelay(1500);
    //OV7670_Light_Mode(lightmode);
    //OV7670_Color_Saturation(saturation);
    //OV7670_Brightness(brightness);
    //OV7670_Contrast(contrast);
    //OV7670_Special_Effects(effect);



	while(1)
	{
	    TEMP = frame_cnt;


	    //LCD_ShowNum(30,30,FPS,2,10);
	    rt_thread_mdelay(1000);

	    FPS = frame_cnt - TEMP;
	    //rt_kprintf("FPS:%d",FPS);暂时不需要打印
	}
}


static int rt_hw_spi1_tfcard(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
    rt_hw_spi_device_attach("spi2", "spi10", GPIOB, GPIO_Pin_12);//CS片选信号
    rt_kprintf(" attach spi device\r\n");
    return msd_init("sd0","spi10");
}




static void sd_mount(void *parameter)
{
    //rt_err_t ret;
    while (1)
    {
        rt_thread_mdelay(500);
        if(rt_device_find("sd0") != RT_NULL)
        {
            if (dfs_mount("sd0", "/", "elm", 0, 0) == RT_EOK)
            {
                rt_kprintf("sd card mount success\r\n");
                LOG_I("sd card mount to '/'");
                rt_sem_release(sd_sem);
                break;
            }
            else
            {

                LOG_W("sd card mount to '/' failed!");
              //ret =  rt_get_errno();
            }
        }
    }
}

int sd_start(void)
{

    rt_thread_t tid;

    tid = rt_thread_create("sd_mount", sd_mount, RT_NULL,
                           2048, 9, 20);
    if (tid != RT_NULL)
    {
        rt_kprintf("enter sd_mount thread\r\n");
        rt_thread_startup(tid);
        return RT_EOK;
    }
    else
    {
        LOG_E("create sd_mount thread err!");
        return RT_ERROR;
    }

}


INIT_DEVICE_EXPORT(rt_hw_spi1_tfcard);
MSH_CMD_EXPORT(sd_start,  start sd thread);







