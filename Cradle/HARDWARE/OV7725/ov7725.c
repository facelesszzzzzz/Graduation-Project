#include "sys.h"
#include "ov7725.h"
#include "ov7725config.h"	  
#include "delay.h"
#include "usart.h"			 
#include "sccb.h"	
#include "FreeRTOS.h"
#include "event_groups.h"
#include "ESP8266.h"
//////////////////////////////////////////////////////////////////////////////////
//ALIENTEK MiniSTM32������
//OV7725 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2017/11/1
//�汾��V1.0		    							    							  
//////////////////////////////////////////////////////////////////////////////////
		    			  

extern u8 ov_sta;	//��exit.c���涨��
//extern u8 ov_frame;	//��timer.c���涨��

//��ʼ��OV7725
//����0:�ɹ�
//��������ֵ:�������
u8 OV7725_Init(void)
{
	u16 reg;
	u16 i=0;	  
	//����IO
 
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);	 //ʹ����ض˿�ʱ��
 
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4;				 // �˿�����
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4);	

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15; //PA15���� ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_15);
	
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_14;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_14);
    
    /* D0~D7 */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//SWD
 
	SCCB_Init();        		//��ʼ��SCCB ��IO��	
 	if(SCCB_WR_Reg(0x12,0x80))return 1;	//��λOV7725
	delay_ms(50); 
	reg=SCCB_RD_Reg(0X1c);		//��ȡ����ID �߰�λ
	reg<<=8;
	reg|=SCCB_RD_Reg(0X1d);		//��ȡ����ID �Ͱ�λ
	if(reg!=OV7725_MID)
	{
		printf("MID:%d\r\n",reg);
		return 1;
	}
	reg=SCCB_RD_Reg(0X0a);		//��ȡ����ID �߰�λ
	reg<<=8;
	reg|=SCCB_RD_Reg(0X0b);		//��ȡ����ID �Ͱ�λ
	if(reg!=OV7725_PID)
	{
		printf("HID:%d\r\n",reg);
		return 2;
	}   
 	//��ʼ�� OV7725,����QVGA�ֱ���(320*240)  
	for(i=0;i<sizeof(ov7725_init_reg_tb1)/sizeof(ov7725_init_reg_tb1[0]);i++)
	{								
	   	SCCB_WR_Reg(ov7725_init_reg_tb1[i][0],ov7725_init_reg_tb1[i][1]);
 	} 
  	return 0x00; 	//ok
} 
////////////////////////////////////////////////////////////////////////////
//OV7725��������
//��ƽ������
//0:�Զ�ģʽ
//1:����
//2,����
//3,�칫��
//4,����
//5,ҹ��
void OV7725_Light_Mode(u8 mode)
{
	switch(mode)
	{
		case 0:	//Auto���Զ�ģʽ
			SCCB_WR_Reg(0x13, 0xff); //AWB on 
			SCCB_WR_Reg(0x0e, 0x65);
			SCCB_WR_Reg(0x2d, 0x00);
			SCCB_WR_Reg(0x2e, 0x00);
			break;
		case 1://sunny������
			SCCB_WR_Reg(0x13, 0xfd); //AWB off
			SCCB_WR_Reg(0x01, 0x5a);
			SCCB_WR_Reg(0x02, 0x5c);
			SCCB_WR_Reg(0x0e, 0x65);
			SCCB_WR_Reg(0x2d, 0x00);
			SCCB_WR_Reg(0x2e, 0x00);
			break;	
		case 2://cloudy������
			SCCB_WR_Reg(0x13, 0xfd); //AWB off
			SCCB_WR_Reg(0x01, 0x58);
			SCCB_WR_Reg(0x02, 0x60);
			SCCB_WR_Reg(0x0e, 0x65);
			SCCB_WR_Reg(0x2d, 0x00);
			SCCB_WR_Reg(0x2e, 0x00);
			break;	
		case 3://office���칫��
			SCCB_WR_Reg(0x13, 0xfd); //AWB off
			SCCB_WR_Reg(0x01, 0x84);
			SCCB_WR_Reg(0x02, 0x4c);
			SCCB_WR_Reg(0x0e, 0x65);
			SCCB_WR_Reg(0x2d, 0x00);
			SCCB_WR_Reg(0x2e, 0x00);
			break;	
		case 4://home������
			SCCB_WR_Reg(0x13, 0xfd); //AWB off
			SCCB_WR_Reg(0x01, 0x96);
			SCCB_WR_Reg(0x02, 0x40);
			SCCB_WR_Reg(0x0e, 0x65);
			SCCB_WR_Reg(0x2d, 0x00);
			SCCB_WR_Reg(0x2e, 0x00);
			break;	

		case 5://night��ҹ��
			SCCB_WR_Reg(0x13, 0xff); //AWB on
			SCCB_WR_Reg(0x0e, 0xe5);
			break;
	}
}		  
//ɫ������
//sat:-4~+4
void OV7725_Color_Saturation(s8 sat)
{
 	if(sat>=-4 && sat<=4)
	{	
		SCCB_WR_Reg(USAT,(sat+4)<<4); 
		SCCB_WR_Reg(VSAT,(sat+4)<<4);
	}
}
//��������
//bright��-4~+4
void OV7725_Brightness(s8 bright)
{
	u8 bright_value,sign;
  	switch(bright)
	{
		case 4:
			bright_value = 0x48;
			sign = 0x06;
			break;
		case 3:
			bright_value = 0x38;
			sign = 0x06;		
			break;	
		case 2:
			bright_value = 0x28;
			sign = 0x06;			
			break;	
		case 1:
			bright_value = 0x18;
			sign = 0x06;			
			break;
		case 0:
			bright_value = 0x08;
			sign = 0x06;			
			break;	
		case -1:
			bright_value = 0x08;
			sign = 0x0e;		
			break;		
		case -2:
			bright_value = 0x18;
			sign = 0x0e;		
			break;	
		case -3:
			bright_value = 0x28;
			sign = 0x0e;		
			break;	
		case -4:
			bright_value = 0x38;
			sign = 0x0e;		
			break;	
	}
	SCCB_WR_Reg(BRIGHT, bright_value);
	SCCB_WR_Reg(SIGN, sign);
}
//�Աȶ�����
//contrast��-4~+4
void OV7725_Contrast(s8 contrast)
{
	if(contrast >= -4 && contrast <=4)
	{
		SCCB_WR_Reg(CNST,(0x30-(4-contrast)*4));
	}
}
//��Ч����
//0:��ͨģʽ    
//1,��Ƭ
//2,�ڰ�   
//3,ƫ��ɫ
//4,ƫ��ɫ
//5,ƫ��ɫ
//6,����	    
void OV7725_Special_Effects(u8 eft)
{
	switch(eft)
	{
		case 0://����
			SCCB_WR_Reg(0xa6, 0x06);
			SCCB_WR_Reg(0x60, 0x80);
			SCCB_WR_Reg(0x61, 0x80);
			break;
		case 1://�ڰ�
			SCCB_WR_Reg(0xa6, 0x26);
			SCCB_WR_Reg(0x60, 0x80);
			SCCB_WR_Reg(0x61, 0x80);
			break;	
		case 2://ƫ��
			SCCB_WR_Reg(0xa6, 0x1e);
			SCCB_WR_Reg(0x60, 0xa0);
			SCCB_WR_Reg(0x61, 0x40);	
			break;
		case 3://����
			SCCB_WR_Reg(0xa6, 0x1e);
			SCCB_WR_Reg(0x60, 0x40);
			SCCB_WR_Reg(0x61, 0xa0);	
			break;	
		case 4://ƫ��
			SCCB_WR_Reg(0xa6, 0x1e);
			SCCB_WR_Reg(0x60, 0x80);
			SCCB_WR_Reg(0x61, 0xc0);		
			break;	
		case 5://ƫ��
			SCCB_WR_Reg(0xa6, 0x1e);
			SCCB_WR_Reg(0x60, 0x60);
			SCCB_WR_Reg(0x61, 0x60);		
			break;	
		case 6://����
			SCCB_WR_Reg(0xa6, 0x46);
			break;	
	}
}	
//����ͼ���������
//width:���ͼ����,<=320
//height:���ͼ��߶�,<=240
//mode:0��QVGA���ģʽ��1��VGA���ģʽ
//QVGAģʽ���ӷ�Χ�㵫���ﲻ�Ǻ�������VGAģʽ���ӷ�ΧС��������
void OV7725_Window_Set(u16 width,u16 height,u8 mode)
{
	u8 raw,temp;
	u16 sx,sy;
	
	if(mode)
	{
		sx=(640-width)/2;
		sy=(480-height)/2;
		SCCB_WR_Reg(COM7,0x06);		//����ΪVGAģʽ
		SCCB_WR_Reg(HSTART,0x23); 	//ˮƽ��ʼλ��
		SCCB_WR_Reg(HSIZE,0xA0); 	//ˮƽ�ߴ�
		SCCB_WR_Reg(VSTRT,0x07); 	//��ֱ��ʼλ��
		SCCB_WR_Reg(VSIZE,0xF0); 	//��ֱ�ߴ�
		SCCB_WR_Reg(HREF,0x00);
		SCCB_WR_Reg(HOutSize,0xA0); //����ߴ�
		SCCB_WR_Reg(VOutSize,0xF0); //����ߴ�
	}
	else
	{
		sx=(320-width)/2;
		sy=(240-height)/2;
		SCCB_WR_Reg(COM7,0x46);		//����ΪQVGAģʽ
		SCCB_WR_Reg(HSTART,0x3f); 	//ˮƽ��ʼλ��
		SCCB_WR_Reg(HSIZE, 0x50); 	//ˮƽ�ߴ�
		SCCB_WR_Reg(VSTRT, 0x03); 	//��ֱ��ʼλ��
		SCCB_WR_Reg(VSIZE, 0x78); 	//��ֱ�ߴ�
		SCCB_WR_Reg(HREF,  0x00);
		SCCB_WR_Reg(HOutSize,0x50);	//����ߴ�
		SCCB_WR_Reg(VOutSize,0x78); //����ߴ�
	}
	raw=SCCB_RD_Reg(HSTART);
	temp=raw+(sx>>2);//sx��8λ����HSTART,��2λ����HREF[5:4]
	SCCB_WR_Reg(HSTART,temp);
	SCCB_WR_Reg(HSIZE,width>>2);//width��8λ����HSIZE,��2λ����HREF[1:0]
	
	raw=SCCB_RD_Reg(VSTRT);
	temp=raw+(sy>>1);//sy��8λ����VSTRT,��1λ����HREF[6]
	SCCB_WR_Reg(VSTRT,temp);
	SCCB_WR_Reg(VSIZE,height>>1);//height��8λ����VSIZE,��1λ����HREF[2]
	
	raw=SCCB_RD_Reg(HREF);
	temp=((sy&0x01)<<6)|((sx&0x03)<<4)|((height&0x01)<<2)|(width&0x03)|raw;
	SCCB_WR_Reg(HREF,temp);
	
	SCCB_WR_Reg(HOutSize,width>>2);
	SCCB_WR_Reg(VOutSize,height>>1);
	
	SCCB_RD_Reg(EXHCH);	
	temp = (raw|(width&0x03)|((height&0x01)<<2));	
	SCCB_WR_Reg(EXHCH,temp);	
}

