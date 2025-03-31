# Simple Introduction:
> This PD board is installed between the robot's on-board computer and the motors. It receives data from the on-board computer (USB VPC data), converts the data to the CAN bus, and communicates with the motors. At the same time, this PD board integrates a PID algorithm, enabling real-time PID control of DJI 2006 motors (it can drive up to 4 channels of DJI 2006 motors). The input voltage of this power distribution board is 24V, and it outputs voltages such as 12V and 5V to supply power to external devices like the on-board lidar. Finally, this PD board is equipped with a soft start function upon power-on. After the power is connected for a certain period of time, the power-on operation will be carried out to prevent issues such as sparking at the moment of power connection.
>

Main Functions：

+ Protocol Conversion：USB&UART->CAN
+ Motor Control: Integrate the PID control algorithm to achieve the control of 4 channels of DJI 2006 motors.
+ Voltage Conversion: 24V input，12V&5V output
+ Safety Protection: Low battery voltage protection, soft start protection upon power-on, and protection against communication failure.

# Hardware design：
## Power Supply Tree：
+ When the robot is in operation, it is powered by the TB47 battery through the 24V IN interface. At this time, the motor can be started, and all functions of the entire board can be realized.
+ In the debugging mode, it can be powered only by USB 5V. At this time, the power domain as a whole is not powered, and only the MCU part is supplied with power, which is convenient for debugging. 

![画板](https://cdn.nlark.com/yuque/0/2025/jpeg/28458595/1742830024031-09bd11e1-73b7-4db8-8a24-970679b1f330.jpeg)

## Functional Diagram：
![画板](https://cdn.nlark.com/yuque/0/2025/jpeg/28458595/1742829958540-9143a489-0b69-4ed0-ae6f-734cd532e6ea.jpeg)

## PCB Design：
The PCB is designed with a 4-layer board. To maintain signal integrity, the first inner layer serves as the ground plane, and the second inner layer acts as the power layer for 24V and 12V. The outer layers are used for routing signals. 

For the convenience of PCB manufacturing, process margins and Mark points are added on both sides, which facilitates the mass manufacturing of SMT. The edge breakaway holes (stamp holes) can be separated without the need for any tools.

![](https://cdn.nlark.com/yuque/0/2025/png/28458595/1742830291501-6b30f349-a720-4ef4-92b8-3bd87b745422.png)![](https://cdn.nlark.com/yuque/0/2025/png/28458595/1742830409150-c7810dcb-c3f2-4ba4-b563-e9e0bb7b7d71.png)

![](https://cdn.nlark.com/yuque/0/2025/png/28458595/1742830625184-d6e97dac-80ff-4ba9-bcc4-3b83bf210f38.png)

# Software Design：
The software adopts co-routine scheduling. Due to the limitations of system resources, the Real-Time Operating System (RTOS) is not used.

Subprogram：：

+ MotorTask_Handle：For the PID control of the motor, with a frequency of 100Hz.
+ BspTask_Handle：ADC Data reading + Voltage correction.
+ ConnTask_Handle：Communication protocol frame parsing + LVP (Low Voltage Protection) + TOP (Timeout Protection).

## Software Control Protocol Frame ：
> Control adopts the PP protocol. 
>
> This protocol sends information in plain text and has the following characteristics:
>
> + <font style="color:rgb(25, 27, 31);">The protocol frame is intuitive and straightforward. You can directly understand the sent data through a serial port debugging assistant without bit - level encoding.</font>
> + <font style="color:rgb(25, 27, 31);"> Any number of data items of arbitrary lengths</font>**<font style="color:rgb(25, 27, 31);">can be transmitted within one frame (the number of data items is only limited by the MCU's memory. Usually, 10 items are sufficient).</font>
> + <font style="color:rgb(25, 27, 31);">The protocol provides simple error handling. That is, if there are erroneous characters in the sent data, the entire packet of data will be automatically discarded.</font>
> + <font style="color:rgb(25, 27, 31);">The protocol provides simple error handling. That is, if there are erroneous characters in the sent data, the entire packet of data will be automatically discarded.</font>
> + <font style="color:rgb(25, 27, 31);">It has high portability. It involves pure mathematical processing and no underlying operations. It is suitable for both polling and interrupt modes.</font>
>

Data format：（ignore the ‘+’）

> <font style="color:rgb(83, 88, 97);">Data Head1(p)+Data Head2(p) + Data 1, + Data 2 , +...+Data n, + Data End 1(\r) + Data End 2(\n)</font>
>

示例如下：

+ <font style="color:rgb(25, 27, 31);">pp12,666,266,1234,\r\n</font>

<font style="color:rgb(25, 27, 31);">On the MCU, the sent data will be automatically parsed into therecvData[]array. The parsing results in the example are as follows:recvData[0] = 12,recvData[1] = 666......（and so on）</font>

<font style="color:rgb(25, 27, 31);">The following example frames can be obtained.：</font>

+ Motor stop：<font style="color:rgb(25, 27, 31);">pp0,0,\r\n</font>
+ <font style="color:rgb(25, 27, 31);">Motor 1 start：pp1,300,0,0,0,\r\n</font>
+ <font style="color:rgb(25, 27, 31);">Motor 2 start：pp1,300,300,0,0,\r\n</font>
+ <font style="color:rgb(25, 27, 31);">（etc.）</font>

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

