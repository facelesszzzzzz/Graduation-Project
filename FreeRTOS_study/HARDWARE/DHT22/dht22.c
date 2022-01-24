#include "dht22.h"
#include "delay.h"
#include "oled.h"
#include "string.h"
#include "beep.h"


static DHT22_t gDHT22;
DHT22_t *pDHT22 = &gDHT22;

/**********************************************************************
* 函数名称：DHT22_Rst
* 功能描述：复位
* 输入参数：无
* 返回值：无
* 补充说明：无
**********************************************************************/

void DHT22_Rst(void)    
{     

    DHT22_DQ_OUT=0; 				//拉低DQ
    delay_ms(20);    				//拉低至少18ms
    DHT22_DQ_OUT=1; 				//DQ=1 
	delay_us(30);      				//主机拉高20~40us
}

/**********************************************************************
* 函数名称：DHT22_Check
* 功能描述：等待DHT22的回应
* 输入参数：无
* 返回值：1（未存在DHT22）、0（存在）
* 补充说明：无
**********************************************************************/

u8 DHT22_Check(void)    
{   
	u8 retry=100;
    while ((!DHT22_DQ_IN)&&retry)	//DHT22会拉低40~80us
    {
		retry--;
		delay_us(1);
	}  
	if(retry<1) return 1;
	else retry=100;
	while (DHT22_DQ_IN&&retry)		//DHT22拉低后会再次拉高40~80us
	{
		retry--;
		delay_us(1);
	}
	if(retry<1) 
		return 1;     
	return 0;
}

/**********************************************************************
* 函数名称：DHT22_Read_Bit
* 功能描述：从DHT22读取一个位
* 输入参数：无
* 返回值：1、0
* 补充说明：无
**********************************************************************/
u8 DHT22_Read_Bit(void)  
{

	u8 dat;
	u8 retry=100;
	while((!DHT22_DQ_IN)&&retry)	//等待变为高电平
	{
		retry--;
		delay_us(1);
	}
	retry=100;
	dat=0;
	delay_us(30);					//等待30us
	if(DHT22_DQ_IN) 
		dat=1;    
	while(DHT22_DQ_IN&&retry)		//等待变低电平
	{
		retry--;
		delay_us(1);
	}
	return dat;
}

/**********************************************************************
* 函数名称：DHT22_Read_Byte
* 功能描述：从DHT22读取一个字节
* 输入参数：无
* 返回值：读到的数据
* 补充说明：无
**********************************************************************/

