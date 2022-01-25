#include "ESP8266.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "Datatype.h"
#include "oled.h"
#include <string.h>
#include "event_groups.h"
#include "delay.h"

#define ESP8266_STACK_LENGTH			1024
#define ESP8266_CMD_LEN                 6
#define ESP8266_QUEUE_LEN               USART3_MAX_RECV_LEN

TaskHandle_t ESP8266_Handle;
QueueHandle_t ESP8266_Queue_Handle;
EventGroupHandle_t ESP8266_EventGroup_Handle;

const char *pESP8266_Cmd[ESP8266_CMD_LEN] = 
{
	"AT+CWMODE=1\r\n",
	"AT+CWJAP=\"Facelesszzz\",\"05720041018\"\r\n",
	"AT+CIPMUX=0\r\n",
	"AT+CIPSTART=\"TCP\",\"172.20.10.2\",8900\r\n",
	"AT+CIPMODE=1\r\n",
	"AT+CIPSEND\r\n"
};

void ESP8266_Task(void *pvParameters)
{
    uint8_t lNum = 0,lY;
    uint8_t lBuf[USART3_MAX_RECV_LEN];
    uint32_t lBlockTime;
	uint8_t lSendBusy = 0;
    while(1)
    {
        if(lNum < ESP8266_CMD_LEN && lSendBusy == 0){
            u3_printf("%s", pESP8266_Cmd[lNum]);
            lY = lNum >= 4 ? (lNum-4)*2 : lNum*2;
            OLED_ShowString(0, lY, (u8 *)pESP8266_Cmd[lNum], 16);
            lBlockTime = 1000/portTICK_PERIOD_MS;
        }else if(lNum == ESP8266_CMD_LEN){
            lNum++;
            lBlockTime = portMAX_DELAY;
            if(ESP8266_EventGroup_Handle != NULL){
                xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_CONNECT_BIT);
            }
            vTaskDelay(2000);
        }else
            lBlockTime = portMAX_DELAY;
        /* 接收app数据 */
        if(ESP8266_Queue_Handle != NULL){
			memset(lBuf, 0x00, USART3_MAX_RECV_LEN);
            if(xQueueReceive(ESP8266_Queue_Handle, lBuf, lBlockTime)){
				if(lNum < ESP8266_CMD_LEN){
					if(strstr(lBuf, "OK")){
                    	lNum++;
						lSendBusy = 0;
	                }else if(strstr(lBuf, "busy")){
	                	lSendBusy++;
	                }else if(strstr(lBuf, "FAIL")){
						lSendBusy = 0;
					}
				}
				else{
	                if(strstr(lBuf, "Look")){
	                    /* 开启拍照模式 */
	                    if(ESP8266_EventGroup_Handle != NULL)
	                        xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_LOOK_BIT);
	                }
				}
            }
        }
		if(lSendBusy > 0){
			lSendBusy++;
			if(lSendBusy >= 5)
				lSendBusy = 0;
			vTaskDelay(1000);
		}
    }
}


void ESP8266_Task_Init(void)
{
    ESP8266_Queue_Handle = xQueueCreate(1, ESP8266_QUEUE_LEN);
    ESP8266_EventGroup_Handle = xEventGroupCreate();
    xTaskCreate((TaskFunction_t) ESP8266_Task,
				(const char * ) "ESP8266_Task",
				ESP8266_STACK_LENGTH,
				(void *) NULL,
				(UBaseType_t) ESP8266_PRIORITY,
				(TaskHandle_t *) &ESP8266_Handle);
}
