/*
 * KeyboardDriver.c
 *
 *  Created on: 2015-2-8
 *      Author: xiaojian
 */
#include "EditProgram.h"
#include "My_Project.h"
#include "Man_DriverScreen.h"
#include "Man_MCX31xAS.h"
#include "ScreenApi.h"
#include "stdlib.h"
#include "Touch_screen.h"

#define HC_LD		GpioDataRegs.GPBDAT.bit.GPIO59	//���
#define HC_SO		GpioDataRegs.GPBDAT.bit.GPIO60	//����
#define HC_CLK		GpioDataRegs.GPBDAT.bit.GPIO61  //���

static unsigned char s_mode_flag = 0;
/********************************
 * �������ܣ���ȡ��ǰʾ��ģʽ
 * ���������NONE
 * ���������0������ģʽ	1��ֱ�߲岹ģʽ
 */
unsigned char GetModeState(void)
{
	return s_mode_flag;
}

/********************************
 * �������ܣ�����ʾ��ģʽ
 * ���������0������Ϊ����ģʽ	1������Ϊֱ�߲岹ģʽ
 * ���������NONE
 */
void SetModeState(unsigned char mode)
{
	if(mode >=2) mode =0;
	s_mode_flag = mode;
	SetButtonValue(7, 65, s_mode_flag);
}

void Clear(void);
/********************************
 * �������ܣ���ʵ�尴��ֵ
 * ���������none
 * ����ֵ��   ����ֵ
 * //������//2018/06/01
 */
unsigned long JudgeKeyboardValue(void)
{
	unsigned long value=0;
	unsigned char i;

	HC_CLK = 0;
	//���²�������
	HC_LD = 1;
	NOP;NOP;NOP;//LD����������15ns
	NOP;NOP;NOP;//LD����������15ns
	HC_LD = 0;	//������������Ĵ���
	NOP;NOP;NOP;//�������ݽ���ʱ������10ns
	NOP;NOP;NOP;//�������ݽ���ʱ������10ns
	HC_LD = 1;	//LD�ߵ�ƽ�ڼ䣬�����������ܱ���ֹ
	NOP;NOP;NOP;
	NOP;NOP;NOP;
	NOP;NOP;NOP;//SH���ٱ���40ns���ܶ�����
	NOP;NOP;NOP;
	NOP;NOP;NOP;
	NOP;NOP;NOP;//SH���ٱ���40ns���ܶ�����
	//��ʼ������
	for(i=0;i<32;i++)
	{
		HC_CLK = 0;
		NOP;NOP;
		NOP;NOP;
		NOP;NOP;//CLK����������25ns
		NOP;NOP;
		NOP;NOP;
		NOP;NOP;//CLK����������25ns
		HC_CLK = 1;
		value <<=1;
		if(HC_SO)//�ߵ�ƽ
		{
			value = value | 0x00000001;
		}
	}
	value = value | 0xFE;//bit1~bit7û�õ����Ѿ��ӵ�
	delay_1us(10);

	return ~value;
}
/********************************
 * �������ܣ���ʵ�尴��ֵ
 * �������������ֵ
 * ����ֵ��	 none
 * ������//2018/06/01
 */
//#pragma CODE_SECTION(ReadKeyboardValue, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned long ReadKeyboardValue(void)
{
	unsigned long key_value_first=0,key_value_second=0;
	delay_1ms(10);
	key_value_first=JudgeKeyboardValue();
	return key_value_first;
	//key_value_second=JudgeKeyboardValue();
/*
	if(key_value_first==key_value_second)
	{
		return key_value_first;
	}
	else
	{
		return 0;
	}
*/
}
/********************************
 * �������ܣ�����ʵ�尴������
 * �������������ֵ
 * ����ֵ��	 none
 */
