/*
 * ScreenApi.h
 *
 *  Created on: 2014-12-21
 *      Author: rd49
 */

#ifndef SCREENAPI_H_
#define SCREENAPI_H_

#define STM32_USART_LEN	100
/***********************************************************
 * ���ܣ������˶�
 * ������pressFlag  ��ť��־λ��  	1����ʾ����   0����ʾ�ɿ�
 * 		 whichAxis	��ʾ�ĸ���	  	1~6�ֱ��ʾ1~6��
 * 		 sign		��ʾ�������� 	0��������	  1��������
 * ����ֵ��0����ȷ
 * 		   1����е�ֶ��������з�������
 * 		   2����е�ֶ����������ɿ��˰�ȫ���أ�ʾ��ʱ��
 */
unsigned char AxisSingleRun(unsigned char pressFlag,unsigned char whichAxis,unsigned char sign);

/********************************************************
 * ���ܣ��ػ�����ֱ���˶�  Base coordinate straight line motion
 * ������pressFlag ��ť��־λ�� 1����ʾ��ť����   0����ť�ɿ�
 * 		 orientation  �����־λ  1��X-  2��X+
 * 		 						  3��Y-	 4��Y+
 * 		 						  5��Z-  6��Z+
 * ����ֵ��0����ȷ	1����������
 */
unsigned char BaseCoordSlineRun(unsigned char pressFlag,unsigned char orientation);


/********************************************************
 * ���ܣ��ػ�����̶�ֱ�߾����˶�  Base coordinate straight line Fixed distance
 * ������movingDistance �ƶ����룬 double���ͣ������������������˷���,��߾��ȵı���
 * 		 orientation  ����  		ֻ������px��py��pz��ָ��
 */
void BaseCoordSlineFixedDistance(unsigned char changeGesture,double movingDistance,double *orientation);

/********************************************************
 * ���ܣ��ػ�����̶�ֱ����С�����˶�,ʹ��ǰ��Ҫ�ȸı�px��py��pz��ֵ��
 * 		 ����ֱ�ӵ���������
 * ����ֵ��0����ȷ		1:��������
 */
//#pragma CODE_SECTION(BaseCoordSlineMinPrecisionRun, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char BaseCoordSlineMinPrecisionRun(void);

/********************************************************
 * ���ܣ��Ե�ǰλ�ã����������������
 * ���������delta_pulse ָ��6����ÿ������Ҫ�ߵ���������ָ��
 * 			//mdoe:0->�ؽڲ岹		1->ֱ�߲岹
 * ����ֵ��    0����ȷ
 * 			1:��е�ֶ��������з�������
 * 			2����е�ֶ����������ɿ��˰�ȫ���أ�ʾ��ʱ��
 * 			3�����������а�����stop��ť
 */
//#pragma CODE_SECTION(BaseCoordSlineMinPrecisionRun, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char MovePulse(long* delta_pulse);

/*****************************************
 * �������ܣ���ԭ��
 * ��������� speed ����ԭ����ٶ�	1~30���Ƽ�8~10��
 */
void ReturnHome(unsigned char speed);

/****************************************
 * �������ܣ�ԭ������
 */
u8 ClearOrigin(void);

/****************************************
 * �������ܣ��������
 */
void ClearAlarm(void);

//�������������Ϣ
void ClearAlarmInfo(void);
/****************************************
 * �������ܣ�����ŷ�������
 * ����ֵ�� 0���ɹ�
 *  		��0����£�bit0~bit5��1�ֱ�����ĸ��ŷ���ȡ���ݳ�����������Ӧ���ŷ�����
 * 					   bit7Ϊ1�����ȡ���������ݳ��������Ǵ�����̳�����������·����
 */

/********************************************
 * �������ܣ���ⴥ��������״̬���޸�
 * ���������NONE
 * ���������NONE
 */
void CheckScreenConnectState(void);

/********************************************
 * ��鱨��
 * ����ֵ�������ᣬbit0~bit5�ֱ����J1~J6
 */
