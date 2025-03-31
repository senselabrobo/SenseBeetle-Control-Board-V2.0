# SenseBeetle Control Board V2.0

[English](https://github.com/senselabrobo/SenseBeetle-Control-Board-V2.0/blob/main/README.md)/中文

# 概述：
> 本分电板存在于机器人车载电脑与电机之间，接收车载电脑数据（USB VPC）数据，并将数据转换到CAN总线并与电机通讯。同时，该分电板集成了PID算法， 可以对DJI 2006电机进行实时PID控制（最多驱动4路2006电机）。该分电板输入电压24V，输出12V，5V等电压，为车载激光雷达等外设进行供电。最后，该分电板集成了上电缓启动，在接通电源一定时间后，才进行上电操作，防止接通电源瞬间打火等问题。
>

主要功能如下：

+ 协议转换：USB&UART->CAN
+ 电机控制：集成PID控制算法，实现4路2006电机控制
+ 电压转换：24V输入，12V&5V输出
+ 安全保护：电池低压保护，上电缓启动保护，无通讯保护

# 硬件设计：
## 电源树：
+ 机器人运行情况下，由TB47电池 经 24V IN 接口供电，此时电机可以启动，可实现整板全部功能。
+ 调试模式下，可以仅接USB 5V供电，此时动力域整体不供电，仅MCU部分供电，方便调试。

![画板](https://cdn.nlark.com/yuque/0/2025/jpeg/28458595/1742830024031-09bd11e1-73b7-4db8-8a24-970679b1f330.jpeg)

## 功能框图：
![画板](https://cdn.nlark.com/yuque/0/2025/jpeg/28458595/1742829958540-9143a489-0b69-4ed0-ae6f-734cd532e6ea.jpeg)

## PCB设计：
PCB采用4层板设计，为了保持信号完整性，内层1为地平面，内层2为24V和12V电源层，外层用于走信号。

PCB为方便制造，在两边增加了工艺边和Mark点，方便进行SMT批量制造。边缘邮票孔可以掰开，无需借助任何工具。

![](https://cdn.nlark.com/yuque/0/2025/png/28458595/1742830291501-6b30f349-a720-4ef4-92b8-3bd87b745422.png)![](https://cdn.nlark.com/yuque/0/2025/png/28458595/1742830409150-c7810dcb-c3f2-4ba4-b563-e9e0bb7b7d71.png)

![](https://cdn.nlark.com/yuque/0/2025/png/28458595/1742830625184-d6e97dac-80ff-4ba9-bcc4-3b83bf210f38.png)

# 软件设计：
软件整体采用协程调度，由于系统资源限制，未使用RTOS。

子程序如下：

+ MotorTask_Handle：用于电机PID控制，频率100Hz
+ BspTask_Handle：ADC数据读取+电压纠偏
+ ConnTask_Handle：通讯协议帧解析+低压保护+超时保护

## 软件控制协议帧：
> 控制采用PP协议
>
> 该协议采用明文发送信息，具有如下特点：
>
> + <font style="color:rgb(25, 27, 31);">协议帧直观，简单粗暴，可以直接通过串口调试助手看懂发送数据，无需按bit编码。</font>
> + <font style="color:rgb(25, 27, 31);">一帧内可以传输</font>**<font style="color:rgb(25, 27, 31);">任意多个任意长度</font>**<font style="color:rgb(25, 27, 31);">的数据（数据个数仅限制于单片机内存，通常10个够用了）</font>
> + <font style="color:rgb(25, 27, 31);">该协议提供简单的错误处理，即发送数据中有错误字符，会自动丢弃整包数据。</font>
> + <font style="color:rgb(25, 27, 31);">高稳定性，高实时性。即使在两帧数据之间夹杂了一些多余字符也不影响正常解析。</font>
> + <font style="color:rgb(25, 27, 31);">高可移植性，纯数学处理，不涉及任何底层操作。适用于轮询或中断。</font>
>

数据格式如下：（忽略加号）

> <font style="color:rgb(83, 88, 97);">数据首1(p)+数据首2(p) + 数据1, + 数据2 , +...+数据n, + 数据尾1(\r) + 数据尾2(\n)</font>
>

示例如下：

+ <font style="color:rgb(25, 27, 31);">pp12,666,266,1234,\r\n</font>

<font style="color:rgb(25, 27, 31);">MCU上会自动将发送的数据解析到recvData[]数组中，示例中的解析结果如下：recvData[0] = 12,recvData[1] = 666......（类推）</font>

<font style="color:rgb(25, 27, 31);">即，可以得到如下示例帧：</font>

+ 电机停转：<font style="color:rgb(25, 27, 31);">pp0,0,\r\n</font>
+ <font style="color:rgb(25, 27, 31);">电机1旋转：pp1,300,0,0,0,\r\n</font>
+ <font style="color:rgb(25, 27, 31);">电机2旋转：pp1,300,300,0,0,\r\n</font>
+ <font style="color:rgb(25, 27, 31);">（类推）</font>

```plain
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
```

