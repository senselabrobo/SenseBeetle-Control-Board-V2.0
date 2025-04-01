#include "DJI_Motor.h"
#include "LimeFilter.h"


DJI_Motor_t DJI_Motor[4] = {0};


void DJI_Motor_Init(void)
{
	for(uint8_t i = 0; i < 4; i++)
	{
		DJI_Motor[i].spdPidSet = 0;
		DJI_Motor[i].spdRawSet = 0;
		YQY_ResetPID(&DJI_Motor[i].SpdPID,2, 0.15, 0);
		YQY_ResetPID_Limit(&DJI_Motor[i].SpdPID,1, 2000, 5000);
		LowPassFilter_Init(&DJI_Motor[i].OutLowPassFilter,0.3);
		DJI_Motor[i].Mode = Mode_PowerOff;
	}
}


void DJI_Motor_PID_Cal(void)
{
	for(uint8_t i = 0; i < 4; i++)
	{
		if(DJI_Motor[i].Mode == Mode_SpdPID)		//Mode_SpdPID
		{
			YQY_PID_Cal(&DJI_Motor[i].SpdPID, DJI_Motor[i].MeaInfo.speed_rpm, DJI_Motor[i].spdPidSet);
			DJI_Motor[i].spdRawSet = DJI_Motor[i].SpdPID.out;
			DJI_Motor[i].spdRawSet = LowPassFilter_Cal(&DJI_Motor[i].OutLowPassFilter, DJI_Motor[i].spdRawSet);
		}
		else if(DJI_Motor[i].Mode == Mode_Raw)	//Mode_Raw
		{
			DJI_Motor[i].spdRawSet = LowPassFilter_Cal(&DJI_Motor[i].OutLowPassFilter, DJI_Motor[i].spdRawSet);
		}
		else																		//Mode_PowerOff
		{
			DJI_Motor[i].spdRawSet = 0;
		}
	}
	
	if(Lime_CanIsOnline())										//MotorOffLine
	{
		CAN_cmd_chassis(DJI_Motor[0].spdRawSet, DJI_Motor[1].spdRawSet, DJI_Motor[2].spdRawSet, DJI_Motor[3].spdRawSet);
	}
	else
	{
		CAN_cmd_chassis(0, 0, 0, 0);
	}
}






