#ifndef __BODY_H
#define __BODY_H	 
#include "sys.h"

#define BODY_GPIO			GPIOC
#define BODY_GPIO_IO		GPIO_Pin_0
#define BODY_READ			GPIO_ReadInputDataBit(BODY_GPIO,BODY_GPIO_IO)

void BODY_Init(void);	//≥ı ºªØ
void BODY_Handle(void);


		 				    
#endif

