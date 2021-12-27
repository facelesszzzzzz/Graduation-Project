#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <string.h>
#include "beep.h"
#include "event_groups.h"
#include "lcd.h"
#include "semphr.h"
#include "esp8266_mqtt.h"
#include "cJSON.h"
#include "malloc.h"

#define TASK_INIT_STACK_LENGTH				50
#define TASK_INIT_PRIORITY					1
TaskHandle_t Task_Init_Handle;

#define MAIN_RUNNING_STACK_LENGTH			256
#define MAIN_RUNNING_PRIORITY				2
TaskHandle_t Main_Running_Handle;

//#define ESP8266_INIT_STACK_LENGTH			128
//#define ESP8266_INIT_PRIORITY				3
//TaskHandle_t ESP8266_Init_Handle;

#define MQTT_TASK_STACK_LENGTH				128
#define MQTT_TASK_PRIORITY					2
TaskHandle_t Mqtt_Task_Handle;

#define ESP8266_REC_STACK_LENGTH			128
#define ESP8266_REC_PRIORITY				4
TaskHandle_t ESP8266_Rec_Handle;
#define ESP8266_SEND_STACK_LENGTH			128
#define ESP8266_SEND_PRIORITY				3
TaskHandle_t ESP8266_Send_Handle;
QueueHandle_t ESP8266_Queue_Handle;
QueueHandle_t Message_Queue_Handle;

#define KEY_TASK_STACK_LENGTH				50
#define KEY_TASK_PRIORITY					5
TaskHandle_t Key_Handle;

EventGroupHandle_t ESP8266_EventGroup_Handle;

TimerHandle_t Timer_Count_Handle;

#define ESP8266_QUEUE_LEN					200
#define ESP8266_CMD_LEN						12
#define ESP8266_CMD_RESET					0x00
#define ESP8266_CMD_SMARTCONFIG				0x06
#define ESP8266_CMD_CWMODE_SET				0x07
#define ESP8266_CMD_TIMEOUT					0xFE
#define ESP8266_CMD_INIT					0xFF

const char * ESP8266_Cmd[] =
{
	"+++",
	"AT+CIPMODE=0\r\n",
	"AT+CIPCLOSE\r\n",
	"AT+CWSTOPSMART\r\n",
	"AT+CWMODE=1\r\n",
//	"AT+CWJAP=\"Faceless\",\"05720041018\"\r\n",
	"AT+CWSTARTSMART\r\n",
	"AT+CWSTOPSMART\r\n",
	"AT+CWMODE=1\r\n",
	"AT+CIPMUX=0\r\n",
//	"AT+CIPSTART=\"TCP\",\"192.168.31.33\",8900\r\n",
	"AT+CIPSTART=\"TCP\",\"A8MD5LL8ZA.iotcloud.tencentdevices.com\",1883\r\n",
	"AT+CIPMODE=1\r\n",
	"AT+CIPSEND\r\n"
};

void SendEventGroup(EventGroupHandle_t mEventGroup, uint8_t mData)
{
	if(mEventGroup != NULL){
		xEventGroupClearBits(mEventGroup, 0xFF);
		xEventGroupSetBits(mEventGroup, mData);
	}
}

