/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-11-04     zm       the first version
 */
#include "bmp.h"

#define PIC_FORMAT_ERR      0x27    //格式错误
#define PIC_SIZE_ERR        0x28    //图片尺寸错误
#define PIC_WINDOW_ERR      0x29    //窗口设定错误
#define PIC_MEM_ERR         0x11    //内存错误

//BMP编码函数
//将当前LCD屏幕的指定区域截图,存为16位格式的BMP文件 RGB565格式.
//保存为rgb565则需要掩码,需要利用原来的调色板位置增加掩码.这里我们已经增加了掩码.
//保存为rgb555格式则需要颜色转换,耗时间比较久,所以保存为565是最快速的办法.
//filename:存放路径
//x,y:在屏幕上的起始坐标
//mode:模式.0,仅仅创建新文件的方式编码;1,如果之前存在文件,则覆盖之前的文件.如果没有,则创建新的文件.
//返回值:0,成功;其他,错误码.
u8 bmp_encode(u8 *filename,u16 x,u16 y,u16 width,u16 height,u8 mode)
{
    //FILE* f_bmp;
    u16 bmpheadsize;            //bmp头大小
    BITMAPINFO hbmp;            //bmp头
    u8 res=0;
    u16 tx,ty;                  //图像尺寸
    u16 *databuf;               //数据缓存区地址
    u16 pixcnt;                 //像素计数器
    u16 bi4width;               //水平像素字节数
    if(width==0||height==0)return PIC_WINDOW_ERR;   //区域错误
    if((x+width-1)>lcddev.width)return PIC_WINDOW_ERR;      //区域错误
    if((y+height-1)>lcddev.height)return PIC_WINDOW_ERR;    //区域错误

#if BMP_USE_MALLOC == 1 //使用malloc
    databuf=(u16*)malloc(1024);       //开辟至少bi4width大小的字节的内存区域 ,对240宽的屏,480个字节就够了.
    if(databuf==NULL)return PIC_MEM_ERR;        //内存申请失败.
/*    f_bmp=(FIL *)malloc(sizeof(FIL)); //开辟FIL字节的内存区域
    if(f_bmp==NULL)                             //内存申请失败.
    {
        pic_memfree(databuf);
        return PIC_MEM_ERR;
    }*/
#else
    databuf=(u16*)bmpreadbuf;
    f_bmp=&f_bfile;
#endif
    bmpheadsize=sizeof(hbmp);//得到bmp文件头的大小
    memset((u8*)&hbmp,0,sizeof(hbmp));//置零空申请到的内存.
    hbmp.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);//信息头大小
    hbmp.bmiHeader.biWidth=width;       //bmp的宽度
    hbmp.bmiHeader.biHeight=height;     //bmp的高度
    hbmp.bmiHeader.biPlanes=1;          //恒为1
    hbmp.bmiHeader.biBitCount=16;       //bmp为16位色bmp
    hbmp.bmiHeader.biCompression=BI_BITFIELDS;//每个象素的比特由指定的掩码决定。
    hbmp.bmiHeader.biSizeImage=hbmp.bmiHeader.biHeight*hbmp.bmiHeader.biWidth*hbmp.bmiHeader.biBitCount/8;//bmp数据区大小

    hbmp.bmfHeader.bfType=((u16)'M'<<8)+'B';//BM格式标志
    hbmp.bmfHeader.bfSize=bmpheadsize+hbmp.bmiHeader.biSizeImage;//整个bmp的大小
    hbmp.bmfHeader.bfOffBits=bmpheadsize;//到数据区的偏移

    hbmp.RGB_MASK[0]=0X00F800;          //红色掩码
    hbmp.RGB_MASK[1]=0X0007E0;          //绿色掩码
    hbmp.RGB_MASK[2]=0X00001F;          //蓝色掩码

    if(mode==1)res=open(filename,O_RDWR|O_APPEND);//尝试打开之前的文件
    if(mode==0||res==0x04)res=open(filename,O_WRONLY|O_CREAT|O_APPEND);//模式0,或者尝试打开失败,则创建新文件
    if((hbmp.bmiHeader.biWidth*2)%4)//水平像素(字节)不为4的倍数
    {
        bi4width=((hbmp.bmiHeader.biWidth*2)/4+1)*4;//实际要写入的宽度像素,必须为4的倍数.
    }else bi4width=hbmp.bmiHeader.biWidth*2;        //刚好为4的倍数
    if(res >= 0)//创建成功
    {
        rt_kprintf("start writing...\r\n");
        write(res,(u8*)&hbmp,bmpheadsize);//写入BMP首部
        for(ty=y+height-1;hbmp.bmiHeader.biHeight;ty--)
        {
            pixcnt=0;
            for(tx=x;pixcnt!=(bi4width/2);)
            {
                if(pixcnt<hbmp.bmiHeader.biWidth)databuf[pixcnt]=LCD_ReadPoint(tx,ty);//读取坐标点的值
                else databuf[pixcnt]=0Xffff;//补充白色的像素.
                pixcnt++;
                tx++;
            }
            hbmp.bmiHeader.biHeight--;
            write(res,(u8*)databuf,bi4width);//写入数据
        }
        fsync(res);
        close(res);
    }
#if BMP_USE_MALLOC == 1 //使用malloc
    free(databuf);
    //free(f_bmp);
#endif
    return RT_EOK;
}

