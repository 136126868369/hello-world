/*
 * ScreenApi.c
 *
 *  Created on: 2014-12-21
 *      Author: rd49
 */
//***************************************************************************
//
// FILE:	ScreenApi.c
//
// TITLE:	������API����
//
//***************************************************************************
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//
//***************************************************************************
#include "My_Project.h"
#include "ScreenApi.h"
#include "Touch_screen.h"
#include "EditProgram.h"
#include "KeyboardDriver.h"
#include "kinematic_explain.h"
#include "Man_DriverScreen.h"
#include "Man_MCX31xAS.h"
#include "Man_Nandflash.h"
#include "ReadEncoder.h"
#include "Sanxie_Servo.h"
#include "DSP28335_Spi.h"
static u32 s_PoutState=0xFFFFFFFF;	//0->�����	1->û���
static unsigned int s_WR2state=0x3003;//����MCX314��WR2�Ĵ���ֵ�����Ҫʹ��ʵ��/���������ʱ��s_WR2state=0x3023
static unsigned int s_WR4state=0x0FFF;//����MCX314��WR4�Ĵ���ֵ
static unsigned char s_SpeedMode=0;//��¼ʾ��ģʽ��0->����ʾ��ģʽ  1->����ʾ��ģʽ
static long s_current_speed[6];//ȫ������ʱ����¼ÿ����������ٶȣ�ȫ�����й����е����ٶ�ʱ��Ҫ�õ�
extern unsigned int USART_RX_STA_B;		// ����b�Ľ���״̬���
extern char USART_B_RX_BUF[32];
extern struct EX_Position_stru EX_POSITION;//���ڱ�����ⲿ��õ�λ�����ݣ�����Ӵ��ڻ��λ��
//static u8 rentangle_flag=0x66;  //������//2018/06/01
//static u32 rentangle_number=0;  //������//2018/06/01
#pragma  DATA_SECTION (DEV_NOW, ".MY_MEM0")
static long DEV_NOW[6]={0,0,0,0,0,0}; //������//2018/06/01
#pragma  DATA_SECTION (DEV_LAST, ".MY_MEM0")
static long DEV_LAST[6]={0,0,0,0,0,0};//������//2018/06/01
extern unsigned char Teach_mode;//������//2018/06/01
/***********************************************************
 * ���ܣ������˶�
 * ������pressFlag  ��ť��־λ��  	1����ʾ����   0����ʾ�ɿ�
 * 					�����һ��δ�ɿ�������һ�ΰ�����Ч
 * 		 whichAxis	��ʾ�ĸ���	  	1~6�ֱ��ʾ1~6��
 * 		 sign		��ʾ�������� 	0��������	  1��������
 * ����ֵ��0����ȷ
 * 		   1����е�ֶ��������з�������
 * 		   2����е�ֶ����������ɿ��˰�ȫ���أ�ʾ��ʱ��
 */
unsigned char AxisSingleRun(unsigned char pressFlag,unsigned char whichAxis,unsigned char sign)
{
	 static unsigned char once=1;
	 unsigned char speed,i,err=0;
	 unsigned int spd,spd_tmp;
	 double f_tmp;
	 int	i_tmp;
	 if(pressFlag == 1)//����
	 {
		 if(once == 1)
		  {
			    SetDriCmd(0x3F,0x27);//��ֹͣ
				G_CTRL_BUTTON.CTRL_BUTTON.POS_LIMITED_FLG=0;
				G_CTRL_BUTTON.CTRL_BUTTON.NEG_LIMITED_FLG=0;

				if(!SAVE_BUTTON)//��ȫ���ذ��²��ܿ���
				{
					if(ROBOT_PARAMETER.MOTOR_SELECT & (0x01<<(whichAxis-1)))//��ѡ�еĲ��ܵ������
					{
						spd = RecoverSpeed();
						speed = GetProgramRunSpeed();//��õ�ǰʾ�̵��ٶ�
						SetAcc(0x3f,120);      //���ٶ�  A*125	 1~8000
						SetStartV(0x3F,(unsigned int)speed*2);      //��ʼ�ٶ�   ��Χ��1~8000
						if(ROBOT_PARAMETER.AXIS_ROTATE.all & (0x01<<(whichAxis-1)))
						{
							sign = !sign;
						}

						  if((ROBOT_PARAMETER.SYS == FOUR_AXIS_B||ROBOT_PARAMETER.SYS == FOUR_AXIS_C)
								&& (whichAxis==3))
						  {
							  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
							  {
								  delay_1us(2);
								  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
								  {
									  delay_200ns();
									  err = ReadRR2(0x01<<whichAxis-1);
									  //����λ��ʱ���ܼ�����������
									  if((err&0x01) && (sign==0))
									  {
										  SetDriCmd(0x3F,0x26);
										  return 1;
									  }
									  //����λ��ʱ���ܼ�����������
									  if((err&0x02) && (sign==1))
									  {
										  SetDriCmd(0x3F,0x26);
										  return 1;
									  }
								  }
							  }
						  }

						if((ROBOT_PARAMETER.SYS==FOUR_AXIS_B || ROBOT_PARAMETER.SYS==FOUR_AXIS_C)
								&& (whichAxis == 4))
						{
							SetStartV(0x3F,1);      //��ʼ�ٶ�   ��Χ��1~8000
							if(ROBOT_PARAMETER.SYS==FOUR_AXIS_B)
							{
								spd_tmp = spd%40;		//
								spd_tmp = 40 - spd_tmp;
								spd = spd + spd_tmp;	//��spd�չ�40�ı���
								SetSpeed(0x08, spd);	//���õ������ٶ�
								spd = (spd/40);
								if(spd<=0) spd = 1;
								SetSpeed(0x04, spd);	//���õ������ٶ�
								SetAcc(0x04,160/40);	//���õ�����ļ��ٶ�
								SetDriCmd(0x0C,0x22+sign);//��������������
							}
							else
							{
								f_tmp = (double)spd/24;
								i_tmp = f_tmp;			//�����������
								f_tmp = f_tmp - i_tmp;	//���С������
								spd = spd + (1-f_tmp)*24 + 0.5;	//�չ�24�ı���
								SetSpeed(0x08, spd);	//���õ������ٶ�
								spd = ((double)spd/24*10);
								if(spd<=0) spd = 1;
								SetSpeed(0x04, spd);	//���õ������ٶ�
								SetAcc(0x08,192);	//���õ�����ļ��ٶ�
								SetAcc(0x04,192/24);//���õ�����ļ��ٶ�8
								SetDriCmd(0x0C,0x22+sign);//��������������
							}
						}
						else
						{
							SetDriCmd(0x01<<(whichAxis-1),0x22+sign);//��������������
						}
					}
				}
				once = 0;//������֮������㣬��ֹ�ظ�����
		  }
		  if((ROBOT_PARAMETER.SYS == FOUR_AXIS_B||ROBOT_PARAMETER.SYS == FOUR_AXIS_C)
				&& (whichAxis==3))
		  {
			  //����λ��ʱ����������,��ʱһ��ʱ�䣬�ȹ���λ�׶�
			  if((err&0x01) && (sign==1))
			  {
				  delay_1ms(50);
			  }
			  //����λ��ʱ����������,��ʱһ��ʱ�䣬�ȹ���λ�׶�
			  if((err&0x02) && (sign==0))
			  {
				  delay_1ms(50);
			  }
		  }
		  delay_200ns();
		  delay_1ms(5);
		  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
		  {
			  delay_1us(2);
			  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
			  {
				  for(i=0;i<6;i++)
				  {
				  		delay_200ns();
				  		err |= ReadRR2(0x01<<i);
				  }
				  if(err)
				  {
					   SetDriCmd(0x3F,0x26);
					   return 1;
				  }
			  }
		  }
		  if(SAVE_BUTTON)
		  {
			  delay_1us(2);
			  if(SAVE_BUTTON)
			  {
				  SetDriCmd(0x3f,0x26);
				  return 2;
			  }
		  }

	 }
	 else//�ɿ�
	 {
		  SetDriCmd(0x3f,0x26);//��������ֹͣ
		  //SetAcc(aix,(unsigned int)r);      //���ٶ�  A*125	1~8000
		  //SetDec(aix,(unsigned int)r*0.2);      //���ٶ�  A*125 1~8000
		  while(ReadDriState())//�ȴ���������
		  {
			  delay_1us(1);
		  }
		  SetStartV(0x3f,8000);      //��ʼ�ٶ�   ��Χ��1~8000
		  once = 1;//ֻ�а�ť�ɿ���once�ű�Ϊ1��ֻ��onceΪ1���ܽ�����������
	 }
	 return 0;
}
/********************************************************
 * ���ܣ��ػ�����ֱ���˶�  Base coordinate straight line motion
 * ������pressFlag ��ť��־λ�� 1����ʾ��ť����   0����ť�ɿ�
 * 					�����һ��δ�ɿ�����ô��һ�ΰ��½����������̬
 * 		 orientation  �����־λ  1��X-  2��X+
 * 		 						  3��Y-	 4��Y+
 * 		 						  5��Z-  6��Z+
 * ����ֵ�� 0����ȷ
 * 			1:��е�ֶ��������з�������
 * 			2����е�ֶ����������ɿ��˰�ȫ���أ�ʾ��ʱ��
 * 			4������������Χ
 * 			5: ������;
 * 			6: �����
 */
unsigned char BaseCoordSlineRun(unsigned char pressFlag,unsigned char orientation)
{
	 static unsigned char once=1;
	 u8 err;
	 if(pressFlag==1)//����
	  {
		 if(once==1)
		 {
			  ClearStopProgramFlag();
			  SetDriCmd(0x3f,0x27);//ֹͣ����
			  UpdateCurrentPositionGesture();//���һ����̬
			  G_CTRL_BUTTON.CTRL_BUTTON.POS_LIMITED_FLG=0;
			  G_CTRL_BUTTON.CTRL_BUTTON.NEG_LIMITED_FLG=0;
			  ClearStopProgramFlag();//�����ͣ��־λ
			  RecoverSpeed();
			  once=0;
		 }
		//if((!SAVE_BUTTON)&&(!G_CTRL_BUTTON.CTRL_BUTTON.SERVO_ON))//��ȫ���ذ��£����ŷ�on�򿪲��ܿ���
		//{
			 switch(orientation)
			 {
				  case 1:	MovePx(-ROBOT_PARAMETER.PRECISION);	break;
				  case 2:	MovePx(ROBOT_PARAMETER.PRECISION);	break;

				  case 3:	MovePy(-ROBOT_PARAMETER.PRECISION);	break;
				  case 4:	MovePy(ROBOT_PARAMETER.PRECISION);	break;

				  case 5:	MovePz(-ROBOT_PARAMETER.PRECISION);	break;
				  case 6:	MovePz(ROBOT_PARAMETER.PRECISION);	break;

				  default : break;
			 }
			 err = BaseCoordSlineMinPrecisionRun();
			 if(err == 4)
			 {
					SetScreen(20);
					SetTextValue(20, 4, "");
					SetTextValue(20, 3, "Out of range!");
					delay_1ms(1500);
					SetScreen(GetScreenId());
			 }
			 else if(err == 6)
			 {
					SetScreen(20);
					SetTextValue(20, 4, "");
					SetTextValue(20, 3, "Singularity!");
					delay_1ms(1500);
					SetScreen(GetScreenId());
			 }
			 return err;

		//}
			 //once=0;
	   }
	  else//�ɿ�
	  {
			 while(ReadDriState())//�ȴ���������
			 {
				  delay_200ns();
				  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
				  {
					   SetDriCmd(0x3F,0x27);
					   return 1;
				  }
				  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:����,ʹ�ð�ȫ���ص�ʱ���ɿ�(�ߵ�ƽ)���˳�
				  {
					  delay_1us(1);
					  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)
					  {
						  SetDriCmd(0x3F,0x27);
						  return 2;
					  }
				  }
				  NOP;NOP;
			 }
			 once=1;//ֻ�а�ť�ɿ���once�ű�Ϊ1��Ȼ���´ν�������������ܻ��һ����̬
	  }
	  return 0;
}

/********************************************************
 * ���ܣ��ػ�����̶�ֱ�߾����˶�  Base coordinate straight line Fixed distance
 * ������changeGes  �����Ƿ�ı���̬  1���������̬    0��ʹ����һ�λ�õ���̬
 * 		 movingDistance �ƶ����룬 double���ͣ�������������������xyz��ķ���,��߾��ȵı���
 * 		 orientation  xyz�᷽��  		ֻ������px��py��pz��ָ��
 */