void ESP8266_Rec_Task(void *pvParameters)
{
	char lBuff[ESP8266_QUEUE_LEN];
	uint8_t lBit = ESP8266_CMD_RESET;
	printf("ESP8266_Rec_Task\r\n");
	while(1){
//		if(pdTRUE == xSemaphoreTake(Key_Semaphore_Handle, 0)){
//			lBit = 0;
//			SendEventGroup(ESP8266_EventGroup_Handle, lBit);
//			/* Ensure key0 control */
//			vTaskResume(ESP8266_Send_Handle);
//			/* Ensure the normal operation of the ESP8266_Send_Task from the beginning*/
//			vTaskDelay(100);
//		}
		
		if(ESP8266_Queue_Handle != NULL){
			memset(lBuff, 0, ESP8266_QUEUE_LEN);	//清除缓冲区
			if(pdTRUE == xQueueReceive(ESP8266_Queue_Handle, lBuff, portMAX_DELAY)){
				if(xEventGroupGetBits(ESP8266_EventGroup_Handle) == ESP8266_CMD_RESET)
					lBit = ESP8266_CMD_RESET;
				
				if(strstr(lBuff,"WIFI GOT IP") && lBit == ESP8266_CMD_RESET){
					lBit = ESP8266_CMD_CWMODE_SET;
					SendEventGroup(ESP8266_EventGroup_Handle, lBit);
				}
				
				/* Re-distribution the network after receive "ESP8266_Reset" */
				if(strstr(lBuff,"ESP8266_Reset")){
					SendEventGroup(ESP8266_EventGroup_Handle, ESP8266_CMD_RESET);
					vTaskResume(ESP8266_Send_Handle);
//					/* Ensure the normal operation of the ESP8266_Send_Task */
//					vTaskDelay(100);
				}
				/* Resume the ESP8266_Send_Task after receive "smartconfig connected wifi" */
				if(strstr(lBuff,"smartconfig connected wifi")){
//					xTimerStop(Timer_Count_Handle, 0);
					vTaskResume(ESP8266_Send_Handle);
				}
				else if(strstr(lBuff,"OK") && (lBit < ESP8266_CMD_LEN)){
					lBit++;
					SendEventGroup(ESP8266_EventGroup_Handle, lBit);
				}
				else if((strstr(lBuff,"ERROR") && (lBit < 4)) || strstr(lBuff,"+++")){
					lBit++;
					SendEventGroup(ESP8266_EventGroup_Handle, lBit);
				}
				/* Main_Running received data display after the transparent transmission mode was started" */
				if(lBit >= 0){//ESP8266_CMD_LEN || lBit >= 7){
					if(Message_Queue_Handle != NULL)
						xQueueSend(Message_Queue_Handle, lBuff, 0);
				}
			}
		}
	}
}

void ESP8266_Send_Task(void *pvParameters)
{
	uint8_t lSelect = 0, lTaskState = 1;
	printf("ESP8266_Send_Task\r\n");
	while(1){
		lSelect = xEventGroupGetBits(ESP8266_EventGroup_Handle);
		if(lSelect < ESP8266_CMD_LEN){
			if(lSelect == ESP8266_CMD_SMARTCONFIG && lTaskState){
				lTaskState = 0;
				xTimerStart(Timer_Count_Handle, 0);
				vTaskSuspend(ESP8266_Send_Handle);
				lSelect = xEventGroupGetBits(ESP8266_EventGroup_Handle);
			}
			u3_printf("%s", ESP8266_Cmd[lSelect]);
			printf("Send = %s", ESP8266_Cmd[lSelect]);
			//LCD_ShowString(0,150,400,16,16,(u8 *)ESP8266_Cmd[lSelect]);
		}
		if(lSelect == 0){
			lTaskState = 1;
			vTaskDelay(1000);
		}
		else
			vTaskDelay(500);
	}
}

