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
#include "dht22.h"
#include "ov7725.h"

#define ESP8266_STACK_LENGTH			1024
#define ESP8266_CMD_LEN                 6
#define ESP8266_QUEUE_LEN               USART3_MAX_RECV_LEN

#define SEND_STACK_LENGTH			    128

TaskHandle_t ESP8266_Handle;
QueueHandle_t ESP8266_Queue_Handle;
EventGroupHandle_t ESP8266_EventGroup_Handle;

TaskHandle_t SEND_Handle;

const char *pESP8266_Cmd[ESP8266_CMD_LEN] = 
{
	"AT+CWMODE=1\r\n",
	"AT+CWJAP=\"Facelesszzz\",\"05720041018\"\r\n",
	"AT+CIPMUX=0\r\n",
	"AT+CIPSTART=\"TCP\",\"172.20.10.11\",8900\r\n",
	"AT+CIPMODE=1\r\n",
	"AT+CIPSEND\r\n"
};
uint8_t lNum;
void Send_Task(void *pvParameters)
{
    uint8_t lY;
    EventBits_t lBitState;
    DHT22_t *pDht22Send = (DHT22_t *)pvPortMalloc(sizeof(DHT22_t));;
    vTaskDelay(3000);
    while(1)
    {
        lBitState= xEventGroupGetBits(ESP8266_EventGroup_Handle);
        /* 已连接WIFI,进行TCP的连接 */
        if((lBitState&ESP8266_GOT_IP_BIT) != 0){
            lNum = 2;
            xEventGroupClearBits(ESP8266_EventGroup_Handle, ESP8266_GOT_IP_BIT);
        }
        /* 配置ESP8266指令并显示在OLED上 */
        if(lNum < ESP8266_CMD_LEN){
            u3_printf("%s", pESP8266_Cmd[lNum]);
            lY = lNum >= 4 ? (lNum-4)*2 : lNum*2;
            OLED_ShowString(0, lY, (u8 *)pESP8266_Cmd[lNum], 16);
            vTaskDelay(1000);
        }else if(lNum == ESP8266_CMD_LEN){
            /* 配置完毕,进行主界面显示 */
            lNum++;
            if(ESP8266_EventGroup_Handle != NULL){
                xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_CONNECT_BIT);
            }
            vTaskDelay(1000);
        }else{
            /* 发送数据给app */
            if((lBitState&ESP8266_LOOK_BIT) != 0){
//                u3_printf("LookSuccess");
                //xEventGroupClearBits(ESP8266_EventGroup_Handle, ESP8266_LOOK_BIT);
                /* 摄像头数据太长，需要延时后再发送 */
//                vTaskDelay(500);
                OV7725_camera_refresh();
//				vTaskDelay(500);
//				u3_printf("LookOver");
//				vTaskDelay(500);
            }
            else{
                pDht22Send = Get_Dht22Value();
                u3_printf("Tem:%s,RH:%s",pDht22Send->Tem_Str, pDht22Send->RH_Str);
            }
            vTaskDelay(500);
        }
    
    }
}

void ESP8266_Task(void *pvParameters)
{
    char lBuf[USART3_MAX_RECV_LEN];
    while(1)
    {
        /* 接收app数据 */
        if(ESP8266_Queue_Handle != NULL){
			memset(lBuf, 0x00, USART3_MAX_RECV_LEN);
            if(xQueueReceive(ESP8266_Queue_Handle, lBuf, portMAX_DELAY)){

                if(strstr(lBuf, "GOT IP") && lNum == 0){
                    if(ESP8266_EventGroup_Handle != NULL)
                        xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_GOT_IP_BIT);
                }
				else if(lNum < ESP8266_CMD_LEN){
					if(strstr(lBuf, "OK")){
                    	lNum++;
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
    xTaskCreate((TaskFunction_t) Send_Task,
				(const char * ) "Send_Task",
				SEND_STACK_LENGTH,
				(void *) NULL,
				(UBaseType_t) SEND_PRIORITY,
				(TaskHandle_t *) &SEND_Handle);
}
