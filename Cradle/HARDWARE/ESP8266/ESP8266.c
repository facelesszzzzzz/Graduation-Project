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
	"AT+CWMODE=1\r\n",                                          //���ÿͻ���ģʽ
	"AT+CWJAP=\"Facelesszzz\",\"05720041018\"\r\n",             //�����ֻ��ȵ�
	"AT+CIPMUX=0\r\n",                                          //���õ�·����ģʽ
	"AT+CIPSTART=\"TCP\",\"172.20.10.11\",8900\r\n",            //��������app��ĵײ������趨���˿�Ĭ��8900
	"AT+CIPMODE=1\r\n",                                         //����͸��ģʽ
	"AT+CIPSEND\r\n"                                            //���������շ�
};
/**********************************************************************
* �������ƣ�Send_Task
* ����������ESP8266������������
* �����������
* ����ֵ����
* ����˵������
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
        /* ������WIFI,����TCP������ */
        if((lBitState&ESP8266_GOT_IP_BIT) != 0){
            lNum = 2;
            xEventGroupClearBits(ESP8266_EventGroup_Handle, ESP8266_GOT_IP_BIT);
        }
        /* ����ESP8266ָ���ʾ��OLED�� */
        if(lNum < ESP8266_CMD_LEN){
            u3_printf("%s", pESP8266_Cmd[lNum]);
            lY = lNum >= 4 ? (lNum-4)*2 : lNum*2;
            OLED_ShowString(0, lY, (u8 *)pESP8266_Cmd[lNum], 16);
            vTaskDelay(1000);
        }else if(lNum == ESP8266_CMD_LEN){
            /* �������,������������ʾ */
            lNum++;
            if(ESP8266_EventGroup_Handle != NULL){
                xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_CONNECT_BIT);
            }
            vTaskDelay(1000);
        }else{
            /* �������ݸ�app */
            pDht22Send = Get_Dht22Value();
            u3_printf("Tem:%s,RH:%s",pDht22Send->Tem_Str, pDht22Send->RH_Str);
            vTaskDelay(500);            //���ͼ��ʱ�䣬���ʵ�����
        }
    
    }
}
/**********************************************************************
* �������ƣ�ESP8266_Task
* ����������ESP8266��Ҫ��������
* �����������
* ����ֵ����
* ����˵���������ɴ����ж�ʵ�ʽ��յ�����
**********************************************************************/
void ESP8266_Task(void *pvParameters)
{
    char lBuf[USART3_MAX_RECV_LEN];
    while(1)
    {
        /* ����app���� */
        if(ESP8266_Queue_Handle != NULL){
			memset(lBuf, 0x00, USART3_MAX_RECV_LEN);
            /* ����ESP8266���У���������̬��ֻ��������Ч�Ż�������� */
            if(xQueueReceive(ESP8266_Queue_Handle, lBuf, portMAX_DELAY)){
                /* �����յ�GOT IP��������Ѿ�����ָ���ȵ� */
                if(strstr(lBuf, "GOT IP") && lNum == 0){
                    if(ESP8266_EventGroup_Handle != NULL)
                        xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_GOT_IP_BIT);
                }
                /* �����յ�OK���������������ķ��� */
				else if(lNum < ESP8266_CMD_LEN){
					if(strstr(lBuf, "OK")){
                    	lNum++;
	                }
				}
                /* �Ѿ�������TCP���������������ݵĽ��մ��� */
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
* �������ƣ�ESP8266_Task_Init
* ����������ESP8266��ʼ������
* �����������
* ����ֵ����
* ����˵��������ESP8266��ض��С��¼��顢���ͺ���������
**********************************************************************/
void ESP8266_Task_Init(void)
{
    ESP8266_Queue_Handle = xQueueCreate(1, ESP8266_QUEUE_LEN);          //ESP8266���д��������ڽ�������
    ESP8266_EventGroup_Handle = xEventGroupCreate();                    //ESP8266�¼���־�鴴�������������ͨ��
    xTaskCreate((TaskFunction_t) ESP8266_Task,                          //ESP8266���������д���
				(const char * ) "ESP8266_Task",
				ESP8266_STACK_LENGTH,
				(void *) NULL,
				(UBaseType_t) ESP8266_PRIORITY,
				(TaskHandle_t *) &ESP8266_Handle);
    xTaskCreate((TaskFunction_t) Send_Task,                             //ESP8266�������񴴽�
				(const char * ) "Send_Task",
				SEND_STACK_LENGTH,
				(void *) NULL,
				(UBaseType_t) SEND_PRIORITY,
				(TaskHandle_t *) &Send_Handle);
}