void Main_Running(void *pvParameters)
{
	char lMessageBuff[ESP8266_QUEUE_LEN];
	uint8_t *lPayload, *lVariable;
	char *lMqtt;
	uint8_t i = 0;
	uint8_t lFixedMessageHeadLen;
	uint16_t lVariableMessageLen;
	uint8_t lPayloadLen;
	cJSON *lpControl, *lpMethod, *lpParams, *lpLed;
	printf("Main_Running\r\n");
	while(1){
		if(Message_Queue_Handle != NULL){
			memset(lMessageBuff, 0, ESP8266_QUEUE_LEN);
			if(pdTRUE == xQueueReceive(Message_Queue_Handle, lMessageBuff, portMAX_DELAY)){
				if(xEventGroupGetBits(ESP8266_EventGroup_Handle) == ESP8266_CMD_LEN){
//					printf("Mqtt_Rec:");
					if(lMessageBuff[0] == 0x30){
						if(lMessageBuff[1] & 0x80){
							lFixedMessageHeadLen = 3;
							if(lMessageBuff[2] & 0x80){
								lFixedMessageHeadLen = 4;
								if(lMessageBuff[3] & 0x80)
									lFixedMessageHeadLen = 5;
							}	
						}else
							lFixedMessageHeadLen = 2;
						lVariableMessageLen = ((uint16_t)lMessageBuff[lFixedMessageHeadLen] << 8) +lMessageBuff[lFixedMessageHeadLen+1];
						lVariable = (uint8_t *)pvPortMalloc(lVariableMessageLen);
						memcpy(lVariable, &lMessageBuff[lFixedMessageHeadLen+2], lVariableMessageLen);
						printf("lVariableLen = %d, lVariable = %s\r\n", lVariableMessageLen, lVariable);
						for(lPayloadLen = 0, i = lFixedMessageHeadLen+lVariableMessageLen+2; lMessageBuff[i] != '\0'; i++){
							lPayloadLen++;
						}
						lPayload = (uint8_t *)pvPortMalloc(lPayloadLen);
						memcpy(lPayload, &lMessageBuff[lFixedMessageHeadLen+lVariableMessageLen+2], lPayloadLen);
						printf("lPayloadLen = %d, lPayload = %s\r\n", lPayloadLen, lPayload);
						vPortFree(lVariable);
						vPortFree(lPayload);
//					if(strstr(lPayload, "control")){
//						lpControl = cJSON_Parse(lPayload);
//						lpMethod = cJSON_GetObjectItem(lpControl, "method");
//						lpParams = cJSON_GetObjectItem(lpControl, "params");
//						lpLed = cJSON_GetObjectItem(lpParams, "LedSwitch");
//						LED0 = !lpLed->valueint;
//						lMqtt = cJSON_Print(lpControl);
//						printf("lpLed->valueint = %d\r\n",lpLed->valueint);
//						printf("cJSON = %s\r\n",lMqtt);
//						cJSON_Delete(lpControl);
//						vPortFree(lMqtt);
//					}
//					printf("lFixedMessageHeadCount = %d, lVariableMessageCount = %d\r\n", lFixedMessageHeadCount, lVariableMessageCount);
//					for(i = 0; i < sizeof(lMessageBuff); i++)
//						printf("%x ", lMessageBuff[i]);
					}
				}else
					printf("Received = %s\r\n", lMessageBuff);
				//LCD_ShowString(0, 150+i*16, 200, 16, 16, (u8 *)lMessageBuff);
			}
		}
	}
}
void Key_Task(void *pvParameters)
{
	uint8_t lKeyValue;
	uint16_t lKeyTriggeredCount = 0;
	printf("Key_Task\r\n");	
	while(1){
		lKeyValue = KEY_Scan(1);
		if(ESP8266_Rec_Handle != NULL){
		switch(lKeyValue)
		{
			case KEY0_PRES:
				if(lKeyTriggeredCount%100 == 0)
					printf("KeyTriggeredSec = %d\r\n", lKeyTriggeredCount/100);
				/* Re-distribution the network after 5 sec*/
				if(lKeyTriggeredCount >= 100*5){
					lKeyTriggeredCount = 0;
					vTaskResume(ESP8266_Send_Handle);
					SendEventGroup(ESP8266_EventGroup_Handle, ESP8266_CMD_RESET);
				}else
					lKeyTriggeredCount++;
				break;
			case KEY1_PRES:
				vTaskSuspend(ESP8266_Rec_Handle);
				printf("ESP8266 Rec Task Suspend");
				//LCD_ShowString(0,200,400,16,16,"ESP8266 Rec Task Suspend");
				break;
			case WKUP_PRES:
				vTaskResume(ESP8266_Rec_Handle);
				printf("ESP8266 Rec Task Resume");
				//LCD_ShowString(0,216,400,16,16,"ESP8266 Rec Task Resume");
				break;
			default:
				lKeyTriggeredCount = 0;
				break;
		}
	}
		vTaskDelay(1);
	}
}

