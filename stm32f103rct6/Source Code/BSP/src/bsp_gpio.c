#include "bsp_gpio.h"
#include "config.h"

static GPIO_Pin_t GetGPIOx_Pinx(GPIO_PinTypeDef GPIOx_Pinx)
{
	GPIO_Pin_t sGet;
	sGet.GPIOx = GPIOx_Pinx / 16;
	sGet.Pinx = GPIOx_Pinx % 16;
	return sGet;
}

void GPIO_PinInit(GPIO_PinTypeDef GPIOx_Pinx, GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_Pin_t sInit; 
	GPIO_InitTypeDef  GPIO_InitStructure;
	uint32_t sGPIOx; 
	sInit = GetGPIOx_Pinx(GPIOx_Pinx);
	sGPIOx = APB2PERIPH_BASE + 0x0800 + (sInit.GPIOx * 0x0400);
	GPIO_InitStructure.GPIO_Pin = SetBit(GPIO_Pin_All,sInit.Pinx);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode; 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			
	GPIO_Init((GPIO_TypeDef*)(sGPIOx), &GPIO_InitStructure);
}

