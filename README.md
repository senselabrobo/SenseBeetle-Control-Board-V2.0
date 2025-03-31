SenseBeetle Control Board V2.0
Introduction:
SenseBeetle Control Board V2.0 is installed between the robot's on-board computer and the motors. It
receives data from the on-board computer (USB VPC data), converts the data to the CAN
bus, and communicates with the motors. At the same time, this PD board integrates a
PID algorithm, enabling real-time PID control of DJI 2006 motors (it can drive up to
4 channels of DJI 2006 motors). The input voltage of this power distribution board is
24V, and it outputs voltages such as 12V and 5V to supply power to external devices
like the on-board lidar. Finally, this PD board is equipped with a soft start function
upon power-on. After the power is connected for a certain period of time, the power-on
operation will be carried out to prevent issues such as sparking at the moment of power
connection.
Main Functions：
 Protocol Conversion: USB&UART->CAN.
 Motor Control: Integrate the PID control algorithm to achieve the control of 4
channels of DJI 2006 motors.
 Voltage Conversion: 24V input，12V&5V output.
 Safety Protection: Low battery voltage protection, soft start protection upon
power-on, and protection against communication failure.
Hardware design：
Power Supply Tree：
 When the robot is in operation, it is powered by the TB47 battery through the 24V
IN interface. At this time, the motor can be started, and all functions of the entire
board can be realized.
 In the debugging mode, it can be powered only by USB 5V. At this time, the power
domain as a whole is not powered, and only the MCU part is supplied with power,
which is convenient for debugging.
Functional Diagram：
Remarks: There are a total of 2 serial ports (independent UARTs) and 5 CAN interfaces
(sharing CAN1).
PCB Design：
The PCB is designed with a 4-layer board. To maintain signal integrity, the first inner
layer serves as the ground plane, and the second inner layer acts as the power layer
for 24V and 12V. The outer layers are used for routing signals.
For the convenience of PCB manufacturing, process margins and Mark points are added
on both sides, which facilitates the mass manufacturing of SMT. The edge breakaway holes
(stamp holes) can be separated without the need for any tools.
Software Design：
The software adopts co-routine scheduling. Due to the limitations of system resources,
the Real-Time Operating System (RTOS) is not used.
Subprogram：
 MotorTask_Handle：For the PID control of the motor, with a frequency of 100Hz
 BspTask_Handle：ADC Data reading + Voltage correction
 ConnTask_Handle：Communication protocol frame parsing + LVP (Low Voltage Protection)
+ TOP (Timeout Protection)
Software Control Protocol Frame ：
Control adopts the PP protocol.
This protocol sends information in plain text and has the following characteristics:
 - The protocol frame is intuitive and straightforward. You can directly understand
the sent data through a serial port debugging assistant without bit - level encoding.
 - Any number of data items of arbitrary lengths can be transmitted within one frame
(the number of data items is only limited by the MCU's memory. Usually, 10 items
are sufficient).
 - The protocol provides simple error handling. That is, if there are erroneous
characters in the sent data, the entire packet of data will be automatically
discarded.
 - It features high stability and high real - time performance. Even if there are
some extra characters between two frames of data, it won't affect normal parsing.
 - It has high portability. It involves pure mathematical processing and no
underlying operations. It is suitable for both polling and interrupt modes.
Data format：（ignore the ‘+’）
Data Head1(p)+Data Head2(p) + Data 1, + Data 2 , +...+Data n, + Data End 1(\r) + Data
End 2(\n)
Example：
 pp12,666,266,1234,\r\n
On the MCU, the sent data will be automatically parsed into therecvData[]array. The
parsing results in the example are as follows:recvData[0] = 12,recvData[1] = 666......
（and so on）
The following example frames can be obtained.：
 Motor stop：pp0,0,\r\n
 Motor 1 start：pp1,300,0,0,0,\r\n
 Motor 2 start：pp1,300,300,0,0,\r\n
 （etc.）
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
