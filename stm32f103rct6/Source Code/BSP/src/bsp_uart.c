#include <stdio.h>
#include <string.h>
#include <stdarg.h>	
#include "bsp_uart.h"
#include "bsp_gpio.h"

static Uart_Data_t gUartx_Data[3];
static Uart_Data_t *pUart1_Data = &gUartx_Data[0];
static Uart_Data_t *pUart2_Data = &gUartx_Data[1];
static Uart_Data_t *pUart3_Data = &gUartx_Data[2];

uint16_t USART_RX_STA;         		//接收状态标记

Uart_Data_t *GetUart_Data_t(uint8_t Uartx)
{
	switch(Uartx)
	{
		case Uart1_NUM:
		#if Uart1_EN
			return pUart1_Data;
		#endif
		case Uart2_NUM:
		#if Uart2_EN
			return pUart2_Data;
		#endif
		case Uart3_NUM:
		#if Uart3_EN
			return pUart3_Data;
		#endif
		default:
			break;
	}
	return NULL;
}

static void Uart1_Init(uint32_t bound)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);						
 	GPIO_PinInit(GPIO_PA9, GPIO_Mode_AF_PP);
	GPIO_PinInit(GPIO_PA10, GPIO_Mode_IN_FLOATING);
	
	//Usart1 NVIC 配置
  	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = Uart1_PreemptionPriority;	//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = Uart1_SubPriority;					//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;										//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);														//根据指定的参数初始化VIC寄存器
  
   	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;										//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;							//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;								//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;									//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//收发模式

  	USART_Init(USART1, &USART_InitStructure); 											//初始化串口1
  	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);										//开启串口接受中断
  	USART_Cmd(USART1, ENABLE); 															//使能串口1 
}

static void Uart2_Init(uint32_t bound)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); 
	GPIO_PinInit(GPIO_PA2, GPIO_Mode_AF_PP);
	GPIO_PinInit(GPIO_PA3, GPIO_Mode_IN_FLOATING);

	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = Uart2_PreemptionPriority;	//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = Uart2_SubPriority;					//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;										//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);														//根据指定的参数初始化VIC寄存器

	USART_InitStructure.USART_BaudRate = bound;											//波特率一般设置为115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;							//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;								//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;									//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//收发模式
  
	USART_Init(USART2, &USART_InitStructure); 											//初始化串口2
	//使能接收中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);										//开启中断  	
	USART_Cmd(USART2, ENABLE);                    //使能串口
}

static void Uart3_Init(uint32_t bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); 							
	GPIO_PinInit(GPIO_PB10, GPIO_Mode_AF_PP);
   	GPIO_PinInit(GPIO_PB11, GPIO_Mode_IN_FLOATING);

	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = Uart3_PreemptionPriority;	//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = Uart3_SubPriority;					//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;										//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);														//根据指定的参数初始化VIC寄存器
	
	USART_InitStructure.USART_BaudRate = bound;											//波特率一般设置为115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;							//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;								//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;									//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//收发模式
  
	USART_Init(USART3, &USART_InitStructure); 											//初始化串口3
	//使能接收中断
  	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);										//开启中断  	
	USART_Cmd(USART3, ENABLE);                    										//使能串口
}

void BSP_Uart_Init(void)
{
#if	Uart1_EN
	Uart1_Init(9600);
#endif

#if	Uart2_EN
	Uart2_Init(9600);
#endif

#if	Uart2_EN
	Uart3_Init(9600);
#endif
}

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

void USART1_IRQHandler(void)
{   
	uint8_t Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					pUart1_Data->RxBuf[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USARTx_RX_LENGTH-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
    } 	 
}   

//void USART1_IRQHandler(void)
//{      	
//	u8 res;	      
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
//	{	 
//		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
//		res = USART_ReceiveData(USART2);		
//		if((USART2_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
//		{ 
//			if(USART2_RX_STA<USART2_MAX_RECV_LEN)	//还可以接收数据
//			{
//				TIM_SetCounter(TIM7,0);//计数器清空          				//计数器清空
//				if(USART2_RX_STA==0) 				//使能定时器7的中断 
//				{
//					TIM_Cmd(TIM7,ENABLE);//使能定时器7
//				}
//				USART2_RX_BUF[USART2_RX_STA++]=res;	//记录接收到的值
//			}else 
//			{
//				USART2_RX_STA|=1<<15;				//强制标记接收完成
//			} 
//		}
//	}  		 
//}   

//void USART3_IRQHandler(void)
//{      	
//	u8 res;	      
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
//	{	 
//		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
//		res = USART_ReceiveData(USART2);		
//		if((USART2_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
//		{ 
//			if(USART2_RX_STA<USART2_MAX_RECV_LEN)	//还可以接收数据
//			{
//				TIM_SetCounter(TIM7,0);//计数器清空          				//计数器清空
//				if(USART2_RX_STA==0) 				//使能定时器7的中断 
//				{
//					TIM_Cmd(TIM7,ENABLE);//使能定时器7
//				}
//				USART2_RX_BUF[USART2_RX_STA++]=res;	//记录接收到的值
//			}else 
//			{
//				USART2_RX_STA|=1<<15;				//强制标记接收完成
//			} 
//		}
//	}  		 
//}   

#if Uart3_Printf_EN
//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)  
{  
	static uint16_t i,j;
	Uart_Data_t *spUart3_Data = GetUart_Data_t(Uart3_NUM);
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)spUart3_Data->TxBuf,fmt,ap);
	va_end(ap);
	i=strlen((const char*)spUart3_Data->TxBuf);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART3,spUart3_Data->TxBuf[j]);
	} 
}
#endif
