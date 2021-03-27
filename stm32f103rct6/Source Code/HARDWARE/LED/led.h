#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
					  
//#define LED0 PAout(7)// PB5
//#define LED1 PEout(5)// PE5	

#define LED_ON					GPIO_SetBits(GPIOB,GPIO_Pin_9);			
#define LED_OFF					GPIO_ResetBits(GPIOC,GPIO_Pin_1);

void LED_Init(void);

		 				    
#endif
