#ifndef __BSP_INTERRUPT_H__
#define	__BSP_INTERRUPT_H__

#include "sys.h"
#include "bsp.h"

#define MAX_TIMER				5
extern uint16_t gTimer[MAX_TIMER];
#define Ev25Ms				gTimer[0]
#define Ev50Ms				gTimer[1]
#define Ev100Ms				gTimer[2]
#define Ev500Ms				gTimer[3]
#define Ev1S				gTimer[4]

#define SysTick_25MS				25
#define SysTick_50MS				50
#define SysTick_100MS				100
#define SysTick_500MS				500
#define SysTick_1S					1000


#if 0
#define EVENT_BUFFER_LENGTH	10

void    EventCollect(void);
void 	PushEvent(uint8_t event);
uint8_t	PopEvent(void);
#endif 

#endif
