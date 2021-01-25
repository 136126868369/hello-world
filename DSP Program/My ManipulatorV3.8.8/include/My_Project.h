/*
 * My_Project.h
 *
 *  Created on: 2015-3-18
 *      Author: rd49
 */

#ifndef MY_PROJECT_H_
#define MY_PROJECT_H_
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "math.h"
#include <string.h>
#include <stdlib.h>
#include "My_Delay.h"
/* --------------------------------------------------- */
/* 		全局变量         					           */
/* ----------------------------------------------------*/
/* xx全局变量 */
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;

#define   TARGET   1

#define CURRENT_SYS_FIVE  "five axis system"
#define CURRENT_SYS_SIX  "six axis system"
#define CURRENT_VERSION  "V1.8.7"

#define FOUR_AXIS_A 41	//大四轴	H4560
#define FOUR_AXIS_B	42	//小/中四轴	H4450/H4440
#define FOUR_AXIS_C	43	//高速 小/中四轴	GH4450/GH4440

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;
#define  NOP    asm(" NOP")
/* 串口全局变量 */
#define USART_REC_LEN  		32  	//定义最大接收字节数 32
//extern unsigned char USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节
extern unsigned int USART_RX_STA;

extern double PulseOfCircle_INVERSE;	////电机精度的倒数
#define PI 					3.1415926535898
#define PI_DIVIDE_180		0.01745329252
#define PI_MULTI_180_INVERSE		57.295779513082  // 1/PI*180
#define PI_DIVIDE_2_INVERSE		0.1591549430919//1/PI/2

//安全开关（按下安全开关，才能示教），上拉输入，低电平表示有按下
#define SAVE_BUTTON				(GpioDataRegs.GPBDAT.bit.GPIO58)
#define F28_PIN1				GpioDataRegs.GPBDAT.bit.GPIO33
#define F28_PIN2				GpioDataRegs.GPBDAT.bit.GPIO32
#define SCREEN_CONNECT_STATE	F28_PIN2
#define SAVE_BTN2				F28_PIN1
#define ENABLE_SPEAKER			GpioDataRegs.GPADAT.bit.GPIO21=1//开蜂鸣器
#define DISABLE_SPEAKER			GpioDataRegs.GPADAT.bit.GPIO21=0//关蜂鸣器
#define RS485DIR				GpioDataRegs.GPADAT.bit.GPIO20	//RS485DIR = 0->接收   1->发送

#define SPEED_MODE_BASE 0.25	//设置低速模式时，最大的示教速度为100*0.3

#define KEY_BOARD				1

struct ButtonStruBits//从bit0开始使用
{
	 Uint16 SERVO_ON:1;		//伺服ON按钮，0:开servo on	1：关servo on，
	 Uint16 ALARM_FLG:1;	//报警标志位，0：无报警		1：有报警
	 Uint16 ORIGIN_FLG:1;	//原点标志位，0：有原点		1：无原点	//无原点不能进行程序运行
	 Uint16 EMG_FLG:1;		//急停标志位，0：无急停		1：急停中
	 Uint16 POS_LIMITED_FLG:1;	//+限位标志位，0：无限位		1：+限位中
	 Uint16 NEG_LIMITED_FLG:1;	//-限位标志位，0：无限位		1：-限位中
	 Uint16 SAVE_BTN:1;		//安全开关，  0：不使用安全开关 1：安全开关使用中
	 Uint16 reserved:9;
};

struct EX_Position_stru{
	char   type;	//1：一个数据	2：两个数据	3：三个数据
	double EX_POSITION[6];
};

union ButtonStru
{
	 Uint16 all;//bit0~bit6：伺服ON、报警、原点、急停、+限位、-限位、安全开关
	 struct ButtonStruBits CTRL_BUTTON;
};

struct MotorDirStruBits
{
	u16 J1:1;
	u16 J2:1;
	u16 J3:1;
	u16 J4:1;
	u16 J5:1;
	u16 J6:1;
	u16 J7:1;
	u16 J8:1;
	u16 reserved:8;
};
union MotorDirStru
{
	u16 all;
	struct MotorDirStruBits bit;
};

struct RobotParameterStru
{
	char NAME[11];
	u16 AXIS_NUM:8;	//轴数目
	u16 BAD:1;		//记录是否坏块	0->坏块	1->好块
	u16 ENCODER_LEFT_SAV:1;	//记录开机是否需要保存编码器零点偏移量 1->开机需要保存 0->不需要
	u16 MOTOR_TYPE:3;	//记录电机种类 1->三菱	2->三协
	u16 FIRST:1;	//1->第一次使用 0->已使用过
	u16 SOUND:1;	//触摸屏点击时候有声音	0->无	1->有
	u16 reserved:1;
	u16 SYS:8;		//系统代号
	u16 HARDWARE_REV:8;	//电路板版本号
	u16 MOTOR_SELECT;		//电机选择(bit0~bit7 分别对应1~8轴)
	u16 MOTOR_SPEED;		//电机每分钟转速
	union MotorDirStru MOTOR_DIR;		//电机旋转方向  1->(*-1)  0->(*1)
	union MotorDirStru AXIS_ROTATE;	//决定单轴旋转时的方向 1->(*-1)  0->(*1)
	long MOTOR_PRECISION;	//电机每圈对应的脉冲数
	long  ENCODER_LEFT[8];	//编码器零点偏移量
	long MOTOR_SN[8];		//电机序列号
	float PRECISION;		//插补精度(单位:mm)
	float ROBOT_PARA[5];	//机械手臂长之类的参数
	float LIMITED_P[8];		//正限位(单位是角度)
	float LIMITED_M[8];		//负限位(单位是角度)
	float ORIGIN_POSITION[8];	//原点位置
	double RATIO[8];			//减速比

};
typedef struct RobotParameterStru RobotParaStru;

struct RobotParameterStru_CRC
{
	RobotParaStru RobotPara;
	u16 CRC16;
};
typedef struct RobotParameterStru_CRC RobotParaStru_CRC;

extern union ButtonStru G_CTRL_BUTTON;
extern RobotParaStru ROBOT_PARAMETER;
//#pragma CODE_SECTION(crc_cal_by_bit, "ramfuncs");//修正于//2018/06/01 for Extend RAM
u16 crc_cal_by_bit(u8* ptr, u32 len);	//校验公式
#endif  // end of MY_PROJECT_H definition
//===========================================================================
// No more.
//===========================================================================
