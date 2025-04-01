#include "ConnTask.h"
#include "stdio.h"
#include "HYQY_PP_Pro.h"
#include "MotorTask.h"
#include "BspTask.h"
#include "usart.h"
#include "BspInfo.h"
#include "string.h"
#include "usbd_cdc_if.h"

HYQY_RECV_Typedef UART1_PP_RecvInfo;
HYQY_RECV_Typedef UART3_PP_RecvInfo;
HYQY_RECV_Typedef USB_PP_RecvInfo;
uint8_t uart1TempData;
uint8_t uart3TempData;

static char tempBuf[60] = {0};

void ConnTask_Init(void)
{
	HAL_UART_Receive_IT(&huart1,&uart1TempData, 1);
	HAL_UART_Receive_IT(&huart3,&uart3TempData, 1);
}

static void SetPPDataToMotor(int32_t recvData[6])
{
	switch(recvData[0])
	{
		case 0:
			MotorTask_MotorPowerOffAll();
			break;
		case 1:
			MotorTask_SetMotorPidSpd(0,recvData[1]);
			MotorTask_SetMotorPidSpd(1,recvData[2]);
			MotorTask_SetMotorPidSpd(2,recvData[3]);
			MotorTask_SetMotorPidSpd(3,recvData[4]);
			break;
		default:
			MotorTask_MotorPowerOffAll();
			break;
	}
	BspInfo_LED_Ctrl('r', 0);
	BspInfo_LED_Ctrl('g', 1);
	BspInfo_LED_Ctrl('b', 0);
}

void ConnTask_Handle(void const * argument)
{	
	static uint32_t lastRunTime = 0;
	static uint32_t lastRecvPPTime = 0;
	
	if(HAL_GetTick() - lastRecvPPTime > 1000)						//超过时间自动关闭所有电机
	{
		MotorTask_MotorPowerOffAll();
		BspInfo_LED_Ctrl('r', 1);
		BspInfo_LED_Ctrl('g', 0);
		BspInfo_LED_Ctrl('b', 0);
	}
	if(BspTask_GetPowerVol() < 22.0f)										//电池电压过低自动关闭所有电机
	{
		MotorTask_MotorPowerOffAll();
		BspInfo_LED_Ctrl('r', 1);
		BspInfo_LED_Ctrl('g', 0);
		BspInfo_LED_Ctrl('b', 1);
	}
	
	if(HAL_GetTick() - lastRunTime < 100)return;
	lastRunTime = HAL_GetTick();
	
	if(IsReceiveDataOK(&UART1_PP_RecvInfo))             //如果收到数据
	{
		printf("UART1-RecvData\n");
		lastRecvPPTime = HAL_GetTick();
		SetPPDataToMotor(UART1_PP_RecvInfo.recvRes);
		ShowDecodeResult(&UART1_PP_RecvInfo);             //展示接收结果
	}
	if(IsReceiveDataOK(&UART3_PP_RecvInfo))             //如果收到数据
	{
		printf("UART3-RecvData\n");
		lastRecvPPTime = HAL_GetTick();
		SetPPDataToMotor(UART1_PP_RecvInfo.recvRes);
		ShowDecodeResult(&UART3_PP_RecvInfo);             //展示接收结果
	}
	if(IsReceiveDataOK(&USB_PP_RecvInfo))            	 	//如果收到数据
	{
		printf("USB-RecvData\n");
		lastRecvPPTime = HAL_GetTick();
		SetPPDataToMotor(USB_PP_RecvInfo.recvRes);
		ShowDecodeResult(&USB_PP_RecvInfo);             	//展示接收结果
		
		memset(tempBuf, 0, sizeof(tempBuf));
		snprintf(tempBuf,55,">>USBrecvOK:%d,%d,%d,%d,%d\n", USB_PP_RecvInfo.recvRes[0],
																												USB_PP_RecvInfo.recvRes[1],
																												USB_PP_RecvInfo.recvRes[2],
																												USB_PP_RecvInfo.recvRes[3],
																												USB_PP_RecvInfo.recvRes[4]);
		CDC_Transmit_FS((uint8_t*)tempBuf, strlen(tempBuf));
	}
}


