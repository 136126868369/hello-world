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
extern unsigned int USART_RX_STA_B;		// 串口b的接收状态标记
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
 * 读取6个编码器脉冲数据
 * 返回值：err:bit0~5,分别代表J1~J6，0：读取正确	1：读取错误
 */
unsigned char ReadEncoderPulseAll(long *pulseData)
{
	unsigned char i,errTmp,err=0,mode;

	//切换三菱和三协电机，别忘了要改电路板上的跳线
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
 * 读取一个编码器的脉冲数据
 * 输入：Axis：将要读取的轴1~6
 * 		 pulseData：指向脉冲数据的指针
 * 		 mode: 1->三菱电机	2->三协
 * 返回值：0：读取成功	1：读取数据有错误
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
 * 读取J1轴脉冲数据
 * 返回值：1：读取错误	0：读取正确
 */
unsigned char ReadPulseJ1(long *pulse)
{
	 unsigned char i;
	 unsigned int t;
	 unsigned char CheckValue=0;
	 unsigned char CheckValueAll=0;
     unsigned long CacheData=0;
     long PulseValue=0;

	 ServoON_OFF_Single(1,1);//J1伺服关
	 delay_1ms(20);
	 ServoON_OFF_Single(1,0);//J1伺服开
     ResetAbsm1;
     delay_1ms(1);
     for(i=0; i<32; i+=2)//数据接收
     {
            WaitAbstReset();   //等待准备数据开
            ResetAbsr;    //开读取数据请求
            WaitAbstSet(); //等待准备数据关
            CacheData =   !J1_ABSB1;
            CacheData <<= 1;
            CacheData |= (!J1_ABSB0);
            CheckValueAll += CacheData; //计算和为校验值
            CacheData <<= i;
            PulseValue |= CacheData;   //获得的脉冲数据
            SetAbsr;       //关读取数据请求
     }
     for(i=0; i<6; i+=2) //校验码接收
     {
            WaitAbstReset();   //等待准备数据开
            ResetAbsr;    //开读取数据请求
            WaitAbstSet(); //等待准备数据关
            CacheData =  !J1_ABSB1;
            CacheData <<= 1;
            CacheData |=(!J1_ABSB0);
            CacheData <<= i;
            CheckValue |= CacheData;
            SetAbsr;      //关读取数据请求
     }
     WaitAbstReset();          //等待准备数据开，确定第19次ABST开后结束,可以在这里校验

     SetAbsm1;            //结束ABS模式
     delay_1ms(1);
     WaitAbstSet();        //等待准备数据关
     t=65535;
     while((!J1_RD)&&(t--))       //等待伺服准备完成
     {
    	  delay_200ns();
     }
	if(CheckValueAll != CheckValue)  //校验不正确
	{
		 return 1;
	}
	*pulse = PulseValue;//返回获得的脉冲数据
	return 0;
}
/************************************
 * 读取J2轴脉冲数据
 * 返回值：1：读取错误	0：读取正确
 */
unsigned char ReadPulseJ2(long *pulse)
{
	 unsigned char i;
	 unsigned int t;
	 unsigned char CheckValue=0;
	 unsigned char CheckValueAll=0;
     unsigned long CacheData=0;
     long PulseValue=0;
     ServoON_OFF_Single(2,1);//J2伺服关
	 delay_1ms(20);
	 ServoON_OFF_Single(2,0);//J2伺服开
     ResetAbsm2;
     delay_1ms(1);
     for(i=0; i<32; i+=2)//数据接收
     {
            WaitAbstReset();   //等待准备数据开
            ResetAbsr;    //开读取数据请求
            WaitAbstSet(); //等待准备数据关
            CacheData =   !J2_ABSB1;
            CacheData <<= 1;
            CacheData |= (!J2_ABSB0);
            CheckValueAll += CacheData; //计算和为校验值
            CacheData <<= i;
            PulseValue |= CacheData;   //获得的脉冲数据
            SetAbsr;       //关读取数据请求
     }
     for(i=0; i<6; i+=2) //校验码接收
     {
            WaitAbstReset();   //等待准备数据开
            ResetAbsr;    //开读取数据请求
            WaitAbstSet(); //等待准备数据关
            CacheData =  !J2_ABSB1;
            CacheData <<= 1;
            CacheData |=(!J2_ABSB0);
            CacheData <<= i;
            CheckValue |= CacheData;
            SetAbsr;      //关读取数据请求
     }
     WaitAbstReset();          //等待准备数据开，确定第19次ABST开后结束,可以在这里校验

     SetAbsm2;            //结束ABS模式
     delay_1ms(1);
     WaitAbstSet();        //等待准备数据关
     t=65535;
     while((!J2_RD)&&(t--))       //等待伺服准备完成
     {
    	  delay_200ns();
     }

	if(CheckValueAll != CheckValue)  //校验不正确
	{
		 return 1;
	}
	*pulse = PulseValue;//返回获得的脉冲数据
	return 0;
}
/************************************
 * 读取J3轴脉冲数据
 * 返回值：1：读取错误	0：读取正确
 */
unsigned char ReadPulseJ3(long *pulse)
{
	 unsigned char i;
	 unsigned int t;
	 unsigned char CheckValue=0;
	 unsigned char CheckValueAll=0;
     unsigned long CacheData=0;
     long PulseValue=0;
     ServoON_OFF_Single(3,1);//J3伺服关
	 delay_1ms(20);
	 ServoON_OFF_Single(3,0);//J3伺服开
     ResetAbsm3;
     delay_1ms(1);
     for(i=0; i<32; i+=2)//数据接收
     {
            WaitAbstReset();   //等待准备数据开
            ResetAbsr;    //开读取数据请求
            WaitAbstSet(); //等待准备数据关
            CacheData =   !J3_ABSB1;
            CacheData <<= 1;
            CacheData |= (!J3_ABSB0);
            CheckValueAll += CacheData; //计算和为校验值
            CacheData <<= i;
            PulseValue |= CacheData;   //获得的脉冲数据
            SetAbsr;       //关读取数据请求
     }
     for(i=0; i<6; i+=2) //校验码接收
     {
            WaitAbstReset();   //等待准备数据开
            ResetAbsr;    //开读取数据请求
            WaitAbstSet(); //等待准备数据关
            CacheData =  !J3_ABSB1;
            CacheData <<= 1;
            CacheData |=(!J3_ABSB0);
            CacheData <<= i;
            CheckValue |= CacheData;
            SetAbsr;      //关读取数据请求
     }
     WaitAbstReset();          //等待准备数据开，确定第19次ABST开后结束,可以在这里校验

     SetAbsm3;            //结束ABS模式
     delay_1ms(1);
     WaitAbstSet();        //等待准备数据关
     t=65535;
     while((!J3_RD)&&(t--))       //等待伺服准备完成
     {
    	  delay_200ns();
     }
	if(CheckValueAll != CheckValue)  //校验不正确
	{
		 return 1;
	}
	*pulse = PulseValue;//返回获得的脉冲数据
	return 0;
}
/************************************
 * 读取J4轴脉冲数据
 * 返回值：1：读取错误	0：读取正确
 */
unsigned char ReadPulseJ4(long *pulse)
{
	 unsigned char i;
	 unsigned int t;
	 unsigned char CheckValue=0;
	 unsigned char CheckValueAll=0;
     unsigned long CacheData=0;
     long PulseValue=0;
     ServoON_OFF_Single(4,1);//J2伺服关
	 delay_1ms(20);
	 ServoON_OFF_Single(4,0);//J2伺服开
     ResetAbsm4;
     delay_1ms(1);
     for(i=0; i<32; i+=2)//数据接收
     {
            WaitAbstReset();   //等待准备数据开
            ResetAbsr;    //开读取数据请求
            WaitAbstSet(); //等待准备数据关
            CacheData =   !J4_ABSB1;
            CacheData <<= 1;
            CacheData |= (!J4_ABSB0);
            CheckValueAll += CacheData; //计算和为校验值
            CacheData <<= i;
            PulseValue |= CacheData;   //获得的脉冲数据
            SetAbsr;       //关读取数据请求
     }
     for(i=0; i<6; i+=2) //校验码接收
     {
            WaitAbstReset();   //等待准备数据开
            ResetAbsr;    //开读取数据请求
            WaitAbstSet(); //等待准备数据关
            CacheData =  !J4_ABSB1;
            CacheData <<= 1;
            CacheData |=(!J4_ABSB0);
            CacheData <<= i;
            CheckValue |= CacheData;
            SetAbsr;      //关读取数据请求
     }
     WaitAbstReset();          //等待准备数据开，确定第19次ABST开后结束,可以在这里校验

     SetAbsm4;            //结束ABS模式
     delay_1ms(1);
     WaitAbstSet();        //等待准备数据关
     t=65535;
     while((!J4_RD)&&(t--))      //等待伺服准备完成
     {
    	  delay_200ns();
     }
	if(CheckValueAll != CheckValue)  //校验不正确
	{
		 return 1;
	}
	*pulse = PulseValue;//返回获得的脉冲数据
	return 0;
}
/************************************
 * 读取J5轴脉冲数据
 * 返回值：1：读取错误	0：读取正确
 */
unsigned char ReadPulseJ5(long *pulse)
{
	 unsigned char i;
	 unsigned int t;
	 unsigned char CheckValue=0;
	 unsigned char CheckValueAll=0;
     unsigned long CacheData=0;
     long PulseValue=0;
     ServoON_OFF_Single(5,1);//J2伺服关
	 delay_1ms(20);
	 ServoON_OFF_Single(5,0);//J2伺服开
     ResetAbsm5;
     delay_1ms(2);
     for(i=0; i<32; i+=2)//数据接收
     {
            WaitAbstReset();   //等待准备数据开
            ResetAbsr;    //开读取数据请求
            delay_200ns();
            WaitAbstSet(); //等待准备数据关
            CacheData =   !J5_ABSB1;
            CacheData <<= 1;
            CacheData |= (!J5_ABSB0);
            CheckValueAll += CacheData; //计算和为校验值
            CacheData <<= i;
            PulseValue |= CacheData;   //获得的脉冲数据
            SetAbsr;       //关读取数据请求
            delay_200ns();
     }
     for(i=0; i<6; i+=2) //校验码接收
     {
            WaitAbstReset();   //等待准备数据开
            ResetAbsr;    //开读取数据请求
            delay_200ns();
            WaitAbstSet(); //等待准备数据关
            CacheData =  !J5_ABSB1;
            CacheData <<= 1;
            CacheData |=(!J5_ABSB0);
            CacheData <<= i;
            CheckValue |= CacheData;
            SetAbsr;      //关读取数据请求
            delay_200ns();
     }
     WaitAbstReset();          //等待准备数据开，确定第19次ABST开后结束,可以在这里校验
     SetAbsm5;            //结束ABS模式
     delay_1ms(2);
     WaitAbstSet();        //等待准备数据关
     t=65535;
     while((!J5_RD)&&(t--))       //等待伺服准备完成
     {
    	  delay_200ns();
     }
	if(CheckValueAll != CheckValue)  //校验不正确
	{
		 return 1;
	}
	*pulse = PulseValue;//返回获得的脉冲数据
	return 0;
}
/************************************
 * 读取J6轴脉冲数据
 * 返回值：1：读取错误	0：读取正确
 */
unsigned char ReadPulseJ6(long *pulse)
{
	 unsigned char i;
	 unsigned int t;
	 unsigned char CheckValue=0;
	 unsigned char CheckValueAll=0;
     unsigned long CacheData=0;
     long PulseValue=0;
     ServoON_OFF_Single(6,1);//J2伺服关
	 delay_1ms(20);
	 ServoON_OFF_Single(6,0);//J2伺服开
     ResetAbsm6;
     delay_1ms(1);
     for(i=0; i<32; i+=2)//数据接收
     {
            WaitAbstReset();   //等待准备数据开
            ResetAbsr;    //开读取数据请求
            WaitAbstSet(); //等待准备数据关
            CacheData =   !J6_ABSB1;
            CacheData <<= 1;
            CacheData |= (!J6_ABSB0);
            CheckValueAll += CacheData; //计算和为校验值
            CacheData <<= i;
            PulseValue |= CacheData;   //获得的脉冲数据
            SetAbsr;       //关读取数据请求
     }
     for(i=0; i<6; i+=2) //校验码接收
     {
            WaitAbstReset();   //等待准备数据开
            ResetAbsr;    //开读取数据请求
            WaitAbstSet(); //等待准备数据关
            CacheData =  !J6_ABSB1;
            CacheData <<= 1;
            CacheData |=(!J6_ABSB0);
            CacheData <<= i;
            CheckValue |= CacheData;
            SetAbsr;      //关读取数据请求
     }
     WaitAbstReset();          //等待准备数据开，确定第19次ABST开后结束,可以在这里校验

     SetAbsm6;            //结束ABS模式
     delay_1ms(1);
     WaitAbstSet();        //等待准备数据关
     t=65535;
     while((!J6_RD)&&(t--))       //等待伺服准备完成
     {
    	  delay_200ns();
     }

	if(CheckValueAll != CheckValue)  //校验不正确
	{
		 return 1;
	}
	*pulse = PulseValue;//返回获得的脉冲数据
	return 0;
}

/**************************************等待******************************************/
void WaitAbstReset(void)
{
	unsigned int t=65535;
	while((ABST) && (t--))  //等待准备数据开
	{
		 delay_200ns();
	}
}
void WaitAbstSet(void)
{
	unsigned int t=65535;
	while((!ABST) && (t--)) //等待准备数据关
	{
		 delay_200ns();
	}
}