void Timer_Count_Task(TimerHandle_t pxTimer)
{
	static uint8_t sConnectedTimeOut, sSmartConfigTimeOut, sSmartConfigCount, sConnectCount;
	uint8_t lGetBits;
	lGetBits = xEventGroupGetBits(ESP8266_EventGroup_Handle);
	if(lGetBits != ESP8266_CMD_LEN)
//		printf("sConnectedTimeOut = %d,sSmartConfigTimeOut = %d, lGetBits = %#x\r\n", sConnectedTimeOut, sSmartConfigTimeOut, lGetBits);
	if(lGetBits == ESP8266_CMD_INIT){
		sConnectedTimeOut++;
		if(sConnectedTimeOut >= 30){
			SendEventGroup(ESP8266_EventGroup_Handle, ESP8266_CMD_RESET);
			sConnectedTimeOut = 0;
		}
	}else if(lGetBits == ESP8266_CMD_SMARTCONFIG){
		sSmartConfigTimeOut++;
		if(sSmartConfigTimeOut >= 60){
			sSmartConfigCount++;
			sSmartConfigTimeOut = 0;
			/* Smartconfig distrbution network mode stop after over 3 timers */
			if(sSmartConfigCount > 3){
				sSmartConfigCount = 0;
				printf("SmartConfig lose\r\n");
				SendEventGroup(ESP8266_EventGroup_Handle, ESP8266_CMD_TIMEOUT);

			}else{
				vTaskResume(ESP8266_Send_Handle);
				SendEventGroup(ESP8266_EventGroup_Handle, ESP8266_CMD_RESET);
			}
		}
	}else if(lGetBits == ESP8266_CMD_RESET){
		sConnectCount++;
		if(sConnectCount >= 10){
			printf("ESP8266 connect failed\r\n");
			SendEventGroup(ESP8266_EventGroup_Handle, ESP8266_CMD_TIMEOUT);
		}
	}else{
		sConnectCount = 0;
		sConnectedTimeOut = 0;
		sSmartConfigTimeOut = 0;
		sSmartConfigCount = 0;
	}
}

//此处是腾讯云服务器的登陆配置
#define MQTT_BROKERADDRESS "A8MD5LL8ZA.iotcloud.tencentdevices.com"
#define MQTT_CLIENTID "A8MD5LL8ZAwater_the_flowers"
#define MQTT_USARNAME "A8MD5LL8ZAwater_the_flowers;12010126;UEMAO;1634017854"
#define MQTT_PASSWD "5b299a52ed1181f14fb04f5df0b3542b9a9471da1bbd8320b1492079612e93d3;hmacsha256"
#define	MQTT_PUBLISH_TOPIC "$thing/up/property/A8MD5LL8ZA/water_the_flowers"
#define MQTT_SUBSCRIBE_TOPIC "$thing/down/property/A8MD5LL8ZA/water_the_flowers"


void Mqtt_Task(void *pvParameters)
{
	uint8_t lTimer = 0, pub_ret;
	char *lMqtt;
//	uint8_t mqtt_message[USART3_MAX_SEND_LEN];

	cJSON * lRoot,* Item;
//	cJSON * lRoot = cJSON_CreateObject();
//	cJSON * Item = cJSON_CreateObject();
	printf("Mqtt_Task running\r\n");
	while(xEventGroupWaitBits(ESP8266_EventGroup_Handle,
						ESP8266_CMD_LEN, 
						pdFALSE, 
						pdFALSE, 
						portMAX_DELAY) != ESP8266_CMD_LEN){
							
	}
	printf("ESP8266阿里云MQTT开始连接\r\n");		
	if(MQTT_Connect(MQTT_CLIENTID, MQTT_USARNAME, MQTT_PASSWD) != 0){
		printf("ESP8266阿里云MQTT登陆成功！\r\n");				
	}else
		printf("ESP8266阿里云MQTT登陆失败！\r\n");

	if(MQTT_SubscribeTopic(MQTT_SUBSCRIBE_TOPIC,0,1) != 0){			
		printf("ESP8266阿里云MQTT订阅主题成功！\r\n");		
	}else{			
		printf("ESP8266阿里云MQTT订阅主题失败！\r\n");
	}
	while(1){
//			printf("Mqtt_Task running\r\n");
			lTimer++;
			if(lTimer > 60)
				lTimer = 0;
//			MQTT_SentHeart();
//			memset(mqtt_message, 0, USART3_MAX_SEND_LEN);
			lRoot = cJSON_CreateObject();
			Item = cJSON_CreateObject();
			if(lRoot && Item){
			cJSON_AddItemToObject(lRoot, "method", cJSON_CreateString("report"));
			cJSON_AddItemToObject(lRoot, "clientToken", cJSON_CreateString("05720041018"));
			cJSON_AddItemToObject(lRoot, "params", Item);
			cJSON_AddItemToObject(Item, "timer", cJSON_CreateNumber(lTimer));
			cJSON_AddItemToObject(lRoot, "version", cJSON_CreateString("V0.05"));
			//组装数据                
//			sprintf(mqtt_message,"{\"method\":\"report\",\"clientToken\":\"05720041018\",\"params\":{\
//			\"timer\":%d},\"version\":\"1.0.0\"}",lTimer);
			lMqtt = cJSON_Print(lRoot);
			printf("%s\r\n", lMqtt);
			//发布数据                
			pub_ret = MQTT_PublishData(MQTT_PUBLISH_TOPIC, (char *)lMqtt, 0);                
			if(pub_ret > 0){                    
				printf("消息发布成功！！！pub_ret=%d\r\n", pub_ret);
			}else{                    
				printf("消息发布成功！！！pub_ret=%d\r\n", pub_ret);
			}
			vPortFree(lMqtt);
			cJSON_Delete(lRoot);
			memcpy(lRoot, 0, sizeof(cJSON));
//			memcpy(Item, 0, sizeof(cJSON));
			}
			printf("Mqtt heap size = %d\r\n", xPortGetFreeHeapSize());
			vTaskDelay(1000);
	}
}

