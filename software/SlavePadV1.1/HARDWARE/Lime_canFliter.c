#include "Lime_canFliter.h"
#include "DJI_Motor.h"
#include "can.h"

uint32_t Lime_LastCanRecvTime = 0;

void Lime_can_filter_init(void)
{
  CAN_FilterTypeDef can_filter_st;
  can_filter_st.FilterActivation = ENABLE;
  can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
  can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
  can_filter_st.FilterIdHigh = 0x0000;
  can_filter_st.FilterIdLow = 0x0000;
  can_filter_st.FilterMaskIdHigh = 0x0000;
  can_filter_st.FilterMaskIdLow = 0x0000;
  can_filter_st.FilterBank = 0;
  can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
  HAL_CAN_ConfigFilter(&hcan, &can_filter_st);
  HAL_CAN_Start(&hcan);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

CAN_TxHeaderTypeDef  chassis_tx_message;
uint8_t       chassis_can_send_data[8];
void CAN_cmd_chassis(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
    uint32_t send_mail_box;
    chassis_tx_message.StdId = 0x200;
    chassis_tx_message.IDE = CAN_ID_STD;
    chassis_tx_message.RTR = CAN_RTR_DATA;
    chassis_tx_message.DLC = 0x08;
    chassis_can_send_data[0] = motor1 >> 8;
    chassis_can_send_data[1] = motor1;
    chassis_can_send_data[2] = motor2 >> 8;
    chassis_can_send_data[3] = motor2;
    chassis_can_send_data[4] = motor3 >> 8;
    chassis_can_send_data[5] = motor3;
    chassis_can_send_data[6] = motor4 >> 8;
    chassis_can_send_data[7] = motor4;

    HAL_CAN_AddTxMessage(&hcan, &chassis_tx_message, chassis_can_send_data, &send_mail_box);
}



uint8_t Lime_CanIsOnline(void)                      //ÅÐ¶ÏCANÊÇ·ñÉÏÏß
{
  if(HAL_GetTick() - Lime_LastCanRecvTime > 500)return 0;
  return 1;
}

void DJI_Motor_Decode(uint16_t StdID, uint8_t buf[8])
{
	if(StdID < 0x201)return;
	if(StdID > 0x204)return;
	
	DJI_Motor[StdID - 0x201].MeaInfo.last_ecd = DJI_Motor[StdID - 0x201].MeaInfo.ecd;  
  DJI_Motor[StdID - 0x201].MeaInfo.ecd = (uint16_t)(buf[0] << 8 | buf[1]);           
  DJI_Motor[StdID - 0x201].MeaInfo.speed_rpm = (int16_t)(buf[2] << 8 | buf[3]);     
  DJI_Motor[StdID - 0x201].MeaInfo.given_current = (uint16_t)(buf[4] << 8 | buf[5]);  
  DJI_Motor[StdID - 0x201].MeaInfo.temperate = buf[6];        
	DJI_Motor[StdID - 0x201].MeaInfo.ElapsTime =  HAL_GetTick() - DJI_Motor[StdID - 0x201].MeaInfo.SnapTime;
	DJI_Motor[StdID - 0x201].MeaInfo.SnapTime = HAL_GetTick();
	Lime_LastCanRecvTime = HAL_GetTick();
}


