#include "run.h"
#include "warning.h"
#include "bsp.h"

int main(void)
{		
	BSP_Init();					//硬件底层初始化
	System_Init();			//模块配置初始化
  	while(1)
	{
		System_Running();	//后台系统的运行
	}	 
}	 
 
