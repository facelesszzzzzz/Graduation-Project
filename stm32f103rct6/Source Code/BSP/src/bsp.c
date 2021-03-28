#include "bsp.h"

static void BSP_SysTick_Init(void)
{
	if(SysTick_Config(SystemCoreClock/1000)){
		while(1);
	}
	NVIC_SetPriority(SysTick_IRQn,0x0);
}

static void BSP_IO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);

	GPIO_PinInit(GPIO_PB12, GPIO_Mode_Out_PP);		/* OLED SDA */
	GPIO_PinInit(GPIO_PB13, GPIO_Mode_Out_PP);		/* OLED SCK */
	
	GPIO_PinInit(GPIO_PC1, GPIO_Mode_Out_OD);		/* MAX30102 SCL */
	GPIO_PinInit(GPIO_PC4, GPIO_Mode_Out_OD);		/* MAX30102 SDA */

//	GPIO_PinInit(GPIO_PE5, GPIO_Mode_Out_PP);
}

void BSP_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	BSP_IO_Init();
	BSP_Uart_Init();
	BSP_SysTick_Init();
}