void BaseCoordSlineFixedDistance(unsigned char changeGes,double movingDistance,double *orientation)
{
	 int moveTimes;
	 double precision;

	 if(changeGes == 1)
	 {
		  UpdateCurrentPositionGesture();//���һ���µ���̬
	 }

	 moveTimes = movingDistance/ROBOT_PARAMETER.PRECISION;
	 if(moveTimes>0)
	 {
		  precision = ROBOT_PARAMETER.PRECISION;
	 }
	 else if(moveTimes<0)
	 {
		  precision = (-ROBOT_PARAMETER.PRECISION);
		  moveTimes = -moveTimes;//������
	 }
	 else
	 {
		  return;//0�Ļ�ֱ���˳�
	 }
	 //for(;moveTimes>0;moveTimes--)
	 while(moveTimes--)
	 {
		  *orientation += precision;
		  if(BaseCoordSlineMinPrecisionRun())
		  {
			  break;
		  }
	 }
	 while(ReadDriState())//�ȴ���������
	 {

	 }
}

/********************************************************
 * ���ܣ��ػ�����̶�ֱ����С�����˶�,ʹ��ǰ��Ҫ�ȸı�px��py��pz��ֵ��
 * 		 ����ֱ�ӵ���������
 * ����ֵ�� 0����ȷ
 * 			1:��е�ֶ��������з�������
 * 			2����е�ֶ����������ɿ��˰�ȫ���أ�ʾ��ʱ��
 * 			3�����������а�����stop��ť
 * 			4������������Χ
 * 			5:������;
 * 			6: �����
 */
unsigned char BaseCoordSlineMinPrecisionRun(void)
{
	 long differPulse[6],currentPulse[6],movedPulse[6];
	 double movedAg[6];//,ag[6];
	 unsigned char i,err=0;//13260
/*
	 long pulse_lon;
	 unsigned char pulse_char[4];
	 char str[58];
     char newline[10]="\r\n";
*/
	 if(GetBestSolution(movedAg))//������Ž�//91021
		 return 4;//����������Χ

//	  for(i=0;i<6;i++)
//	  {
//		  ag[i] = *(movedAg+i)*PI_MULTI_180_INVERSE;//����ת�Ƕ�
//	  }

	 while(ReadDriState())//�ȴ���������
	 {
		  //delay_200ns();
		  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
		  {
			  delay_1us(1);
			  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
			  {
				  for(i=0;i<6;i++)
				  {
				  		delay_200ns();
				  		err |= ReadRR2(0x01<<i);
				  }
				  if(err)
				  {
					   SetDriCmd(0x3F,0x27);
					   return 1;
				  }
			  }
		  }
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:����,ʹ�ð�ȫ���ص�ʱ���ɿ�(�ߵ�ƽ)���˳�
		  {
			  delay_1us(1);
			  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)
			  {
				  SetDriCmd(0x3F,0x27);
				  return 2;
			  }
		  }
		  if(KeyBoard_StopSpeed())
		  {
			   SetDriCmd(0x3F,0x26);	//����ֹͣ
			   return 3;
		  }
		  NOP;NOP;
	 }
	  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
	  {
		  delay_1us(1);
		  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
		  {
			  for(i=0;i<6;i++)
			  {
			  		delay_200ns();
			  		err |= ReadRR2(0x01<<i);
			  }
			  if(err)
			  {
				   SetDriCmd(0x3F,0x27);
				   return 1;
			  }
		  }
	  }
	  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:����,ʹ�ð�ȫ���ص�ʱ���ɿ�(�ߵ�ƽ)���˳�
	  {
		  delay_1us(1);
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)
		  {
			  SetDriCmd(0x3F,0x27);	//����ֹͣ
			  return 2;
		  }
	  }
	  if(KeyBoard_StopSpeed())
	  {
		   SetDriCmd(0x3F,0x26);//����ֹͣ
		   return 3;
	  }
	 ReadCurrentPulseLp(currentPulse);//��õ�ǰ��λ�õ����������߼�ֵ��
	 AngleToPulse(movedAg,movedPulse);//�������õ������Ž⻻�������
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
	   //���ƽ�ƺ��λ�õ�������������ֵ��
	   differPulse[i]=movedPulse[i]-currentPulse[i];//��Ҫ�ƶ���������������ֵ�뵱ǰֵ�Ĳ�ֵ
	  //memset(str,0,sizeof(str));
	  //longToString(differPulse[i],str,12);//������ת��Ϊ�ַ�����С�����3λ
	  //strcat(str," ");
	  //STM32_USART_SentData(str,strlen(str)+1);
	 }

		//STM32_USART_SentData(newline,strlen(newline)+1);

	 if(ROBOT_PARAMETER.AXIS_NUM == 6)
	 {
		 //��4���6�����ͻ��ʱ
		 if((differPulse[3] > ROBOT_PARAMETER.RATIO[3]*ROBOT_PARAMETER.MOTOR_PRECISION*0.09) ||
				 (differPulse[5] >ROBOT_PARAMETER.RATIO[5]*ROBOT_PARAMETER.MOTOR_PRECISION*0.09))
		 {
			 //�ж�1,2,3,5��仯�����Ƿ��С
			 if((differPulse[0]>-(ROBOT_PARAMETER.RATIO[0]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)
				&&differPulse[0]<(ROBOT_PARAMETER.RATIO[0]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)) &&
				(differPulse[1]>-(ROBOT_PARAMETER.RATIO[1]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)
				&&differPulse[1]<(ROBOT_PARAMETER.RATIO[1]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)) &&
				(differPulse[2]>-(ROBOT_PARAMETER.RATIO[2]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)
				&&differPulse[2]<(ROBOT_PARAMETER.RATIO[2]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)) &&
				(differPulse[4]>-(ROBOT_PARAMETER.RATIO[4]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)
				&&differPulse[4]<(ROBOT_PARAMETER.RATIO[4]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)) )
			 {
				 //�ж�4,5,6�Ƿ���һ��ֱ��(����5���Ƿ�ӽ�0��)
				 if((currentPulse[4]>-(ROBOT_PARAMETER.RATIO[4]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)
					&&currentPulse[4]<(ROBOT_PARAMETER.RATIO[4]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)))
				 return 6;
			 }
		 }
	 }
	 TranslationProccess(differPulse,1);//ƽ�����ݴ��������������//7000
	 return 0;
}


/********************************************************
 * ���ܣ��Ե�ǰλ�ã����������������
 * ���������delta_pulse ָ��6����ÿ������Ҫ�ߵ���������ָ��
 * 			//mdoe:0->�ؽڲ岹		1->ֱ�߲岹
 * ����ֵ��    0����ȷ
 * 			1:��е�ֶ��������з�������
 * 			2����е�ֶ����������ɿ��˰�ȫ���أ�ʾ��ʱ��
 * 			3�����������а�����stop��ť
 */
unsigned char MovePulse(long* delta_pulse)
{
	 unsigned char i,err=0;//13260
	 while(ReadDriState())//�ȴ���������
	 {
		  //delay_200ns();
		  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
		  {
			  delay_1us(1);
			  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
			  {
				  for(i=0;i<6;i++)
				  {
				  		delay_200ns();
				  		err |= ReadRR2(0x01<<i);
				  }
				  if(err)
				  {
					   SetDriCmd(0x3F,0x27);
					   return 1;
				  }
			  }
		  }
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:����,ʹ�ð�ȫ���ص�ʱ���ɿ�(�ߵ�ƽ)���˳�
		  {
			  delay_1us(1);
			  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)
			  {
				  SetDriCmd(0x3F,0x27);
				  return 2;
			  }
		  }
		  if(KeyBoard_StopSpeed())
		  {
			   SetDriCmd(0x3F,0x27);
			   return 3;
		  }
		  NOP;NOP;
	 }
	  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
	  {
		  delay_1us(1);
		  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
		  {
			  for(i=0;i<6;i++)
			  {
			  		delay_200ns();
			  		err |= ReadRR2(0x01<<i);
			  }
			  if(err)
			  {
				   SetDriCmd(0x3F,0x27);
				   return 1;
			  }
		  }
	  }
	  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:����,ʹ�ð�ȫ���ص�ʱ���ɿ�(�ߵ�ƽ)���˳�
	  {
		  delay_1us(1);
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)
		  {
			  SetDriCmd(0x3F,0x27);
			  return 2;
		  }
	  }
	  if(KeyBoard_StopSpeed())
	  {
		   SetDriCmd(0x3F,0x27);
		   return 3;
	  }
	 TranslationProccess(delta_pulse,1);//ƽ�����ݴ��������������//7000
	 return 0;
}

/*
 * ĩ������ֱ���˶�
 */
void XEndCoordSlineRunP(unsigned char pressFlag)
{

}
void XEndCoordSlineRunM(unsigned char pressFlag)
{

}
void YEndCoordSlineRunP(unsigned char pressFlag)
{

}
void YEndCoordSlineRunM(unsigned char pressFlag)
{

}
void ZEndCoordSlineRunP(unsigned char pressFlag)
{

}
void ZEndCoordSlineRunM(unsigned char pressFlag)
{

}


/******************************************
 * �������ܣ����µ�ǰλ����̬�������Ե�ǰ����̬ƽ�ƣ��Ȼ��/������̬
 * ���������none
 * ����ֵ��	 none
 */
void UpdateCurrentPositionGesture(void)
{
	 long current_pulse[6];
	 double angle[6];
	 //ReadCurrentPulseEp(current_pulse);	//��õ�ǰ������
	 ReadCurrentPulseLp(current_pulse);//������//2018/06/01
	 //��û���
	 PulseToAngle(current_pulse,angle);
	 UpdatePositionGesture(angle);	//����λ����̬��
	 //currentEp[2] = -currentEp[2];
	 //currentEp[4] = -currentEp[4];
	 /*for(i=0;i<6;i++)
	 {
		  SetLp(0x01<<i,currentEp[i]);//xxx:ÿ�ζ�ʵ��ֵ��ʱ�򣬸����߼�ֵ��ʹ����ͳһ(ֱ��Բ���岹��ʱ����Lp�����������Ep)
	 }*/
}
/****************************************
 *���ܣ����˶�оƬ��ȡʵ������ֵ
 */
void ReadCurrentPulseEp(long *pulse)
{
/*
	unsigned char i;
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
		  *(pulse+i)=ReadPulseLp(0x01<<i);//��Lp�޸�ΪEp//2018/06/01
	 }
*/
	unsigned char i;
	/*
	//ͨ�����ڶ�ȡ�ŷ��������ı�����λ������
	//UpdatePulseFromServoForEp(pulse);
	*/
	//ͨ����ȡ�ŷ��������ķ���λ��������������������λ��
	for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	{
		*(pulse+i)=ReadPulseEp(0x01<<i);//2018/06/01
	}
	if(ROBOT_PARAMETER.MOTOR_TYPE == 2)
	{
		//��Э�����Ҫ��ȥ���ƫ�����������������
		for(i=0;i<6;i++)
		{
			pulse[i] = pulse[i] - ROBOT_PARAMETER.ENCODER_LEFT[i];
		}
	}
	//�ŷ��Ŵ�����������˷�������ת�Ļ������������������෴��
	for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	{
		if(ROBOT_PARAMETER.MOTOR_DIR.all & (0x01<<i))
		{
			pulse[i] = -pulse[i];
		}
		else
		{
			pulse[i] = pulse[i];
		}
	}
	//��ȥ���״̬,�õ����������ʵ�ʵ�λ��
	for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	{
		pulse[i] = pulse[i] + ROBOT_PARAMETER.RATIO[i]*ROBOT_PARAMETER.MOTOR_PRECISION*
								 	(ROBOT_PARAMETER.ORIGIN_POSITION[i]/360.0);
	}
	//4�Ż����ݲ���(TU2һ¥Ԫ���о��Լ�����)(100W�õ����֮��)
	//-()�ĸ�����ӣ��������
	// ()�ĸ���������������
	if(ROBOT_PARAMETER.AXIS_NUM == 6)
	{
		pulse[0]=-(pulse[0]-5555);//-5554
		pulse[1]=pulse[1]+19724;//19667
		pulse[2]=-pulse[2]+744;//2103
		pulse[3]=-(pulse[3]-3944);//-3638
		pulse[4]=-(pulse[4]-8479);//-8731
		pulse[5]=-(pulse[5]-15708);//-14977
	}
	WriteMCX312_WR4(s_PoutState&0xffff);//��Lp��Epʱ�������Ĵ����ᱻ����
	WriteWR2(0x3F,s_WR2state);
}
/****************************************
 *���ܣ����˶�оƬ��ȡ�߼�����ֵ
 */
