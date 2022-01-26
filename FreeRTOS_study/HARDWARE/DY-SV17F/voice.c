#include "voice.h"
//#include "usart2.h"
#include <string.h>
#include "delay.h"

#ifdef VOICE_UART
typedef struct
{
	u8 Start;
	u8 Data;
	u8 Length;
	u8 Check;
}stVOICE;

stVOICE gVOICE;
stVOICE *pVOICE = &gVOICE;


/**********************************************************************
* 函数名称：VOICE_Send
* 功能描述：语音模块发送指令
* 输入参数：data
* 返回值：无
* 补充说明：指令格式				AA xx 00 (AA+00+xx) 
* 			播放(02)、暂停(03)、停止(04)、上一曲(05)、下一曲(06)
*			音量+(14)、音量-(15)	
**********************************************************************/
void VOICE_Send(u8 data)
{
	u8 i;
	pVOICE->Start = 0xAA;											//指令码
	pVOICE->Data = data;											//指令数据
	pVOICE->Length = 0x00;											//指令长度
	pVOICE->Check = pVOICE->Start + pVOICE->Data + pVOICE->Length;	//校验和
	memcpy(USART2_TX_BUF,pVOICE,VOICE_DATA_LENGHT);
	for(i = 0; i < VOICE_DATA_LENGHT; i++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); 		//循环发送,直到发送完毕   
		USART_SendData(USART2,USART2_TX_BUF[i]);
	} 
}


/**********************************************************************
* 函数名称：VOICE_Mode
* 功能描述：语音模块工作模式配置
* 输入参数：无
* 返回值：无
* 补充说明：无
**********************************************************************/
void VOICE_Mode(void)
{
	VOICE_CON1_0;
	VOICE_CON2_0;
	VOICE_CON3_1;
	usart2_init(9600);
}
#endif
/**********************************************************************
* 函数名称：VOICE_Select
* 功能描述：播放特定曲目
* 输入参数：VOICE_Song_e 枚举
* 返回值：无
* 补充说明：
**********************************************************************/
void VOICE_Select(uint8_t mSongNum)
{
    uint8_t i;
    GPIO_TypeDef* lGpio[] = {GPIOA,GPIOA,GPIOC,GPIOC,GPIOC,GPIOC,GPIOA,GPIOA};
    uint16_t lGpio_pin[] = {GPIO_Pin_3,GPIO_Pin_2,GPIO_Pin_3,GPIO_Pin_2,GPIO_Pin_1,GPIO_Pin_0,GPIO_Pin_7,GPIO_Pin_6};
    for(i = 0; i < 8; i++)
    {
        GPIO_SetBits(lGpio[i], lGpio_pin[i]);
        if((mSongNum >> i) == 1){
            GPIO_ResetBits(lGpio[i], lGpio_pin[i]);
        }
    }
}

/**********************************************************************
* 函数名称：VOICE_Init
* 功能描述：初始化语音模块为串口控制
* 输入参数：无
* 返回值：无
* 补充说明：
**********************************************************************/
void VOICE_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;		//配置语音模块GPIO口
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						                //配置为推挽输出模式 		 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 				                //配置输出速率50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);							                        //使能GIPO口
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;		//配置语音模块GPIO口
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						                //配置为推挽输出模式 		 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 				                //配置输出速率50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);							                        //使能GIPO口
//	VOICE_Mode();															                //配置语音模块工作模式
	VOICE_Select(Voice_none);
//	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
//	GPIO_SetBits(GPIOA, GPIO_Pin_2);
//	GPIO_SetBits(GPIOC, GPIO_Pin_3);
//	GPIO_SetBits(GPIOC, GPIO_Pin_2);
//	GPIO_SetBits(GPIOC, GPIO_Pin_1);
//	GPIO_SetBits(GPIOC, GPIO_Pin_0);
//	GPIO_SetBits(GPIOA, GPIO_Pin_7);
//	GPIO_SetBits(GPIOA, GPIO_Pin_6);
}




