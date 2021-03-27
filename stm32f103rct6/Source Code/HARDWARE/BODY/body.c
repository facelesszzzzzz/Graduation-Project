#include "body.h"
#include "glob.h"
#include "usart3.h"
#include "oled.h"
/**********************************************************************
* 函数名称：BODY_Init
* 功能描述：人体红外模块
* 输入参数：无
* 返回值：无
* 补充说明：
**********************************************************************/
void BODY_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 	 	//使能GPIOC端口时钟
 
 GPIO_InitStructure.GPIO_Pin = BODY_GPIO_IO;						//BODY-->PC.0 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 			//浮空输入
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 			//速度为50MHz
 GPIO_Init(BODY_GPIO, &GPIO_InitStructure);							//根据参数初始化GPIOC.0
}
/**********************************************************************
* 函数名称：BODY_Handle
* 功能描述：在防盗模式下进行人体检测，若有人则报警
* 输入参数：无
* 返回值：无
* 补充说明：增加发送次数限制，检测一次就发送一次数据，避免重复发送
**********************************************************************/
void BODY_Handle()
{
	if(BODY_Flag)
	{
		if(BODY_READ == 1)
		{
			gState.ALL |= 0x01;
			if(BODY_Num)//限制发送次数
			{
				BODY_Num = 0;
				u3_printf("protect");
			}
		}else
		{
			BODY_Num = 1;
			gState.ALL &= ~0x01;
		}
//		OLED_SHOWBMP(gState);
	}
}