void ReadCurrentPulseLp(long *pulse)
{
	 unsigned char i;
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
		  *(pulse+i)=ReadPulseLp(0x01<<i);//��Lp�޸�ΪEp//2018/06/01
	 }
	 WriteMCX312_WR4(s_PoutState&0xffff);//��Lp��Epʱ�������Ĵ����ᱻ����
	 WriteWR2(0x3F,s_WR2state);
}
/****************************************
 *���ܣ���õ�ǰÿ��ĽǶ�
 *������angle �Ƕ���ָ��
 */
void GetCurrentAngle(double* angle)
{
	 unsigned char i;
	 long pulse[6];
	 ReadCurrentPulseLp(pulse);
	 PulseToAngle(pulse,angle);
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
		  *(angle+i) = *(angle+i)*PI_MULTI_180_INVERSE;//����ת�Ƕ�
	 }
}

/****************************************
 *���ܣ���õ�ǰÿ��Ļ���
 *������angle �Ƕ���ָ��
 */
void GetCurrentRadian(double* angle)
{
	 long pulse[6];
	 //ReadCurrentPulseEp(pulse);
	 ReadCurrentPulseLp(pulse);//������//2018/06/01
	 PulseToAngle(pulse,angle);
}

/****************************************
 *���ܣ����ⲿ(����)���λ������
 *������position ָ��λ�õ�ָ��
 *����ֵ�����ݸ��� 1->һ������  2->��������  3->�������� 6->��������
 *����ֵ��-1 -> ͨ�ų���
 */
char GetPosition_SCI(double* position)//���ⲿ���λ������
{
	int i;
	for(i=0;i<6;i++)
	{
		position[i] = EX_POSITION.EX_POSITION[i];
	}

	return EX_POSITION.type;
}

/****************************************
 *���ܣ���STM32(����)���λ������
 *������data:ָ�����ݵ�ָ��
 *����ֵ��0->���ݻ�ȡ�ɹ�		1->ʧ��
 */
extern int RX_POSITION_STA;
char Data_To_Position(char *data)
{
	int i=0;
	u8 flg;

	//ֱ����������
	while((data[i] != '+') && (data[i] != '-') &&
			(data[i] < '0' || data[i] > '9'))
	{
		i++;
		if(i>(STM32_USART_LEN-1))break;
	}
	flg = data[i];//��һ�����ݴ����Ƿ���Ч,'1'��Ч  '0'��Ч
	i++;
	if(flg == '1')
	{
		//ֱ����������
		while((data[i] != '+') && (data[i] != '-') &&
				(data[i] < '0' || data[i] > '9'))
		{
			i++;
			if(i>(STM32_USART_LEN-1))break;
		}
		EX_POSITION.EX_POSITION[0] = atof(&data[i]);//��ȡ����
		EX_POSITION.type = 1;
		//��������
		while((data[i] == '+') || (data[i] == '-') ||
				(data[i] == '.') ||
				(data[i] >= '0' && data[i] <= '9'))
		{
			i++;
			if(i>(STM32_USART_LEN-1))
			{
				EX_POSITION.type = 0;
				return 1;
			}
		}
		//ֱ����������
		while((data[i] != '+') && (data[i] != '-') &&
				(data[i] < '0' || data[i] > '9'))
		{
			i++;
			if(i>(STM32_USART_LEN-1))
			{
				EX_POSITION.type = 0;
				return 1;
			}
		}
		EX_POSITION.EX_POSITION[1] = atof(&data[i]);
		EX_POSITION.type = 2;
		//��������
		while((data[i] == '+') || (data[i] == '-') ||
				(data[i] == '.') ||
				(data[i] >= '0' && data[i] <= '9'))
		{
			i++;
			if(i>(STM32_USART_LEN-1))
			{
				EX_POSITION.type = 0;
				return 1;
			}
		}

		if(data[i] != 0x0d && data[i] != 0x0)//������0x0d����ʾ��������
		{
			//ֱ����������
			while((data[i] != '+') && (data[i] != '-') &&
					(data[i] < '0' || data[i] > '9'))
			{
				i++;
				if(i>(STM32_USART_LEN-1))
				{
					return 1;
				}
			}
			EX_POSITION.EX_POSITION[2] = atof(&data[i]);
			EX_POSITION.type = 3;
			//��������
			while((data[i] == '+') || (data[i] == '-') ||
					(data[i] == '.') ||
					(data[i] >= '0' && data[i] <= '9'))
			{
				i++;
				if(i>(STM32_USART_LEN-1))
				{
					EX_POSITION.type = 0;
					return 1;
				}
			}
		}

		if(data[i] != 0x0d && data[i] != 0x0)//������0x0d����ʾ��������
		{
			//ֱ����������
			while((data[i] != '+') && (data[i] != '-') &&
					(data[i] < '0' || data[i] > '9'))
			{
				i++;
				if(i>(STM32_USART_LEN-1))
				{
					return 1;
				}
			}
			EX_POSITION.EX_POSITION[3] = atof(&data[i]);
			EX_POSITION.type = 4;
			//��������
			while((data[i] == '+') || (data[i] == '-') ||
					(data[i] == '.') ||
					(data[i] >= '0' && data[i] <= '9'))
			{
				i++;
				if(i>(STM32_USART_LEN-1))
				{
					EX_POSITION.type = 0;
					return 1;
				}
			}
		}

		if(data[i] != 0x0d && data[i] != 0x0)//������0x0d����ʾ��������
		{
			//ֱ����������
			while((data[i] != '+') && (data[i] != '-') &&
					(data[i] < '0' || data[i] > '9'))
			{
				i++;
				if(i>(STM32_USART_LEN-1))
				{
					return 1;
				}
			}
			EX_POSITION.EX_POSITION[4] = atof(&data[i]);
			EX_POSITION.type = 5;
			//��������
			while((data[i] == '+') || (data[i] == '-') ||
					(data[i] == '.') ||
					(data[i] >= '0' && data[i] <= '9'))
			{
				i++;
				if(i>(STM32_USART_LEN-1))
				{
					EX_POSITION.type = 0;
					return 1;
				}
			}
		}

		if(data[i] != 0x0d && data[i] != 0x0)//������0x0d����ʾ��������
		{
			//ֱ����������
			while((data[i] != '+') && (data[i] != '-') &&
					(data[i] < '0' || data[i] > '9'))
			{
				i++;
				if(i>(STM32_USART_LEN-1))
				{
					return 1;
				}
			}
			EX_POSITION.EX_POSITION[5] = atof(&data[i]);
			EX_POSITION.type = 6;
			//��������
			while((data[i] == '+') || (data[i] == '-') ||
					(data[i] == '.') ||
					(data[i] >= '0' && data[i] <= '9'))
			{
				i++;
				if(i>(STM32_USART_LEN-1))
				{
					EX_POSITION.type = 0;
					return 1;
				}
			}
		}

		if(EX_POSITION.type == 6)
		{	//�Ƕ�ת��Ϊ����
			EX_POSITION.EX_POSITION[3] = EX_POSITION.EX_POSITION[3]*PI_DIVIDE_180;
			EX_POSITION.EX_POSITION[4] = EX_POSITION.EX_POSITION[4]*PI_DIVIDE_180;
			EX_POSITION.EX_POSITION[5] = EX_POSITION.EX_POSITION[5]*PI_DIVIDE_180;
		}

		if((EX_POSITION.EX_POSITION[0]==0) && (EX_POSITION.EX_POSITION[1]==0))//������0˵������������
		{
			RX_POSITION_STA=0;
			EX_POSITION.type = 0;
		}
		else
			RX_POSITION_STA=1;
	}
	else
	{
		RX_POSITION_STA = 0;
	}
	return 0;
}
/*****************************************
 * ���ܣ������������Ƶ��
 * ����	aix:bit0~5�ֱ��Ӧ1~6�ᣬ 0����ʾûѡ��  1����ʾѡ��
 * 		r:�ٶ�ֵ��1~8000��1���ٶ���С��8000���ٶ����
 */
void SetSpeed(unsigned char aix,unsigned int r)
{
		//RANGE_MCX=RANGE/r;//(RANGE = 800,000,000)
		//SetRange(0x3f,RANGE_MCX/100);  //(8000000/R)
	 if(r <= 1)	r=1;
	 //SetAcc(aix,(unsigned int)r);      //���ٶ�  A*125	1~8000
	 //SetDec(aix,(unsigned int)r*0.2);      //���ٶ�  A*125 1~8000
	 //SetStartV(aix,(unsigned int)r);      //��ʼ�ٶ�   ��Χ��1~8000
	 SetPulseFreq(aix,r);	//�����ٶ� 1~8000���������Ƶ�ʣ�
}

/*****************************************
 * ���ܣ������ٶȣ�������ĺ������ܲ�ͬ�����������ڳ�ʼ���ٶȣ�
 * 		 ��������������ڻ�е�ֶ��������е����ٶ�,�޸ĵ���ȫ���ٶ�
 * ���������ovrd��ȫ���ٶ�ֵ����Χ1~100
 */
void UpdateSpeed(unsigned char ovrd)
{
	unsigned char i,current_all_speed,new_all_speed;
	float persentage;
	if(ovrd <= 0)
	{
		ovrd = 1;
	}
	else if(ovrd > 100)
	{
		ovrd = 100;
	}
	current_all_speed = GetProgramRunSpeed();
	new_all_speed  = ModifyOvrd(ovrd);

	persentage = (float)new_all_speed/current_all_speed;//�ٶ����ӵİٷֱ�
	for(i=0;i<6;i++)
	{
		s_current_speed[i] = s_current_speed[i] * persentage;
		if(s_current_speed[i]>8000)
		{
			s_current_speed[i] = 8000;
		}
		if(s_current_speed[i]<1)
		{
			s_current_speed[i] = 1;
		}
		SetPulseFreq(0x01<<i,s_current_speed[i]);
	}
}
/*****************************************
 * ��IO��(��ص�ͨ)
 * ���������num�������򿪵�IO��,��Χ0~23���ֱ��ӦIO 0~23
 * ����ֵ�� 0->OK 1->��STM32ͨ���쳣 2->IO num������Χ
 */
u8 SetPoutState(unsigned int num)
{
	 u8 tx_cmd[5],rx_cmd[5],t=0;
	 if(num >= MAX_POUT_NUM) return 2;
	 s_PoutState = s_PoutState&(~((long)0x01<<(num+4)));//0~3Ԥ����ɲ��


	 if(num<=11)
	 {
		 WriteMCX312_WR4(s_PoutState&0xffff);//bit0~bit15�ֱ��Ӧ��·���ϵ�EOUT1~EOUT16��
								  //�͵�ƽIO����ص�ͨ���ߵ�ƽIO����ز���ͨ
		 SetButtonValue(12,num+2,1);//��ť������
	 }
	 else
	 {
		 //ͨ��spi�������ݸ�stm32������IO��IO12~23��stm32���Ƶ�
		 tx_cmd[0] = T_CMD_SET_IO;
		 tx_cmd[1] = (s_PoutState>>24)&0x00ff;
		 tx_cmd[2] = (s_PoutState>>16)&0x00ff;
		 tx_cmd[3] = 0;
		 tx_cmd[4] = 0;

		while(1)
		{
			SpiSendCmd(tx_cmd);
			delay_1ms(1);
			if(SpiReceiveCmd(rx_cmd) == 0)
			{
				if(rx_cmd[0] != TR_CMD_OK)
				{
					return 1;
				}
				else
				{
					break;
				}
			}
			t++;
			delay_1ms(1);
			if(t >MXA_REPEAT) return 1;
		}

		SetButtonValue(12,num+8,1);//��ť������
	 }
	 return 0;
}
/*****************************************
 * �ر�IO��(��ز���ͨ)
 * ���������num�������رյ�IO��,��Χ0~23���ֱ��ӦIO 0~23
 * ����ֵ�� 0->OK 1->��STM32ͨ���쳣 2->IO num������Χ
 */
