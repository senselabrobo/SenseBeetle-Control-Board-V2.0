#ifndef __LIME_FILTER_H
#define __LIME_FILTER_H

#include "main.h"

#define MIDDLE_DATA_MAX            20          	//中值滤波采集数据个数
typedef struct
{
    float data[MIDDLE_DATA_MAX];                //存储数据
    float result;                               //存储本次计算结果
}middleFilter_t;                                //中值滤波

                                                //中值滤波计算函数，传入测量值，返回滤波结果
float middleFilter_Cal(middleFilter_t* fil, float in_data);


typedef struct
{
  float k_History;                              //滤波系数（历史权重）
  float input;                                  //此次测量值
  float result;                                 //结果
}LowPassFilter_t;

void LowPassFilter_Init(LowPassFilter_t * initPin,float k);
float LowPassFilter_Cal(LowPassFilter_t * FilterPin, float input);


#endif //__LIME_FILTER_H
