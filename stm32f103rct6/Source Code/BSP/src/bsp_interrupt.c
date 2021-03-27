#include "bsp_interrupt.h"

static TimeTypeDef_t gEvTime;
static TimeTypeDef_t *pEvTime = &gEvTime;

uint16_t gTimer[MAX_TIMER];

void SysTick_Handler()
{
	uint8_t i;
	for(i = 0; i < MAX_TIMER; i++)
	{
		if(gTimer[i]) gTimer[i]--;
	}
#if 0
	static uint8_t sTime1MS, sTime100MS;
	sTime1MS++;
	if(sTime1MS%25 == 5)
	{
		pEvTime->Ev25MsFlag = 1;   		// 25Ms事件
	}
	if(sTime1MS == 20 || sTime1MS == 70)
	{
		pEvTime->Ev50MsFlag = 1;		// 50Ms事件
	}  
	if(sTime1MS == 40)
	{
		pEvTime->Ev100MsFlag = 1;		// 100Ms事件
	}
	if(sTime1MS >= 100)
	{
		sTime1MS = 0;
		sTime100MS++;
		if(sTime100MS == 2 || sTime100MS == 7)
			pEvTime->Ev500MsFlag = 1;	// 500Ms事件
		if(sTime100MS == 5)
			pEvTime->Ev1SFlag = 1;		// 1S事件
		if(sTime100MS >= 10)
		{
			sTime100MS = 0;
		}
	}
#endif
}


#if 0

static uint8_t gWriteEventCount, gReadEventCount;
static uint8_t gEventBuffer[EVENT_BUFFER_LENGTH];

void 	PushEvent(uint8_t event)
{	
	uint8_t temp;
	
	if(gWriteEventCount >= EVENT_BUFFER_LENGTH)
	{
		gWriteEventCount = 0;
	}
	for(temp = 0; temp <= gWriteEventCount; temp++)
	{
		if(event == gEventBuffer[temp])
		{
			return;
		}
	}
	gEventBuffer[gWriteEventCount] = event;
	gWriteEventCount++;	
}

uint8_t	PopEvent(void)
{	
	uint8_t event;
	if(gReadEventCount >= EVENT_BUFFER_LENGTH)
	{
		gReadEventCount = 0;
		event = gEventBuffer[gReadEventCount];
	}else
	{	
		if(gReadEventCount >= gWriteEventCount)
		{	
			gReadEventCount = 0;
			gWriteEventCount = 0;
			event = 0xff;
		}else
		{
			event = gEventBuffer[gReadEventCount];
			gEventBuffer[gReadEventCount] = 0xff;
			gReadEventCount++;
		}
	}
	return (event);
}

void    EventCollect(void) 
{	
//	if(EvSysResetFlag)
//	{
//		EvSysResetFlag = 0;
//		PushEvent(evSysRest);
//	}
//	if(EvReceiveControlFlag)
//	{
//		EvReceiveControlFlag = 0;
//		PushEvent(evReceiveControl);
//	}
//	if(EvReceiveCoreFlag)
//	{
//		EvReceiveCoreFlag = 0;
//		PushEvent(evReceiveCore);
//	}
	if(pEvTime->Ev25MsFlag)
	{
		pEvTime->Ev25MsFlag = 0;
		PushEvent(evTime25MS);
	}
	if(pEvTime->Ev50MsFlag)
	{
		pEvTime->Ev50MsFlag = 0;
		PushEvent(evTime50MS);
	}
	if(pEvTime->Ev100MsFlag)
	{
		pEvTime->Ev100MsFlag = 0;
		PushEvent(evTime100MS);
	}
	if(pEvTime->Ev500MsFlag)
	{
		pEvTime->Ev500MsFlag = 0;
		PushEvent(evTime500MS);
	}
	if(pEvTime->Ev1SFlag)
	{
		pEvTime->Ev1SFlag = 0;
		PushEvent(evTime1S);
	}
	
}

void SystemReset()
{
	
}

void Do25MsEvent()
{
	
}

void Do50MsEvent()
{
	
}

void Do100MsEvent()
{
	
}

void Do500MsEvent()
{
	
}

void DoSecMsEvent()
{
	
}

void System_Running()
{
	EventCollect();	//事件收集
	
	//事件处理
	switch(PopEvent())
	{
		case evSysReset:
			SystemReset();		break;
//		case evReceiveControl:
//			ReceiveDataFromControl();	break;
		case evTime25MS:
			Do25MsEvent();		break;
		case evTime50MS:
			Do50MsEvent();		break;
		case evTime100MS:
			Do100MsEvent();		break;
		case evTime500MS:
			Do500MsEvent();		break;
		case evTime1S:
			DoSecMsEvent();		break;
	}
}
#endif