u8 ResetPoutState(unsigned char num)
{
	 u8 tx_cmd[5],rx_cmd[5],t=0;
	 if(num>=MAX_POUT_NUM) return 2;
	 s_PoutState = s_PoutState|((long)0x01<<(num+4));//0~3Ԥ����ɲ��


	 if(num<=11)
	 {
		 WriteMCX312_WR4(s_PoutState&0xffff);//bit0~bit23�ֱ��Ӧ��·���ϵ�EOUT1~EOUT24��
								  //�͵�ƽIO����ص�ͨ���ߵ�ƽIO����ز���ͨ
		 SetButtonValue(12,num+2,0);//��ť����ȥ
	 }
	 else
	 {
		 //ͨ��spi�������ݸ�stm32������IO��IO12~23��stm32���Ƶ�
		 tx_cmd[0] = T_CMD_SET_IO;
		 tx_cmd[1] = (s_PoutState>>24)&0x00ff;
		 tx_cmd[2] = (s_PoutState>>16)&0x00ff;
		 tx_cmd[3] = 0;
		 tx_cmd[4] = 0;

		while(1)
		{
			SpiSendCmd(tx_cmd);
			delay_1ms(1);
			if(SpiReceiveCmd(rx_cmd) == 0)
			{
				if(rx_cmd[0] != TR_CMD_OK)
				{
					return 1;
				}
				else
				{
					break;
				}
			}
			t++;
			delay_1ms(1);
			if(t >MXA_REPEAT)break;
		}

		 SetButtonValue(12,num+8,0);//��ť����ȥ
	 }
	 return 0;
}

/*****************************************
 * ����������ʱ������ƣ��������ֵƹر�
 * ���������none
 * ����ֵ��  none
 */
void AlarmOutput(void)
{
	SetPoutState(9);	//�����
	SetPoutState(10);	//�����
	ResetPoutState(7);	//�ر��̵�
	ResetPoutState(8);	//�رջƵ�
//	delay_1ms(3);
}

/*****************************************
 * ������ʱ�����Ƶƣ��������ֵƹر�
 * ���������none
 * ����ֵ��  none
 */
void StandbyOutput(void)
{
	SetPoutState(8);	//���Ƶ�
	ResetPoutState(7);	//�ر��̵�
	ResetPoutState(9);	//�رպ��
	ResetPoutState(10);	//�رպ��
//	delay_1ms(3);
}

/*****************************************
 * �����е�ʱ�����̵ƣ��������ֵƹر�
 * ���������none
 * ����ֵ��  none
 */
void RunningOutput(void)
{
	ResetPoutState(9);	//�رպ��
	ResetPoutState(10);	//�رպ��
	ResetPoutState(8);
//	SetPoutState(11);	//���̵�
//	ResetPoutState(10);	//�رջƵ�
//	delay_1ms(3);
}

/*****************************************
 * ��ɲ��
 * ���������num�������򿪵�ɲ��,���뷶Χ0~2���ֱ��ӦEOUT14~16,Ҳ����1~3��ĵ���ƶ���ɲ����
 * ����ֵ��  none
 */
void OpenBrake(unsigned char num)
{
	 if(num>=3) return;
	 s_PoutState = s_PoutState&(~(0x0001<<(num)));
	 WriteMCX312_WR4(s_PoutState&0xffff);//bit0~bit15�ֱ��Ӧ��·���ϵ�EOUT1~EOUT16��
							  //�ߵ�ƽIO����ص�ͨ���͵�ƽIO����ز���ͨ
	 SetButtonValue(12,num+15,1);//��ť������
}

/*****************************************
 * �ر�ɲ��
 * ���������num�������رյ�ɲ��,���뷶Χ0~2���ֱ��ӦEOUT14~16,Ҳ����1~3��ĵ���ƶ���ɲ����
 * ����ֵ��  none
 */
void CloseBrake(unsigned char num)
{
	 if(num>=3) return;
	 s_PoutState = s_PoutState|(0x0001<<(num));
	 WriteMCX312_WR4(s_PoutState&0xffff);//bit0~bit15�ֱ��Ӧ��·���ϵ�EOUT1~EOUT16��
							  //�ߵ�ƽIO����ص�ͨ���͵�ƽIO����ز���ͨ
	 SetButtonValue(12,num+15,0);//��ť����ȥ
}

/*****************************************
 * �������ܣ��򿪷���
 * ���������NONE
 * ���������NONE
 */
void OpenFan(void)
{
	 RunningOutput();	//���̵�
	 s_PoutState = s_PoutState&(~(0x0001<<3));
	 WriteMCX312_WR4(s_PoutState&0xffff);//bit0~bit15�ֱ��Ӧ��·���ϵ�EOUT1~EOUT16��
								  //�ߵ�ƽIO����ص�ͨ���͵�ƽIO����ز���ͨ
}

/*****************************************
 * �������ܣ��رշ���
 * ���������NONE
 * ���������NONE
 */
void CloseFan(void)
{
	StandbyOutput();	//���Ƶ�
	s_PoutState = s_PoutState|(0x0001<<3);
	WriteMCX312_WR4(s_PoutState&0xffff);//bit0~bit15�ֱ��Ӧ��·���ϵ�EOUT1~EOUT16��
}

/*****************************************
 * �����IO״̬
 * ����ֵ��s_PoutState:IO״̬,bit0~bit23�ֱ����IO 0~23,0:��״̬	1��IO���ڹ�״̬
 */
u32 ReadPoutState(void)
{
	 return s_PoutState>>4;//0000 0000 0000 1111 bit0~3ɲ��ռ��
}

/*****************************************
 * �������ܣ���ɲ��״̬
 * ����ֵ��bit0~bit2���ֱ����1~3���ɲ��
 * 		   1:ɲ���ѹر� 	0:ɲ���Ѵ�
 */
unsigned char ReadBrakeState(void)
{
	return s_PoutState&0x0f;//0000 0000 0000 1111
}
/*****************************************
 * ������IO״̬(�ѽ���������)
 * ����ֵ������IO״̬��ǰ18bits��1�����źţ�0��û�źţ�
 */
u32 ReadPinState(void)
{
	 u32 state=0;
	 state = ReadMCX312_RR4();
	 state |= ReadMCX312_RR5()<<6;
	 state |= (long)ReadMCX314_RR4()<<12;
	 return ~state;
}

/*****************************************
 * ���ܣ��趨��λ
 * ������plus:  ����λֵ
 * 		 minus������λֵ
 */
void SetLimitedPosition(long *plus,long *minus)
{
	 unsigned char i;
	 for(i=0;i<6;i++)
	 {
		  SetCompp(0x01<<i,*(plus+i));
		  SetCompm(0x01<<i,*(minus+i));
	 }
	 WriteMCX312_WR4(s_PoutState&0xffff);
}
/*****************************************
 * �ؽڲ岹 Joint interpolation
 * ����ֵ�� 0����ȷ
 * 			1�����������з�������
 * 			2�������������ɿ��˰�ȫ����
 * 			3�����������а�����stop��ť
 */
extern unsigned char THE_FOUR_AIXS_XUANZHUAN;
unsigned char JointInterpolation(long* next_pulse)
{
	 unsigned char i,err=0;
	 long current_pulse[6],differ_pulse[6];
	 //ReadCurrentPulseEp(current_pulse);
	 ReadCurrentPulseLp(current_pulse);//������//2018/06/01
	 //WriteWR2(0x3F,0x3003);//����ʹ�ܱ�������Ϊֱ�߲岹������Ĵ������޶˶˱����㣩
	 for(i=0;i<6;i++)
	 {
		  differ_pulse[i] = *(next_pulse+i) - current_pulse[i];
	 }
	 SetStartV(0x3F,(unsigned int)GetProgramRunSpeed()*6);//���ó��ٶ�
	 //SetStartV(0x3F,8000);//���ó��ٶ�
	 TranslationProccess(differ_pulse,0);//��ʼ�˶�
	 while(ReadDriState())//�ȴ���������
	 {
//		  if((ROBOT_PARAMETER.AXIS_NUM == 4) && (THE_FOUR_AIXS_XUANZHUAN==1))
//		  {
//			  if(ReadPinState()&(0x0001<<9))//�����9����IOΪ��Ч����ֹͣ����
//			  {
//				  SetDriCmd(0x3F,0x27);
//				  break;
//			  }
//		  }
		  delay_1us(10);
		  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
		  {
			  i=0;
			  for(i=0;i<6;i++)
			  {
			  		delay_200ns();
			  		err |= ReadRR2(0x01<<i);
			  }
			  if(err)
			  {
				   SetDriCmd(0x3F,0x27);
				   SetStartV(0x3F,8000);//���ٶȻָ������ֵ
				   return 1;
			  }
		  }
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:����,ʹ�ð�ȫ���ص�ʱ���ɿ�(�ߵ�ƽ)���˳�
		  {
			  delay_1us(1);
			  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)
			  {
				  SetDriCmd(0x3F,0x27);
				  SetStartV(0x3F,8000);
			   	   return 2;
			  }
		  }
		  if(KeyBoard_StopSpeed())
		  {
			   SetDriCmd(0x3F,0x26);	//����ֹͣ
			   SetStartV(0x3F,8000);
			   return 3;
		  }
		  NOP;NOP;
	 }
	  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
	  {
		  delay_1us(1);
		  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
		  {
			  for(i=0;i<6;i++)
			  {
			  		delay_200ns();
			  		err |= ReadRR2(0x01<<i);
			  }
			  if(err)
			  {
				   SetDriCmd(0x3F,0x27);
				   return 1;
			  }
		  }
	  }
	  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:����,ʹ�ð�ȫ���ص�ʱ���ɿ�(�ߵ�ƽ)���˳�
	  {
		  delay_1us(1);
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)
		  {
			  SetDriCmd(0x3F,0x27);
			  return 2;
		  }
	  }
	  if(KeyBoard_StopSpeed())
	  {
		   SetDriCmd(0x3F,0x26);	//����ֹͣ
		   return 3;
	  }
	 SetStartV(0x3F,8000);
	 return 0;
}
/*****************************************
 * ֱ�߲岹 Linear interpolation
 * ����ֵ�� 0��ֱ�߲岹�ɹ�
 * 			1�����������з�������
 * 			2�������������ɿ��˰�ȫ����
 * 			3�����������а�����stop��ť
 * 			4������������Χ
 * 			5�����гɹ�������̬��һ��
 * 			6:�����
 */
