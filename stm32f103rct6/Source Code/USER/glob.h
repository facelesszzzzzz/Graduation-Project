#ifndef __GLOB_H__
#define __GLOB_H__
#include "sys.h"
#include "datatype.h"

extern stWIFI *pESP8266;

extern u8 gUsartCount,ReceiveFlag,gReceive[];
extern stReceived *pReceived;
extern u8 gRecDataFormat[];

//串口接收缓存区 	
extern u8 USART3_RX_BUF[]; 			//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
extern u8 USART3_TX_BUF[]; 			//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern u16 USART3_RX_STA; 

extern stDHT22 *pDHT22;

extern u8 dht22_RH_str[];			//湿度字符串变量
extern u8 dht22_Tem_str[];			//温度字符串变量
extern u8 send_Tem[];
extern u8 send_RH[];
extern u16 dht22_RH;				//湿度整型变量
extern u16 dht22_Tem;				//温度整型变量

extern stMQ2 *pMQ2;

extern Byte8 gState;
extern u8 gSend[];

extern u8 BODY_Flag;
extern u8 BODY_Num;



#endif