unsigned char ErrCheck(unsigned char screen_id);

/********************************************
 * �ŷ���
 */
unsigned char  ServoON(unsigned char mode);

/********************************************
 * �ŷ���
 */
void ServoOFF(void);

/********************************************
 * �����ŷ����أ�bit0~bit5�ֱ����J1~J6,0���ŷ���1���ŷ�
 */
void ServoON_OFF(unsigned int state);

/********************************************
 * ���Ƶ����ŷ�ON�Ŀ��أ�
 * Axis�����뷶Χ1~6������J1~J6
 * state��0���ŷ���1���ŷ�
 */
void ServoON_OFF_Single(unsigned char axis,unsigned char state);

/*******************************************
 * ����ŷ�����
 */
void ClearServoAlarm(void);

/************************************
 * ����ǰ����/������Ϣ
 * ����ֵ	bit0=1���ŷ�onû��
 * 			bit1=1���о���
 * 			bit2=1����ԭ���ԭ�����
 * 			bit3=1����ͣ��
 */
unsigned int ReadErrInfo(void);

/****************************************
 * �������ܣ�����ŷ�������
 * ����ֵ�� mode=0,������������
 * 			mode=1,ǿ�Ƹ����������ݣ����ܶ�ȡ�����Ƿ���ȷ
 * ����ֵ�� 0���ɹ�
 *  		��0����£�bit0~bit5��1�ֱ�����ĸ��ŷ���ȡ���ݳ�����������Ӧ���ŷ�����
 */
unsigned char UpdatePulseFromServo(void);
unsigned char UpdatePulseFromServoForEp(long *pulse);


//ĩ������ֱ���˶�
void XEndCoordSlineRunP(unsigned char pressFlag);
void XEndCoordSlineRunM(unsigned char pressFlag);
void YEndCoordSlineRunP(unsigned char pressFlag);
void YEndCoordSlineRunM(unsigned char pressFlag);
void ZEndCoordSlineRunP(unsigned char pressFlag);
void ZEndCoordSlineRunM(unsigned char pressFlag);

//��������ת		Revolve  ��ת
void XBaseCoordRevolRunP(unsigned char pressFlag);
void XBaseCoordRevolRunM(unsigned char pressFlag);
void YBaseCoordRevolRunP(unsigned char pressFlag);
void YBaseCoordRevolRunM(unsigned char pressFlag);
void ZBaseCoordRevolRunP(unsigned char pressFlag);
void ZBaseCoordRevolRunM(unsigned char pressFlag);

//ĩ��������ת
void XEndCoordRevolRunP(unsigned char pressFlag);
void XEndCoordRevolRunM(unsigned char pressFlag);
void YEndCoordRevolRunP(unsigned char pressFlag);
void YEndCoordRevolRunM(unsigned char pressFlag);
void ZEndCoordRevolRunP(unsigned char pressFlag);
void ZEndCoordRevolRunM(unsigned char pressFlag);

void OpenBuzzer(void);//�򿪷�����
void CloseBuzzer(void);//�رշ�����
void ReadCurrentPulseEp(long *pulse);
void ReadCurrentPulseLp(long *pulse);
void SetSpeed(unsigned char aix,unsigned int r);


/**************************************
 * �������ܣ�����ȫ����״̬
 * ����ֵ��	 0����ȫ�����Ѱ���
 * 			 1����ȫ����δ����
 */
unsigned char ReadSaveButtonState(void);

/************************************
 * �������ܣ������ŷ���������λ����Ч/��Ч
 * ���������flag
 * 			 bit0��	0:�ŷ�������Ч		1:�ŷ�������Ч
 * 			 bit1��	0:��λ��Ч			1:��λ��Ч
 */
void SetAlarmLimitedState(unsigned char flag);