unsigned char LinearInterpolation(long* next_pulse)
{
	 unsigned char err=0,speed,gestrue_flg=0,j;
	 unsigned int t=0;
	 long current_pulse[6],i;
	 double current_angle[6],next_angle[6];
	 double differ_p[3],delta_p[3],t_p;
	 unsigned int times[3],max;
	 long delta_pulse[6],currentPulse[6],movedPulse[6],delta_pulse_456[3];
	 double movedAg[6];
	 PositionGesture current_ges_pos,next_ges_pos;
	 UpdateCurrentPositionGesture();			//���µ�ǰλ����̬
	 PulseToAngle(next_pulse,next_angle);		//����һ������λ��ת��Ϊ�Ƕ�
	 //ReadCurrentPulseEp(current_pulse);			//����ǰ����λ��
	 ReadCurrentPulseLp(current_pulse);//������//2018/06/01
	 PulseToAngle(current_pulse,current_angle);	//����ǰ����λ��ת��Ϊ�Ƕ�
	 next_ges_pos = GetPositionGesture(next_angle);// xxx �����Ż�
	 current_ges_pos = GetPositionGesture(current_angle);
	 if(CompareTowGesture(next_ges_pos,current_ges_pos))
	 {
		 return 5;//�����returnע�͵�,�Ϳ�����̬��һ��ʱ,��ֱ�߱߾��ȱ仯��̬��ʽ����

		 gestrue_flg = 1;//�����̬��һ��
		 if(ROBOT_PARAMETER.AXIS_NUM == 4)
		 {
			 return 5;
		 }
	 }

	 differ_p[0] =  next_ges_pos.px - current_ges_pos.px;//��Ҫ�ƶ��ľ���
	 differ_p[1] =  next_ges_pos.py - current_ges_pos.py;
	 differ_p[2] =  next_ges_pos.pz - current_ges_pos.pz;

	 for(i=0;i<3;i++)
	 {
		  if(differ_p[i]<0)
		  {
			   differ_p[i] = -differ_p[i];
			   t |= 0x01<<i;//��¼�ĸ��Ǹ���
		  }
	 }
	 speed = GetProgramRunSpeed();

	 if(gestrue_flg == 1)	//��̬��һ��
	 {
		 if(speed > 80)
		 {
			 t_p = ROBOT_PARAMETER.PRECISION+0.25;
		 }
		 else if(speed < 50)
		 {
			 t_p = ROBOT_PARAMETER.PRECISION;
		 }
		 else
		 {
			 t_p = ROBOT_PARAMETER.PRECISION+0.1;
		 }
		 for(i=0;i<3;i++)
		 {
			  times[i] = differ_p[i]/t_p;
		 }
	 }
	 else	//��̬һ��
	 {
		 if(speed > 80)
		 {
			 t_p = ROBOT_PARAMETER.PRECISION+0.2;
		 }
		 else if(speed < 50)
		 {
			 t_p = ROBOT_PARAMETER.PRECISION;
		 }
		 else
		 {
			 t_p = ROBOT_PARAMETER.PRECISION+0.1;
		 }
		 for(i=0;i<3;i++)
		 {
			  times[i] = differ_p[i]/t_p;
		 }
	 }
	 max = times[0]>times[1]?times[0]:times[1];
	 max = max>times[2]?max:times[2];		//�ƶ��Ĵ���

	 for(i=0;i<3;i++)
	 {
		  if(t&(0x01<<i))
		  {
			   delta_p[i] = -differ_p[i]/max;
		  }
		  else
		  {
			   delta_p[i] = differ_p[i]/max;
		  }
	 }
//	 for(i=0;i<max;i++)	//�ƶ����ܾ���=ÿ���ƶ��ľ���*����;
//	 {
//		  MovePxyz(delta_p[0],delta_p[1],delta_p[2]);
//		  err = BaseCoordSlineMinPrecisionRun();
//		  if(err) return err;
//	 }
	 if(gestrue_flg == 0)//��̬һ��ʱ
	 {
		 for(i=0;i<max;i++)	//�ƶ����ܾ���=ÿ���ƶ��ľ���*����;
		 {
			  MovePxyz(delta_p[0],delta_p[1],delta_p[2]);
			  err = BaseCoordSlineMinPrecisionRun();
			  if(err) return err;
		 }
	 }
	 else//��̬��һ��ʱ
	 {
		 /*
		  * ���ܣ���̬��һ��ʱ������Ҫͨ��STM32��������Է������ݣ�������ܵ���ʱ������
		  * ������//2018/06/01
		 //char str[]={'*','*','*','*','*'};
		 //STM32_USART_SentData(str,4);
		 */
		 ReadCurrentPulseLp(currentPulse);//��õ�ǰ��λ�õ����������߼�ֵ��
		 //4,5,6���ÿ������������
		 delta_pulse_456[0] = (next_pulse[3] - currentPulse[3])/max;
		 delta_pulse_456[1] = (next_pulse[4] - currentPulse[4])/max;
		 delta_pulse_456[2] = (next_pulse[5] - currentPulse[5])/max;

		 for(i=0;i<max;i++)	//�ƶ����ܾ���=ÿ���ƶ��ľ���*����;
		 {
			  MovePxyz(delta_p[0],delta_p[1],delta_p[2]);
			  if(GetBestSolution(movedAg))//������Ž�
				  return 4;	//����������Χ
			  ReadCurrentPulseLp(currentPulse);//��õ�ǰ��λ�õ����������߼�ֵ��
			  AngleToPulse(movedAg,movedPulse);//�������õ������Ž⻻�������
			  for(j=0;j<3;j++)
			  {
				  //���ƽ�ƺ��λ�õ�������������ֵ��
				  delta_pulse[j]=movedPulse[j]-currentPulse[j];//��Ҫ�ƶ���������������ֵ�뵱ǰֵ�Ĳ�ֵ
			  }
			  delta_pulse[3] = delta_pulse_456[0];
			  delta_pulse[4] = delta_pulse_456[1];
			  delta_pulse[5] = delta_pulse_456[2];

			  err = MovePulse(delta_pulse);
			  if(err) return err;
		 }
	 }
	 while(ReadDriState())//�ȴ���������
	 {
		  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
		  {
			  delay_1us(5);
			  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
			  {
				  for(i=0;i<6;i++)
				  {
				  		delay_200ns();
				  		err |= ReadRR2(0x01<<i);
				  }
				  if(err)
				  {
					   SetDriCmd(0x3F,0x27);
					   return 1;
				  }
			  }
		  }
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:����,ʹ�ð�ȫ���ص�ʱ���ɿ�(�ߵ�ƽ)���˳�
		  {
			  delay_1us(1);
			  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)
			  {
				  SetDriCmd(0x3F,0x27);
				  return 2;
			  }
		  }
		  if(KeyBoard_StopSpeed())
		  {
			   SetDriCmd(0x3F,0x26);	//����ֹͣ
			   return 3;
		  }
	 }
	 if(gestrue_flg)
	 {
		 return 5;//���гɹ�������̬��һ��
	 }
	 return 0;
}
/*****************************************
 * ��Բ
 * ���������next_pulse	��һ������λ��
 * 			 orientation 	��Բ�ķ���	0��������	1��������
 * ����ֵ�� 0��Բ���岹�ɹ�
 * 			1�����������з�������
 * 			2�������������ɿ��˰�ȫ����
 * 			3�����������а�����stop��ť
 * 			4������������Χ
 * 			5��Բ���岹ʧ��
 * 			6: �����
 * 			7: ��̬��һ��
 */
unsigned char CircleInterpolation(long* next_pulse,unsigned char orientation)
{
	 unsigned char err=0,speed;
	 unsigned int i,n;
	 long current_pulse[6];
	 double current_angle[6],next_angle[6];
	 double differ_p[3];
	 double r,R,delta_height,t_p;
	 PositionGesture current_ges_pos,next_ges_pos;
	 UpdateCurrentPositionGesture();			//���µ�ǰλ����̬
	 PulseToAngle(next_pulse,next_angle);			//����һ������λ��ת��Ϊ�Ƕ�
	 //ReadCurrentPulseEp(current_pulse);			//����ǰ����λ��
	 ReadCurrentPulseLp(current_pulse);//������//2018/06/01
	 PulseToAngle(current_pulse,current_angle);	//����ǰ����λ��ת��Ϊ�Ƕ�
	 next_ges_pos = GetPositionGesture(next_angle);
	 current_ges_pos = GetPositionGesture(current_angle);
	 if(CompareTowGesture(next_ges_pos,current_ges_pos))
	 {
		  return 7;
	 }

	 differ_p[0] =  (next_ges_pos.px - current_ges_pos.px)*0.5;
	 differ_p[1] =  (next_ges_pos.py - current_ges_pos.py)*0.5;
	 differ_p[2] =  (next_ges_pos.pz - current_ges_pos.pz);//��λ�õĴ�ֱ�߶�
	 //if(differ_p[2]>5) return 4;//
	 circle_parameter(differ_p[0],differ_p[1]);//Բ�뾶
	 R = sqrt(differ_p[0]*differ_p[0]+differ_p[1]*differ_p[1]);//Բ�뾶
	 speed = GetProgramRunSpeed();
	 if(speed > 80)
	 {
		 t_p = ROBOT_PARAMETER.PRECISION*3;
	 }
	 else if(speed < 50)
	 {
		 t_p = ROBOT_PARAMETER.PRECISION;
	 }
	 else
	 {
		 t_p = ROBOT_PARAMETER.PRECISION*2;
	 }
	 r = t_p/R;//ÿ���߶��Ļ���
	 n = PI/r; //�ܹ�Ҫ�ߵĴ���   n*r=PI,��Ȧ
	 delta_height = differ_p[2]/n;//��ֱ�߶�ÿ���߶��ľ���
	 if(orientation)	r = -r;//����
	 for(i=0;i<n;i++)//
	 {
		  circle_xy(r,delta_height);////ÿ���߶�r����
		  err = BaseCoordSlineMinPrecisionRun();//����
		  if(err) return err;
	 }
	 while(ReadDriState())//�ȴ���������
	 {
		  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
		  {
			  delay_1us(5);
			  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
			  {
				  for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
				  {
				  		delay_200ns();
				  		err |= ReadRR2(0x01<<i);
				  }
				  if(err)
				  {
					   SetDriCmd(0x3F,0x27);
					   return 1;
				  }
			  }
		  }
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:����,ʹ�ð�ȫ���ص�ʱ���ɿ�(�ߵ�ƽ)���˳�
		  {
			  delay_1us(1);
			  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)
			  {
				  SetDriCmd(0x3F,0x27);
				  return 2;
			  }
		  }
		  if(KeyBoard_StopSpeed())
		  {
			   SetDriCmd(0x3F,0x26);	//����ֹͣ
			   return 3;
		  }
	 }
	 return 0;
}
/*****************************************
 * �岹�˶�
 * ���������differpulse ��Ҫ�ƶ���������
 * 			mode:0->�ؽڲ岹		1->ֱ�߲岹
 */
void TranslationProccess(long *differPulse,unsigned char mode)
{
	 unsigned char i,t=0,speed;
	 long freq[6],max;
	 float R[6],data_base;
	 //������2018/06/01
	 static long currentLPPulse[6];
	 static long currentEPPulse[6];
	 //char str[58];
	 //char newline[5]="\r\n";
	 //char distinguish[3]=" ";
/*
	 //delay_1ms(20);
	 //ͨ�����ڶ����ŷ����������ݣ�����LP��ֵ����
	 //ReadCurrentPulseEp(currentEPPulse);
 */
    if(Teach_mode==1)
    {
    	//����LP��ֵ��������һ���ݴ��������currentLPPulse��
    	ReadCurrentPulseLp(currentLPPulse);
    	ReadCurrentPulseEp(currentEPPulse);
    	for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
    	{
		 DEV_NOW[i]=currentLPPulse[i]-currentEPPulse[i];
		 differPulse[i]=differPulse[i]+((DEV_NOW[i]-DEV_LAST[i])/2)+DEV_NOW[i]/10;
		 DEV_LAST[i]=DEV_NOW[i];
    	}
    }

/*
	 if(rentangle_flag==0x66)
	 {
		STM32_USART_SentData(distinguish,strlen(distinguish)+1);
		//������2018/06/01
		for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
		{
			memset(str,0,sizeof(str));
			longToString(Deviation[i],str,12);//������ת��Ϊ�ַ�����С�����3λ
			//longToString(currentEPPulse[i],str,12);//������ת��Ϊ�ַ�����С�����3λ
			strcat(str," ");
			STM32_USART_SentData(str,strlen(str)+1);
		}
		STM32_USART_SentData(newline,strlen(newline)+1);
	 }
	 STM32_USART_SentData(newline,strlen(newline)+1);
*/
	 //�������
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
		  if(*(differPulse+i) >= 0)
		  {
			   SetPulse(0x01<<i,*(differPulse+i));
		  }
		  else
		  {
			   *(differPulse+i) = -(*(differPulse+i));//ȡ����1��������
			   SetPulse(0x01<<i,*(differPulse+i));
			   t=t|(0x01<<i);//��¼�ĸ����Ǹ���  bit0~5   1������  0: ����
		  }
	 }
	 max=*differPulse;
	 for(i=1;i<ROBOT_PARAMETER.AXIS_NUM;i++)//������ֵ
	 {
		  if(*(differPulse+i) > max)
		  {
			   max = *(differPulse+i);
		  }
	 }
	 speed = GetProgramRunSpeed();

	 //ֱ���˶��У�����˶����ӽ�����λ��ʱ�����ת�ٻ���죬�л����ؽڲ岹

	 if(max > 5000)	mode = 0;
	 if(mode == 0)
	 {
		  for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
		  {
			 R[i]=(float)(*(differPulse+i))/max;//max���ֵ
		  }
		  for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
		  {
			  s_current_speed[i]= R[i]*80*speed;//�������������趨����Ƶ�ʣ���������������������������ͬʱ��������ͬ��������
									 //������ķ�����һ������Ϊ��������������ʱ���ڶ�ʱ���������ٶȻ����
			 if(s_current_speed[i]<1)
			 {
				 s_current_speed[i]=1;
			 }else if(s_current_speed[i]>8000)
			 {
				 s_current_speed[i]=8000;//��ֹ�ٶȹ���
			 }
			 SetSpeed(0x01<<i,s_current_speed[i]);
		  }
	 }
	 else
	 {
		  if(speed > 80)
		  {
			  data_base = 0.25;
		  }
		  else if(speed < 2)
		  {
			  data_base = 0.09;
		  }
		  else
		  {
			  data_base = 0.2;
		  }
		  for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
		  {
			   freq[i]=(float)speed*(*(differPulse+i))*data_base;//0.2
			   if(freq[i]>8000)//��ֹ������С
			   {
					freq[i]=8000;
			   }
			   else if(freq[i]<1)
			   {
					freq[i]=1;
			   }
			   //�������������趨�ٶȣ������˶�������ͬʱ�䣨����20ms��������ͬ��·�̣�����0.2mm��
			   SetSpeed(0x01<<i,freq[i]);//�����ٶ�
		  }
	 }
	 //��ʼ����
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
		  if((t>>i)&0x01)//����Ǹ��� ��JҪ��ʱ��ת��
		  {
			   SetDriCmd(0x01<<i,0x21);//������������(�����ʱ����ת)
		  }
		  else//��������� ��JҪ˳ʱ��ת��
		  {
			   SetDriCmd(0x01<<i,0x20);//������������
		  }
	 }
}
/*****************************************
 * ���ܣ�����ת��������
 * ������
 * 		angle:�Ƕ�
 * 		pulse:ת���ɵ�����
 */
