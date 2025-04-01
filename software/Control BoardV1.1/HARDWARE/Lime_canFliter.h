#ifndef __LIME_CAN_FLITER_H
#define __LIME_CAN_FLITER_H

#include "main.h"


typedef struct
{
	int32_t last_ecd, ecd;
	int16_t speed_rpm;
	int16_t given_current;
	int32_t temperate;
	uint32_t SnapTime;						//�ϴβ���ʱ��
	uint32_t ElapsTime;						//���β������ʱ��
}DJI_MotorMea_t;


void Lime_can_filter_init(void);
void CAN_cmd_chassis(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
void DJI_Motor_Decode(uint16_t StdID, uint8_t buf[8]);

uint8_t Lime_CanIsOnline(void);                     //�ж�CAN�Ƿ�����

#endif  //__LIME_CAN_FLITER_H

