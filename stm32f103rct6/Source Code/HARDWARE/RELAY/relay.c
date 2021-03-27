#include "relay.h"

/**********************************************************************
* 函数名称：RELAY_Init
* 功能描述：继电器 IO初始化
* 输入参数：无
* 返回值：无
* 补充说明：
**********************************************************************/
void RELAY_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能PC端口时钟
	
 	GPIO_InitStructure.GPIO_Pin = DOOR_GPIO_IO|MOTOR_GPIO_IO;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 	GPIO_Init(MOTOR_GPIO, &GPIO_InitStructure);					 //根据设定参数初始化
}
 
