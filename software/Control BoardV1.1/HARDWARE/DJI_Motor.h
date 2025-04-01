#ifndef __DJI_MOTOR_H
#define __DJI_MOTOR_H

#include "main.h"
#include "Lime_canFliter.h"
#include "LimePID.h"
#include "LimeFilter.h"

typedef enum
{
	Mode_PowerOff = 0,
	Mode_Raw = 1,
	Mode_SpdPID = 2,
	
}DJI_Motor_Mode_t;


typedef struct
{
	DJI_MotorMea_t MeaInfo;			//(auto)
	yqyPid_t SpdPID;						//(auto)
	int16_t spdPidSet;					//(user)Mode == Mode_SpdPID
	int16_t spdRawSet;					//(user/auto)(user)Mode == Mode_Raw	(auto)Mode == Mode_SpdPID
															//(auto)
	LowPassFilter_t OutLowPassFilter;
	
	DJI_Motor_Mode_t Mode;			//(user)change Mode
}DJI_Motor_t;


extern DJI_Motor_t DJI_Motor[4];

void DJI_Motor_Init(void);
void DJI_Motor_PID_Cal(void);


#endif	//__DJI_MOTOR_H
