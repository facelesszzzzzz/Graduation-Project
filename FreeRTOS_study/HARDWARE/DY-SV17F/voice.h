#ifndef _VOICE_H_
#define _VOICE_H_
#include "sys.h"

typedef enum
{
    Voice_none = 0,
    Voice_one = 1,
    Voice_two = 2,
    Voice_three = 4,
    Voice_four = 8,
    Voice_five = 16,
    Voice_six = 32,
    Voice_seven = 64,
    Voice_eight = 128
}VOICE_Song_e;

#define VOICE_ENABLE                1
#define VOICE_DISABLE               0

#ifdef VOICE_UART
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
#endif

void VOICE_Init(void);
void VOICE_Mode(void);
void VOICE_Send(u8 data);
void VOICE_Select(uint8_t mSongNum);


#endif



