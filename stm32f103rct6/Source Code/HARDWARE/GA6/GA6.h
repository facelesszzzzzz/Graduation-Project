#ifndef __GA6_H__
#define __GA6_H__
#include "sys.h"
#include "bsp.h"

#define GY6_SENDTIME	2000

void GA6_Init(void);
uint8_t GA6_Send_Cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime);
uint8_t* GA6_Check_Cmd(uint8_t *str);


#endif 
