#include "beep.h"  

static uint8_t gBeep_State;
//初始化PB8为输出口.并使能这个口的时钟		    
//蜂鸣器初始化
void BEEP_Init(void)
{
 
    GPIO_InitTypeDef  GPIO_InitStructure;
    	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能GPIOB端口时钟

    GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_IO;			//BEEP-->PB.8 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	    //速度为50MHz
    GPIO_Init(BEEP_GPIO, &GPIO_InitStructure);	            //根据参数初始化GPIOB.8

    GPIO_ResetBits(BEEP_GPIO,BEEP_GPIO_IO);                //输出0，关闭蜂鸣器输出

}
uint8_t Get_BeepState(void)
{
    return gBeep_State;
}

void BEEP_Config(uint8_t mState)
{
    gBeep_State = mState;
}

void BEEP_Handle(void)
{
    if(gBeep_State)
        GPIO_SetBits(BEEP_GPIO,BEEP_GPIO_IO);
    else
        GPIO_ResetBits(BEEP_GPIO,BEEP_GPIO_IO);
}

void Sound_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能GPIOB端口时钟

    GPIO_InitStructure.GPIO_Pin = SOUND_GPIO_IO;			
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		    
    GPIO_Init(SOUND_GPIO, &GPIO_InitStructure);	           
}


