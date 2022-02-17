#ifndef _OV7725_H
#define _OV7725_H
#include "sys.h"
#include "sccb.h"
//////////////////////////////////////////////////////////////////////////////////
//ALIENTEK MiniSTM32开发板
//OV7725 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2017/11/1
//版本：V1.0		    							    							  
//////////////////////////////////////////////////////////////////////////////////


//传感器名字定义
#define  OV7725 1
#define  OV7670 2

//由于OV7725传感器安装方式原因,OV7725_WINDOW_WIDTH相当于LCD的高度，OV7725_WINDOW_HEIGHT相当于LCD的宽度
//注意：此宏定义只对OV7725有效
#define  OV7725_WINDOW_WIDTH		40 // <=320
#define  OV7725_WINDOW_HEIGHT		40 // <=240

#define OV7725_MID				0X7FA2    
#define OV7725_PID				0X7721

#define OV7725_WRST		PAout(0)		//写指针复位
#define OV7725_RCK		PAout(1)		//读数据时钟
#define OV7725_RRST		PAout(4)  		//读指针复位
#define OV7725_CS		PAout(11)  		//片选信号(OE)
#define OV7725_WREN		PAout(12)		//写入FIFO使能
#define OV7725_VSYNC  	PAin(15)		//同步信号检测IO
															  					 
#define OV7725_DATA   GPIOB->IDR&0x01FB //数据输入端口

/////////////////////////////////////////									
	    				 
u8   OV7725_Init(void);		  	   		 
void OV7725_Light_Mode(u8 mode);
void OV7725_Color_Saturation(s8 sat);
void OV7725_Brightness(s8 bright);
void OV7725_Contrast(s8 contrast);
void OV7725_Special_Effects(u8 eft);
void OV7725_Window_Set(u16 width,u16 height,u8 mode);
void OV7725_camera_refresh(void);
void OV7725_Configuration(void);


#endif





















