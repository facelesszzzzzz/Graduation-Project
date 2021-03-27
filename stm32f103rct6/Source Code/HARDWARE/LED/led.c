#include "led.h"

/**********************************************************************
* 函数名称：LED_Init
* 功能描述：LED IO初始化
* 输入参数：无
* 返回值：无
* 补充说明：
**********************************************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能PC端口时钟
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //LED-->PC.4 端口配置
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 	//GPIO_SetBits(GPIOB,GPIO_Pin_9);						 //PC.4 输出高
 	//LED_ON; 
}
 
