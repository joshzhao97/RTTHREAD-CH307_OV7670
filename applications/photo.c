/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-11-10     zm       the first version
 */
#include "photo.h"
rt_thread_t thread_photo = RT_NULL;
extern rt_sem_t sd_sem;

void GPIO_KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_INIT_Structure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

    GPIO_INIT_Structure.GPIO_Pin = GPIO_KEY_PIN;
    GPIO_INIT_Structure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIO_KEY, &GPIO_INIT_Structure);
}

//文件名自增（避免覆盖）
//组合成:形如"0:PHOTO/PIC13141.bmp"的文件名
void camera_new_pathname(u8 *pname)
{
    u8 res;
    u16 index=0;
    struct stat buf;
    while(index<0XFFFF)
    {
        sprintf((char*)pname,"/pic%05d.bmp",index);
        res = stat(pname,&buf);
        if(res == 0) index++;
        else
        break;       //该文件名不存在  正是我们需要的.

    }
}


rt_uint8_t KEY_Scan(void)
{
    if(Read_KEY==0)
    {
        rt_thread_mdelay(10);
        if(Read_KEY == 0)
        return 1;
        else {
            return 0;
        }
    }
    return 0;
}

static void takephto_entry(void *parameter)
{
    rt_uint8_t key = 0;
    u8 *pname;
    int ret = 0;
    pname = malloc(30);
    rt_thread_mdelay(300);
    if(pname == RT_NULL)
        rt_kprintf("分配内存出错\r\n");

    while(1)
    {
        rt_sem_take(sd_sem, RT_WAITING_FOREVER);//防止文件系统没挂载就写入
        key = KEY_Scan();
        if(key == 1)
        {
            camera_new_pathname(pname);
            rt_kprintf("pname:%s\r\n",pname);
            if((ret = bmp_encode(pname,(lcddev.width-320)/2,(lcddev.height-240)/2,320,240,0)) != 0)//拍照有误
                rt_kprintf("写入照片文件出错，error code:%d\r\n",ret);
            else {
                rt_kprintf("写入照片文件成功,保存为:%s\r\n",pname);
            }


        }
        rt_sem_release(sd_sem);
        rt_thread_mdelay(500);
    }
}


int takephoto_start(void)
{
    GPIO_KEY_Init();
    thread_photo = rt_thread_create("takephto_start", takephto_entry, RT_NULL, 1024 *4, 4, 20);
    if(thread_photo == RT_NULL)
        return RT_ERROR;
    else {
        rt_thread_startup(thread_photo);
        return RT_EOK;
    }
}
MSH_CMD_EXPORT(takephoto_start,takephoto to save sd);
