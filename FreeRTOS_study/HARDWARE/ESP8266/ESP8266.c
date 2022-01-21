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

#define ESP8266_STACK_LENGTH			128
#define ESP8266_CMD_LEN                 6
#define ESP8266_QUEUE_LEN               USART3_MAX_RECV_LEN

TaskHandle_t ESP8266_Handle;
QueueHandle_t ESP8266_Queue_Handle;
EventGroupHandle_t ESP8266_EventGroup_Handle;

const char *pESP8266_Cmd[ESP8266_CMD_LEN] = 
{
	"AT+CWMODE=1\r\n",
	"AT+CWJAP=\"Faceless\",\"05720041018\"\r\n",
	"AT+CIPMUX=0\r\n",
	"AT+CIPSTART=\"TCP\",\"192.168.31.33\",8900\r\n",
	"AT+CIPMODE=1\r\n",
	"AT+CIPSEND\r\n"
};

void ESP8266_Task(void *pvParameters)
{
    uint8_t lNum,lY;
    char lBuf[USART3_MAX_RECV_LEN];
    uint32_t lBlockTime;
    while(1)
    {
        if(lNum < ESP8266_CMD_LEN){
            u3_printf("%s", pESP8266_Cmd[lNum]);
            lY = lNum >= 4 ? (lNum-4)*2 : lNum*2;
            OLED_ShowString(0, lY, (u8 *)pESP8266_Cmd[lNum], 16);
            lBlockTime = 1000/configTICK_RATE_HZ;
        }else if(lNum == ESP8266_CMD_LEN){
            lNum++;
            lBlockTime = portMAX_DELAY;
            if(ESP8266_EventGroup_Handle != NULL){
                xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_CONNECT_BIT);
            }
            delay_ms(2000);
            OLED_Clear();
        }else
            lBlockTime = portMAX_DELAY;

        
        if(ESP8266_Queue_Handle != NULL){
            if(xQueueReceive(ESP8266_Queue_Handle, lBuf, lBlockTime)){
                if(strstr(lBuf, "OK") && lNum < ESP8266_CMD_LEN){
                    lNum++;
                }
            }
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