void AngleToPulse(double *angle,long *pulse)
{
	 *(pulse+0)=(*(angle+0))*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[0]*PI_DIVIDE_2_INVERSE;
	 *(pulse+1)=(*(angle+1))*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[1]*PI_DIVIDE_2_INVERSE;
	 *(pulse+2)=(*(angle+2))*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[2]*PI_DIVIDE_2_INVERSE;
	 *(pulse+3)=(*(angle+3))*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[3]*PI_DIVIDE_2_INVERSE;
	 *(pulse+4)=(*(angle+4))*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[4]*PI_DIVIDE_2_INVERSE;
	 *(pulse+5)=(*(angle+5))*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[5]*PI_DIVIDE_2_INVERSE;
}

/*****************************************
 * ���ܣ�����ת���ɻ���
 * ������
 * 		pulse:����ȥҪת��������
 * 		angle:����ת���ɵĻ���
 */
void PulseToAngle(long *pulse,double *angle)
{
	 *(angle+0)=(double)(*(pulse+0))*PulseOfCircle_INVERSE/ROBOT_PARAMETER.RATIO[0]*PI*2.0;
	 *(angle+1)=(double)(*(pulse+1))*PulseOfCircle_INVERSE/ROBOT_PARAMETER.RATIO[1]*PI*2.0;
	 *(angle+2)=(double)(*(pulse+2))*PulseOfCircle_INVERSE/ROBOT_PARAMETER.RATIO[2]*PI*2.0;
	 *(angle+3)=(double)(*(pulse+3))*PulseOfCircle_INVERSE/ROBOT_PARAMETER.RATIO[3]*PI*2.0;
	 *(angle+4)=(double)(*(pulse+4))*PulseOfCircle_INVERSE/ROBOT_PARAMETER.RATIO[4]*PI*2.0;
	 *(angle+5)=(double)(*(pulse+5))*PulseOfCircle_INVERSE/ROBOT_PARAMETER.RATIO[5]*PI*2.0;
}

/*****************************************
 * �������ܣ���ԭ��
 *
 */
void ReturnHome(unsigned char speed)
{
	 unsigned char i;
	 long movedPulse[6],currentPulse[6],differPulse[6];
	 static unsigned char mode_status=0;
	 for(i=0;i<6;i++)
	 {
		 movedPulse[i] = ROBOT_PARAMETER.RATIO[i]*ROBOT_PARAMETER.MOTOR_PRECISION*(ROBOT_PARAMETER.ORIGIN_POSITION[i]/360.0);
	 }
	 //ReadCurrentPulseEp(currentPulse);//��õ�ǰ��λ�õ���������ʵ��ֵ��
	 ReadCurrentPulseLp(currentPulse);//������//2018/06/01
	 for(i=0;i<6;i++)
	 {
		  differPulse[i] =  movedPulse[i] - currentPulse[i];
	 }
	 if(speed>15) speed =15;
	 ModifyAllSpeed(speed);
	 mode_status=Teach_mode;//������//2018/06/01
	 Teach_mode=0;//������//2018/06/01
	 TranslationProccess(differPulse,0);
	 Teach_mode=mode_status;//������//2018/06/01
}
/****************************************
 * �������ܣ�ԭ������/ԭ������
 * ���������NONE
 * ���������bit0:0->����ɹ� 1->����ʧ��
 * 			bit1:0->�������ƫ��ʧ�� 1->����ɹ�
 */
u8 ClearOrigin(void)
{
	 long pulse;
	 u8 i,err=0,err1;
	 //u16 times;

	 if(ROBOT_PARAMETER.MOTOR_TYPE == 1)//������ԭ������
	 {
		 WriteWR4(0xFFC0);          //CR : U0=1
		 delay_1ms(200);
		 WriteWR4(0xEFC0);          //CR : U0=0
		 delay_200ns();delay_200ns();
	 }
	 else// if(ROBOT_PARAMETER.MOTOR_TYPE == 2)//��Э���ԭ������
	 {
		 ServoOFF();	//���ŷ�
		 delay_1ms(200);
		 //���ͶԵ�����������������
		 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;)
		 {
			 //SendCmdToServo(4,i+1,0x1f,0x02);
			 if(ROBOT_PARAMETER.MOTOR_SELECT & (0x01<<i))
			 {
				 err |= SetServoPARAM(i+1,SX_CLEAR_EA05_DATA,2,0);
				 i++;
			 }

//			 USART_RX_STA_B = 0;
//			 times = 0;
//			 while(1)
//			 {
//				 if(USART_RX_STA_B&0x8000)
//				{
//					if(USART_RX_STA_B&0x4000)
//					{
//						GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485����ģʽ
//						delay_1ms(50);
//						USART_RX_STA_B = 0;
//						err = 1;
//					}
//
//					if(USART_B_RX_BUF[2] != 0x80)
//					{
//						err = 1;
//					}
//					USART_RX_STA_B = 0;
//					break;
//				}
//				 if(times > 3000)
//				 {
//					 GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485����ģʽ
//					 err = 1;	//��ʱ 300ms
//					 break;
//				 }
//				 delay_1us(100);
//				 times++;
//			 }
		 }
		 if(err) return 1;
		 //GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485����ģʽ
		 //��Э�ŷ����������б�������������Ҫ������������Ч��
		 ROBOT_PARAMETER.ENCODER_LEFT_SAV = 1;	//�����Ҫ����ԭ��
		 err1 = SaveRobotPara(ROBOT_PARAMETER);//�����е�ֲ���
		 err |= (err1<<1);
	 }

	 for(i=0;i<6;i++)
	 {
		 pulse = ROBOT_PARAMETER.RATIO[i]*ROBOT_PARAMETER.MOTOR_PRECISION*(ROBOT_PARAMETER.ORIGIN_POSITION[i]/360.0);
	     SetLp(0x01<<i,pulse);
	     SetEp(0x01<<i,pulse);
	 }

     if(ROBOT_PARAMETER.MOTOR_TYPE == 2)	//��Э�����ԭ��������Ҫ������Ч
     {
		 if(err==0)
		 {
			 SetScreen(20);
			 SetTextValue(20, 4, "");
			 SetTextValue(20, 3, "Locked successful,please restarted!");
			 while(1);
		 }
     }
     return err;
}
/****************************************
 * �������ܣ��������
 */
void ClearAlarm(void)
{
	 ClearServoAlarm();//����ŷ�����
	 SetDriCmd(0x3f,0x27);//�����ͣ����
}
//�������������Ϣ
void ClearAlarmInfo(void)
{
	 G_CTRL_BUTTON.CTRL_BUTTON.EMG_FLG = 0;
	 G_CTRL_BUTTON.CTRL_BUTTON.NEG_LIMITED_FLG = 0;
	 G_CTRL_BUTTON.CTRL_BUTTON.POS_LIMITED_FLG = 0;
	 G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG=0;//����ޱ���
}
/****************************************
 * �������ܣ�����ŷ�������
 * ����ֵ�� mode=0,������������
 * 			mode=1,ǿ�Ƹ����������ݣ����ܶ�ȡ�����Ƿ���ȷ
 * ����ֵ�� 0���ɹ�
 *  		��0����£�bit0~bit5��1�ֱ�����ĸ��ŷ���ȡ���ݳ�����������Ӧ���ŷ�����
 *  		bit6
 */
unsigned char UpdatePulseFromServo(void)
{
	 unsigned char i=0,err;
	 long pulse[6]={0};

	 err = ReadEncoderPulseAll(pulse);//������������ʵ����ʵ������Ep��ֻҪ�ڶ������600000�Ļ�����
										 //���Ե��������ݾ�ֱ�Ӹ���Ep��Ȼ��J3��J5ȡ���͸�Lp
	 if(err & ROBOT_PARAMETER.MOTOR_SELECT)
	 {
		  return err;//��������
	 }

	 if(ROBOT_PARAMETER.MOTOR_TYPE == 2)
	 {
		 //��Э�����Ҫ��ȥ���ƫ�����������������
		 for(i=0;i<6;i++)
		 {
			 pulse[i] = pulse[i] - ROBOT_PARAMETER.ENCODER_LEFT[i];
		 }
	 }

	 //�ŷ��Ŵ�����������˷�������ת�Ļ������������������෴��
	 for(i=0;i<6;i++)
	 {
		 if(ROBOT_PARAMETER.MOTOR_DIR.all & (0x01<<i))
		 {
			 pulse[i] = -pulse[i];
		 }
		 else
		 {
			 pulse[i] = pulse[i];
		 }
	 }

	 for(i=0;i<6;i++)
	 {
		 SetEp(0x01<<i,pulse[i]);
	 }
	 //��ȥ���״̬,�õ����������ʵ�ʵ�λ��
	 for(i=0;i<6;i++)
	 {
		 pulse[i] = pulse[i] + ROBOT_PARAMETER.RATIO[i]*ROBOT_PARAMETER.MOTOR_PRECISION*
				 	(ROBOT_PARAMETER.ORIGIN_POSITION[i]/360.0);
	 }
	 for(i=0;i<6;i++)
	 {
		 SetLp(0x01<<i,pulse[i]);
	 }

	 return 0;
}
/****************************************
 * �������ܣ�����ŷ�������
 * ����ֵ�� mode=0,������������
 * 			mode=1,ǿ�Ƹ����������ݣ����ܶ�ȡ�����Ƿ���ȷ
 * ����ֵ�� 0���ɹ�
 *  		��0����£�bit0~bit5��1�ֱ�����ĸ��ŷ���ȡ���ݳ�����������Ӧ���ŷ�����
 *  		bit6
 */
unsigned char UpdatePulseFromServoForEp(long *pulse)
{
	 unsigned char i=0,err;
	 //long pulse[6]={0};

	 err = ReadEncoderPulseAll(pulse);//������������ʵ����ʵ������Ep��ֻҪ�ڶ������600000�Ļ�����
										 //���Ե��������ݾ�ֱ�Ӹ���Ep��Ȼ��J3��J5ȡ���͸�Lp
	 if(err & ROBOT_PARAMETER.MOTOR_SELECT)
	 {
		  return err;//��������
	 }

	 if(ROBOT_PARAMETER.MOTOR_TYPE == 2)
	 {
		 //��Э�����Ҫ��ȥ���ƫ�����������������
		 for(i=0;i<6;i++)
		 {
			 pulse[i] = pulse[i] - ROBOT_PARAMETER.ENCODER_LEFT[i];
		 }
	 }

	 //�ŷ��Ŵ�����������˷�������ת�Ļ������������������෴��
	 for(i=0;i<6;i++)
	 {
		 if(ROBOT_PARAMETER.MOTOR_DIR.all & (0x01<<i))
		 {
			 pulse[i] = -pulse[i];
		 }
		 else
		 {
			 pulse[i] = pulse[i];
		 }
	 }
	 //��ȥ���״̬,�õ����������ʵ�ʵ�λ��
	 for(i=0;i<6;i++)
	 {
		 pulse[i] = pulse[i] + ROBOT_PARAMETER.RATIO[i]*ROBOT_PARAMETER.MOTOR_PRECISION*
				 	(ROBOT_PARAMETER.ORIGIN_POSITION[i]/360.0);
	 }
	 return 0;
}
/********************************************
 * �ŷ���
 * ���������mode=0����������������ȷ���ŷ�on��
 * 			 mode=1��ǿ�ƿ��ŷ�on�������������ݶ�ȡ�����Ƿ���ȷ(������������)
 * ����ֵ��	 err:  bit0~bit5Ϊ1����Ӧ�����ȡ�������
 */
