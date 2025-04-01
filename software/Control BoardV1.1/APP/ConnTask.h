#ifndef __CONN_TASK_H
#define __CONN_TASK_H

#include "main.h"
#include "HYQY_PP_Pro.h"

void ConnTask_Init(void);
void ConnTask_Handle(void const * argument);

extern HYQY_RECV_Typedef UART1_PP_RecvInfo;
extern HYQY_RECV_Typedef UART3_PP_RecvInfo;
extern HYQY_RECV_Typedef USB_PP_RecvInfo;
extern uint8_t uart1TempData;
extern uint8_t uart3TempData;

#endif	//__CONN_TASK_H


