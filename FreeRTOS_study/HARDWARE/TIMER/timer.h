#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

#define MAX_TIMER				2
extern u8 gTimer[MAX_TIMER];
#define UART_TIMER				gTimer[0]
#define DHT22_TIMER				gTimer[1]


//#define T2_1MS					1
#define T2_2MS					2
#define T2_3MS					3
#define T2_4MS					4
#define T2_5MS					5
#define T2_6MS					6
#define T2_7MS					7
#define T2_8MS					8
#define T2_9MS					9
#define T2_5000MS				5000

#define SYS_50MS				1
#define SYS_100MS				2
#define SYS_200MS				4
#define SYS_500MS				10
#define SYS_1S					20
#define SYS_2S					40


void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM7_Int_Init(u16 arr,u16 psc);
void TIM4_PWM_Init(u16 arr,u16 psc);

 
#endif
