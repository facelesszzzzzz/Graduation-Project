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

//ALIENTEK Mini STM32开发板扩展实验9
//摄像头实验
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  

//传感器名字定义
#define  OV7725 1
#define  OV7670 2

//由于OV7725传感器安装方式原因,OV7725_WINDOW_WIDTH相当于LCD的高度，OV7725_WINDOW_HEIGHT相当于LCD的宽度
//注意：此宏定义只对OV7725有效
#define  OV7725_WINDOW_WIDTH		260 // <=320
#define  OV7725_WINDOW_HEIGHT		240 // <=240

extern u8 ov_sta;	//在exit.c里面定义
extern u8 ov_frame;	//在timer.c里面定义
extern volatile uint8_t Ov7725_Vsync;


//更新LCD显示(OV7725)
void OV7725_camera_refresh(void)
{
	u32 i,j;
 	u16 color;
	if(ov_sta==2)
	{
		LCD_Scan_Dir(U2D_L2R);		//从上到下,从左到右 
		LCD_Set_Window(60,0,OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT);
		//LCD_Set_Window((lcddev.width-OV7725_WINDOW_WIDTH)/2,(lcddev.height-OV7725_WINDOW_HEIGHT)/2,OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT);//将显示区域设置到屏幕中央
		if(lcddev.id==0X1963)
			LCD_Set_Window(60,0,OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT);
			//LCD_Set_Window((lcddev.width-OV7725_WINDOW_WIDTH)/2,(lcddev.height-OV7725_WINDOW_HEIGHT)/2,OV7725_WINDOW_HEIGHT,OV7725_WINDOW_WIDTH);//将显示区域设置到屏幕中央
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		OV7725_CS=0;		
		OV7725_RRST=0;				//开始复位读指针 
		OV7725_RCK=0;
		OV7725_RCK=1;
		OV7725_RCK=0;
		OV7725_RRST=1;				//复位读指针结束 
		OV7725_RCK=1; 
		for(i=0;i<OV7725_WINDOW_HEIGHT;i++)
		{
			for(j=0;j<OV7725_WINDOW_WIDTH;j++)
			{
				GPIOB->CRL=0X88888888;
				OV7725_RCK=0;
				color=OV7725_DATA;	//读数据
				OV7725_RCK=1; 
				color<<=8;  
				OV7725_RCK=0;
				color|=OV7725_DATA;	//读数据
				OV7725_RCK=1;
				GPIOB->CRL=0X33333333;
				LCD_WR_DATA(color); 
			}
		}
		OV7725_CS=1;
		OV7725_RCK=0;
		OV7725_RCK=1;
		EXTI->PR=1<<15;
 		ov_sta=0;					//清零帧中断标记
		ov_frame++; 
		LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认扫描方向 
	} 
}
float TU=0.0;
float TU_value=0.0;
float TU_calibration=0.0;
float temp_data=0.0;
float K_Value=3047.19;
unsigned char  Tx[20];   //无线发送缓存


// ADC1转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue;

// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal;  
/**************TDS值采集函数***************/
void TU_Value_Conversion()
{
		TU =(float) ADC_ConvertedValue/4096*3.3; // 读取转换的AD值
	
		TU_calibration=-0.0192*(temp_data/10-25)+TU;  
	  TU_value=-865.68*TU_calibration + K_Value;
	
		if(TU_value<=0){TU_value=0;}
		if(TU_value>=3000){TU_value=3000;}
	
		/*显示TDS*/
		Tx[0]=(int)(TU_value)/1000+'0';
		Tx[1]=(int)(TU_value)%1000/100+'0';	
		Tx[2]=(int)(TU_value)%100/10+'0';
		Tx[3]=(int)(TU_value)%10+'0';
}