unsigned char ServoON(unsigned char mode)
{
	 unsigned char err;
	 if(G_CTRL_BUTTON.CTRL_BUTTON.SERVO_ON == 0)//����ŷ��Ѿ���
	 {
		 SetScreen(20);
		 SetTextValue(20, 4, "");
		 SetTextValue(20, 3, "Servo is turned on!");
		 delay_1ms(1100);
		 return 0;
	 }
	 ServoOFF();	//�ص��ŷ��ٶ�ȡ
	 err = UpdatePulseFromServo();//��ȡ��������������
	 G_CTRL_BUTTON.CTRL_BUTTON.ORIGIN_FLG = 0;
	 if(err)
	 {
		  if(mode==0)	//�������ǿ�ƴ��ŷ�
		  {
			   ServoON_OFF(0x3f);
			   delay_1ms(10);
			   G_CTRL_BUTTON.CTRL_BUTTON.ORIGIN_FLG = 1;
			   return err;
		  }
	 }
	 ServoON_OFF(0);	//���ŷ�
	 delay_1ms(100);
	 OpenBrake(0);	//��1~3���ɲ��
	 OpenBrake(1);
	 OpenBrake(2);
	 delay_1ms(100);
	 G_CTRL_BUTTON.CTRL_BUTTON.SERVO_ON = 0;//�ŷ��� ��־λ
	 SetButtonValue(2, 13, 1);//�����ŷ����ذ�ť����Ϊ��״̬
	 SetButtonValue(44, 13, 1);
	 return err;
}
/********************************************
 * �ŷ���
 */
void ServoOFF(void)
{
	 CloseBrake(0);//�ر�1~3���ɲ��
	 CloseBrake(1);
	 CloseBrake(2);
	 delay_1ms(200);
	 ServoON_OFF(0x3f);
	 delay_1ms(10);
	 G_CTRL_BUTTON.CTRL_BUTTON.SERVO_ON = 1;//�ŷ��رձ�־λ
	 SetButtonValue(2, 13, 0);//�����ŷ����ذ�ť����Ϊ�ر�״̬
	 SetButtonValue(44, 13, 0);
}
/********************************************
 * �����ŷ����أ�bit0~bit5�ֱ����J1~J6,0���ŷ���1���ŷ�
 */
void ServoON_OFF(unsigned int state)
{
	 state &= 0x3F;
	 s_WR4state = (s_WR4state&0xFFC0) | state;
	 WriteWR4(s_WR4state);
}
/********************************************
 * ���Ƶ����ŷ�ON�Ŀ��أ�
 * Axis�����뷶Χ1~6������J1~J6
 * state��0���ŷ���1���ŷ�
 */
void ServoON_OFF_Single(unsigned char axis,unsigned char state)
{
	 s_WR4state = (s_WR4state&(~(0x0001<<(axis-1))));//����ڣ�axis-1��λ
	 s_WR4state = s_WR4state | (state<<(axis-1));
	 WriteWR4(s_WR4state);
}
/*******************************************
 * ����ŷ�����
 */
void ClearServoAlarm(void)
{
	 s_WR4state = s_WR4state & 0xF03F;//����bit6~bit11λ
	 WriteWR4(s_WR4state);
	 delay_1ms(55);
	 s_WR4state = s_WR4state | 0x0FC0;
	 WriteWR4(s_WR4state);
	 delay_1ms(50);
}

/********************************************
 * �������ܣ���ⴥ��������״̬���޸�
 * ���������NONE
 * ���������NONE
 */
void CheckScreenConnectState(void)
{
	 unsigned char screen_connect_state = 0;	// 0:������������	1:�ѶϿ�
	 static unsigned char s_ScreenConnectState = 0;	// 0:������������	1:�ѶϿ�
	 static unsigned int s_num=0;
	 static unsigned char s_flag=0;			//0:��ʱ����	1����ʱ��ʼ
	 if(SCREEN_CONNECT_STATE == 1)
	 {
		 delay_1us(3);
		 if(SCREEN_CONNECT_STATE == 1)
		 {
			 screen_connect_state = 1;
		 }
	 }
	 else
	 {
		 delay_1us(3);
		 if(SCREEN_CONNECT_STATE == 0)
		 {
			 screen_connect_state = 0;
		 }
	 }

	 if(s_ScreenConnectState != screen_connect_state)
	 {
		 if(s_ScreenConnectState == 1)
		 {
			 if(ProgramRuningFlag(0) == 0)
			 {
				 s_num = 8000;//������������
			 }
			 else
			 {
				 s_num = 16000;//����ֹͣ����
			 }
			 s_flag = 1;
		 }
		 s_ScreenConnectState = screen_connect_state;
	 }

	 if(s_flag == 1)
	 {
		 if(s_num == 0)
		 {
			 s_flag = 0;
			 InitSci();//�������������Ӻ�Ҫ��DSP�Ĵ��ڳ�ʼ��������DSP�޷��������ݣ�ԭ��δ֪
			 delay_1us(50);
			 SetCurrentScreen();
		 }
	 }
	 s_num--;
}

/********************************************
 * ��鱨��
 * ���������screen_id,��������ʱ�����id=0�򷵻ص�ǰҳ�棬����ֵʱ��������ֵ��ҳ��
 * ����ֵ err 0���ޱ���	����ֵ���б���
 */
unsigned char ErrCheck(unsigned char screen_id)
{
	 static u8 s_ALM_STATE = 0;	//��¼�����ͼ�ͣ״̬
	 unsigned char i,err,err2=0;
	 CheckScreenConnectState();
	 delay_1us(7);
	 err=ReadErrState();//bit0~bit5:J1~6,������״̬
	 /*for(i=0;i<6;i++)
	 {
		 delay_200ns();
		 err_rr2 |= ReadRR2(0x01<<i);
	 }*/
	 err = err & ROBOT_PARAMETER.MOTOR_SELECT;
	 if(err)
	 {
		 delay_1us(2);
		 err=ReadErrState();
		 err = err & ROBOT_PARAMETER.MOTOR_SELECT;
	 }
	 if(!err)
	 {
		   G_CTRL_BUTTON.CTRL_BUTTON.EMG_FLG=0;
		   G_CTRL_BUTTON.CTRL_BUTTON.POS_LIMITED_FLG=0;
		   G_CTRL_BUTTON.CTRL_BUTTON.NEG_LIMITED_FLG=0;
		   G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG=0;

	 }

	 if(G_CTRL_BUTTON.CTRL_BUTTON.EMG_FLG==0 && G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG==0)
	 {
		   if(s_ALM_STATE == 1)
		   {
			   StandbyOutput();
			   s_ALM_STATE = 0;	//����
		   }
	 }

	 if(err)//
	 {
		   SetDriCmd(0x3F,0x27);
		   ProgramRuningFlag(3);//����ǳ��������о�ֹͣ��������
		   for(i=0;i<6;i++)
		   {
				if(err&(0x01<<i))//����ĸ����д���
				{
					 delay_200ns();
					 err2 = ReadRR2(0x01<<i);//����ϸ������Ϣ

					 //if((err2&0x10) == 0) G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG=0;
					 //else G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG=1;

					 if((err2&0x20) == 0) G_CTRL_BUTTON.CTRL_BUTTON.EMG_FLG=0;//�����Ѿ��б�������λ������ͣҲ����ʾ

					 if(G_CTRL_BUTTON.all&0x38)//����Ѿ��м�ͣ����λ
					 {
						  if(err2&0x10)//������ŷ�����
							   break;//���˳�
					 }
					 else
					 {
						  break;//���֮ǰû�м�ͣ��λ��������ֱ���˳�
					 }
				}
		   }
		   if((err2 & 0x10) && !G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG)//���֮ǰû�б���
		   {
			   s_ALM_STATE = 1;	//��Ǽ�ͣ
			    AlarmOutput();	//�����
			    ServoON_OFF(0x3f);
				ServoOFF();
				G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG=1;//����б���
				G_CTRL_BUTTON.CTRL_BUTTON.EMG_FLG=1;
				display_servo_alarm(i+1,err2,screen_id);//�����������ᷢ��ʲô����
		   }
		   else if((err2&0x20) && (!G_CTRL_BUTTON.CTRL_BUTTON.EMG_FLG))//���֮ǰû�м�ͣ
		   {
			   s_ALM_STATE = 1;	//��Ǳ���
			   	AlarmOutput();	//�����
				G_CTRL_BUTTON.CTRL_BUTTON.EMG_FLG=1;
				err2 = err2 & (~0x10);//���ε�����������ֻ����ʾ����������ʾ��ͣ
				display_servo_alarm(i+1,err2,screen_id);
		   }
		   else if((err2&0x01) && (!G_CTRL_BUTTON.CTRL_BUTTON.POS_LIMITED_FLG))//���֮ǰû����λ
		   {
				G_CTRL_BUTTON.CTRL_BUTTON.POS_LIMITED_FLG=1;//����λ
				G_CTRL_BUTTON.CTRL_BUTTON.NEG_LIMITED_FLG=0;//���㸺��λ
				display_servo_alarm(i+1,err2,screen_id);
		   }
		   else if((err2&0x02) && (!G_CTRL_BUTTON.CTRL_BUTTON.NEG_LIMITED_FLG))//���֮ǰû����λ
		   {
				G_CTRL_BUTTON.CTRL_BUTTON.NEG_LIMITED_FLG=1;//����λ
				G_CTRL_BUTTON.CTRL_BUTTON.POS_LIMITED_FLG=0;//����λ
				display_servo_alarm(i+1,err2,screen_id);
		   }
		   SetDriCmd(0x3F,0x25);
	 }
	 return err;
}

/************************************
 * �������ܣ��򿪷�����
 */
void OpenBuzzer(void)
{
	ENABLE_SPEAKER;
}

/************************************
 * �������ܣ��رշ�����
 */
void CloseBuzzer(void)
{
	DISABLE_SPEAKER;
}

/**************************************
 * �������ܣ�����ȫ����״̬
 * ����ֵ��	 0����ȫ�����Ѱ���
 * 			 1����ȫ����δ����
 */
