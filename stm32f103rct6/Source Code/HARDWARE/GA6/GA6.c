#include "GA6.h"
#include "delay.h"
#include "GA6.h"
#include "string.h"	

extern uint16_t USART_RX_STA;
extern Oled_Data_Show_t *pData_Show;

const static uint8_t *gSearchSim = "AT+CPIN?";
const static uint8_t *gOK = "OK";

void GA6_Init()
{
	while(GA6_Send_Cmd((uint8_t *)gSearchSim, (uint8_t *)gOK, GY6_SENDTIME));	//查询SIM卡是否存在
}
/*****************************
 * 发送的短信格式如下
 * HeartRate:85
 * Spo2:95.22
 * BodyTemperature:36.4
 * Numerical abnormality!!!
 * Attention!
 * 		From XiaoKang
 *****************************/
void GA6_SendMessage(Oled_Data_Show_t data)
{
	uint8_t sHeartRate[15], sSpo2[12], sTemp[23];
	uint8_t sValue[60];
	uint8_t sSendMessageBuf[100];
	sprintf(sHeartRate, "HeartRate:%s", data.HeartRate);
	sprintf(sSpo2, "Spo2:%s", data.Spo2);
	sprintf(sTemp, "BodyTemperature:%s", data.BodyTemp);
	sprintf(sValue, "%s\r\n%s\r\n%s\r\n", sHeartRate, sSpo2, sTemp);
	sprintf(sSendMessageBuf, "%sNumerical abnormality!!!\r\nAttention!\r\n		From XiaoKang", sValue);
	printf(sSendMessageBuf);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
		USART_SendData(USART1, GY6_SENDEND);
}


/**********************************************************************
* 函数名称：GA6_Send_Cmd
* 功能描述：向GA6发送命令
* 输入参数：cmd(发送的命令字符串)、ack(期待应答结果,如果为空,则表示不需要等待应答)
			waittime(等待时间 单位:1ms)
* 返回值：0(发送成功，得到期待的应答结果),1(发送失败)
* 补充说明：无
**********************************************************************/
uint8_t GA6_Send_Cmd(uint8_t *cmd,uint8_t *ack,u16 waittime)
{
	uint8_t res=0; 
	USART_RX_STA=0;
	printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(1);
			if(USART_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(GA6_Check_Cmd(ack))
				{
					break;			//得到有效数据 
				}
				USART_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
/**********************************************************************
* 函数名称：GA6_Check_Cmd
* 功能描述：GA6发送命令后,检测接收到的应答
* 输入参数：str(期待的应答结果)
* 返回值：0(没有得到期待的应答结果),其他(期待应答结果str的位置)
* 补充说明：无
**********************************************************************/

uint8_t* GA6_Check_Cmd(uint8_t *str)
{
	char *strx=0;
	Uart_Data_t *spUart_Data = GetUart_Data_t(Uart1_NUM);
	if(USART_RX_STA&0X8000)										//接收到一次数据了
	{ 
		spUart_Data->RxBuf[USART_RX_STA&0X7FFF]=0;						//添加结束符
		strx=strstr((const char*)spUart_Data->RxBuf,(const char*)str);
	} 
	return (u8*)strx;
}