u8 DHT22_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
	    dat<<=1; 
	    dat|=DHT22_Read_Bit();
    }     
    return dat;
}
/**********************************************************************
* 函数名称：DHT22_RH2Ten
* 功能描述：将湿度整数位32bit数据的16进制转换成10进制
* 输入参数：RH_data_H、RH_data_L
* 返回值：温度整型数据
* 补充说明：无
**********************************************************************/
u32 DHT22_RH2Ten(u8 RH_data_H,u8 RH_data_L)
{
	//湿度提取
	u8 dht22_RH_HH4;//高8位前4位
	u8 dht22_RH_HL4;//高8位后4位
	u8 dht22_RH_LH4;//低8位前4位
	u8 dht22_RH_LL4;//低8位后4位
	dht22_RH_HH4 = RH_data_H >> 4;
	dht22_RH_HL4 = RH_data_H & 0x0f;
	dht22_RH_LH4 = RH_data_L >> 4;
	dht22_RH_LL4 = RH_data_L & 0x0f;
	return dht22_RH_HH4 * 4096 + dht22_RH_HL4 * 256 + dht22_RH_LH4 * 16 + dht22_RH_LL4;
}
/**********************************************************************
* 函数名称：DHT22_Tem2Ten
* 功能描述：将温度整数位32bit数据的16进制转换成10进制
* 输入参数：Tem_data_H、Tem_data_L
* 返回值：温度整型数据
* 补充说明：无
**********************************************************************/
u32 DHT22_Tem2Ten(u8 Tem_data_H, u8 Tem_data_L)
{
	//温度提取
	u8 dht22_Tem_HH4;//高8位前4位
	u8 dht22_Tem_HL4;//高8位后4位
	u8 dht22_Tem_LH4;//低8位前4位
	u8 dht22_Tem_LL4;//低8位后4位
	dht22_Tem_HH4 = Tem_data_H >> 4;
	dht22_Tem_HL4 = Tem_data_H & 0x0f;
	dht22_Tem_LH4 = Tem_data_L >> 4;
	dht22_Tem_LL4 = Tem_data_L & 0x0f;
	return dht22_Tem_HH4 * 4096 + dht22_Tem_HL4 * 256 + dht22_Tem_LH4 * 16 + dht22_Tem_LL4;
}
/**********************************************************************
* 函数名称：RH_Tem_U32toString
* 功能描述：将DHT22的整型数据转换成字符串数据
* 输入参数：RH、Tem
* 返回值：无
* 补充说明：无
**********************************************************************/
void RH_Tem_U32toString(u32 RH,u32 Tem)
{
	u8 i = 4;
	while(i)
	{	
		i--;
		if(i == 2)			//遇到小数点位置跳过
		{
			continue;
		}
		//将数据从最低位往前存入字符串
		pDHT22->RH_Str[i] = RH % 10 + 0x30;
		pDHT22->Tem_Str[i] = Tem % 10 + 0x30;
//		send_Tem[i+4] = dht22_Tem_str[i];
//		send_RH[i+3] = dht22_RH_str[i];
		RH /= 10;
		Tem /= 10;
	}
}


DHT22_t *Get_Dht22Value(void)
{
    return pDHT22;
}

/**********************************************************************
* 函数名称：DHT22_Read_Data
* 功能描述：DHT22读取一次数据
* 输入参数：*temp(待定)
* 返回值：1（失败）、0（正常）
* 补充说明：温度值(范围:0~50°)、湿度值(范围:20%~90%)
**********************************************************************/

u8 DHT22_Read_Data()    
{        
	u8 buf[5];
	u8 i;
	DHT22_Rst();
	if(DHT22_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT22_Read_Byte();
		}
//		memcpy(pDHT22,buf,sizeof(stDHT22));
		//校验和累加时存在相当概率会超出8位(即值大于1个char,故做强制类型转换，只考虑低8位)
		if((unsigned char)(buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])  
		{
			pDHT22->RH_Value = DHT22_RH2Ten(buf[0],buf[1]);		                    //湿度数据转换整型数字量
			pDHT22->Tem_Value = DHT22_Tem2Ten(buf[2],buf[3]);	                    //温度数据转换整型数字量
//			DHT22_LIMIT(gState);
			RH_Tem_U32toString(pDHT22->RH_Value,pDHT22->Tem_Value);							//将温湿度整型数据转换成字符串数据	
//			OLED_ShowString(20, 4, pDHT22->RH_Str, 16);
//			OLED_ShowString(84, 4, pDHT22->Tem_Str, 16);
			return 0;
		}
	}
	return 1;   
}
/**********************************************************************
* 函数名称：DHT22_LIMIT
* 功能描述：对DHT22温湿度进行比较，若超限则报警
* 输入参数：
* 返回值：无
* 补充说明：无
**********************************************************************/
void DHT22_LIMIT()
{
	if(pDHT22->RH_Value > DHT22_HUMI_LIMIT * 10)
        BEEP_Config(Beep_On);
	else
		BEEP_Config(Beep_Off);
}
 
/**********************************************************************
* 函数名称：DHT22_Init
* 功能描述：DHT22端口配置初始化并检测DHT22的存在
* 输入参数：无
* 返回值：1（不存在）、0（存在）
* 补充说明：无
**********************************************************************/
u8 DHT22_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = DHT22_GPIO_IO;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(DHT22_GPIO, &GPIO_InitStructure);
	DHT22_Rst();
	return DHT22_Check();
}
