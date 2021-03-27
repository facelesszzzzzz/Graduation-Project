#ifndef __MQ2_H__
#define __MQ2_H__
#include "sys.h"
#include "datatype.h"

#define MQ2_GPIO			GPIOA
#define MQ2_GPIO_IO			GPIO_Pin_0
#define MQ2_ADC				ADC1
#define MQ2_ADC_CHANNEL		ADC_Channel_0		
#define MQ2_ADC_IO			0

#define MQ2PPM_LIMIT			2000

void MQ2_Init(void);
void MQ2_Handle(void);
void MQ2_LIMIT(Byte8 data);



#endif
