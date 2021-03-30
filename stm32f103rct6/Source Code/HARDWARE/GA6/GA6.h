#ifndef __GA6_H__
#define __GA6_H__
#include "sys.h"
#include "bsp.h"
#include "stdio.h"

#define GY6_SENDTIME	2000
#define GY6_SENDEND		0x1A

/* 此处为11位手机号码，必须慎重修改 */
const static uint8_t *gSendMessageCmd = "AT+CMGS=\"18065695539\"\r\n";
#define GA6_SendMessageCmd		printf(gSendMessageCmd)

void GA6_Init(void);
uint8_t GA6_Send_Cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime);
uint8_t* GA6_Check_Cmd(uint8_t *str);
void GA6_SendMessage(Oled_Data_Show_t data);


#endif 
