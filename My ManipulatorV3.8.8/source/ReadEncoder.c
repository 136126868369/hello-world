/*
 * ReadEncoder.c
 *
 *  Created on: 2015-3-19
 *      Author: rd49
 */
#include "ReadEncoder.h"
#include "ScreenApi.h"
#include "My_USART.h"
#include "Sanxie_Servo.h"
extern unsigned int USART_RX_STA_B;		// ����b�Ľ���״̬���
extern char USART_B_RX_BUF[32];
void WaitAbstReset(void);
void WaitAbstSet(void);
unsigned char ReadPulseJ1(long *pulse);
unsigned char ReadPulseJ2(long *pulse);
unsigned char ReadPulseJ3(long *pulse);
unsigned char ReadPulseJ4(long *pulse);
unsigned char ReadPulseJ5(long *pulse);
unsigned char ReadPulseJ6(long *pulse);
/****************************
 * ��ȡ6����������������
 * ����ֵ��err:bit0~5,�ֱ����J1~J6��0����ȡ��ȷ	1����ȡ����
 */
unsigned char ReadEncoderPulseAll(long *pulseData)
{
	unsigned char i,errTmp,err=0,mode;

	//�л��������Э�����������Ҫ�ĵ�·���ϵ�����
	mode = ROBOT_PARAMETER.MOTOR_TYPE;
	for(i=0;i<8;i++)
	{
		 if(ROBOT_PARAMETER.MOTOR_SELECT&(0x01<<i))
		 {
			 errTmp = ReadEncoderPulse(i+1,pulseData+i,mode);
			 if(errTmp)
			 {
				  errTmp = ReadEncoderPulse(i+1,pulseData+i,mode);
				  if(errTmp)
				  {
					   errTmp = ReadEncoderPulse(i+1,pulseData+i,mode);
				  }
			 }
		 }
		 else
		 {
			 errTmp = 0;
		 }
		 err |= errTmp<<i;
	}
    return err;
}
/*****************************************
 * ��ȡһ������������������
 * ���룺Axis����Ҫ��ȡ����1~6
 * 		 pulseData��ָ���������ݵ�ָ��
 * 		 mode: 1->������	2->��Э
 * ����ֵ��0����ȡ�ɹ�	1����ȡ�����д���
 */
unsigned char ReadEncoderPulse(unsigned char Axis,long *pulseData,unsigned char mode)
{
	unsigned char i,err=0;

	u8 data[4];
	if(mode == 1)
	{
		switch(Axis)
		{
		case 1:
			err = ReadPulseJ1(pulseData);
			break;
		case 2:
			err = ReadPulseJ2(pulseData);
			break;
		case 3:
			err = ReadPulseJ3(pulseData);
			break;
		case 4:
			err = ReadPulseJ4(pulseData);
			break;
		case 5:
			err = ReadPulseJ5(pulseData);
			break;
		case 6:
			err = ReadPulseJ6(pulseData);
			break;
		default:
			err = 1;
			break;
		}
	}
	else
	{
		err = GetServoPARAM(Axis,SX_GET_STATE_VALUE_4,ABS_FEEDBACK,data,0);
		if(err == 0)
		{
			*pulseData = 0;
			for(i=0;i<4;i++)
			{
				*pulseData |= ((long)data[i])<<((3-i)*8);
			}
		}
	}
	return err;
}


/************************************
 * ��ȡJ1����������
 * ����ֵ��1����ȡ����	0����ȡ��ȷ
 */
