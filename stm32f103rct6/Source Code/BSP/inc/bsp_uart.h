#ifndef __BSP_UART_H__
#define __BSP_UART_H__
#include "sys.h"

#define Uart1_NUM	1
#define Uart2_NUM	2
#define Uart3_NUM	3

#define Uart1_EN	1
#define Uart2_EN	0
#define Uart3_EN	1

#define Uart1_Printf_EN	0
#define Uart2_Printf_EN	0
#define Uart3_Printf_EN	0

#define Uart1_PreemptionPriority	0
#define Uart1_SubPriority			0

#define Uart2_PreemptionPriority	0
#define Uart2_SubPriority			1

#define Uart3_PreemptionPriority	0
#define Uart3_SubPriority			2

#define USARTx_TX_LENGTH		200
#define USARTx_RX_LENGTH		200


typedef struct
{
	uint8_t TxBuf[USARTx_TX_LENGTH];
	uint8_t TxLen;
	uint8_t RxResBuf[USARTx_RX_LENGTH];
	uint8_t RxBuf[USARTx_RX_LENGTH];
	uint8_t RxLen;
}Uart_Data_t;

extern uint16_t USART_RX_STA;         		//接收状态标记
//extern Uart_Data_t *pUart1_Data, *pUart2_Data, *pUart3_Data;

void BSP_Uart_Init(void);
Uart_Data_t *GetUart_Data_t(uint8_t Uartx);
void u1_printf(char* fmt,...);
void u2_printf(char* fmt,...);
void u3_printf(char* fmt,...);


#endif
