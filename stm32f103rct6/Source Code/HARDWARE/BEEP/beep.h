#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"

#define BEEP_GPIO				GPIOC
#define BEEP_GPIO_IO			GPIO_Pin_2

#define BEEP_ON					GPIO_SetBits(BEEP_GPIO,BEEP_GPIO_IO)
#define BEEP_OFF				GPIO_ResetBits(BEEP_GPIO,BEEP_GPIO_IO)		   

void BEEP_Init(void);
void DiDa(u16 ms, u8 num);

		 				    
#endif

