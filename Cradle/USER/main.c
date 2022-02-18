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
/**********************************************************************
* 函数名称：MainRunning_Task
* 功能描述：主要程序运行任务
* 输入参数：void *pvParameters
* 返回值：无
* 补充说明：整个程序最重要的部分，类似裸机的while(1)
**********************************************************************/
void MainRunning_Task(void *pvParameters)
{
    uint8_t lCurrentMode = DEFAULT_MODE;
    uint8_t *pModeStr[] = {"Mode:Default","Mode:Care   "};
    uint8_t lSoundCount = 0;
    uint8_t lSoundState = VOICE_DISABLE;
	uint8_t lSoundSelect = 0;
    DHT22_t *pDht22_Show = (DHT22_t *)pvPortMalloc(sizeof(DHT22_t)); 
    /* 等待连上服务器 */
    xEventGroupWaitBits((EventGroupHandle_t) ESP8266_EventGroup_Handle,
                        (EventBits_t) ESP8266_CONNECT_BIT,
                        (BaseType_t) pdTRUE,
                        (BaseType_t) pdFALSE,
                        (TickType_t) portMAX_DELAY);

	OLED_Clear();
    while(1)
    {
        /* 按键处理 */
        switch(KEY_Scan(0))
        {
            case KEY0_PRES:
                /* 模式切换 */
                lCurrentMode = lCurrentMode == DEFAULT_MODE ? CARE_MODE : DEFAULT_MODE;
                break;
            case KEY1_PRES:
                /* 警报开关 */
                lCurrentMode = DEFAULT_MODE;
                if(Beep_On == Get_BeepState())
                    BEEP_Config(Beep_Off);
                else
                    BEEP_Config(Beep_On);
                break;
            case WKUP_PRES:
                lCurrentMode = DEFAULT_MODE;
				lSoundSelect++;
				if(1 == lSoundSelect){
					/* 无声 */
					VOICE_Select(Voice_three);
					vTaskDelay(1000);
					VOICE_Select(Voice_none);
				}else if(2 == lSoundSelect){
					/* 小星星 */
					VOICE_Select(Voice_one);
					vTaskDelay(1000);
					VOICE_Select(Voice_none);
				}else if(3 == lSoundSelect){
					/* 两只老虎 */
					VOICE_Select(Voice_two);
					vTaskDelay(1000);
					VOICE_Select(Voice_none);
					lSoundSelect= 0;
				}

                break;
            default:
                break;
        }
        /* 温湿度数据读取 */
        DHT22_Read_Data();  
        /* 模式处理 */
        switch(lCurrentMode)
        {
            case DEFAULT_MODE:
                break;
            case CARE_MODE:
				lSoundSelect = 0;
                /* 湿度超限处理 */
                DHT22_LIMIT();
                if(SOUND_READ){
                    lSoundCount++;
                    /* 检测到哭啼声 */
                    if(lSoundCount > 3 && VOICE_DISABLE == lSoundState){
                        lSoundState = VOICE_ENABLE;
                        VOICE_Select(Voice_one);
						vTaskDelay(1000);
						VOICE_Select(Voice_none);
                    }
                }else{
                    lSoundState = VOICE_DISABLE;
                    VOICE_Select(Voice_none);
                    lSoundCount = 0;
                }
                break;
            default:
                break;
        }
        BEEP_Handle();
        pDht22_Show = Get_Dht22Value();
        /* OLED内容显示 */
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
* 函数名称：Task_Init
* 功能描述：初始化任务
* 输入参数：void *pvParameters
* 返回值：无
* 补充说明：主要用于创建新的任务
**********************************************************************/
void Task_Init(void *pvParameters)
{
	taskENTER_CRITICAL();                               //进入任务临界区，临界区内的任务不会被中断
    ESP8266_Task_Init();                                //ESP8266 wifi任务初始化
    xTaskCreate((TaskFunction_t) MainRunning_Task,      //主要程序运行任务创建
				(const char * ) "MainRunning_Task",
				MAIN_RUNNING_STACK_LENGTH,
				(void *) NULL,
				(UBaseType_t) MAIN_RUNNING_PRIORITY,
				(TaskHandle_t *) &MainRunning_Handle);
	vTaskDelete(Task_Init_Handle);
	taskEXIT_CRITICAL();                                //退出任务临界区，临界区内的任务可被中断，进行任务切换
}
/**********************************************************************
* 函数名称：main
* 功能描述：主函数，程序入口处
* 输入参数：无
* 返回值：int
* 补充说明：无
**********************************************************************/
int main(void)
{		 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //设置NVIC中断分组4位抢占优先级
	delay_init();                                   //延时函数初始化
	OLED_Init();                                    //Oled初始化
	uart_init(115200);	                            //串口初始化为115200
    usart3_init(115200);                            //串口3初始化为115200
    TIM5_PWM_Init(200-1,7200-1);                    //50hz舵机使用PWM初始化
    OLED_ShowString(0, 0, "Bsp_Initialize", 16); 
	KEY_Init();                                     //初始化与按键连接的硬件接口
	BEEP_Init();                                    //蜂鸣器初始化
    OLED_ShowString(0, 2, "Beep_initialize", 16);
	Sound_Init();                                   //声音传感器初始化
    OLED_ShowString(0, 4, "Voice_initialize", 16);
    VOICE_Init();                                   //音乐播放器初始化
    OLED_ShowString(0, 6, "DHT22_Initialize", 16);
    while(DHT22_Init());                            //温湿度传感器初始化 							  		
    delay_ms(1000); 
    OLED_Clear();                                   //Oled清屏
	xTaskCreate((TaskFunction_t) Task_Init,         //FreeRTOS任务创建
				(const char * ) "Task_Init",        //FreeRTOS任务别名
				TASK_INIT_STACK_LENGTH,             //FreeRTOS任务堆栈
				(void *) NULL,                      //FreeRTOS任务传入参数
				(UBaseType_t) TASK_INIT_PRIORITY,   //FreeRTOS任务优先级
				(TaskHandle_t *) &Task_Init_Handle);//FreeRTOS任务句柄
	vTaskStartScheduler();                          //开启任务调度
 	while(1); 
 }

