#include "delay.h"
#include "esp8266.h" 
#include "string.h"	
#include "stdio.h"
#include "glob.h"  
#include "beep.h"
#include "oled.h"
#include "bsp_uart.h"

/**********************************************************************
* 函数名称：ESP8266_Init
* 功能描述：配置ESP8266的AT指令
* 输入参数：无
* 返回值：无
* 补充说明：无
**********************************************************************/
void ESP8266_Init(void)
{
//	usart3_init(115200);
	pESP8266->OK = (u8*)"OK";
	pESP8266->WokeMode = (u8*)"AT+CWMODE=1";
	pESP8266->HopSpot = (u8*)"AT+CWJAP=\"Faceless\",\"lmc05720041018\"";
	pESP8266->ConnectMode = (u8*)"AT+CIPMUX=0";
	pESP8266->ConnectTCP = (u8*)"AT+CIPSTART=\"TCP\",\"192.168.31.160\",8900";
	pESP8266->CipMode = (u8*)"AT+CIPMODE=1";
	pESP8266->CipSend = (u8*)"AT+CIPSEND";
}
/**********************************************************************
* 函数名称：WIFI_Connect
* 功能描述：ESP8266连接上位机服务器
* 输入参数：无
* 返回值：无
* 补充说明：无
**********************************************************************/
void WIFI_Connect(void)
{
	ESP8266_Init();
	while(ESP8266_Send_Cmd(pESP8266->WokeMode,pESP8266->OK,CONNECT_10MS));
	while(ESP8266_Send_Cmd(pESP8266->HopSpot,pESP8266->OK,CONNECT_10MS));
	while(ESP8266_Send_Cmd(pESP8266->ConnectMode,pESP8266->OK,CONNECT_10MS));
	while(ESP8266_Send_Cmd(pESP8266->ConnectTCP,pESP8266->OK,CONNECT_10MS));
	while(ESP8266_Send_Cmd(pESP8266->CipMode,pESP8266->OK,CONNECT_10MS));
	while(ESP8266_Send_Cmd(pESP8266->CipSend,pESP8266->OK,CONNECT_10MS));
	USART3_RX_STA=0;
	//ESP8266_SendToApp();
//	USART3_RX_STA|=0x8000;
}
/**********************************************************************
* 函数名称：ESP8266_Send_Cmd
* 功能描述：向ESP8266发送命令
* 输入参数：cmd(发送的命令字符串)、ack(期待应答结果,如果为空,则表示不需要等待应答)
			waittime(等待时间 单位:10ms)
* 返回值：0(发送成功，得到期待的应答结果),1(发送失败)
* 补充说明：无
**********************************************************************/
u8 ESP8266_Send_Cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	//printf("%s\r\n",cmd);	//发送命令
	u3_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				//printf("%s\r\n",USART3_RX_BUF);
				if(ESP8266_Check_Cmd(ack))
				{
//					DiDa(100,1);
					//printf("ack:%s\r\n",ack);
					break;//得到有效数据 
				}
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
/**********************************************************************
* 函数名称：ESP8266_Check_Cmd
* 功能描述：ESP8266发送命令后,检测接收到的应答
* 输入参数：str(期待的应答结果)
* 返回值：0(没有得到期待的应答结果),其他(期待应答结果str的位置)
* 补充说明：无
**********************************************************************/
u8* ESP8266_Check_Cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)										//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;						//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
		//printf("%sr\n",strx);
	} 
	return (u8*)strx;
}
/**********************************************************************
* 函数名称：ESP8266_SendToApp
* 功能描述：通过ESP8266将数据发送给APP
* 输入参数：无
* 返回值：无
* 补充说明：无
**********************************************************************/
void ESP8266_SendToApp()
{
	u8 i,res = 0;
	gSend[0] = 0xDE;
	gSend[1] = 0x97;
	memcpy(&gSend[2], pMQ2, 2);
	memcpy(&gSend[4], pDHT22, 4);
	gSend[8] = gState.ALL;
	for(i = 0; i < SEND_DATA_THENGTH - 1; i++)
	{
		res += gSend[i];
	}
	gSend[i] = res;
	for(i = 0; i < SEND_DATA_THENGTH; i++)							//循环发送数据
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕	 
			USART_SendData(USART3,gSend[i]);
	}
} 
/**********************************************************************
* 函数名称：ESP8266_RecToAPP
* 功能描述：通过ESP8266收到上位机发送的数据
* 输入参数：无
* 返回值：无
* 补充说明：无
**********************************************************************/
void ESP8266_RecToAPP()
{
	u8 i;
	if(USART3_RX_STA&0X8000)
	{
		//接收到防盗保护和关闭指令
		if(ESP8266_Check_Cmd("protect"))
		{
			BODY_Flag = 1;	
		}
		else if(ESP8266_Check_Cmd("close"))
		{
			BODY_Flag = 0;
		}
		//接收到数据帧
		for(i = 0; i < RECEIVE_DATA_THENGTH - 1; i++)
		{
			if(USART3_RX_BUF[i] == gRecDataFormat[i])
			{
				continue;
			}
			else
			{
				break;
			}
		}
		//将接收到的数据进行状态更新
		if(i == 4)
		{
			i = 0;
			memcpy(pReceived,USART3_RX_BUF,RECEIVE_DATA_THENGTH);
			gState.ALL = (pReceived->Data[0].ALL - '0') * 10 + pReceived->Data[1].ALL - '0';
		}
		USART3_RX_STA = 0;
	}
}


















