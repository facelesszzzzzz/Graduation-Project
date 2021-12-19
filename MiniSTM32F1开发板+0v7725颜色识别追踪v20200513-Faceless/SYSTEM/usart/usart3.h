#ifndef __USART3_H
#define __USART3_H
#include "sys.h"

#define USART2_MAX_RECV_LEN		600					//最大接收缓存字节数
#define USART2_MAX_SEND_LEN		600					//最大发送缓存字节数
#define USART2_RX_EN 			1					//0,不接收;1,接收.

//extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
//extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
void USART2_init(u32 bound);				//串口3初始化 
void u2_printf(char* fmt,...);
u8* USART2_check_cmd(u8 *str);
#endif
