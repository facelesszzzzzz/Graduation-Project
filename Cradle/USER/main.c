#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "semphr.h"
#include "malloc.h"

#include "Datatype.h"
#include "key.h"
#include "ESP8266.h"
#include "oled.h"
#include "dht22.h"
#include "voice.h"
#include "beep.h"

#define DEFAULT_MODE                        0
#define CARE_MODE                           1

#define TASK_INIT_STACK_LENGTH				50
#define MAIN_RUNNING_STACK_LENGTH			128
TaskHandle_t Task_Init_Handle;
TaskHandle_t MainRunning_Handle;

extern EventGroupHandle_t ESP8266_EventGroup_Handle;

#define Voice_Play(x)           VOICE_Select(x);\
				                vTaskDelay(1000);\
//				                VOICE_Select(Voice_none);      

/**********************************************************************
* �������ƣ�MainRunning_Task
* ������������Ҫ������������
* ���������void *pvParameters
* ����ֵ����
* ����˵����������������Ҫ�Ĳ��֣����������while(1)
**********************************************************************/
void MainRunning_Task(void *pvParameters)
{
    uint8_t lCurrentMode = DEFAULT_MODE;
    uint8_t *pModeStr[] = {"Mode:Default","Mode:Care   "};
    uint8_t lSoundCount = 0;
    uint8_t lSoundState = VOICE_DISABLE;
	uint8_t lSoundSelect = 0;
    uint8_t lRecKey = 0;
    uint8_t lKeyAnswer, lKeyValue;
    uint8_t lVoicePlay[] = {
                            ESP8266_SILENCE_BIT,            //����
                            ESP8266_FIRST_BIT,              //С���ӹԹ�
                            ESP8266_SECOND_BIT,             //����ֻ�������
                            ESP8266_THIRD_BIT,              //��Ѽ��
                            ESP8266_FOURTH_BIT,             //С����
                            ESP8266_FIFTH_BIT               //�����
                            };
    EventBits_t lBitState;
    DHT22_t *pDht22_Show = (DHT22_t *)pvPortMalloc(sizeof(DHT22_t)); 
    /* �ȴ����Ϸ����� */
    xEventGroupWaitBits((EventGroupHandle_t) ESP8266_EventGroup_Handle,
                        (EventBits_t) ESP8266_CONNECT_BIT,
                        (BaseType_t) pdTRUE,
                        (BaseType_t) pdFALSE,
                        (TickType_t) portMAX_DELAY);

	OLED_Clear();
    while(1)
    {
        lBitState = xEventGroupGetBits(ESP8266_EventGroup_Handle);
        /* ���ֲ��Ŵ��� */
        if((lBitState&ESP8266_FIRST_BIT) != 0){                         //С���ӹԹ�
            Voice_Play(Voice_one);
            xEventGroupClearBits(ESP8266_EventGroup_Handle, ESP8266_FIRST_BIT);
        }else if((lBitState&ESP8266_SECOND_BIT) != 0){                  //����ֻ�������
            Voice_Play(Voice_two);
            xEventGroupClearBits(ESP8266_EventGroup_Handle, ESP8266_SECOND_BIT);
        }else if((lBitState&ESP8266_THIRD_BIT) != 0){                   //��Ѽ��
            Voice_Play(Voice_three);
            xEventGroupClearBits(ESP8266_EventGroup_Handle, ESP8266_THIRD_BIT);
        }else if((lBitState&ESP8266_FOURTH_BIT) != 0){                  //С����
            Voice_Play(Voice_four);
            xEventGroupClearBits(ESP8266_EventGroup_Handle, ESP8266_FOURTH_BIT);
        }else if((lBitState&ESP8266_FIFTH_BIT) != 0){                   //�����
            Voice_Play(Voice_five);
            xEventGroupClearBits(ESP8266_EventGroup_Handle, ESP8266_FIFTH_BIT);
        }else if((lBitState&ESP8266_SILENCE_BIT) != 0){                 //����
            Voice_Play(Voice_six);
            xEventGroupClearBits(ESP8266_EventGroup_Handle, ESP8266_SILENCE_BIT);
        }
        
        /* �������� */
        if((lBitState&ESP8266_MODE_BIT) != 0){
            lRecKey = 1;
            xEventGroupClearBits(ESP8266_EventGroup_Handle, ESP8266_MODE_BIT);
        }else if((lBitState&ESP8266_BEEP_BIT) != 0){
            lRecKey = 2;
            xEventGroupClearBits(ESP8266_EventGroup_Handle, ESP8266_BEEP_BIT);
        }else if((lBitState&ESP8266_MUSIC_BIT) != 0){
            lRecKey = 3;
            xEventGroupClearBits(ESP8266_EventGroup_Handle, ESP8266_MUSIC_BIT);
        }
				lKeyValue = KEY_Scan(0);
				
        lKeyAnswer = lKeyValue ? lKeyValue : lRecKey;          //Ӳ��������Ӧ���ȼ����
        switch(lKeyAnswer)
        {
            case KEY0_PRES:
                /* ģʽ�л� */
                lCurrentMode = lCurrentMode == DEFAULT_MODE ? CARE_MODE : DEFAULT_MODE;
								 if(ESP8266_EventGroup_Handle != NULL)
									 xEventGroupSetBits(ESP8266_EventGroup_Handle, lVoicePlay[0]);
                break;
            case KEY1_PRES:
                /* �������� */
                lCurrentMode = DEFAULT_MODE;
                if(Beep_On == Get_BeepState())
                    BEEP_Config(Beep_Off);
                else
                    BEEP_Config(Beep_On);
                break;
            case WKUP_PRES:
                lCurrentMode = DEFAULT_MODE;
                /* ����˳�򲥷� */
                if(lSoundSelect >= sizeof(lVoicePlay)){
                   lSoundSelect = 0;
                }
								if(ESP8266_EventGroup_Handle != NULL)
                     xEventGroupSetBits(ESP8266_EventGroup_Handle, lVoicePlay[lSoundSelect]);
                lSoundSelect++;
                break;
            default:
                break;
        }
        lRecKey = 0;
        /* ��ʪ�����ݶ�ȡ */
        DHT22_Read_Data();  
        /* ģʽ���� */
        switch(lCurrentMode)
        {
            case DEFAULT_MODE:
                break;
            case CARE_MODE:
				lSoundSelect = 0;
                /* ʪ�ȳ��޴��� */
                DHT22_LIMIT();
                if(SOUND_READ){
                    lSoundCount++;
                    /* ��⵽������ */
                    if(lSoundCount > 3 && VOICE_DISABLE == lSoundState){
                        lSoundState = VOICE_ENABLE;
                        if(ESP8266_EventGroup_Handle != NULL)
                            xEventGroupSetBits(ESP8266_EventGroup_Handle, lVoicePlay[4]);
                    }
                }else{
                    lSoundState = VOICE_DISABLE;
//                    VOICE_Select(Voice_none);
                    lSoundCount = 0;
                }
                break;
            default:
                break;
        }
        BEEP_Handle();
        pDht22_Show = Get_Dht22Value();
        /* OLED������ʾ */
        OLED_ShowString(40  , 0 , "Cradle"              , 16);
        OLED_ShowString(0   , 2 , "Tem:"                , 16);
        OLED_ShowString(32  , 2 , pDht22_Show->Tem_Str  , 16);
        OLED_ShowString(0   , 4 , "RH:"                 , 16);
        OLED_ShowString(24  , 4 , pDht22_Show->RH_Str   , 16);
        OLED_ShowString(0   , 6 , pModeStr[lCurrentMode], 16);
        vTaskDelay(10);
    }
}
/**********************************************************************
* �������ƣ�Task_Init
* ������������ʼ������
* ���������void *pvParameters
* ����ֵ����
* ����˵������Ҫ���ڴ����µ�����
**********************************************************************/
void Task_Init(void *pvParameters)
{
	taskENTER_CRITICAL();                               //���������ٽ������ٽ����ڵ����񲻻ᱻ�ж�
    ESP8266_Task_Init();                                //ESP8266 wifi�����ʼ��
    xTaskCreate((TaskFunction_t) MainRunning_Task,      //��Ҫ�����������񴴽�
				(const char * ) "MainRunning_Task",
				MAIN_RUNNING_STACK_LENGTH,
				(void *) NULL,
				(UBaseType_t) MAIN_RUNNING_PRIORITY,
				(TaskHandle_t *) &MainRunning_Handle);
	vTaskDelete(Task_Init_Handle);
	taskEXIT_CRITICAL();                                //�˳������ٽ������ٽ����ڵ�����ɱ��жϣ����������л�
}
/**********************************************************************
* �������ƣ�main
* ������������������������ڴ�
* �����������
* ����ֵ��int
* ����˵������
**********************************************************************/
int main(void)
{		 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //����NVIC�жϷ���4λ��ռ���ȼ�
	delay_init();                                   //��ʱ������ʼ��
	OLED_Init();                                    //Oled��ʼ��
	uart_init(115200);	                            //���ڳ�ʼ��Ϊ115200
    usart3_init(115200);                            //����3��ʼ��Ϊ115200
    TIM5_PWM_Init(200-1,7200-1);                    //50hz���ʹ��PWM��ʼ��
    OLED_ShowString(0, 0, "Bsp_Initialize", 16); 
	KEY_Init();                                     //��ʼ���밴�����ӵ�Ӳ���ӿ�
	BEEP_Init();                                    //��������ʼ��
    OLED_ShowString(0, 2, "Beep_initialize", 16);
	Sound_Init();                                   //������������ʼ��
    OLED_ShowString(0, 4, "Voice_initialize", 16);
    VOICE_Init();                                   //���ֲ�������ʼ��
    OLED_ShowString(0, 6, "DHT22_Initialize", 16);
    while(DHT22_Init());                            //��ʪ�ȴ�������ʼ�� 							  		
    delay_ms(1000); 
    OLED_Clear();                                   //Oled����
	xTaskCreate((TaskFunction_t) Task_Init,         //FreeRTOS���񴴽�
				(const char * ) "Task_Init",        //FreeRTOS�������
				TASK_INIT_STACK_LENGTH,             //FreeRTOS�����ջ
				(void *) NULL,                      //FreeRTOS���������
				(UBaseType_t) TASK_INIT_PRIORITY,   //FreeRTOS�������ȼ�
				(TaskHandle_t *) &Task_Init_Handle);//FreeRTOS������
	vTaskStartScheduler();                          //�����������
 	while(1); 
 }