//#pragma CODE_SECTION(KeyboardDataProcessing, "ramfuncs");//������//2018/06/01 for Extend RAM
void KeyboardDataProcessing(unsigned long value)
{
	static unsigned long value_save=0;
	static unsigned char s_sta=0;//����״̬
	if(value == value_save)//���µİ�ť����һ����ͬ
	{
		  if(((value_save & 0x00000001)&&(value_save & 0xfc3f0000)) ||
				(value_save & 0xfc3f0000)  )//�岹�˶�(shift + Jx) �� �����˶�(Jx)
		  {
			   /*if(s_sta == 0)
			   {
				   RecoverSpeed();//ֱ�߲岹�����˶�ǰ���ָ����������ٶ�
			   }*/
			   s_sta = 0;
		  }
		  else
		  {
			   return;
		  }
	}
	//else{
	//	 value_save = value;
	//}
	/*if(value_save & 0xfc3f0000)//�����һ�ΰ��µ��ǵ����˶��İ�ť
	{
		 AxisSingleRun(0,0,0);//��ô��������������ֹͣ����ֹ�Ȱ������ٰ�shift����ʱ�������
		 BaseCoordSlineRun(0,0);//�����갴ť�ɿ�����
	}*/

	G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//ʹ�ð�ȫ����
	switch(value)
	{
	case (unsigned long)0x00000001://32 SHIFT
			  if(s_sta == 0){
				   s_sta = 1;
//				   s_mode_flag++;
//				   s_mode_flag %= 2;
			  }
			  break;

	//case (unsigned long)0x00000002:
	//		  if(s_sta == 0){
	//			   s_sta = 1;
	//		  }
	//		  break;//31

	//case (unsigned long)0x00000004:
//		  if(s_sta == 0){
//			   s_sta = 1;
//		  }
	//		  break;//30

	//case (unsigned long)0x00000008:
			  //		  if(s_sta == 0){
			  //			   s_sta = 1;
			  //		  }
	//		  break;//29

	//case (unsigned long)0x00000010:
			  //		  if(s_sta == 0){
			  //			   s_sta = 1;
			  //		  }
	//		  break;//28

	//case (unsigned long)0x00000020:
			  //		  if(s_sta == 0){
			  //			   s_sta = 1;
			  //		  }
	//		  break;//27

	//case (unsigned long)0x00000040:
			  //		  if(s_sta == 0){
			  //			   s_sta = 1;
			  //		  }
	//		  break;//26

	//case (unsigned long)0x00000080:
			  //		  if(s_sta == 0){
			  //			   s_sta = 1;
			  //		  }
	//		  break;//25

	case (unsigned long)0x00000100://24	��
			  if(s_sta == 0){
				  keyboard_speed_meter(22);
				   s_sta = 1;
			  }
			  break;

	case (unsigned long)0x00000200://23	��
			  if(s_sta == 0){
				   s_sta = 1;
			  }
			  break;

	case (unsigned long)0x00000400://22	��
			  if(s_sta == 0){
				   s_sta = 1;
			  }
			  break;

	case (unsigned long)0x00000800://21	��
			  if(s_sta == 0){
				  keyboard_speed_meter(23);
				   s_sta = 1;
			  }
			  break;


	case (unsigned long)0x00001000://20	MOTOR
			  if(s_sta == 0){
				   s_sta = 1;
				   servo_motor(1);//���ŷ�
			  }
			  break;

	case (unsigned long)0x00002000://LOCK
			  if(s_sta == 0){
				   s_sta = 1;
			  }
			  break;//

	case (unsigned long)0x00004000://R-SET
			  if(s_sta == 0){
				  //clear_alarm();
				   s_sta = 1;
			  }
			  break;//

	case (unsigned long)0x00008000://M-MOD
			  if(s_sta == 0){
				   //IO_Screen_Switch();
				   s_sta = 1;
			  }
			  break;//

	case (unsigned long)0x00010000:	//J2-
			if(s_mode_flag == 0)//����ģʽ
			{
				  if(!DisplayErrInfo(0x14B))//0001 0100 1011
				  {
					  if(s_sta == 0){
						   AxisSingleRun(1,2,1);//J2-
						   s_sta = 1;
					  }
				  }
			}
			else//ֱ��ģʽ
			{
				  if(!DisplayErrInfo(0x3fB))
				  {
					  if(s_sta == 0){
						   BaseCoordSlineRun(1,3);//Y-
						   s_sta = 1;
					  }
				  }
			}
			break;

	case (unsigned long)0x00020000:	//J1-
			if(s_mode_flag == 0)//����ģʽ
			{
				  if(!DisplayErrInfo(0xCB))//0000 1100 1011
				  {
					  if(s_sta == 0){
						   AxisSingleRun(1,1,1);//J1-
						   s_sta = 1;
					  }
				  }
			}
			else//ֱ��ģʽ
			{
				  if(!DisplayErrInfo(0x3fB))
				  {
					  if(s_sta == 0){
						   BaseCoordSlineRun(1,1);//X-
						   s_sta = 1;
					  }
				  }
			}
			break;

	case (unsigned long)0x00040000:	//CANCEL
			  if(s_sta == 0){
				   s_sta = 1;
			  }
			  break;//

	case (unsigned long)0x00080000:	//J2+
			if(s_mode_flag == 0)//����ģʽ
			{
				  if(!DisplayErrInfo(0x14B))//0001 0100 1011
				  {
					  if(s_sta == 0){
						   AxisSingleRun(1,2,0);//J2+
						   s_sta = 1;
					  }
				  }
			}
			else//ֱ��ģʽ
			{
				  if(!DisplayErrInfo(0x3fB))
				  {
					  if(s_sta == 0){
						   BaseCoordSlineRun(1,4);//Y+
						   s_sta = 1;
					  }
				  }
			}
		    break;



	case (unsigned long)0x00100000:	//J1+
			if(s_mode_flag == 0)//����ģʽ
			{
				  if(!DisplayErrInfo(0xCB))//0000 1100 1011
				  {
					  if(s_sta == 0){
						   AxisSingleRun(1,1,0);//J1+
						   s_sta = 1;
					  }
				  }
			}
			else//ֱ��ģʽ
			{
				  if(!DisplayErrInfo(0x3fB))
				  {
					  if(s_sta == 0){
						   BaseCoordSlineRun(1,2);//X+
						   s_sta = 1;
					  }
				  }
			}
			break;


	case (unsigned long)0x00200000:	//OK
			  if(s_sta == 0){
				   s_sta = 1;
			  }
			  break;//

	case (unsigned long)0x00400000://STOP
			  if(s_sta == 0){
				   SetDriCmd(0x3F,0x27);
				   s_sta = 1;
			  }
			  break;


	case (unsigned long)0x00800000://SPEED
			  if(s_sta == 0){
				  keyboard_speed_meter(20);
				   s_sta = 1;
			  }
			  break;//

	case (unsigned long)0x01000000://J6+
			if(s_mode_flag == 0)//����ģʽ
			{
				  if(!DisplayErrInfo(0x4B))//0000 0100 1011
				  {
					  if(s_sta == 0){
						   AxisSingleRun(1,6,0);//J6+
						   s_sta = 1;
					  }
				  }
			}
			else//ֱ��ģʽ
			{
			}
		    break;

	case (unsigned long)0x02000000://J5+
			if(s_mode_flag == 0)//����ģʽ
			{
				  if(!DisplayErrInfo(0x4B))//0000 0100 1011
				  {
					  if(s_sta == 0){
						   AxisSingleRun(1,5,0);//J5+
						   s_sta = 1;
					  }
				  }
			}
			else//ֱ��ģʽ
			{
			}
		    break;

	case (unsigned long)0x04000000:	//J4+
			if(s_mode_flag == 0)//����ģʽ
			{
				  if(!DisplayErrInfo(0x4B))//0000 0100 1011
				  {
					  if(s_sta == 0){
						   AxisSingleRun(1,4,0);//J4+
						   s_sta = 1;
					  }
				  }
			}
			else//ֱ��ģʽ
			{
			}
		    break;

	case (unsigned long)0x08000000://J6-
			if(s_mode_flag == 0)//����ģʽ
			{
				  if(!DisplayErrInfo(0x4B))//0000 0100 1011
				  {
					  if(s_sta == 0){
						   AxisSingleRun(1,6,1);//J6-
						   s_sta = 1;
					  }
				  }
			}
			else//ֱ��ģʽ
			{
			}
			break;

	case (unsigned long)0x10000000://J5-
			if(s_mode_flag == 0)//����ģʽ
			{
				  if(!DisplayErrInfo(0x4B))//0000 0100 1011
				  {
					  if(s_sta == 0){
						   AxisSingleRun(1,5,1);//J5-
						   s_sta = 1;
					  }
				  }
			}
			else//ֱ��ģʽ
			{
			}
			break;


	case (unsigned long)0x20000000:	//J4-
			if(s_mode_flag == 0)//����ģʽ
			{
				  if(!DisplayErrInfo(0x4B))//0000 0100 1011
				  {
					  if(s_sta == 0){
						   AxisSingleRun(1,4,1);//J4-
						   s_sta = 1;
					  }
				  }
			}
			else//ֱ��ģʽ
			{
			}
			break;


	case (unsigned long)0x40000000:	//J3+
			if(s_mode_flag == 0)//����ģʽ
			{
				  if(!DisplayErrInfo(0x24B))//0010 0100 1011
				  {
					  if(s_sta == 0){
						   AxisSingleRun(1,3,0);//J3+
						   s_sta = 1;
					  }
				  }
			}
			else//ֱ��ģʽ
			{
				  if(!DisplayErrInfo(0x3fB))
				  {
					  if(s_sta == 0){
						   BaseCoordSlineRun(1,6);//Z+
						   s_sta = 1;
					  }
				  }
			}
		    break;

	case (unsigned long)0x80000000:	//J3-
			if(s_mode_flag == 0)//����ģʽ
			{
				  if(!DisplayErrInfo(0x24B))//0010 0100 1011
				  {
					  if(s_sta == 0){
						   AxisSingleRun(1,3,1);//J3-
						   s_sta = 1;
					  }
				  }
			}
			else//ֱ��ģʽ
			{
				  if(!DisplayErrInfo(0x3fB))
				  {
					  if(s_sta == 0){
						   BaseCoordSlineRun(1,5);//Z-
						   s_sta = 1;
					  }
				  }
			}
			break;

	default:
		 Clear();
		 display_sixangle(7);//ȫ������ʱ��ʾ������ĽǶ�
		 s_sta = 0;
		 break;
	}
	value_save = value;//������εİ���ֵ
	USART_RX_STA = 0;//ʵ�尴��ʹ�ù����У��㴥������Ч
}