unsigned char ReadSaveButtonState(void)
{
	if(SAVE_BUTTON == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/************************************
 * �������ܣ������ŷ���������λ����Ч/��Ч
 * ���������flag
 * 			 bit0��	0:�ŷ�����������Ч		1:�ŷ�����������Ч
 * 			 bit1��	0:��λ������Ч			1:��λ������Ч
 */
void SetAlarmLimitedState(unsigned char flag)
{
	if(flag & 0x01)
	{
		s_WR2state = s_WR2state | 0x3000;
	}
	else
	{
		s_WR2state = s_WR2state & 0xcfff;
	}

	if(flag & 0x02)
	{
		s_WR2state = s_WR2state | 0x0003;
	}
	else
	{
		s_WR2state = s_WR2state & 0xfffc;
	}

	WriteWR2(0x3F,s_WR2state);
}

/************************************
 * �������ܣ��ŷ�����������Ч
 */
void DisableServoAlarm(void)
{
	s_WR2state = s_WR2state & 0xcfff;
	WriteWR2(0x3F,s_WR2state);
	G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG = 0;//����������λ
}

/************************************
 * �������ܣ��ŷ�����������Ч
 */
void EnableServoAlarm(void)
{
	s_WR2state = s_WR2state | 0x3000;
	WriteWR2(0x3F,s_WR2state);
}

/************************************
 * �������ܣ���λ������Ч
 */
void DisableLimited(void)
{
	s_WR2state = s_WR2state & 0xfffc;
	WriteWR2(0x3F,s_WR2state);
}

/************************************
 * �������ܣ���λ������Ч
 */
void EnableLimited(void)
{
	s_WR2state = s_WR2state | 0x0003;
	WriteWR2(0x3F,s_WR2state);
}

/************************************
 * �������ܣ�����Ϊ����ʾ��ģʽ
 */
void SetSpeedMode_High(void)
{
	s_SpeedMode = 1;
}

/************************************
 * �������ܣ�����Ϊ����ʾ��ģʽ
 */
void SetSpeedMode_Low(void)
{
	s_SpeedMode = 0;
}

/************************************
 * �������ܣ����ʾ���ٶ�ģʽ
 * ����ֵ��0->����ʾ��ģʽ
 * 		   1->����ʾ��ģʽ
 */
unsigned char GetSpeedMode(void)
{
	return s_SpeedMode;
}
/************************************
 * �������ܣ�������ģʽ����
 */
void DeveloperMode_On(void)
{
	//RecoverSpeed();
}

/************************************
 * �������ܣ�������ģʽ����
 */
void DeveloperMode_Off(void)
{
	SetSpeedMode_Low();
	EnableLimited();
	delay_1ms(2);
	EnableServoAlarm();
	Teach_mode=0;//������//2018/06/01;
}
/************************************
 * �������ܣ��ָ�MCX31x��WR2�Ĵ���
 * ���ڸ��Ż��ȶ�������WR2�Ĵ����ᱻ���㣬���±�������λ
 * ��Ч�����ԣ��˺������ڶ�����е��֮ǰ���Ȼָ�WR2�Ĵ�����
 */
void RecoverWR2(void)
{
	WriteWR2(0x3F,s_WR2state);
}
/************************************
 * �������ܣ���ȡWR2�Ĵ�����ֵ
 ********WR2ģʽ�Ĵ���*****
 *bit0,1 ������λ��Ч��Ч 0��Ч  1��Ч
 *bit5   COMP+/-�Ĵ����Ƚ϶���  0���߼�   1��ʵλ
 *bit12  nALARM�����źŵ��߼���ƽ��0���͵�ƽ��Ч  1���ߵ�ƽ��Ч
 *bit13  nALARM   0:��Ч  1����Ч
 */
unsigned int GetWR2state(void)
{
	return s_WR2state;
}
/************************************
 * �������ܣ�����ǰ����/������Ϣ
 * ����ֵ��	bit0=1���ŷ�onû��
 * 			bit1=1���о���
 * 			bit2=1����ԭ���ԭ�����
 * 			bit3=1����ͣ��
 * 			bit4=1, +��λ��
 * 			bit5=1, -��λ��
 * 			bit6=1, ��ȫ����δ����
 * 			bit7=1, 1��ɲ��δ��
 * 			bit8=1, 2��ɲ��δ��
 * 			bit9=1, 3��ɲ��δ��
 */
unsigned int ReadErrInfo(void)
{
	unsigned int err;
	err = (G_CTRL_BUTTON.all & 0x3F);
	err |= SAVE_BUTTON<<6;
	err |= ( ReadBrakeState() & 0x07 )<<7;
	return err;
}
/************************************
 * �������ܣ�����е�ֲ���
 * ���������ָ��robot_para��ָ��
 * ����ֵ��	0->��ȡ�ɹ�	1->��ȡʧ��  2->��Ҫ���³�ʼ���趨��е�ֲ���
 */
unsigned char ReadRobotPara(RobotParaStru* robot_para)
{
	unsigned char addr;
	u16 crc16;
	RobotParaStru_CRC robot_pata_crc;
	addr = INFO_BLOCK_ADDR1;
	ReadNandFlashData(addr,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));
	if(robot_pata_crc.RobotPara.BAD == 0)//INFO_BLOCK_ADDR1�ǻ���
	{
		addr = INFO_BLOCK_ADDR2;
	}
	if(robot_pata_crc.RobotPara.FIRST == 1)
	{
		*robot_para = robot_pata_crc.RobotPara;
		return 2;	//������δ�趨
	}

	crc16 = crc_cal_by_bit((u8*)(&(robot_pata_crc.RobotPara)),sizeof(robot_pata_crc.RobotPara));//У�����
	if(crc16 == robot_pata_crc.CRC16)
	{
		if(robot_pata_crc.RobotPara.HARDWARE_REV == 0)
			robot_pata_crc.RobotPara.HARDWARE_REV = 8;	//�����֮ǰ�ϰ汾�ģ����ֵĬ����0�����ڸ�ΪĬ�϶���8
		*robot_para = robot_pata_crc.RobotPara;
		return 0;
	}
	//��ȡʧ��ʱ�ٶ�ȡһ��
	ReadNandFlashData(addr,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));
	crc16 = crc_cal_by_bit((u8*)(&(robot_pata_crc.RobotPara)),sizeof(robot_pata_crc.RobotPara));//У�����

	if(robot_pata_crc.RobotPara.HARDWARE_REV == 0)
		robot_pata_crc.RobotPara.HARDWARE_REV = 8;	//�����֮ǰ�ϰ汾�ģ����ֵĬ����0�����ڸ�ΪĬ�϶���8
	*robot_para = robot_pata_crc.RobotPara;

	if(crc16 == robot_pata_crc.CRC16)
	{
		//*robot_para = robot_pata_crc.RobotPara;
		return 0;
	}
	return 1;
}

/************************************
 * �������ܣ������е�ֲ���
 * ����ֵ��	0->����ɹ�	1->����ʧ��
 */
unsigned char SaveRobotPara(RobotParaStru robot_para)
{
	unsigned char err=0,addr;
	RobotParaStru_CRC robot_pata_crc;
	u16 crc16;
	addr = INFO_BLOCK_ADDR1;

	err = ReadRobotPara(&(robot_pata_crc.RobotPara));
//	if(err ==1)
//	{
//		return 1;
//	}

//	if(err == 0)//��ȡ�ɹ�
//	{
		if(robot_pata_crc.RobotPara.BAD == 0)//���INFO_BLOCK_ADDR1�ǻ���
		{
			addr = INFO_BLOCK_ADDR2;
		}
//	}
	//У�����
	crc16 = crc_cal_by_bit((u8*)(&robot_para),sizeof(RobotParaStru));
	robot_pata_crc.CRC16 = crc16;
	robot_pata_crc.RobotPara = robot_para;
	robot_pata_crc.RobotPara.FIRST = 0;	//���Ϊ�ѳ�ʼ����
	robot_pata_crc.RobotPara.BAD = 1;	//���Ϊ�ÿ�
	err  = EraseNandFlashBlock(addr);	//������Ϣ��
	err |= WriteNandFlashData(addr,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));
	//����ʧ��ʱ���ٱ���һ��
	if(err)
	{
		err  = EraseNandFlashBlock(addr);	//������Ϣ��
		err |= WriteNandFlashData(addr,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));
		if(err)
		{
			if(addr == INFO_BLOCK_ADDR1)
			{

				err  = EraseNandFlashBlock(addr);	//������Ϣ��1
				robot_pata_crc.RobotPara.BAD = 0;	//���Ϊ����
				err |= WriteNandFlashData(addr,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));

				robot_pata_crc.RobotPara.BAD = 1;	//���Ϊ����
				err  = EraseNandFlashBlock(INFO_BLOCK_ADDR2);	//������Ϣ��2
				err |= WriteNandFlashData(INFO_BLOCK_ADDR2,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));
				if(err)
				{
					err  = EraseNandFlashBlock(INFO_BLOCK_ADDR2);	//������Ϣ��2
					err |= WriteNandFlashData(INFO_BLOCK_ADDR2,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));
				}
			}
		}
	}
	return err;
}

/************************************
 * �������ܣ����stm32�汾��
 * �������:data,�������ݵ�ָ�룬���ݳ���4�ֽ�
 * ����ֵ�� 0->OK 1->��STM32ͨ���쳣
 */
u8 GetStm32Version(u8 *data)
{
	u8 tx_cmd[5],rx_cmd[5],t=0;
	memset(tx_cmd,0,sizeof(tx_cmd));
	tx_cmd[0] = T_CMD_VERSION;

	while(1)
	{
		SpiSendCmd(tx_cmd);
		delay_1ms(10);
		if(SpiReceiveCmd(rx_cmd) == 0)
		{
			if(rx_cmd[0] != TR_CMD_OK)
			{
				return 1;
			}
			else
			{
				break;
			}
		}
		t++;
		delay_1ms(1);
		if(t >MXA_REPEAT) return 1;
	}
	data[0] = rx_cmd[1];	//1
	data[1] = rx_cmd[2];	//.
	data[2] = rx_cmd[3];	//0
	data[3] = rx_cmd[4];	//1
	return 0;
}

/************************************
 * �������ܣ����stm32���ڽ�������״̬
 * �������:data->������������ݣ���ô���ָ��ָ����յ�������
 * ����ֵ�� 0->�����н��յ����� 1->�����޽��յ����� 2->ͨ���쳣
 */
u8 GetSTM32USART_STA(char* data)
{
	u8 tx_cmd[5],rx_cmd[5],t=0,i;
	Uint32 data_len;

	memset(tx_cmd,0,sizeof(tx_cmd));
	tx_cmd[0] = R_CMD_USART_RX_STA;

	while(1)
	{
		SpiSendCmd(tx_cmd);	//���Ͳ�ѯ����
		delay_1ms(1);
		if(SpiReceiveCmd(rx_cmd) == 0)
		{
			if(rx_cmd[0] == R_CMD_USART_RX_OK)
			{
				data_len = 0;
				for(i=0;i<4;i++)
				{
					data_len |= (Uint32)rx_cmd[i+1]<<(i*8);//���ݳ���
				}

				if(SpiReceiveData(data_len,(u8*)data,0))
				{
					delay_1ms(100);//�ȴ�stm32�˳�����ģʽ
					return 2;
				}
				break;
			}
			else if(rx_cmd[0] == R_CMD_USART_RX_NO)
			{
				return 1;
			}
			else
			{
				continue;
			}
		}
		t++;
		delay_1ms(1);
		if(t >MXA_REPEAT) return 2;
	}
	return 0;
}

/************************************
 * �������ܣ���ȡͨ��STM32���ڽ��յ�������
 * �������:data:���32�ֽڳ���
 * ����ֵ�� 0->���ճɹ� 1->����ʧ�� 2->ͨ���쳣
 */
//u8 Read_STM32_USART_Data(char* data)
//{
//	u8 tx_cmd[5],rx_cmd[5],i;
//	Uint32 data_len;
//
////	memset(tx_cmd,0,sizeof(tx_cmd));
////	tx_cmd[0] = R_CMD_USART;
////	SpiSendCmd(tx_cmd);	//���Ͷ�ȡ��������
////	delay_1ms(1);
////	if(SpiReceiveCmd(rx_cmd))
////	{
////		delay_1ms(100);//�ȴ�stm32�˳�����ģʽ
////		return 2;
////	}
//	//��ʼ��������
//	data_len = 0;
//	for(i=0;i<4;i++)
//	{
//		data_len |= (Uint32)rx_cmd[i+1]<<(i*8);//���ݳ���
//	}
//	if(SpiReceiveData(data_len,data,0))
//	{
//		delay_1ms(100);//�ȴ�stm32�˳�����ģʽ
//		return 2;
//	}
//	return 0;
//}

/************************************
 * �������ܣ�ͨ��STM32���ڷ�������
 * �������:NONE
 * ����ֵ�� 0->�������ݳɹ� 1->��������ʧ�� 2->ͨ���쳣
 */
u8 STM32_USART_SentData(char* data,u16 len)
{
	unsigned char rx_cmd[5],tx_cmd[5],i,t=0;
	//int dly=0;
	//dly = 1/(CURRENT_BAUD*0.1)*10000000;	//������ԽС����Ҫ��ʱ��ʱ��Խ��    (��ʱ���� *10)
	memset(tx_cmd,0,sizeof(tx_cmd));
	tx_cmd[0] = T_CMD_USART;
	for(i=0;i<4;i++)
	{
		tx_cmd[i+1] = len>>(i*8)&0x000000ff;
	}
	while(1)
	{
		SpiSendCmd(tx_cmd);	//������Ҫ���͵����ݵĳ���
		delay_1us(10);
		SpiSendData(len,(u8*)data,0);//������Ҫ���͵�����
		delay_1us(100);
		for(i=0;i<len;i++)	//��������Խ�࣬��Ҫ��ʱ��ʱ��Խ��
		{
			DELAY_US(900);
		}
		delay_1us(1000);                      //
		if(SpiReceiveCmd(rx_cmd) == 0)	//���Է�����״̬
		{
			if(rx_cmd[0] == TR_CMD_OK)
			{
				return 0;
			}
			else
			{
				delay_1us(100);
				if(SpiReceiveCmd(rx_cmd) == 0)	//���Է�����״̬
				{
					if(rx_cmd[0] == TR_CMD_OK)
					{
						return 0;
					}
				}
			}
			//return 0;
//			else	//�Է����ճ���
//			{
//				continue;
//			}
		}
		t++;
		delay_1ms(1);
		if(t>5)break;
	}
	return 2;
}

//===========================================================================
// No more.
//===========================================================================
