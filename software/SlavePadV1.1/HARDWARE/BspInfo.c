#include "BspInfo.h"
#include "adc.h"
#include "LimeFilter.h"
#include "stdio.h"

BspInfo_t BspInfo;


#define VOL_ADC_CALIBRATE_24V				2850



void ScanBsp_Init(void)
{	
	//更新ADC校准参数
	BspInfo.PowVol_CALI_24V = 2845;					//24V输入下实际ADC读取到的数值
	BspInfo.PowVol_CALI_22V = 2610;					//22V输入下实际ADC读取到的数值
	ScanBsp_refreshVolAdcKB();							//计算校准系数
	
	
	HAL_ADC_Start(&hadc);
	if(HAL_ADC_PollForConversion(&hadc, 20) != HAL_OK)
	{
		return;
	}
	uint32_t tempADCVal = HAL_ADC_GetValue(&hadc);
	for(uint32_t i = 0; i < 20; i++)				//将数据填满全部缓冲区
		middleFilter_Cal(&BspInfo.VolFilter, tempADCVal);
}


void ScanBsp_refreshVolAdcKB(void)
{
	BspInfo.Vol_k = (2.0f) / (float)(BspInfo.PowVol_CALI_24V - BspInfo.PowVol_CALI_22V);
	BspInfo.Vol_b = 24.0f - BspInfo.Vol_k * BspInfo.PowVol_CALI_24V;
	
	printf("ScanBsp_refreshVolAdcKB: y  = %.2f x + %.2f\n", BspInfo.Vol_k, BspInfo.Vol_b);
}

float ScanBsp_PwrVol(void)
{
	HAL_ADC_Start(&hadc);
	
	if(HAL_ADC_PollForConversion(&hadc, 20) != HAL_OK)
	{
		return 0;
	}
	
	BspInfo.ADC_RawVal = HAL_ADC_GetValue(&hadc);
	BspInfo.ADC_RawVal = middleFilter_Cal(&BspInfo.VolFilter, BspInfo.ADC_RawVal);
	BspInfo.PowerVol = BspInfo.Vol_k * (float)BspInfo.ADC_RawVal + BspInfo.Vol_b;
//	printf("RawVal:%d Vol:%.2f\n", BspInfo.ADC_RawVal,BspInfo.PowerVol);
	
	return BspInfo.PowerVol;
}

void BspInfo_LED_Ctrl(char led, uint8_t status)
{
	GPIO_PinState PinState;
	
	if(status)
		PinState = GPIO_PIN_RESET;
	else
		PinState = GPIO_PIN_SET;
	
	switch(led)
	{
		case 'r':
		case 'R':
			HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, PinState);
			break;
		case 'g':
		case 'G':
			HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, PinState);
			break;
		case 'b':
		case 'B':
			HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, PinState);
			break;
	}
}








