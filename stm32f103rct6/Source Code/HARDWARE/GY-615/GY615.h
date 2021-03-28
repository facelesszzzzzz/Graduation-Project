#ifndef __GY615_H__
#define __GY615_H__
#include "sys.h"


#define	GY615_HEAD				0xA4
#define	GY615_READ				0x03
#define GY615_WRITE				0x06
#define GY615_REGISTER			0x07
#define GY615_REGISTERNUM		0x07

#define GY615_REC_LENGTH		12

#define GY615_READ_CHECKSUM		0xB5

void Search_GY615_Data(void);		//查询GY615数据
void GY615_Handle(void);			//GY615数据处理
#endif

