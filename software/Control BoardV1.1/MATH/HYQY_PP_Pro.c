#include "HYQY_PP_Pro.h"
#include "main.h"
#include "usart.h"
#include "stdio.h"
#include "math.h"

HYQY_RECV_Typedef HYQY_recvInfo;

/* 初始化接收（***需移植***） */
void HYQY_PP_Recv_Init(HYQY_RECV_Typedef* recv_Type)
{
	uint8_t i = 0;
	for(i =0;i < RECV_BUFFER_MAX; i++)
	{
		recv_Type->recvBuff[i] = 0;
	}
	for(i =0;i < RECV_RES_MAX; i++)
	{
		recv_Type->recvRes[i] = 0;
	}
  recv_Type->recvNum = 0;                           //清空接收的字节个数
  recv_Type->recvPin_Now = 0;
  recv_Type->findBegin = 0;
  recv_Type->findEnd = 0;
  recv_Type->DataIsOK = 0;
  recv_Type->LastRecv = 0;
}

static inline uint8_t checkLegalityFailed(char x)
{
  if(x == '-')return 0;
  if(x == ',')return 0;
  if((x == RECV_BEG1) || (x == RECV_BEG2))return 0;
  if((x == RECV_END1) || (x == RECV_END2))return 0;
  if((x >= '0') && (x <= '9'))return 0;
  return 1;
}

/* 放入接收中断内，用于接收一次字符（***需移植***）  */
void HYQY_PP_Recv_Handle(HYQY_RECV_Typedef* recv_Type,uint8_t chr)
{
  //找到头
  if((recv_Type->LastRecv == RECV_BEG1) && (chr == RECV_BEG2))   
  {
    recv_Type->findBegin = 1;                       //标志位置1
    recv_Type->LastRecv = 0;                        //清空上次记录
    return;                                         //直接结束本次
  }
  if(recv_Type->findBegin)                          //已经出现头
  {                                                 //将结果存到缓冲区
#if (LEGALITY_CHECK)
    if(checkLegalityFailed(chr))                    //检测到非法字符
    {
      recv_Type->findBegin = 0;
      recv_Type->recvPin_Now = 0;
      return;
    }
#else
    #warning No Use LEGALITY Check
#endif
    recv_Type->recvBuff[recv_Type->recvPin_Now++] = chr;    
    if((recv_Type->LastRecv == RECV_END1) && (chr == RECV_END2))
    {
        recv_Type->findBegin = 0;                   //找到尾，置0
        recv_Type->findEnd   = 1;                   //找到尾，置1
        recv_Type->LastRecv  = 0;                   //清空上次的记录
    }
    else recv_Type->LastRecv = chr;                 //记录上个字符
  }
  if(recv_Type->findEnd)                            //已经出现尾巴，证明结束了
  {
    //数据解码
    HYQY_DeBuf(recv_Type, recv_Type->recvBuff, recv_Type->recvPin_Now - 2);
    //清空标志位置
    recv_Type->findEnd = 0;
    recv_Type->recvPin_Now = 0;
  }
  else recv_Type->LastRecv = chr;
}

static inline int8_t MyU8ABS(int8_t x)
{
  if(x < 0)return -x;
  return x;
}

static void HYQY_DeBuf(HYQY_RECV_Typedef* recv_Type,uint8_t* buf,uint16_t num)
{
  uint8_t NumPin = 0;                                 //存储第几个数据？
  uint8_t i = 0;
  uint8_t NegFlag = 0;                                //判断数字是否是负数
  for(i = 0;i < recv_Type->recvNum + 1; i++)
    recv_Type->recvRes[i] = 0;
  for(i = 0;i < num; i++)
  {
    if(buf[i] == '-')                                 //负数
    {
      NegFlag = 1;
    }
    else if(buf[i] == ',')
    {
      if(NegFlag)                                     //如果是负数
      {
        recv_Type->recvRes[NumPin] *= -1;
        NegFlag = 0;
      }
      NumPin ++;
      //if(NumPin == RECV_RES_MAX)NumPin --;            //防止溢出
    }
    else
    {
      if(MyU8ABS((buf[i] - '0')) > 9)continue;
      recv_Type->recvRes[NumPin] *= 10;
      recv_Type->recvRes[NumPin] += buf[i] - '0';
    }
  }
  recv_Type->recvNum = NumPin;                        //一共找到了这么多个数据
  recv_Type->DataIsOK = 1;
  recv_Type->RecvTime = HAL_GetTick();
  //printf("-----Decode--finish-----\r\n");
}

uint8_t IsReceiveDataOK(HYQY_RECV_Typedef* recv_Type) //判断接收是否完成
{
  if(recv_Type->DataIsOK)
  {
    recv_Type->DataIsOK = 0;
    return 1;
  }
  return 0;
}

void ShowDecodeResult(HYQY_RECV_Typedef* recv_Type)    //展示解码信息
{
  uint8_t i = 0;
  printf("\n UART decode %d datas:\r\n", recv_Type->recvNum);
  for(i = 0; i < recv_Type->recvNum; i++)
  {
    printf("recvRes[%d]:%d\r\n", i, recv_Type->recvRes[i]);
  }
  printf("recvTime:%d\r\n", recv_Type->RecvTime);
}

/*
测试用例：
在main循环中加入如下
int main()
{
  while(1)
  {
    if(IsReceiveDataOK(&HYQY_recvInfo))             //如果收到数据
      ShowDecodeResult(&HYQY_recvInfo);             //展示接收结果
  } 
}



*/

