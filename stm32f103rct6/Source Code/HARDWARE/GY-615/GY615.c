#include "GY615.h"
#include "bsp.h"
#include <stdio.h>

void Search_GY615_Data()
{
	uint8_t i;
	uint8_t sSend_Buf[] = {GY615_HEAD, GY615_READ, GY615_REGISTER, 
		GY615_REGISTERNUM, GY615_READ_CHECKSUM};
	for(i = 0; i < sizeof(sSend_Buf); i++)
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕	 
		USART_SendData(USART3,sSend_Buf[i]);
	}
}
extern Oled_Data_Show_t *pData_Show;
void GY615_Handle()
{
	Uart_Data_t *sUart3_Data = GetUart_Data_t(Uart3_NUM);
	uint16_t sTO = 0, sTA = 0, sBO = 0;
	sTO = (sUart3_Data->RxBuf[5] << 8) + sUart3_Data->RxBuf[6];
	sTA = (sUart3_Data->RxBuf[7] << 8) + sUart3_Data->RxBuf[8];
	sBO = (sUart3_Data->RxBuf[9] << 8) + sUart3_Data->RxBuf[10];
	sprintf(pData_Show->TargetTemp, "%.2f", (float)sTO/100);
	sprintf(pData_Show->Environmenttemp, "%.2f", (float)sTA/100);
	sprintf(pData_Show->BodyTemp, "%.2f", (float)sBO/100);
}



