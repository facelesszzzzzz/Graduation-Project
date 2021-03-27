#include "glob.h"
#include "usart3.h"

//-----------------------------------------------
//----------------通信数据-----------------------
stWIFI gESP8266;
stWIFI *pESP8266 = &gESP8266;

u8 gUsartCount = 0,ReceiveFlag = 0,gReceive[RECEIVE_DATA_THENGTH];
stReceived gReceived;
stReceived *pReceived = &gReceived;
u8 gRecDataFormat[4] = {'d','e','9','9'};

u8 USART3_RX_BUF[USART3_MAX_RECV_LEN];	//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN];	//发送缓冲,最大USART3_MAX_SEND_LEN个字节
u16 USART3_RX_STA;

Byte8 gState;
u8 gSend[SEND_DATA_THENGTH];
//-----------------------------------------------
//--------------传感器数据-----------------------
stDHT22 gDHT22;
stDHT22 *pDHT22 = &gDHT22;

u8 dht22_RH_str[] = "00.0";				//湿度字符串变量
u8 dht22_Tem_str[] = "00.0";			//温度字符串变量
u8 send_Tem[] = "Tem:00.0";
u8 send_RH[] = "RH:00.0";
u16 dht22_RH;							//湿度整型变量
u16 dht22_Tem;							//温度整型变量

stMQ2 gMQ2;
stMQ2 *pMQ2 = &gMQ2;

u8 BODY_Flag = 0;
u8 BODY_Num = 1;