unsigned char ReadPulseJ1(long *pulse)
{
	 unsigned char i;
	 unsigned int t;
	 unsigned char CheckValue=0;
	 unsigned char CheckValueAll=0;
     unsigned long CacheData=0;
     long PulseValue=0;

	 ServoON_OFF_Single(1,1);//J1�ŷ���
	 delay_1ms(20);
	 ServoON_OFF_Single(1,0);//J1�ŷ���
     ResetAbsm1;
     delay_1ms(1);
     for(i=0; i<32; i+=2)//���ݽ���
     {
            WaitAbstReset();   //�ȴ�׼�����ݿ�
            ResetAbsr;    //����ȡ��������
            WaitAbstSet(); //�ȴ�׼�����ݹ�
            CacheData =   !J1_ABSB1;
            CacheData <<= 1;
            CacheData |= (!J1_ABSB0);
            CheckValueAll += CacheData; //�����ΪУ��ֵ
            CacheData <<= i;
            PulseValue |= CacheData;   //��õ���������
            SetAbsr;       //�ض�ȡ��������
     }
     for(i=0; i<6; i+=2) //У�������
     {
            WaitAbstReset();   //�ȴ�׼�����ݿ�
            ResetAbsr;    //����ȡ��������
            WaitAbstSet(); //�ȴ�׼�����ݹ�
            CacheData =  !J1_ABSB1;
            CacheData <<= 1;
            CacheData |=(!J1_ABSB0);
            CacheData <<= i;
            CheckValue |= CacheData;
            SetAbsr;      //�ض�ȡ��������
     }
     WaitAbstReset();          //�ȴ�׼�����ݿ���ȷ����19��ABST�������,����������У��

     SetAbsm1;            //����ABSģʽ
     delay_1ms(1);
     WaitAbstSet();        //�ȴ�׼�����ݹ�
     t=65535;
     while((!J1_RD)&&(t--))       //�ȴ��ŷ�׼�����
     {
    	  delay_200ns();
     }
	if(CheckValueAll != CheckValue)  //У�鲻��ȷ
	{
		 return 1;
	}
	*pulse = PulseValue;//���ػ�õ���������
	return 0;
}
/************************************
 * ��ȡJ2����������
 * ����ֵ��1����ȡ����	0����ȡ��ȷ
 */
unsigned char ReadPulseJ2(long *pulse)
{
	 unsigned char i;
	 unsigned int t;
	 unsigned char CheckValue=0;
	 unsigned char CheckValueAll=0;
     unsigned long CacheData=0;
     long PulseValue=0;
     ServoON_OFF_Single(2,1);//J2�ŷ���
	 delay_1ms(20);
	 ServoON_OFF_Single(2,0);//J2�ŷ���
     ResetAbsm2;
     delay_1ms(1);
     for(i=0; i<32; i+=2)//���ݽ���
     {
            WaitAbstReset();   //�ȴ�׼�����ݿ�
            ResetAbsr;    //����ȡ��������
            WaitAbstSet(); //�ȴ�׼�����ݹ�
            CacheData =   !J2_ABSB1;
            CacheData <<= 1;
            CacheData |= (!J2_ABSB0);
            CheckValueAll += CacheData; //�����ΪУ��ֵ
            CacheData <<= i;
            PulseValue |= CacheData;   //��õ���������
            SetAbsr;       //�ض�ȡ��������
     }
     for(i=0; i<6; i+=2) //У�������
     {
            WaitAbstReset();   //�ȴ�׼�����ݿ�
            ResetAbsr;    //����ȡ��������
            WaitAbstSet(); //�ȴ�׼�����ݹ�
            CacheData =  !J2_ABSB1;
            CacheData <<= 1;
            CacheData |=(!J2_ABSB0);
            CacheData <<= i;
            CheckValue |= CacheData;
            SetAbsr;      //�ض�ȡ��������
     }
     WaitAbstReset();          //�ȴ�׼�����ݿ���ȷ����19��ABST�������,����������У��

     SetAbsm2;            //����ABSģʽ
     delay_1ms(1);
     WaitAbstSet();        //�ȴ�׼�����ݹ�
     t=65535;
     while((!J2_RD)&&(t--))       //�ȴ��ŷ�׼�����
     {
    	  delay_200ns();
     }

	if(CheckValueAll != CheckValue)  //У�鲻��ȷ
	{
		 return 1;
	}
	*pulse = PulseValue;//���ػ�õ���������
	return 0;
}
/************************************
 * ��ȡJ3����������
 * ����ֵ��1����ȡ����	0����ȡ��ȷ
 */
