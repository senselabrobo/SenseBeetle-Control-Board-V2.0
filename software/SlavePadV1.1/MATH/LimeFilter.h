#ifndef __LIME_FILTER_H
#define __LIME_FILTER_H

#include "main.h"

#define MIDDLE_DATA_MAX            20          	//��ֵ�˲��ɼ����ݸ���
typedef struct
{
    float data[MIDDLE_DATA_MAX];                //�洢����
    float result;                               //�洢���μ�����
}middleFilter_t;                                //��ֵ�˲�

                                                //��ֵ�˲����㺯�����������ֵ�������˲����
float middleFilter_Cal(middleFilter_t* fil, float in_data);


typedef struct
{
  float k_History;                              //�˲�ϵ������ʷȨ�أ�
  float input;                                  //�˴β���ֵ
  float result;                                 //���
}LowPassFilter_t;

void LowPassFilter_Init(LowPassFilter_t * initPin,float k);
float LowPassFilter_Cal(LowPassFilter_t * FilterPin, float input);


#endif //__LIME_FILTER_H
