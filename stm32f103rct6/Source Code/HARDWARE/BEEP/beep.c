#include "beep.h"
#include "delay.h"

/**********************************************************************
* 函数名称：BEEP_Init
* 功能描述：蜂鸣器初始化
* 输入参数：无
* 返回值：无
* 补充说明：初始化PC2为输出口.并使能这个口的时钟	
**********************************************************************/
void BEEP_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 	//使能GPIOC端口时钟
 
 GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_IO;				 	//BEEP-->PC2 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 	//推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//速度为50MHz
 GPIO_Init(BEEP_GPIO, &GPIO_InitStructure);						//根据参数初始化GPIOC.2
 
 //GPIO_SetBits(GPIOC,GPIO_Pin_2);							//输出1，开启蜂鸣器输出
}

/**********************************************************************
* 函数名称：DiDa
* 功能描述：设置蜂鸣器响应模式
* 输入参数：ms(延时)、num(次数)
* 返回值：无
* 补充说明：无
**********************************************************************/
void DiDa(u16 ms, u8 num)
{
	u8 i;
	for(i = 0; i < num; i++)
	{
		BEEP_ON;
		delay_ms(ms);
		BEEP_OFF;	
	}
}