void Clear(void)
{
	 AxisSingleRun(0,0,0);//�ɿ���ť�Ĵ���
	 BaseCoordSlineRun(0,0);//�����갴ť�ɿ�����
}

/********************************
 * �������ܣ��ж��Ƿ���OK��ť
 * ����ֵ��1������	0��û����
 */
unsigned char KeyBoard_PressOK(void)
{
	if(ReadKeyboardValue() == 0x00200000)
	{
		return 1;
	}
	return 0;
}

/********************************
 * �������ܣ��ж��Ƿ���stop��ť
 * ����ֵ��1������	0��û����
 */
unsigned char KeyBoard_PressStop(void)
{
	if(ReadKeyboardValue() == 0x00400000)
	{
		return 1;
	}
	return 0;
}

/********************************
 * �������ܣ��ж��Ƿ��� up ��ť
 * ����ֵ��1������	0��û����
 */
unsigned char KeyBoard_PressUp(void)
{
	if(ReadKeyboardValue() == 0x00000800)
	{
		return 1;
	}
	return 0;
}

/********************************
 * �������ܣ��ж��Ƿ��� down ��ť
 * ����ֵ��1������	0��û����
 */
unsigned char KeyBoard_PressDown(void)
{
	if(ReadKeyboardValue() == 0x00000100)
	{
		return 1;
	}
	return 0;
}

