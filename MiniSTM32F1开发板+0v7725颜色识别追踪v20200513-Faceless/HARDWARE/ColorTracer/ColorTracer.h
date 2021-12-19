#ifndef __COLORTRACER_H
#define __COLORTRACER_H

#include "stm32f10x.h"


#define IMG_X 0							  //ͼƬx����
#define IMG_Y 60               //ͼƬy����
#define IMG_W 240             //ͼƬ���
#define IMG_H 260            //ͼƬ�߶�

#define ALLOW_FAIL_PER 3    //�ݴ���
#define ITERATER_NUM   8    //��������

typedef struct							//�ж�ΪĿ�������
{
	unsigned char H_MIN;			//Ŀ����Сɫ��
	unsigned char H_MAX;			//Ŀ�����ɫ��
	
	unsigned char S_MIN;			//Ŀ����С���Ͷ�
	unsigned char S_MAX;			//Ŀ����󱥺Ͷ�
	
	unsigned char L_MIN;			//Ŀ����С����
	unsigned char L_MAX;			//Ŀ���������
	
	unsigned int WIDTH_MIN;		//Ŀ����С���
	unsigned int HEIGHT_MIN;	//Ŀ����С�߶�
	
	unsigned int WIDTH_MAX;		//Ŀ�������
	unsigned int HEIGHT_MAX;	//Ŀ�����߶�
}TARGET_CONDITION;

typedef struct				//���
{
	unsigned int x;			//Ŀ��x����
	unsigned int y;			//Ŀ��y����
	unsigned int w;			//Ŀ����
	unsigned int h;			//Ŀ��߶�
}RESULT;

extern RESULT result;
extern TARGET_CONDITION condition0;
extern TARGET_CONDITION condition1;
extern TARGET_CONDITION condition2;
extern TARGET_CONDITION condition3;
extern TARGET_CONDITION condition4;
extern TARGET_CONDITION condition5;
extern TARGET_CONDITION condition6;
extern TARGET_CONDITION condition7;
extern TARGET_CONDITION condition8;


int Trace(const TARGET_CONDITION* condition, RESULT* result_final);


#endif


