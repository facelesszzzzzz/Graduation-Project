#ifndef __USART3_H
#define __USART3_H
#include "sys.h"

#define USART3_MAX_RECV_LEN		600					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		600					//最大发送缓存字节数
#define USART3_RX_EN 			1					//0,不接收;1,接收.

void usart3_init(u32 bound);				//串口3初始化 
void u3_printf(char* fmt,...);
#endif