int main(void)
{	
	u8 sensor=0;
	u8 i;	
	delay_init();	    	 //延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(115200);
	LED_Init();		  		//初始化与LED连接的硬件接口
	LCD_Init();			   	//初始化LCD
	ADCx_Init();				//ADC 初始化
	usmart_dev.init(72);	//初始化USMART
  font_init();	 				//字库初始化
 	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(30,50,200,200,16,"Mini STM32");	
	LCD_ShowString(30,70,200,200,16,"OV7725_OV7670 TEST");	
	LCD_ShowString(30,90,200,200,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,200,16,"2019/06/02");  	 
	LCD_ShowString(30,130,200,200,16,"Use USMART To Set!");	 
  LCD_ShowString(30,150,200,200,16,"OV7725_OV7670 Init...");	  
	while(1)//初始化OV7725_OV7670
	{
		if(OV7725_Init()==0)
		{
			sensor=OV7725;
			LCD_ShowString(30,150,200,16,16,"OV7725 Init OK       ");				
			delay_ms(1500);
			//以下为摄像头的系统配置
			OV7725_Light_Mode(0);
			OV7725_Color_Saturation(0);
			OV7725_Brightness(0);
			OV7725_Contrast(0);
			OV7725_Special_Effects(0);
			OV7725_Window_Set(OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT,0);//QVGA模式输出
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
	TIM3_Int_Init(10000,7199);			//TIM3,10Khz计数频率,1秒钟中断									  
	EXTI15_Init();						//使能定时器捕获					 	 
 	LCD_Clear(BACK_COLOR);
	POINT_COLOR=BLACK;
	Show_Str(5,5,200,24,"识别物质：",24,0);
  Show_Str(5,29,200,24,"识别浓度：",24,0);
	POINT_COLOR=RED;
	while(1)
	{	
		if(sensor==OV7725)
			OV7725_camera_refresh();//摄像头屏幕刷新
		// if(Trace(&condition0, &result))//识别绿色为氧化铜
		// {
		// 	POINT_COLOR=GREEN;
		// 	Show_Str(125,5,200,24,"氧化铜",24,0);//氧化铜
		// 	TU_Value_Conversion();		//浓度的AD转换获取	
		// 	Show_Str(125,29,200,24,Tx,24,0);//数据存到Tx数组
		// 	POINT_COLOR=RED;
		// 	delay_ms(500);
		// }
		// else if(Trace(&condition1, &result))//识别蓝色为硫酸铝
		// {
		// 	POINT_COLOR=BLUE;
		// 	Show_Str(125,5,200,24,"硫酸铝",24,0);//硫酸铝
		// 	TU_Value_Conversion();	
		// 	Show_Str(125,29,200,24,Tx,24,0);
		// 	POINT_COLOR=RED;
		// 	delay_ms(500);
		// }
		// else if(Trace(&condition8, &result))//识别红色为铁离子
		// {
		// 	POINT_COLOR=RED;
		// 	Show_Str(125,5,200,24,"铁离子",24,0);//铁离子
		// 	TU_Value_Conversion();	
		// 	Show_Str(125,29,200,24,Tx,24,0);
		// 	POINT_COLOR=RED;
		// 	delay_ms(500);
		// }
		// else if(Trace(&condition5, &result))//识别黄色为其他
		// {
		// 	POINT_COLOR=YELLOW;
		// 	Show_Str(125,5,200,24,"其他  ",24,0);//其他
		// 	TU_Value_Conversion();	
		// 	Show_Str(125,29,200,24,Tx,24,0);
		// 	POINT_COLOR=RED;
		// 	delay_ms(500);
		// }
		// else
		// {
		// 	POINT_COLOR=BLACK;
		// 	Show_Str(125,5,200,24,"未识别",24,0);//未识别
		// 	Show_Str(125,29,200,24,"0000",24,0);
		// 	POINT_COLOR=RED;
		// }
 		if(i!=ov_frame)		//DS0闪烁.
		{
			i=ov_frame;
			LED0=!LED0;
 		}
	}   
}

