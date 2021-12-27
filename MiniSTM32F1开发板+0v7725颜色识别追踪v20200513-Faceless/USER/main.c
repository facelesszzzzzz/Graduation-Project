#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"
#include "usmart.h" 
#include "lcd.h" 
#include "ov7670.h" 
#include "ov7725.h"
#include "exti.h" 
#include "timer.h" 
#include "ColorTracer.h"
#include "malloc.h"  
#include "MMC_SD.h" 
#include "ff.h"  
#include "exfuns.h"
#include "fontupd.h"
#include "text.h"	
#include "bsp_adc.h"
#include "usart3.h"

//ALIENTEK Mini STM32��������չʵ��9
//����ͷʵ��
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  

//���������ֶ���
#define  OV7725 1
#define  OV7670 2

//����OV7725��������װ��ʽԭ��,OV7725_WINDOW_WIDTH�൱��LCD�ĸ߶ȣ�OV7725_WINDOW_HEIGHT�൱��LCD�Ŀ��
//ע�⣺�˺궨��ֻ��OV7725��Ч
#define  OV7725_WINDOW_WIDTH		260 // <=320
#define  OV7725_WINDOW_HEIGHT		240 // <=240

extern u8 ov_sta;	//��exit.c���涨��
extern u8 ov_frame;	//��timer.c���涨��
extern volatile uint8_t Ov7725_Vsync;


//����LCD��ʾ(OV7725)
void OV7725_camera_refresh(void)
{
	u32 i,j;
 	u16 color;
	if(ov_sta==2)
	{
		LCD_Scan_Dir(U2D_L2R);		//���ϵ���,������ 
		LCD_Set_Window(60,0,OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT);
		//LCD_Set_Window((lcddev.width-OV7725_WINDOW_WIDTH)/2,(lcddev.height-OV7725_WINDOW_HEIGHT)/2,OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT);//����ʾ�������õ���Ļ����
		if(lcddev.id==0X1963)
			LCD_Set_Window(60,0,OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT);
			//LCD_Set_Window((lcddev.width-OV7725_WINDOW_WIDTH)/2,(lcddev.height-OV7725_WINDOW_HEIGHT)/2,OV7725_WINDOW_HEIGHT,OV7725_WINDOW_WIDTH);//����ʾ�������õ���Ļ����
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM
		OV7725_CS=0;		
		OV7725_RRST=0;				//��ʼ��λ��ָ�� 
		OV7725_RCK=0;
		OV7725_RCK=1;
		OV7725_RCK=0;
		OV7725_RRST=1;				//��λ��ָ����� 
		OV7725_RCK=1; 
		for(i=0;i<OV7725_WINDOW_HEIGHT;i++)
		{
			for(j=0;j<OV7725_WINDOW_WIDTH;j++)
			{
				GPIOB->CRL=0X88888888;
				OV7725_RCK=0;
				color=OV7725_DATA;	//������
				OV7725_RCK=1; 
				color<<=8;  
				OV7725_RCK=0;
				color|=OV7725_DATA;	//������
				OV7725_RCK=1;
				GPIOB->CRL=0X33333333;
				LCD_WR_DATA(color); 
			}
		}
		OV7725_CS=1;
		OV7725_RCK=0;
		OV7725_RCK=1;
		EXTI->PR=1<<15;
 		ov_sta=0;					//����֡�жϱ��
		ov_frame++; 
		LCD_Scan_Dir(DFT_SCAN_DIR);	//�ָ�Ĭ��ɨ�跽�� 
	} 
}
float TU=0.0;
float TU_value=0.0;
float TU_calibration=0.0;
float temp_data=0.0;
float K_Value=3047.19;
unsigned char  Tx[20];   //���߷��ͻ���


// ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
extern __IO uint16_t ADC_ConvertedValue;

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
float ADC_ConvertedValueLocal;  
/**************TDSֵ�ɼ�����***************/
void TU_Value_Conversion()
{
		TU =(float) ADC_ConvertedValue/4096*3.3; // ��ȡת����ADֵ
	
		TU_calibration=-0.0192*(temp_data/10-25)+TU;  
	  TU_value=-865.68*TU_calibration + K_Value;
	
		if(TU_value<=0){TU_value=0;}
		if(TU_value>=3000){TU_value=3000;}
	
		/*��ʾTDS*/
		Tx[0]=(int)(TU_value)/1000+'0';
		Tx[1]=(int)(TU_value)%1000/100+'0';	
		Tx[2]=(int)(TU_value)%100/10+'0';
		Tx[3]=(int)(TU_value)%10+'0';
}

