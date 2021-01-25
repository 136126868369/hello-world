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
/* 		ȫ�ֱ���         					           */
/* ----------------------------------------------------*/
/* xxȫ�ֱ��� */
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;

#define   TARGET   1

#define CURRENT_SYS_FIVE  "five axis system"
#define CURRENT_SYS_SIX  "six axis system"
#define CURRENT_VERSION  "V1.8.7"

#define FOUR_AXIS_A 41	//������	H4560
#define FOUR_AXIS_B	42	//С/������	H4450/H4440
#define FOUR_AXIS_C	43	//���� С/������	GH4450/GH4440

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;
#define  NOP    asm(" NOP")
/* ����ȫ�ֱ��� */
#define USART_REC_LEN  		32  	//�����������ֽ��� 32
//extern unsigned char USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�
extern unsigned int USART_RX_STA;

extern double PulseOfCircle_INVERSE;	////������ȵĵ���
#define PI 					3.1415926535898
#define PI_DIVIDE_180		0.01745329252
#define PI_MULTI_180_INVERSE		57.295779513082  // 1/PI*180
#define PI_DIVIDE_2_INVERSE		0.1591549430919//1/PI/2

//��ȫ���أ����°�ȫ���أ�����ʾ�̣����������룬�͵�ƽ��ʾ�а���
#define SAVE_BUTTON				(GpioDataRegs.GPBDAT.bit.GPIO58)
#define F28_PIN1				GpioDataRegs.GPBDAT.bit.GPIO33
#define F28_PIN2				GpioDataRegs.GPBDAT.bit.GPIO32
#define SCREEN_CONNECT_STATE	F28_PIN2
#define SAVE_BTN2				F28_PIN1
#define ENABLE_SPEAKER			GpioDataRegs.GPADAT.bit.GPIO21=1//��������
#define DISABLE_SPEAKER			GpioDataRegs.GPADAT.bit.GPIO21=0//�ط�����
#define RS485DIR				GpioDataRegs.GPADAT.bit.GPIO20	//RS485DIR = 0->����   1->����

#define SPEED_MODE_BASE 0.25	//���õ���ģʽʱ������ʾ���ٶ�Ϊ100*0.3

#define KEY_BOARD				1

struct ButtonStruBits//��bit0��ʼʹ��
{
	 Uint16 SERVO_ON:1;		//�ŷ�ON��ť��0:��servo on	1����servo on��
	 Uint16 ALARM_FLG:1;	//������־λ��0���ޱ���		1���б���
	 Uint16 ORIGIN_FLG:1;	//ԭ���־λ��0����ԭ��		1����ԭ��	//��ԭ�㲻�ܽ��г�������
	 Uint16 EMG_FLG:1;		//��ͣ��־λ��0���޼�ͣ		1����ͣ��
	 Uint16 POS_LIMITED_FLG:1;	//+��λ��־λ��0������λ		1��+��λ��
	 Uint16 NEG_LIMITED_FLG:1;	//-��λ��־λ��0������λ		1��-��λ��
	 Uint16 SAVE_BTN:1;		//��ȫ���أ�  0����ʹ�ð�ȫ���� 1����ȫ����ʹ����
	 Uint16 reserved:9;
};

struct EX_Position_stru{
	char   type;	//1��һ������	2����������	3����������
	double EX_POSITION[6];
};

union ButtonStru
{
	 Uint16 all;//bit0~bit6���ŷ�ON��������ԭ�㡢��ͣ��+��λ��-��λ����ȫ����
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
	u16 AXIS_NUM:8;	//����Ŀ
	u16 BAD:1;		//��¼�Ƿ񻵿�	0->����	1->�ÿ�
	u16 ENCODER_LEFT_SAV:1;	//��¼�����Ƿ���Ҫ������������ƫ���� 1->������Ҫ���� 0->����Ҫ
	u16 MOTOR_TYPE:3;	//��¼������� 1->����	2->��Э
	u16 FIRST:1;	//1->��һ��ʹ�� 0->��ʹ�ù�
	u16 SOUND:1;	//���������ʱ��������	0->��	1->��
	u16 reserved:1;
	u16 SYS:8;		//ϵͳ����
	u16 HARDWARE_REV:8;	//��·��汾��
	u16 MOTOR_SELECT;		//���ѡ��(bit0~bit7 �ֱ��Ӧ1~8��)
	u16 MOTOR_SPEED;		//���ÿ����ת��
	union MotorDirStru MOTOR_DIR;		//�����ת����  1->(*-1)  0->(*1)
	union MotorDirStru AXIS_ROTATE;	//����������תʱ�ķ��� 1->(*-1)  0->(*1)
	long MOTOR_PRECISION;	//���ÿȦ��Ӧ��������
	long  ENCODER_LEFT[8];	//���������ƫ����
	long MOTOR_SN[8];		//������к�
	float PRECISION;		//�岹����(��λ:mm)
	float ROBOT_PARA[5];	//��е�ֱ۳�֮��Ĳ���
	float LIMITED_P[8];		//����λ(��λ�ǽǶ�)
	float LIMITED_M[8];		//����λ(��λ�ǽǶ�)
	float ORIGIN_POSITION[8];	//ԭ��λ��
	double RATIO[8];			//���ٱ�

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
//#pragma CODE_SECTION(crc_cal_by_bit, "ramfuncs");//������//2018/06/01 for Extend RAM
u16 crc_cal_by_bit(u8* ptr, u32 len);	//У�鹫ʽ
#endif  // end of MY_PROJECT_H definition
//===========================================================================
// No more.
//===========================================================================