void Task_Init(void *pvParameters)
{
//	printf("Task Running\r\n");
	taskENTER_CRITICAL();
	ESP8266_Queue_Handle = xQueueCreate(1, ESP8266_QUEUE_LEN);
	Message_Queue_Handle = xQueueCreate(1, ESP8266_QUEUE_LEN);
	ESP8266_EventGroup_Handle = xEventGroupCreate();
	SendEventGroup(ESP8266_EventGroup_Handle, 0xFF);
	Timer_Count_Handle = xTimerCreate("TimerCount_Task",
										1000,
										pdTRUE,
										0,
										(TimerCallbackFunction_t)Timer_Count_Task);
	xTimerStart(Timer_Count_Handle, 0);
//	xTaskCreate(ESP8266_Init,
//				"ESP8266_Init",
//				ESP8266_INIT_STACK_LENGTH,
//				NULL,
//				(UBaseType_t)ESP8266_INIT_PRIORITY,
//				(TaskHandle_t *)ESP8266_Init_Handle);
	xTaskCreate(ESP8266_Send_Task,
				"ESP8266_Send_Task",
				ESP8266_SEND_STACK_LENGTH,
				NULL,
				(UBaseType_t)ESP8266_SEND_PRIORITY,
				(TaskHandle_t *)&ESP8266_Send_Handle);
	xTaskCreate(ESP8266_Rec_Task,
				"ESP8266_Rec_Task",
				ESP8266_REC_STACK_LENGTH,
				NULL,
				(UBaseType_t)ESP8266_REC_PRIORITY,
				(TaskHandle_t *)&ESP8266_Rec_Handle);	
	
	xTaskCreate(Mqtt_Task,
				"Mqtt_Task",
				MQTT_TASK_STACK_LENGTH,
				NULL,
				(UBaseType_t)MQTT_TASK_PRIORITY,
				(TaskHandle_t *)&Mqtt_Task_Handle);	
	
	xTaskCreate(Main_Running,
				"Main_Running",
				MAIN_RUNNING_STACK_LENGTH,
				NULL,
				(UBaseType_t)MAIN_RUNNING_PRIORITY,
				(TaskHandle_t *)&Main_Running_Handle);	
	
	xTaskCreate(Key_Task,
				"Key_Task",
				KEY_TASK_STACK_LENGTH,
				NULL,
				(UBaseType_t)KEY_TASK_PRIORITY,
				(TaskHandle_t *)&Key_Handle);	
	vTaskDelete(Task_Init_Handle);
	taskEXIT_CRITICAL();
	
}

int main (void)
{		
// 	u16 t;  
//	u16 len;	
//	u16 times=0;  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置NVIC中断分组4位抢占优先级
	uart_init(115200);	 //串口初始化为115200
	usart3_init(115200);
	delay_init();
 	LED_Init();			     //LED端口初始化
	KEY_Init();          //初始化与按键连接的硬件接口
//	mem_init();            				//初始化内部内存池
//	LCD_Init();
//	POINT_COLOR = RED;
//	LCD_ShowString(0, 0, 200, 16, 16, "Hello Lcd");
	xTaskCreate(Task_Init,
				"Task_Init",
				TASK_INIT_STACK_LENGTH,
				NULL,
				(UBaseType_t)TASK_INIT_PRIORITY,
				(TaskHandle_t *)&Task_Init_Handle);
	vTaskStartScheduler();
 	while(1); 
 }

