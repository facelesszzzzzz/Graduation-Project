#ifndef __RELAY_H
#define __RELAY_H	 
#include "sys.h"

#define DOOR_GPIO				GPIOB
#define DOOR_GPIO_IO			GPIO_Pin_8
#define MOTOR_GPIO				GPIOB	
#define	MOTOR_GPIO_IO			GPIO_Pin_9

#define MOTOR_ON					GPIO_SetBits(MOTOR_GPIO,MOTOR_GPIO_IO)			
#define MOTOR_OFF				GPIO_ResetBits(MOTOR_GPIO,MOTOR_GPIO_IO)
#define DOOR_ON					GPIO_SetBits(DOOR_GPIO,DOOR_GPIO_IO)		
#define DOOR_OFF				GPIO_ResetBits(DOOR_GPIO,DOOR_GPIO_IO)

void RELAY_Init(void);

		 				    
#endif
