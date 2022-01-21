#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//蜂鸣器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
//蜂鸣器端口定义
#define BEEP_GPIO				GPIOC
#define BEEP_GPIO_IO			GPIO_Pin_6 

//#define BEEP_ON                 GPIO_ResetBits(BEEP_GPIO,BEEP_GPIO_IO)
//#define BEEP_OFF                GPIO_SetBits(BEEP_GPIO,BEEP_GPIO_IO)

#define SOUND_GPIO              GPIOB
#define SOUND_GPIO_IO           GPIO_Pin_15

#define SOUND_READ              GPIO_ReadInputDataBit(SOUND_GPIO, SOUND_GPIO_IO)

typedef enum
{
    Beep_Off = 0,
    Beep_On,
}Beep_State_e;

void BEEP_Init(void);	//初始化
void Sound_Init(void);
uint8_t Get_BeepState(void);
void BEEP_Config(uint8_t mState);
void BEEP_Handle(void);
		 				    
#endif

