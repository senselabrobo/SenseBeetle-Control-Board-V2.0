#ifndef __MOTOR_TASK_H
#define __MOTOR_TASK_H

#include "main.h"

void MotorTask_Handle(void const * argument);

void MotorTask_SetMotorPidSpd(uint8_t id, int16_t spd);			//(API)���õ���ٶ�id0-3��spd -5000-5000
void MotorTask_MotorPowerOffAll(void);											//(API)���е�������ϵ�


#endif	//__MOTOR_TASK_H

