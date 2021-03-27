#include "run.h"
#include "warning.h"
#include "bsp.h"

int main(void)
{		
	BSP_Init();
	System_Init();
  	while(1)
	{
		System_Running();
	}	 
}	 
 
