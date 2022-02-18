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
* �������ƣ�VOICE_Send
* ��������������ģ�鷢��ָ��
* ���������data
* ����ֵ����
* ����˵����ָ���ʽ				AA xx 00 (AA+00+xx) 
* 			����(02)����ͣ(03)��ֹͣ(04)����һ��(05)����һ��(06)
*			����+(14)������-(15)	
**********************************************************************/
void VOICE_Send(u8 data)
{
	u8 i;
	pVOICE->Start = 0xAA;											//ָ����
	pVOICE->Data = data;											//ָ������
	pVOICE->Length = 0x00;											//ָ���
	pVOICE->Check = pVOICE->Start + pVOICE->Data + pVOICE->Length;	//У���
	memcpy(USART2_TX_BUF,pVOICE,VOICE_DATA_LENGHT);
	for(i = 0; i < VOICE_DATA_LENGHT; i++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); 		//ѭ������,ֱ���������   
		USART_SendData(USART2,USART2_TX_BUF[i]);
	} 
}


/**********************************************************************
* �������ƣ�VOICE_Mode
* ��������������ģ�鹤��ģʽ����
* �����������
* ����ֵ����
* ����˵������
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
* �������ƣ�VOICE_Select
* ���������������ض���Ŀ
* �����������Ŀ
* ����ֵ����
* ����˵������
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
* �������ƣ�VOICE_Init
* ������������ʼ������ģ��Ϊ���ڿ���
* �����������
* ����ֵ����
* ����˵����
**********************************************************************/
void VOICE_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;		//��������ģ��GPIO��
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						                //����Ϊ�������ģʽ 		 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 				                //�����������50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);							                        //ʹ��GIPO��
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;		//��������ģ��GPIO��
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						                //����Ϊ�������ģʽ 		 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 				                //�����������50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);							                        //ʹ��GIPO��
 	
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




