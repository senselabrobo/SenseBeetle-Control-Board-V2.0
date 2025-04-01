#ifndef _HYQY_PP_PRO_H
#define _HYQY_PP_PRO_H

#include "main.h"

/**
 * 
 * 使用方法如下：
 * 发送任意长（小于RECV_BUFFER_MAX）的字符串，包含多个数据，数据结尾添加英文逗号
 * 注意保证整帧为：
 *          数据首1、数据首2 + 数据1, + 数据2 , +...+数据n, + 数据尾1 + 数据尾2
 * 例如(char)：pp12,666,266,1234,\r\n
 *
 * 注意：本程序为纯软件逻辑部分，请在外部自行实现中断单字符接收！
 *
 * 作者：YQY 
 * 版本更新记录：
 * V2版本：新增数据合法性检测
 */

#define RECV_BUFFER_MAX     64              //接收缓冲区大小
#define RECV_RES_MAX        10              //接收数据个数
#define RECV_BEG1           0x70            //数据首1   'p'(建议不要更改)
#define RECV_BEG2           0x70            //数据首2   'p'(建议不要更改)

#define RECV_END1           0x0D            //数据尾1   '\r'(建议不要更改)
#define RECV_END2           0x0A            //数据尾2   '\n'(建议不要更改)

#define LEGALITY_CHECK      1               //数据合法性检测，如果使能，则收到错误字符时丢弃整包数据！


typedef struct 
{
  uint8_t       recvDR;                     //临时存储当前字符
  uint8_t       recvBuff[RECV_BUFFER_MAX];  //临时缓冲区
  uint16_t      recvNum;                    //记录接收了几个数据了
  uint16_t      recvPin_Now;                //指针，指向当前缓冲位
  uint8_t       findBegin;                  //如果找到头，置1
  uint8_t       findEnd;                    //如果找到尾，置1，头置0
  uint8_t       DataIsOK;                   //数据接收完成，解码完成
  int32_t       recvRes[RECV_RES_MAX];      //接收的成品数据（重点关注！！！）
  uint8_t       LastRecv;                   //记录上次接收的字符
  uint32_t      RecvTime;                   //接收时间
}HYQY_RECV_Typedef;

extern HYQY_RECV_Typedef HYQY_recvInfo;  

/* 初始化接收（***需移植***） */
void HYQY_PP_Recv_Init(HYQY_RECV_Typedef* recv_Type);

/* 放入接收中断内，用于接收一次字符（***需移植***）  */
void HYQY_PP_Recv_Handle(HYQY_RECV_Typedef* recv_Type,uint8_t chr);

/* 对缓冲区数据解码 */
void HYQY_DeBuf(HYQY_RECV_Typedef* recv_Type,uint8_t* buf,uint16_t num);

/* 判断接收是否完成 */
uint8_t IsReceiveDataOK(HYQY_RECV_Typedef* recv_Type);

/* 检测接收到的单字符是否合法 */
uint8_t checkLegalityFailed(char x);

/* 展示解码完成的信息 */
void ShowDecodeResult(HYQY_RECV_Typedef* recv_Type);

#endif      //_HYQY_PP_PRO_H