void DisableServoAlarm(void);//�ŷ�������Ч
void EnableServoAlarm(void);//�ŷ�������Ч
void DisableLimited(void);//��λ��Ч
void EnableLimited(void);//��λ��Ч
//#pragma CODE_SECTION(GetWR2state, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned int GetWR2state(void);//��ȡWR2�Ĵ�����ֵ
void RecoverWR2(void);//�ָ�WR2�Ĵ���
void SetSpeedMode_High(void);//����Ϊ����ʾ��ģʽ
void SetSpeedMode_Low(void);//����Ϊ����ʾ��ģʽ
unsigned char GetSpeedMode(void);//��õ�ǰʾ��ģʽ
void DeveloperMode_On(void);//���ÿ�����ģʽΪ����
void DeveloperMode_Off(void);//���ÿ�����ģʽΪ����


/*****************************************
 * ���ܣ������ٶȣ�������ĺ������ܲ�ͬ�����������ڳ�ʼ���ٶȣ�
 * 		 ��������������ڻ�е�ֶ��������е����ٶȣ��޸ĵ���ȫ���ٶ�
 * ���������speed���ٶ�ֵ����Χ1~100
 */
void UpdateSpeed(unsigned char speed);
u8 SetPoutState(unsigned int data);//�����IO
u8 ResetPoutState(unsigned char num);//�ر����IO
void OpenBrake(unsigned char num);//��ɲ��
void CloseBrake(unsigned char num);//�ر�ɲ��
void AlarmOutput(void);		//ֻ�����
void StandbyOutput(void);	//ֻ���Ƶ�
void RunningOutput(void);	//ֻ���̵�
void OpenFan(void);//�򿪷���
void CloseFan(void);//�رշ���
u32 ReadPoutState(void);//�����IO״̬
unsigned char ReadBrakeState(void);//��ɲ��״̬
u32 ReadPinState(void);//������IO״̬
void SetLimitedPosition(long *plus,long *minus);//��λ����

//#pragma CODE_SECTION(UpdateCurrentPositionGesture, "ramfuncs");//������//2018/06/01 for Extend RAM
void UpdateCurrentPositionGesture(void);	//����λ��
//#pragma CODE_SECTION(JointInterpolation, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char JointInterpolation(long* next_pulse);	//�ؽڲ岹
//#pragma CODE_SECTION(LinearInterpolation, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char  LinearInterpolation(long* next_pulse);	//ֱ�߲岹
//#pragma CODE_SECTION(CircleInterpolation, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char CircleInterpolation(long* next_pulse,unsigned char orientation);//Բ���岹
//#pragma CODE_SECTION(TranslationProccess, "ramfuncs");//������//2018/06/01 for Extend RAM
void TranslationProccess(long *differPulse,unsigned char mode);//�岹�˶�

//#pragma CODE_SECTION(AngleToPulse, "ramfuncs");//������//2018/06/01 for Extend RAM
void AngleToPulse(double *angle,long *pulse);//����ת��������
//#pragma CODE_SECTION(PulseToAngle, "ramfuncs");//������//2018/06/01 for Extend RAM
void PulseToAngle(long *pulse,double *angle);//����ת���ɻ���
//#pragma CODE_SECTION(GetCurrentAngle, "ramfuncs");//������//2018/06/01 for Extend RAM
void GetCurrentAngle(double* angle);//��õ�ǰ�Ƕ�
//#pragma CODE_SECTION(GetCurrentRadian, "ramfuncs");//������//2018/06/01 for Extend RAM
void GetCurrentRadian(double* angle);//��õ�ǰ����

char GetPosition_SCI(double* position);//���ⲿ(����)���λ������
char Data_To_Position(char *data);
unsigned char ReadRobotPara(RobotParaStru* robot_para);
unsigned char SaveRobotPara(RobotParaStru robot_para);

u8 GetStm32Version(u8 *data);

u8 GetSTM32USART_STA(char *data);
//u8 Read_STM32_USART_Data(char* data);
u8 STM32_USART_SentData(char* data,u16 len);
#endif /* SCREENAPI_H_ */