/********************************
 * �������ܣ��ж��Ƿ��� Shift ��ť
 * ����ֵ��1������	0��û����
 */
unsigned char KeyBoard_Shift(void)
{

	if(ReadKeyboardValue() == 0x00000001)
	{
		return 1;
	}
	return 0;
}

static unsigned char s_stop_flag = 0;//������¼��ͣ��û�а���
/*********************************
 * ֹͣ�������У����������е��ô˺���������ͣ��������
 */
void StopProgramRunning(void)
{
	s_stop_flag = 1;
}

void ClearStopProgramFlag(void)
{
	s_stop_flag = 0;
}
//#pragma CODE_SECTION(KeyBoard_StopSpeed, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char KeyBoard_StopSpeed(void)
{
	static unsigned long s_key_value=0;
	char ovrd,str[4];
	unsigned long key_value;
	CheckScreenConnectState();
	if(s_stop_flag == 1)
	{
		//s_stop_flag = 0;
		return 1;
	}
	key_value = ReadKeyboardValue();
	if(key_value != ReadKeyboardValue())
	{
		return 0;
	}

	if(key_value == s_key_value) return 0;

	switch(key_value)
	{
	case 0x00400000://stop
		return 1;
	case 0x00800000://speed
		break;

	case 0x00000800://up
		ovrd = GetOvrd();
		if(ovrd>=100) break;
		if(ovrd >= 50)
		{
			ovrd += 10;
		}
		else if(ovrd >= 5)
		{
			ovrd += 5;
		}
		else
		{
			ovrd += 1;
		}

		if(ovrd > 100)
		{
			ovrd = 100;
		}
		UpdateSpeed(ovrd);
		ltoa(ovrd,str);
		SetTextValue(6, 19, str);//������ʾovrd
		break;

	case 0x00000100://down
		ovrd = GetOvrd();

		if(ovrd <= 1) break;
		if(ovrd >= 60)
		{
			ovrd -= 10;
		}
		else if(ovrd >= 10)
		{
			ovrd -= 5;
		}
		else
		{
			ovrd -= 1;
		}

		if(ovrd <= 0)
		{
			ovrd = 1;
		}
		UpdateSpeed(ovrd);
		ltoa(ovrd,str);
		SetTextValue(6, 19, str);//������ʾovrd
		break;
	default:
		break;
	}
	s_key_value = key_value;
	return 0;
}
//===========================================================================
// No more.
//===========================================================================
