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

#define ESP8266_STACK_LENGTH			1024
#define ESP8266_CMD_LEN                 6
#define ESP8266_QUEUE_LEN               USART3_MAX_RECV_LEN

#define SEND_STACK_LENGTH			    128

TaskHandle_t ESP8266_Handle;
QueueHandle_t ESP8266_Queue_Handle;
EventGroupHandle_t ESP8266_EventGroup_Handle;

TaskHandle_t Send_Handle;

const char *pESP8266_Cmd[ESP8266_CMD_LEN] = 
{
	"AT+CWMODE=1\r\n",                                          //设置客户端模式
	"AT+CWJAP=\"Facelesszzz\",\"05720041018\"\r\n",             //连接手机热点
	"AT+CIPMUX=0\r\n",                                          //设置单路连接模式
	"AT+CIPSTART=\"TCP\",\"172.20.10.11\",8900\r\n",            //根据启动app后的底部弹窗设定，端口默认8900
	"AT+CIPMODE=1\r\n",                                         //设置透传模式
	"AT+CIPSEND\r\n"                                            //启动数据收发
};
/**********************************************************************
* 函数名称：Send_Task
* 功能描述：ESP8266发送数据任务
* 输入参数：无
* 返回值：无
* 补充说明：无
**********************************************************************/
uint8_t lNum;
void Send_Task(void *pvParameters)
{
    uint8_t lY;
    EventBits_t lBitState;
    DHT22_t *pDht22Send = (DHT22_t *)pvPortMalloc(sizeof(DHT22_t));
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
            pDht22Send = Get_Dht22Value();
            u3_printf("Tem:%s,RH:%s",pDht22Send->Tem_Str, pDht22Send->RH_Str);
            vTaskDelay(500);            //发送间隔时间，可适当调整
        }
    
    }
}
/**********************************************************************
* 函数名称：ESP8266_Task
* 功能描述：ESP8266主要运行任务
* 输入参数：无
* 返回值：无
* 补充说明：处理由串口中断实际接收的数据
**********************************************************************/
void ESP8266_Task(void *pvParameters)
{
    char lBuf[USART3_MAX_RECV_LEN];
    while(1)
    {
        /* 接收app数据 */
        if(ESP8266_Queue_Handle != NULL){
			memset(lBuf, 0x00, USART3_MAX_RECV_LEN);
            /* 接收ESP8266队列，处于阻塞态，只有数据有效才会进来处理 */
            if(xQueueReceive(ESP8266_Queue_Handle, lBuf, portMAX_DELAY)){
                /* 若接收到GOT IP，则表明已经连上指定热点 */
                if(strstr(lBuf, "GOT IP") && lNum == 0){
                    if(ESP8266_EventGroup_Handle != NULL)
                        xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_GOT_IP_BIT);
                }
                /* 若接收到OK，则进行下条命令的发送 */
				else if(lNum < ESP8266_CMD_LEN){
					if(strstr(lBuf, "OK")){
                    	lNum++;
	                }
				}
                /* 已经连接上TCP服务器，进行数据的接收处理 */
				else{
                    if(strstr(lBuf, "first")){
                    	if(ESP8266_EventGroup_Handle != NULL)
                            xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_FIRST_BIT);
	                }else if(strstr(lBuf, "second")){
	                    if(ESP8266_EventGroup_Handle != NULL)
                            xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_SECOND_BIT);
	                }else if(strstr(lBuf, "third")){
	                    if(ESP8266_EventGroup_Handle != NULL)
                            xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_THIRD_BIT);
	                }else if(strstr(lBuf, "fourth")){
	                    if(ESP8266_EventGroup_Handle != NULL)
                            xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_FOURTH_BIT);
	                }else if(strstr(lBuf, "fifth")){
	                    if(ESP8266_EventGroup_Handle != NULL)
                            xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_FIFTH_BIT);
	                }else if(strstr(lBuf, "sixth")){
	                    if(ESP8266_EventGroup_Handle != NULL)
                            xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_SILENCE_BIT);
	                }else if(strstr(lBuf, "mode")){
	                    if(ESP8266_EventGroup_Handle != NULL)
                            xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_SILENCE_BIT);
	                }else if(strstr(lBuf, "beep")){
	                    if(ESP8266_EventGroup_Handle != NULL)
                            xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_BEEP_BIT);
	                }else if(strstr(lBuf, "music")){
	                    if(ESP8266_EventGroup_Handle != NULL)
                            xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_MUSIC_BIT);
	                }     
				}
            }
        }
    }
}

/**********************************************************************
* 函数名称：ESP8266_Task_Init
* 功能描述：ESP8266初始化任务
* 输入参数：无
* 返回值：无
* 补充说明：创建ESP8266相关队列、事件组、发送和运行任务
**********************************************************************/
void ESP8266_Task_Init(void)
{
    ESP8266_Queue_Handle = xQueueCreate(1, ESP8266_QUEUE_LEN);          //ESP8266队列创建，用于接收数据
    ESP8266_EventGroup_Handle = xEventGroupCreate();                    //ESP8266事件标志组创建，用于任务间通信
    xTaskCreate((TaskFunction_t) ESP8266_Task,                          //ESP8266主任务运行创建
				(const char * ) "ESP8266_Task",
				ESP8266_STACK_LENGTH,
				(void *) NULL,
				(UBaseType_t) ESP8266_PRIORITY,
				(TaskHandle_t *) &ESP8266_Handle);
    xTaskCreate((TaskFunction_t) Send_Task,                             //ESP8266发送任务创建
				(const char * ) "Send_Task",
				SEND_STACK_LENGTH,
				(void *) NULL,
				(UBaseType_t) SEND_PRIORITY,
				(TaskHandle_t *) &Send_Handle);
}
