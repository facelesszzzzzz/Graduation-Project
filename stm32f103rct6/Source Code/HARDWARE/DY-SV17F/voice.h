#ifndef _VOICE_H_
#define _VOICE_H_
#include "sys.h"

#define VOICE_DATA_LENGHT 			4

#define VOICE_CON_GPIO 				GPIOC
#define VOICE_CON1					GPIO_Pin_10
#define VOICE_CON2					GPIO_Pin_11
#define VOICE_CON3					GPIO_Pin_12

#define VOICE_CON1_1				GPIO_SetBits(VOICE_CON_GPIO, VOICE_CON1)
#define VOICE_CON1_0				GPIO_ResetBits(VOICE_CON_GPIO, VOICE_CON1)

#define VOICE_CON2_1				GPIO_SetBits(VOICE_CON_GPIO, VOICE_CON2)
#define VOICE_CON2_0				GPIO_ResetBits(VOICE_CON_GPIO, VOICE_CON2)

#define VOICE_CON3_1				GPIO_SetBits(VOICE_CON_GPIO, VOICE_CON3)
#define VOICE_CON3_0				GPIO_ResetBits(VOICE_CON_GPIO, VOICE_CON3)

#define VOICE_PLAY					0x02
#define VOICE_PAUSE					0x03
#define VOICE_STOP					0x04
#define VOICE_LAST					0x05
#define VOICE_Next					0x06
#define VOICE_SOUND_ADD				0x14
#define VOICE_SOUND_SUB				0x15

void VOICE_Init(void);
void VOICE_Mode(void);
void VOICE_Send(u8 data);


#endif



