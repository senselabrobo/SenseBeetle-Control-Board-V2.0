#ifndef __BSP_INFO_H
#define __BSP_INFO_H

#include "main.h"
#include "LimeFilter.h"

typedef struct
{
	uint32_t ADC_RawVal;
	float PowerVol;
	middleFilter_t VolFilter;
	
	uint16_t PowVol_CALI_24V;
	uint16_t PowVol_CALI_22V;
	
	float Vol_k, Vol_b;						//vol = kx + b
	
}BspInfo_t;

extern BspInfo_t BspInfo;

void ScanBsp_Init(void);									//校准初始化
void ScanBsp_refreshVolAdcKB(void);			//更新ADC校准参数
float ScanBsp_PwrVol(void);								//更新24V电源电压

void BspInfo_LED_Ctrl(char led, uint8_t status);


#endif	//__BSP_INFO_H