void OV7725_Configuration(void)
{
	//����Ϊ����ͷ��ϵͳ����
	OV7725_Light_Mode(0);
	OV7725_Color_Saturation(0);
	OV7725_Brightness(0);
	OV7725_Contrast(0);
	OV7725_Special_Effects(0);
	OV7725_Window_Set(OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT,0);//QVGAģʽ���
	OV7725_CS=0;
}

void Send_Pic_Div(u16 color)
{
	u8 temp;		 
	temp = color&0x00ff;						//�Ͱ�λ
	USART_SendData(USART1,temp);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);

	temp = color>>8;								//�߰�λ
	USART_SendData(USART1,temp);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
}

extern EventGroupHandle_t ESP8266_EventGroup_Handle;
//����LCD��ʾ(OV7725)
void OV7725_camera_refresh(void)
{
	u32 i,j;
 	u16 color;
	EventBits_t lBit;
//	if(ESP8266_EventGroup_Handle != NULL)
//		xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_LOOK_RESET_BIT);
//	printf("ready");
//	lBit = xEventGroupWaitBits(ESP8266_EventGroup_Handle,
//								ESP8266_LOOK_SET_BIT,
//								pdFALSE,
//								pdFALSE,
//								portMAX_DELAY);
//	printf("start");
//	if((lBit&ESP8266_LOOK_SET_BIT) != 0)
	if(ov_sta == 2)
	{
//		LCD_Scan_Dir(U2D_L2R);		//���ϵ���,������ 
//		LCD_Set_Window(60,0,OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT);
//		//LCD_Set_Window((lcddev.width-OV7725_WINDOW_WIDTH)/2,(lcddev.height-OV7725_WINDOW_HEIGHT)/2,OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT);//����ʾ�������õ���Ļ����
//		if(lcddev.id==0X1963)
//			LCD_Set_Window(60,0,OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT);
//			//LCD_Set_Window((lcddev.width-OV7725_WINDOW_WIDTH)/2,(lcddev.height-OV7725_WINDOW_HEIGHT)/2,OV7725_WINDOW_HEIGHT,OV7725_WINDOW_WIDTH);//����ʾ�������õ���Ļ����
//		LCD_WriteRAM_Prepare();     //��ʼд��GRAM
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
//				GPIOB->CRL=0X88888888;
				OV7725_RCK=0;
				color=(GPIOB->IDR&0x03)|((GPIOB->IDR&0x1F8)>>1);//OV7725_DATA;	//������
				//color=OV7725_DATA;	//������
				OV7725_RCK=1; 
				color<<=8;  
				OV7725_RCK=0;
				color|=((GPIOB->IDR&0x03)|((GPIOB->IDR&0x1F8)>>1));	//������
				//color|=OV7725_DATA;	//������
				OV7725_RCK=1;
//				GPIOB->CRL=0X33333333;
//				LCD_WR_DATA(color); 
				Send_Pic_Div(color);
			}
		}
		OV7725_CS=1;
		OV7725_RCK=0;
		OV7725_RCK=1;
		EXTI->PR=1<<15;
//		xEventGroupSetBits(ESP8266_EventGroup_Handle, ESP8266_LOOK_RESET_BIT);
 		ov_sta=0;					//����֡�жϱ��
//		ov_frame++; 
//		LCD_Scan_Dir(DFT_SCAN_DIR);	//�ָ�Ĭ��ɨ�跽�� 
	} 
}