unsigned char ReadPulseJ3(long *pulse)
{
	 unsigned char i;
	 unsigned int t;
	 unsigned char CheckValue=0;
	 unsigned char CheckValueAll=0;
     unsigned long CacheData=0;
     long PulseValue=0;
     ServoON_OFF_Single(3,1);//J3�ŷ���
	 delay_1ms(20);
	 ServoON_OFF_Single(3,0);//J3�ŷ���
     ResetAbsm3;
     delay_1ms(1);
     for(i=0; i<32; i+=2)//���ݽ���
     {
            WaitAbstReset();   //�ȴ�׼�����ݿ�
            ResetAbsr;    //����ȡ��������
            WaitAbstSet(); //�ȴ�׼�����ݹ�
            CacheData =   !J3_ABSB1;
            CacheData <<= 1;
            CacheData |= (!J3_ABSB0);
            CheckValueAll += CacheData; //�����ΪУ��ֵ
            CacheData <<= i;
            PulseValue |= CacheData;   //��õ���������
            SetAbsr;       //�ض�ȡ��������
     }
     for(i=0; i<6; i+=2) //У�������
     {
            WaitAbstReset();   //�ȴ�׼�����ݿ�
            ResetAbsr;    //����ȡ��������
            WaitAbstSet(); //�ȴ�׼�����ݹ�
            CacheData =  !J3_ABSB1;
            CacheData <<= 1;
            CacheData |=(!J3_ABSB0);
            CacheData <<= i;
            CheckValue |= CacheData;
            SetAbsr;      //�ض�ȡ��������
     }
     WaitAbstReset();          //�ȴ�׼�����ݿ���ȷ����19��ABST�������,����������У��

     SetAbsm3;            //����ABSģʽ
     delay_1ms(1);
     WaitAbstSet();        //�ȴ�׼�����ݹ�
     t=65535;
     while((!J3_RD)&&(t--))       //�ȴ��ŷ�׼�����
     {
    	  delay_200ns();
     }
	if(CheckValueAll != CheckValue)  //У�鲻��ȷ
	{
		 return 1;
	}
	*pulse = PulseValue;//���ػ�õ���������
	return 0;
}
/************************************
 * ��ȡJ4����������
 * ����ֵ��1����ȡ����	0����ȡ��ȷ
 */
unsigned char ReadPulseJ4(long *pulse)
{
	 unsigned char i;
	 unsigned int t;
	 unsigned char CheckValue=0;
	 unsigned char CheckValueAll=0;
     unsigned long CacheData=0;
     long PulseValue=0;
     ServoON_OFF_Single(4,1);//J2�ŷ���
	 delay_1ms(20);
	 ServoON_OFF_Single(4,0);//J2�ŷ���
     ResetAbsm4;
     delay_1ms(1);
     for(i=0; i<32; i+=2)//���ݽ���
     {
            WaitAbstReset();   //�ȴ�׼�����ݿ�
            ResetAbsr;    //����ȡ��������
            WaitAbstSet(); //�ȴ�׼�����ݹ�
            CacheData =   !J4_ABSB1;
            CacheData <<= 1;
            CacheData |= (!J4_ABSB0);
            CheckValueAll += CacheData; //�����ΪУ��ֵ
            CacheData <<= i;
            PulseValue |= CacheData;   //��õ���������
            SetAbsr;       //�ض�ȡ��������
     }
     for(i=0; i<6; i+=2) //У�������
     {
            WaitAbstReset();   //�ȴ�׼�����ݿ�
            ResetAbsr;    //����ȡ��������
            WaitAbstSet(); //�ȴ�׼�����ݹ�
            CacheData =  !J4_ABSB1;
            CacheData <<= 1;
            CacheData |=(!J4_ABSB0);
            CacheData <<= i;
            CheckValue |= CacheData;
            SetAbsr;      //�ض�ȡ��������
     }
     WaitAbstReset();          //�ȴ�׼�����ݿ���ȷ����19��ABST�������,����������У��

     SetAbsm4;            //����ABSģʽ
     delay_1ms(1);
     WaitAbstSet();        //�ȴ�׼�����ݹ�
     t=65535;
     while((!J4_RD)&&(t--))      //�ȴ��ŷ�׼�����
     {
    	  delay_200ns();
     }
	if(CheckValueAll != CheckValue)  //У�鲻��ȷ
	{
		 return 1;
	}
	*pulse = PulseValue;//���ػ�õ���������
	return 0;
}
/************************************
 * ��ȡJ5����������
 * ����ֵ��1����ȡ����	0����ȡ��ȷ
 */
