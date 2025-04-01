#include "BspTask.h"
#include "BspInfo.h"

#include "stdio.h"
#include "string.h"
#include "usbd_cdc_if.h"

static char tempBuf[40] = {0};

void BspTask_Handle(void const * argument)
{
	static uint32_t lastRunTime = 0;
	static uint32_t lastSendBackVolTime = 0;
	
	if(HAL_GetTick() - lastRunTime < 10)return;
	lastRunTime = HAL_GetTick();
	
	ScanBsp_PwrVol();
	
	if(HAL_GetTick() - lastSendBackVolTime > 1000)
	{
		lastSendBackVolTime = HAL_GetTick();
		printf(">>TH_SlavePad:Vol:%.2f\r\n", BspInfo.PowerVol);
		
		memset(tempBuf, 0, sizeof(tempBuf));
		snprintf(tempBuf,26,">>TH_SlavePad:Vol:%.2f\r\n", BspInfo.PowerVol);
		CDC_Transmit_FS((uint8_t*)tempBuf, strlen(tempBuf));
	}
}

float BspTask_GetPowerVol(void)
{
	return BspInfo.PowerVol;
}



