#ifndef __RUN_H__
#define __RUN_H__
#include "sys.h"

void System_Init(void);
void System_Running(void);

void Task_ReceiveHandle(void);
void Task_UARTHandle(void);
void Task_BodyHandle(void);
void Task_KEYHandle(void);
void Task_RC522Handle(void);
void Task_DHT22Handle(void);
void Task_MQ2Handle(void);

#endif