unsigned char ReadPulseJ5(long *pulse)
{
	 unsigned char i;
	 unsigned int t;
	 unsigned char CheckValue=0;
	 unsigned char CheckValueAll=0;
     unsigned long CacheData=0;
     long PulseValue=0;
     ServoON_OFF_Single(5,1);//J2�ŷ���
	 delay_1ms(20);
	 ServoON_OFF_Single(5,0);//J2�ŷ���
     ResetAbsm5;
     delay_1ms(2);
     for(i=0; i<32; i+=2)//���ݽ���
     {
            WaitAbstReset();   //�ȴ�׼�����ݿ�
            ResetAbsr;    //����ȡ��������
            delay_200ns();
            WaitAbstSet(); //�ȴ�׼�����ݹ�
            CacheData =   !J5_ABSB1;
            CacheData <<= 1;
            CacheData |= (!J5_ABSB0);
            CheckValueAll += CacheData; //�����ΪУ��ֵ
            CacheData <<= i;
            PulseValue |= CacheData;   //��õ���������
            SetAbsr;       //�ض�ȡ��������
            delay_200ns();
     }
     for(i=0; i<6; i+=2) //У�������
     {
            WaitAbstReset();   //�ȴ�׼�����ݿ�
            ResetAbsr;    //����ȡ��������
            delay_200ns();
            WaitAbstSet(); //�ȴ�׼�����ݹ�
            CacheData =  !J5_ABSB1;
            CacheData <<= 1;
            CacheData |=(!J5_ABSB0);
            CacheData <<= i;
            CheckValue |= CacheData;
            SetAbsr;      //�ض�ȡ��������
            delay_200ns();
     }
     WaitAbstReset();          //�ȴ�׼�����ݿ���ȷ����19��ABST�������,����������У��
     SetAbsm5;            //����ABSģʽ
     delay_1ms(2);
     WaitAbstSet();        //�ȴ�׼�����ݹ�
     t=65535;
     while((!J5_RD)&&(t--))       //�ȴ��ŷ�׼�����
     {
    	  delay_200ns();
     }
	if(CheckValueAll != CheckValue)  //У�鲻��ȷ
	{
		 return 1;
	}
	*pulse = PulseValue;//���ػ�õ���������
	return 0;
}
/************************************
 * ��ȡJ6����������
 * ����ֵ��1����ȡ����	0����ȡ��ȷ
 */
unsigned char ReadPulseJ6(long *pulse)
{
	 unsigned char i;
	 unsigned int t;
	 unsigned char CheckValue=0;
	 unsigned char CheckValueAll=0;
     unsigned long CacheData=0;
     long PulseValue=0;
     ServoON_OFF_Single(6,1);//J2�ŷ���
	 delay_1ms(20);
	 ServoON_OFF_Single(6,0);//J2�ŷ���
     ResetAbsm6;
     delay_1ms(1);
     for(i=0; i<32; i+=2)//���ݽ���
     {
            WaitAbstReset();   //�ȴ�׼�����ݿ�
            ResetAbsr;    //����ȡ��������
            WaitAbstSet(); //�ȴ�׼�����ݹ�
            CacheData =   !J6_ABSB1;
            CacheData <<= 1;
            CacheData |= (!J6_ABSB0);
            CheckValueAll += CacheData; //�����ΪУ��ֵ
            CacheData <<= i;
            PulseValue |= CacheData;   //��õ���������
            SetAbsr;       //�ض�ȡ��������
     }
     for(i=0; i<6; i+=2) //У�������
     {
            WaitAbstReset();   //�ȴ�׼�����ݿ�
            ResetAbsr;    //����ȡ��������
            WaitAbstSet(); //�ȴ�׼�����ݹ�
            CacheData =  !J6_ABSB1;
            CacheData <<= 1;
            CacheData |=(!J6_ABSB0);
            CacheData <<= i;
            CheckValue |= CacheData;
            SetAbsr;      //�ض�ȡ��������
     }
     WaitAbstReset();          //�ȴ�׼�����ݿ���ȷ����19��ABST�������,����������У��

     SetAbsm6;            //����ABSģʽ
     delay_1ms(1);
     WaitAbstSet();        //�ȴ�׼�����ݹ�
     t=65535;
     while((!J6_RD)&&(t--))       //�ȴ��ŷ�׼�����
     {
    	  delay_200ns();
     }

	if(CheckValueAll != CheckValue)  //У�鲻��ȷ
	{
		 return 1;
	}
	*pulse = PulseValue;//���ػ�õ���������
	return 0;
}

/**************************************�ȴ�******************************************/
void WaitAbstReset(void)
{
	unsigned int t=65535;
	while((ABST) && (t--))  //�ȴ�׼�����ݿ�
	{
		 delay_200ns();
	}
}
void WaitAbstSet(void)
{
	unsigned int t=65535;
	while((!ABST) && (t--)) //�ȴ�׼�����ݹ�
	{
		 delay_200ns();
	}
}


