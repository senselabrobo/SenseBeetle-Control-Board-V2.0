#include "MotorTask.h"
#include "stdio.h"
#include "DJI_Motor.h"


void MotorTask_SetMotorPidSpd(uint8_t id, int16_t spd)
{
	DJI_Motor[id].spdPidSet = spd;
	DJI_Motor[id].Mode = Mode_SpdPID;
}
void MotorTask_MotorPowerOffAll(void)
{
	for(uint8_t i = 0; i < 4; i++)
	{
		DJI_Motor[i].Mode = Mode_PowerOff;
	}
}


void MotorTask_Handle(void const * argument)
{
	static uint32_t lastRunTime = 0;
	
	if(HAL_GetTick() - lastRunTime < 10)return;
	lastRunTime = HAL_GetTick();
	
	DJI_Motor_PID_Cal();
}






