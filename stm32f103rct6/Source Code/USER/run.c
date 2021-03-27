#include "run.h"
#include "bsp.h"
#include <stdio.h>
#include "max30102.h"


void System_Init()
{
	MAX30102_Init();
//	delay_init();	    	 							//延时函数初始化	  
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
//	//uart_init(115200);	 							//串口初始化为115200
//	OLED_Init();										//显示屏初始化
//	RELAY_Init();										//继电器初始化
//	BEEP_Init();										//蜂鸣器初始化
//	KEY_Init();											//按键初始化
//	//VOICE_Init();								
//	while(DHT22_Init());								//温湿度传感器初始化
//	//VOICE_Send(0x02);
//	RC522_Init();										//RFID初始化
//	MQ2_Init();											//烟雾传感器初始化
//	WIFI_Connect();										//wifi连接
//	OLED_MAIN();										//OLED显示
//	TIM3_Int_Init(499,7199);							//72Mhz/(7199+1)=10Khz的.计数频率，0.1ms,  计数到(499+1)为50ms  
}

void Task_25MsHandle()
{
	if(Ev25Ms) return;
	Ev25Ms = SysTick_25MS;
}

void Task_50MsHandle()
{
	if(Ev50Ms) return;
	Ev50Ms = SysTick_50MS;

}

void Task_100MsHandle()
{
	if(Ev100Ms) return;
	Ev100Ms = SysTick_100MS;
}

void Task_500MsHandle()
{
	if(Ev500Ms) return;
	Ev500Ms = SysTick_500MS;
}

void Task_1SHandle()
{
	if(Ev1S) return;
	Ev1S = SysTick_1S;
		MAX30102_Handle();
}

void System_Running()
{
	Task_25MsHandle();
	Task_50MsHandle();
	Task_100MsHandle();
	Task_500MsHandle();
	Task_1SHandle();
}

//void Task_ReceiveHandle()
//{
//	ESP8266_RecToAPP();
//	OLED_SHOWBMP(gState);
//}

//void Task_BodyHandle()
//{
//	BODY_Handle();
//}

//void Task_UARTHandle()
//{
//	if(UART_TIMER) return;
//	UART_TIMER = SYS_50MS;
//	ESP8266_SendToApp();
//}
//void Task_KEYHandle()
//{
//  	if(KEY_TIMER) return;
//  	KEY_TIMER = SYS_100MS;
//	KEY_Handle();
//}
//void Task_RC522Handle()
//{
//	if(RC522_TIMER) return;
//	RC522_TIMER = SYS_200MS;
//	RC522_Handle();
//}
//void Task_DHT22Handle()
//{
//	if(DHT22_TIMER) return;
//	DHT22_TIMER = SYS_500MS;
//	while(DHT22_Read_Data());
//}

//void Task_MQ2Handle()
//{
//	if(MQ2_TIMER) return;
//	MQ2_TIMER = SYS_1S;
//	MQ2_Handle();
//}
//	uint8_t t;
//	Uart_Data_t *sUart1_Data = GetUart_Data_t(Uart1_NUM);
//	if(USART_RX_STA&0x8000)
//	{					   
//		sUart1_Data->RxLen = USART_RX_STA&0x3fff;//得到此次接收到的数据长度
//		printf("\r\n您发送的消息为:\r\n\r\n");
//		for(t=0;t<sUart1_Data->RxLen;t++)
//		{
//			USART_SendData(USART1, sUart1_Data->RxBuf[t]);//向串口1发送数据
//			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//		}
//		printf("\r\n\r\n");//插入换行
//		USART_RX_STA=0;
//	}

