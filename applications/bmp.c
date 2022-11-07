/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-11-04     zm       the first version
 */
#include "ov7670.h"
#include "ili9431.h"


#if 0

//////////////////////////////////////////用户配置区///////////////////////////////
#define BMP_USE_MALLOC      1       //定义是否使用malloc,这里我们选择使用malloc
#define BMP_DBUF_SIZE       2048    //定义bmp解码数组的大小(最少应为LCD宽度*3)
//////////////////////////////////////////////END/////////////////////////////////

//BMP信息头
typedef __packed struct
{
    u32 biSize ;            //说明BITMAPINFOHEADER结构所需要的字数。
    long  biWidth ;         //说明图象的宽度，以象素为单位
    long  biHeight ;        //说明图象的高度，以象素为单位
    u16  biPlanes ;         //为目标设备说明位面数，其值将总是被设为1
    u16  biBitCount ;       //说明比特数/象素，其值为1、4、8、16、24、或32
    u32 biCompression ;     //说明图象数据压缩的类型。其值可以是下述值之一：
    //BI_RGB：没有压缩；
    //BI_RLE8：每个象素8比特的RLE压缩编码，压缩格式由2字节组成(重复象素计数和颜色索引)；
    //BI_RLE4：每个象素4比特的RLE压缩编码，压缩格式由2字节组成
    //BI_BITFIELDS：每个象素的比特由指定的掩码决定。
    u32 biSizeImage ;       //说明图象的大小，以字节为单位。当用BI_RGB格式时，可设置为0
    long  biXPelsPerMeter ; //说明水平分辨率，用象素/米表示
    long  biYPelsPerMeter ; //说明垂直分辨率，用象素/米表示
    u32 biClrUsed ;         //说明位图实际使用的彩色表中的颜色索引数
    u32 biClrImportant ;    //说明对图象显示有重要影响的颜色索引的数目，如果是0，表示都重要。
}BITMAPINFOHEADER ;
//BMP头文件
typedef __packed struct
{
    u16  bfType ;     //文件标志.只对'BM',用来识别BMP位图类型
    u32  bfSize ;     //文件大小,占四个字节
    u16  bfReserved1 ;//保留
    u16  bfReserved2 ;//保留
    u32  bfOffBits ;  //从文件开始到位图数据(bitmap data)开始之间的的偏移量
}BITMAPFILEHEADER ;
//彩色表
typedef __packed struct
{
    u8 rgbBlue ;    //指定蓝色强度
    u8 rgbGreen ;   //指定绿色强度
    u8 rgbRed ;     //指定红色强度
    u8 rgbReserved ;//保留，设置为0
}RGBQUAD ;
//位图信息头
typedef  struct
{
    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bmiHeader;
    u32 RGB_MASK[3];            //调色板用于存放RGB掩码.
    //RGBQUAD bmiColors[256];
}BITMAPINFO;
typedef RGBQUAD * LPRGBQUAD;//彩色表

//图象数据压缩的类型
#define BI_RGB          0  //没有压缩.RGB 5,5,5.
#define BI_RLE8         1  //每个象素8比特的RLE压缩编码，压缩格式由2字节组成(重复象素计数和颜色索引)；
#define BI_RLE4         2  //每个象素4比特的RLE压缩编码，压缩格式由2字节组成
#define BI_BITFIELDS    3  //每个象素的比特由指定的掩码决定。
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
    FILE* f_bmp;
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
    databuf=(u16*)pic_memalloc(1024);       //开辟至少bi4width大小的字节的内存区域 ,对240宽的屏,480个字节就够了.
    if(databuf==NULL)return PIC_MEM_ERR;        //内存申请失败.
    f_bmp=(FIL *)pic_memalloc(sizeof(FIL)); //开辟FIL字节的内存区域
    if(f_bmp==NULL)                             //内存申请失败.
    {
        pic_memfree(databuf);
        return PIC_MEM_ERR;
    }
#else
    databuf=(u16*)bmpreadbuf;
    f_bmp=&f_bfile;
#endif
    bmpheadsize=sizeof(hbmp);//得到bmp文件头的大小
    mymemset((u8*)&hbmp,0,sizeof(hbmp));//置零空申请到的内存.
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

    if(mode==1)res=f_open(f_bmp,(const TCHAR*)filename,FA_READ|FA_WRITE);//尝试打开之前的文件
    if(mode==0||res==0x04)res=f_open(f_bmp,(const TCHAR*)filename,FA_WRITE|FA_CREATE_NEW);//模式0,或者尝试打开失败,则创建新文件
    if((hbmp.bmiHeader.biWidth*2)%4)//水平像素(字节)不为4的倍数
    {
        bi4width=((hbmp.bmiHeader.biWidth*2)/4+1)*4;//实际要写入的宽度像素,必须为4的倍数.
    }else bi4width=hbmp.bmiHeader.biWidth*2;        //刚好为4的倍数
    if(res==FR_OK)//创建成功
    {
        res=f_write(f_bmp,(u8*)&hbmp,bmpheadsize,&bw);//写入BMP首部
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
            res=f_write(f_bmp,(u8*)databuf,bi4width,&bw);//写入数据
        }
        f_close(f_bmp);
    }
#if BMP_USE_MALLOC == 1 //使用malloc
    pic_memfree(databuf);
    pic_memfree(f_bmp);
#endif
    return res;
}
#endif
