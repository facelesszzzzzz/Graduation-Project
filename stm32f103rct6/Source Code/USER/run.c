#include "run.h"
#include "bsp.h"
#include <stdio.h>
#include "max30102.h"
#include "oled.h"
#include "GY615.h"
#include "GA6.h"
#include <string.h>


void System_Init()
{
	MAX30102_Init();			//MAX30102初始化			
	OLED_Init();				//显示屏初始化
	GA6_Init();					//GA6模块初始化
	OLED_MAIN();				//OLED显示
}
/***************
 * gHandleState
 * b0:心率偏低
 * b1:心率偏高
 * b2:血氧偏低
 * b3:血氧偏高
 * b4:体温偏高
 **************/
Byte8 gHandleState;
extern Oled_Data_Show_t *pData_Show;
void State1sTimeHandle()
{
	static Oled_Data_Show_t sData;
	static uint8_t sStateTime;					
	static uint8_t sStartSend, sStartSendTime;
	static uint8_t sTimeInterval, sStateTimeInterval;//时间间隔和状态间隔
	if(gHandleState.ALL > 0 && !sStateTimeInterval){
		sStateTime++;
		if(sStateTime >= 30){			//超过30s就进行短信报警
			sStateTime = 0;
			GA6_SendMessageCmd;
			memcpy(&sData, pData_Show, sizeof(Oled_Data_Show_t));
			sStartSend = 1;
		}
	}
	else{
		sStateTime = 0;
	}
	if(sStartSend){
		sStartSendTime++;
		if(sStartSendTime >= 5){		//5s之后发送短信内容
			sStartSend = 0;
			sStateTimeInterval = 1;
			//发送数据处理
			GA6_SendMessage(sData);
		}
	}
	if(sStateTimeInterval){				//发送过一次之后要间隔1分钟才能进行下次发送
		sTimeInterval++;
		if(sTimeInterval >= 60){
			sStateTimeInterval = 0;		//允许短信发送
			sTimeInterval = 0;
		}
	}
}
/**********************************************************************
* 函数名称：Task_25MsHandle()
* 功能描述：25MS事件处理
* 输入参数：无
* 返回值：无
* 补充说明：无
**********************************************************************/
void Task_25MsHandle()
{
	if(Ev25Ms) return;
	Ev25Ms = SysTick_25MS;
	OLED_SHOWDATA();
	Search_GY615_Data();
}
/**********************************************************************
* 函数名称：Task_50MsHandle()
* 功能描述：50MS事件处理
* 输入参数：无
* 返回值：无
* 补充说明：无
**********************************************************************/
void Task_50MsHandle()
{
	if(Ev50Ms) return;
	Ev50Ms = SysTick_50MS;
	GY615_Handle();
}
/**********************************************************************
* 函数名称：Task_100MsHandle()
* 功能描述：100MS事件处理
* 输入参数：无
* 返回值：无
* 补充说明：无
**********************************************************************/
void Task_100MsHandle()
{
	if(Ev100Ms) return;
	Ev100Ms = SysTick_100MS;
}
/**********************************************************************
* 函数名称：Task_500MsHandle()
* 功能描述：500MS事件处理
* 输入参数：无
* 返回值：无
* 补充说明：无
**********************************************************************/
void Task_500MsHandle()
{
	if(Ev500Ms) return;
	Ev500Ms = SysTick_500MS;
	MAX30102_Handle();
}
/**********************************************************************
* 函数名称：Task_1SHandle()
* 功能描述：1S事件处理
* 输入参数：无
* 返回值：无
* 补充说明：无
**********************************************************************/
void Task_1SHandle()
{
	if(Ev1S) return;
	Ev1S = SysTick_1S;
	State1sTimeHandle();
}
/**********************************************************************
* 函数名称：System_Running()
* 功能描述：后台系统运行
* 输入参数：无
* 返回值：无
* 补充说明：包含20、50、100、500、1000Ms的事件处理
**********************************************************************/
void System_Running()
{
	Task_25MsHandle();
	Task_50MsHandle();
	Task_100MsHandle();
	Task_500MsHandle();
	Task_1SHandle();
}