int main(void)
{	
	u8 sensor=0;
	u8 i;	
	delay_init();	    	 //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	uart_init(115200);
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	LCD_Init();			   	//��ʼ��LCD
	ADCx_Init();				//ADC ��ʼ��
	usmart_dev.init(72);	//��ʼ��USMART
  font_init();	 				//�ֿ��ʼ��
 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,200,16,"Mini STM32");	
	LCD_ShowString(30,70,200,200,16,"OV7725_OV7670 TEST");	
	LCD_ShowString(30,90,200,200,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,200,16,"2019/06/02");  	 
	LCD_ShowString(30,130,200,200,16,"Use USMART To Set!");	 
  LCD_ShowString(30,150,200,200,16,"OV7725_OV7670 Init...");	  
	while(1)//��ʼ��OV7725_OV7670
	{
		if(OV7725_Init()==0)
		{
			sensor=OV7725;
			LCD_ShowString(30,150,200,16,16,"OV7725 Init OK       ");				
			delay_ms(1500);
			//����Ϊ����ͷ��ϵͳ����
			OV7725_Light_Mode(0);
			OV7725_Color_Saturation(0);
			OV7725_Brightness(0);
			OV7725_Contrast(0);
			OV7725_Special_Effects(0);
			OV7725_Window_Set(OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT,0);//QVGAģʽ���
			OV7725_CS=0;
			break;
		}
		else
		{
			LCD_ShowString(30,150,200,16,16,"OV7725_OV7670 Error!!");
			delay_ms(200);
			LCD_Fill(30,150,200,246,WHITE);
			delay_ms(200);
		}
	} 	  
	TIM3_Int_Init(10000,7199);			//TIM3,10Khz����Ƶ��,1�����ж�									  
	EXTI15_Init();						//ʹ�ܶ�ʱ������					 	 
 	LCD_Clear(BACK_COLOR);
	POINT_COLOR=BLACK;
	Show_Str(5,5,200,24,"ʶ�����ʣ�",24,0);
  Show_Str(5,29,200,24,"ʶ��Ũ�ȣ�",24,0);
	POINT_COLOR=RED;
	while(1)
	{	
		if(sensor==OV7725)
			OV7725_camera_refresh();//����ͷ��Ļˢ��
		// if(Trace(&condition0, &result))//ʶ����ɫΪ����ͭ
		// {
		// 	POINT_COLOR=GREEN;
		// 	Show_Str(125,5,200,24,"����ͭ",24,0);//����ͭ
		// 	TU_Value_Conversion();		//Ũ�ȵ�ADת����ȡ	
		// 	Show_Str(125,29,200,24,Tx,24,0);//���ݴ浽Tx����
		// 	POINT_COLOR=RED;
		// 	delay_ms(500);
		// }
		// else if(Trace(&condition1, &result))//ʶ����ɫΪ������
		// {
		// 	POINT_COLOR=BLUE;
		// 	Show_Str(125,5,200,24,"������",24,0);//������
		// 	TU_Value_Conversion();	
		// 	Show_Str(125,29,200,24,Tx,24,0);
		// 	POINT_COLOR=RED;
		// 	delay_ms(500);
		// }
		// else if(Trace(&condition8, &result))//ʶ���ɫΪ������
		// {
		// 	POINT_COLOR=RED;
		// 	Show_Str(125,5,200,24,"������",24,0);//������
		// 	TU_Value_Conversion();	
		// 	Show_Str(125,29,200,24,Tx,24,0);
		// 	POINT_COLOR=RED;
		// 	delay_ms(500);
		// }
		// else if(Trace(&condition5, &result))//ʶ���ɫΪ����
		// {
		// 	POINT_COLOR=YELLOW;
		// 	Show_Str(125,5,200,24,"����  ",24,0);//����
		// 	TU_Value_Conversion();	
		// 	Show_Str(125,29,200,24,Tx,24,0);
		// 	POINT_COLOR=RED;
		// 	delay_ms(500);
		// }
		// else
		// {
		// 	POINT_COLOR=BLACK;
		// 	Show_Str(125,5,200,24,"δʶ��",24,0);//δʶ��
		// 	Show_Str(125,29,200,24,"0000",24,0);
		// 	POINT_COLOR=RED;
		// }
 		if(i!=ov_frame)		//DS0��˸.
		{
			i=ov_frame;
			LED0=!LED0;
 		}
	}   
}

