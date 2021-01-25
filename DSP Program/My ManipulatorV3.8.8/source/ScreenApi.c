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
// TITLE:	触摸屏API函数
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
static u32 s_PoutState=0xFFFFFFFF;	//0->有输出	1->没输出
static unsigned int s_WR2state=0x3003;//保存MCX314的WR2寄存器值，如果要使用实际/反馈脉冲的时候，s_WR2state=0x3023
static unsigned int s_WR4state=0x0FFF;//保存MCX314的WR4寄存器值
static unsigned char s_SpeedMode=0;//记录示教模式：0->低速示教模式  1->高速示教模式
static long s_current_speed[6];//全速运行时，记录每个轴的运行速度，全速运行过程中调整速度时需要用到
extern unsigned int USART_RX_STA_B;		// 串口b的接收状态标记
extern char USART_B_RX_BUF[32];
extern struct EX_Position_stru EX_POSITION;//用于保存从外部获得的位置数据，例如从串口获得位置
//static u8 rentangle_flag=0x66;  //修正于//2018/06/01
//static u32 rentangle_number=0;  //修正于//2018/06/01
#pragma  DATA_SECTION (DEV_NOW, ".MY_MEM0")
static long DEV_NOW[6]={0,0,0,0,0,0}; //修正于//2018/06/01
#pragma  DATA_SECTION (DEV_LAST, ".MY_MEM0")
static long DEV_LAST[6]={0,0,0,0,0,0};//修正于//2018/06/01
extern unsigned char Teach_mode;//修正于//2018/06/01
/***********************************************************
 * 功能：单轴运动
 * 参数：pressFlag  按钮标志位；  	1：表示按下   0：表示松开
 * 					如果上一次未松开，则下一次按下无效
 * 		 whichAxis	表示哪个轴	  	1~6分别表示1~6轴
 * 		 sign		表示驱动方向 	0：正方向	  1：负方向
 * 返回值：0：正确
 * 		   1：机械手动作过程中发生报警
 * 		   2：机械手动作过程中松开了安全开关（示教时）
 */
unsigned char AxisSingleRun(unsigned char pressFlag,unsigned char whichAxis,unsigned char sign)
{
	 static unsigned char once=1;
	 unsigned char speed,i,err=0;
	 unsigned int spd,spd_tmp;
	 double f_tmp;
	 int	i_tmp;
	 if(pressFlag == 1)//按下
	 {
		 if(once == 1)
		  {
			    SetDriCmd(0x3F,0x27);//先停止
				G_CTRL_BUTTON.CTRL_BUTTON.POS_LIMITED_FLG=0;
				G_CTRL_BUTTON.CTRL_BUTTON.NEG_LIMITED_FLG=0;

				if(!SAVE_BUTTON)//安全开关按下才能控制
				{
					if(ROBOT_PARAMETER.MOTOR_SELECT & (0x01<<(whichAxis-1)))//轴选中的才能单轴操作
					{
						spd = RecoverSpeed();
						speed = GetProgramRunSpeed();//获得当前示教的速度
						SetAcc(0x3f,120);      //加速度  A*125	 1~8000
						SetStartV(0x3F,(unsigned int)speed*2);      //初始速度   范围：1~8000
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
									  //正限位的时候不能继续正方向走
									  if((err&0x01) && (sign==0))
									  {
										  SetDriCmd(0x3F,0x26);
										  return 1;
									  }
									  //负限位的时候不能继续负方向走
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
							SetStartV(0x3F,1);      //初始速度   范围：1~8000
							if(ROBOT_PARAMETER.SYS==FOUR_AXIS_B)
							{
								spd_tmp = spd%40;		//
								spd_tmp = 40 - spd_tmp;
								spd = spd + spd_tmp;	//将spd凑够40的倍数
								SetSpeed(0x08, spd);	//设置第四轴速度
								spd = (spd/40);
								if(spd<=0) spd = 1;
								SetSpeed(0x04, spd);	//设置第三轴速度
								SetAcc(0x04,160/40);	//设置第三轴的加速度
								SetDriCmd(0x0C,0x22+sign);//正方向连续驱动
							}
							else
							{
								f_tmp = (double)spd/24;
								i_tmp = f_tmp;			//获得整数部分
								f_tmp = f_tmp - i_tmp;	//获得小数部分
								spd = spd + (1-f_tmp)*24 + 0.5;	//凑够24的倍数
								SetSpeed(0x08, spd);	//设置第四轴速度
								spd = ((double)spd/24*10);
								if(spd<=0) spd = 1;
								SetSpeed(0x04, spd);	//设置第三轴速度
								SetAcc(0x08,192);	//设置第四轴的加速度
								SetAcc(0x04,192/24);//设置第三轴的加速度8
								SetDriCmd(0x0C,0x22+sign);//正方向连续驱动
							}
						}
						else
						{
							SetDriCmd(0x01<<(whichAxis-1),0x22+sign);//正方向连续驱动
						}
					}
				}
				once = 0;//处理完之后就清零，防止重复进入
		  }
		  if((ROBOT_PARAMETER.SYS == FOUR_AXIS_B||ROBOT_PARAMETER.SYS == FOUR_AXIS_C)
				&& (whichAxis==3))
		  {
			  //正限位的时候正方向走,延时一段时间，度过限位阶段
			  if((err&0x01) && (sign==1))
			  {
				  delay_1ms(50);
			  }
			  //负限位的时候正方向走,延时一段时间，度过限位阶段
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
	 else//松开
	 {
		  SetDriCmd(0x3f,0x26);//驱动减速停止
		  //SetAcc(aix,(unsigned int)r);      //加速度  A*125	1~8000
		  //SetDec(aix,(unsigned int)r*0.2);      //减速度  A*125 1~8000
		  while(ReadDriState())//等待驱动结束
		  {
			  delay_1us(1);
		  }
		  SetStartV(0x3f,8000);      //初始速度   范围：1~8000
		  once = 1;//只有按钮松开，once才变为1，只有once为1才能进入连续驱动
	 }
	 return 0;
}
/********************************************************
 * 功能：沿基坐标直线运动  Base coordinate straight line motion
 * 参数：pressFlag 按钮标志位， 1：表示按钮按下   0：按钮松开
 * 					如果上一次未松开，那么下一次按下将不会更新姿态
 * 		 orientation  方向标志位  1：X-  2：X+
 * 		 						  3：Y-	 4：Y+
 * 		 						  5：Z-  6：Z+
 * 返回值： 0：正确
 * 			1:机械手动作过程中发生报警
 * 			2：机械手动作过程中松开了安全开关（示教时）
 * 			4：超出动作范围
 * 			5: 其他用途
 * 			6: 奇异点
 */
unsigned char BaseCoordSlineRun(unsigned char pressFlag,unsigned char orientation)
{
	 static unsigned char once=1;
	 u8 err;
	 if(pressFlag==1)//按下
	  {
		 if(once==1)
		 {
			  ClearStopProgramFlag();
			  SetDriCmd(0x3f,0x27);//停止驱动
			  UpdateCurrentPositionGesture();//获得一次姿态
			  G_CTRL_BUTTON.CTRL_BUTTON.POS_LIMITED_FLG=0;
			  G_CTRL_BUTTON.CTRL_BUTTON.NEG_LIMITED_FLG=0;
			  ClearStopProgramFlag();//清除暂停标志位
			  RecoverSpeed();
			  once=0;
		 }
		//if((!SAVE_BUTTON)&&(!G_CTRL_BUTTON.CTRL_BUTTON.SERVO_ON))//安全开关按下，且伺服on打开才能控制
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
	  else//松开
	  {
			 while(ReadDriState())//等待驱动结束
			 {
				  delay_200ns();
				  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
				  {
					   SetDriCmd(0x3F,0x27);
					   return 1;
				  }
				  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:上拉,使用安全开关的时候，松开(高电平)就退出
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
			 once=1;//只有按钮松开，once才变为1；然后下次进入这个函数才能获得一次姿态
	  }
	  return 0;
}

/********************************************************
 * 功能：沿基坐标固定直线距离运动  Base coordinate straight line Fixed distance
 * 参数：changeGes  决定是否改变姿态  1：获得新姿态    0：使用上一次获得的姿态
 * 		 movingDistance 移动距离， double类型，可正负，正负代表了xyz轴的方向,最高精度的倍数
 * 		 orientation  xyz轴方向  		只能输入px，py，pz的指针
 */
void BaseCoordSlineFixedDistance(unsigned char changeGes,double movingDistance,double *orientation)
{
	 int moveTimes;
	 double precision;

	 if(changeGes == 1)
	 {
		  UpdateCurrentPositionGesture();//获得一次新的姿态
	 }

	 moveTimes = movingDistance/ROBOT_PARAMETER.PRECISION;
	 if(moveTimes>0)
	 {
		  precision = ROBOT_PARAMETER.PRECISION;
	 }
	 else if(moveTimes<0)
	 {
		  precision = (-ROBOT_PARAMETER.PRECISION);
		  moveTimes = -moveTimes;//负变正
	 }
	 else
	 {
		  return;//0的话直接退出
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
	 while(ReadDriState())//等待驱动结束
	 {

	 }
}

/********************************************************
 * 功能：沿基坐标固定直线最小精度运动,使用前需要先改变px，py，pz的值，
 * 		 否则直接调用无意义
 * 返回值： 0：正确
 * 			1:机械手动作过程中发生报警
 * 			2：机械手动作过程中松开了安全开关（示教时）
 * 			3：动作过程中按下了stop按钮
 * 			4：超出动作范围
 * 			5:其他用途
 * 			6: 奇异点
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
	 if(GetBestSolution(movedAg))//获得最优解//91021
		 return 4;//超出动作范围

//	  for(i=0;i<6;i++)
//	  {
//		  ag[i] = *(movedAg+i)*PI_MULTI_180_INVERSE;//弧度转角度
//	  }

	 while(ReadDriState())//等待驱动结束
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
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:上拉,使用安全开关的时候，松开(高电平)就退出
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
			   SetDriCmd(0x3F,0x26);	//减速停止
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
	  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:上拉,使用安全开关的时候，松开(高电平)就退出
	  {
		  delay_1us(1);
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)
		  {
			  SetDriCmd(0x3F,0x27);	//立即停止
			  return 2;
		  }
	  }
	  if(KeyBoard_StopSpeed())
	  {
		   SetDriCmd(0x3F,0x26);//减速停止
		   return 3;
	  }
	 ReadCurrentPulseLp(currentPulse);//获得当前的位置的脉冲数（逻辑值）
	 AngleToPulse(movedAg,movedPulse);//计算逆解得到的最优解换算成脉冲
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
	   //获得平移后的位置的脉冲数（理论值）
	   differPulse[i]=movedPulse[i]-currentPulse[i];//需要移动的脉冲数，理论值与当前值的差值
	  //memset(str,0,sizeof(str));
	  //longToString(differPulse[i],str,12);//浮点型转换为字符串，小数点后3位
	  //strcat(str," ");
	  //STM32_USART_SentData(str,strlen(str)+1);
	 }

		//STM32_USART_SentData(newline,strlen(newline)+1);

	 if(ROBOT_PARAMETER.AXIS_NUM == 6)
	 {
		 //第4或第6轴出现突变时
		 if((differPulse[3] > ROBOT_PARAMETER.RATIO[3]*ROBOT_PARAMETER.MOTOR_PRECISION*0.09) ||
				 (differPulse[5] >ROBOT_PARAMETER.RATIO[5]*ROBOT_PARAMETER.MOTOR_PRECISION*0.09))
		 {
			 //判断1,2,3,5轴变化幅度是否很小
			 if((differPulse[0]>-(ROBOT_PARAMETER.RATIO[0]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)
				&&differPulse[0]<(ROBOT_PARAMETER.RATIO[0]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)) &&
				(differPulse[1]>-(ROBOT_PARAMETER.RATIO[1]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)
				&&differPulse[1]<(ROBOT_PARAMETER.RATIO[1]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)) &&
				(differPulse[2]>-(ROBOT_PARAMETER.RATIO[2]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)
				&&differPulse[2]<(ROBOT_PARAMETER.RATIO[2]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)) &&
				(differPulse[4]>-(ROBOT_PARAMETER.RATIO[4]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)
				&&differPulse[4]<(ROBOT_PARAMETER.RATIO[4]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)) )
			 {
				 //判断4,5,6是否是一条直线(即第5轴是否接近0度)
				 if((currentPulse[4]>-(ROBOT_PARAMETER.RATIO[4]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)
					&&currentPulse[4]<(ROBOT_PARAMETER.RATIO[4]*ROBOT_PARAMETER.MOTOR_PRECISION/360*0.05)))
				 return 6;
			 }
		 }
	 }
	 TranslationProccess(differPulse,1);//平移数据处理，驱动输出脉冲//7000
	 return 0;
}


/********************************************************
 * 功能：以当前位置，行走输入的脉冲数
 * 输入参数：delta_pulse 指向6个轴每个轴需要走的脉冲数的指针
 * 			//mdoe:0->关节插补		1->直线插补
 * 返回值：    0：正确
 * 			1:机械手动作过程中发生报警
 * 			2：机械手动作过程中松开了安全开关（示教时）
 * 			3：动作过程中按下了stop按钮
 */
unsigned char MovePulse(long* delta_pulse)
{
	 unsigned char i,err=0;//13260
	 while(ReadDriState())//等待驱动结束
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
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:上拉,使用安全开关的时候，松开(高电平)就退出
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
	  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:上拉,使用安全开关的时候，松开(高电平)就退出
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
	 TranslationProccess(delta_pulse,1);//平移数据处理，驱动输出脉冲//7000
	 return 0;
}

/*
 * 末端坐标直线运动
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
 * 函数功能：更新当前位置姿态，若想以当前的姿态平移，先获得/更新姿态
 * 输入参数：none
 * 返回值：	 none
 */
void UpdateCurrentPositionGesture(void)
{
	 long current_pulse[6];
	 double angle[6];
	 //ReadCurrentPulseEp(current_pulse);	//获得当前脉冲数
	 ReadCurrentPulseLp(current_pulse);//修正于//2018/06/01
	 //获得弧度
	 PulseToAngle(current_pulse,angle);
	 UpdatePositionGesture(angle);	//更新位置姿态，
	 //currentEp[2] = -currentEp[2];
	 //currentEp[4] = -currentEp[4];
	 /*for(i=0;i<6;i++)
	 {
		  SetLp(0x01<<i,currentEp[i]);//xxx:每次读实际值的时候，更改逻辑值，使他们统一(直线圆弧插补的时候用Lp，其他情况用Ep)
	 }*/
}
/****************************************
 *功能：从运动芯片读取实际脉冲值
 */
void ReadCurrentPulseEp(long *pulse)
{
/*
	unsigned char i;
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
		  *(pulse+i)=ReadPulseLp(0x01<<i);//把Lp修改为Ep//2018/06/01
	 }
*/
	unsigned char i;
	/*
	//通过串口读取伺服驱动器的编码器位置数据
	//UpdatePulseFromServoForEp(pulse);
	*/
	//通过读取伺服驱动器的反馈位置脉冲读出脉冲编码器的位置
	for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	{
		*(pulse+i)=ReadPulseEp(0x01<<i);//2018/06/01
	}
	if(ROBOT_PARAMETER.MOTOR_TYPE == 2)
	{
		//三协电机需要减去零点偏移量才是真正的零点
		for(i=0;i<6;i++)
		{
			pulse[i] = pulse[i] - ROBOT_PARAMETER.ENCODER_LEFT[i];
		}
	}
	//伺服放大器如果设置了反方向旋转的话，读回来的脉冲是相反数
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
	//减去零点状态,得到的脉冲才是实际的位置
	for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	{
		pulse[i] = pulse[i] + ROBOT_PARAMETER.RATIO[i]*ROBOT_PARAMETER.MOTOR_PRECISION*
								 	(ROBOT_PARAMETER.ORIGIN_POSITION[i]/360.0);
	}
	//4号机数据补偿(TU2一楼元件夹具以及摆盘)(100W拿掉外壳之后)
	//-()的负数相加，正数相减
	// ()的负数相减，正数相加
	if(ROBOT_PARAMETER.AXIS_NUM == 6)
	{
		pulse[0]=-(pulse[0]-5555);//-5554
		pulse[1]=pulse[1]+19724;//19667
		pulse[2]=-pulse[2]+744;//2103
		pulse[3]=-(pulse[3]-3944);//-3638
		pulse[4]=-(pulse[4]-8479);//-8731
		pulse[5]=-(pulse[5]-15708);//-14977
	}
	WriteMCX312_WR4(s_PoutState&0xffff);//读Lp或Ep时这两个寄存器会被清零
	WriteWR2(0x3F,s_WR2state);
}
/****************************************
 *功能：从运动芯片读取逻辑脉冲值
 */
void ReadCurrentPulseLp(long *pulse)
{
	 unsigned char i;
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
		  *(pulse+i)=ReadPulseLp(0x01<<i);//把Lp修改为Ep//2018/06/01
	 }
	 WriteMCX312_WR4(s_PoutState&0xffff);//读Lp或Ep时这两个寄存器会被清零
	 WriteWR2(0x3F,s_WR2state);
}
/****************************************
 *功能：获得当前每轴的角度
 *参数：angle 角度首指针
 */
void GetCurrentAngle(double* angle)
{
	 unsigned char i;
	 long pulse[6];
	 ReadCurrentPulseLp(pulse);
	 PulseToAngle(pulse,angle);
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
		  *(angle+i) = *(angle+i)*PI_MULTI_180_INVERSE;//弧度转角度
	 }
}

/****************************************
 *功能：获得当前每轴的弧度
 *参数：angle 角度首指针
 */
void GetCurrentRadian(double* angle)
{
	 long pulse[6];
	 //ReadCurrentPulseEp(pulse);
	 ReadCurrentPulseLp(pulse);//修正于//2018/06/01
	 PulseToAngle(pulse,angle);
}

/****************************************
 *功能：从外部(串口)获得位置数据
 *参数：position 指向位置的指针
 *返回值：数据个数 1->一个数据  2->两个数据  3->三个数据 6->六个数据
 *返回值：-1 -> 通信出错
 */
char GetPosition_SCI(double* position)//从外部获得位置数据
{
	int i;
	for(i=0;i<6;i++)
	{
		position[i] = EX_POSITION.EX_POSITION[i];
	}

	return EX_POSITION.type;
}

/****************************************
 *功能：从STM32(串口)获得位置数据
 *参数：data:指向数据的指针
 *返回值：0->数据获取成功		1->失败
 */
extern int RX_POSITION_STA;
char Data_To_Position(char *data)
{
	int i=0;
	u8 flg;

	//直到遇上数字
	while((data[i] != '+') && (data[i] != '-') &&
			(data[i] < '0' || data[i] > '9'))
	{
		i++;
		if(i>(STM32_USART_LEN-1))break;
	}
	flg = data[i];//第一个数据代表是否有效,'1'有效  '0'无效
	i++;
	if(flg == '1')
	{
		//直到遇上数字
		while((data[i] != '+') && (data[i] != '-') &&
				(data[i] < '0' || data[i] > '9'))
		{
			i++;
			if(i>(STM32_USART_LEN-1))break;
		}
		EX_POSITION.EX_POSITION[0] = atof(&data[i]);//提取数据
		EX_POSITION.type = 1;
		//跳过数据
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
		//直到遇上数字
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
		//跳过数据
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

		if(data[i] != 0x0d && data[i] != 0x0)//不等于0x0d，表示还有数据
		{
			//直到遇上数字
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
			//跳过数据
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

		if(data[i] != 0x0d && data[i] != 0x0)//不等于0x0d，表示还有数据
		{
			//直到遇上数字
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
			//跳过数据
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

		if(data[i] != 0x0d && data[i] != 0x0)//不等于0x0d，表示还有数据
		{
			//直到遇上数字
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
			//跳过数据
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

		if(data[i] != 0x0d && data[i] != 0x0)//不等于0x0d，表示还有数据
		{
			//直到遇上数字
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
			//跳过数据
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
		{	//角度转换为弧度
			EX_POSITION.EX_POSITION[3] = EX_POSITION.EX_POSITION[3]*PI_DIVIDE_180;
			EX_POSITION.EX_POSITION[4] = EX_POSITION.EX_POSITION[4]*PI_DIVIDE_180;
			EX_POSITION.EX_POSITION[5] = EX_POSITION.EX_POSITION[5]*PI_DIVIDE_180;
		}

		if((EX_POSITION.EX_POSITION[0]==0) && (EX_POSITION.EX_POSITION[1]==0))//都等于0说明数据有问题
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
 * 功能：设置脉冲输出频率
 * 参数	aix:bit0~5分别对应1~6轴， 0：表示没选中  1：表示选中
 * 		r:速度值，1~8000；1，速度最小；8000，速度最大
 */
void SetSpeed(unsigned char aix,unsigned int r)
{
		//RANGE_MCX=RANGE/r;//(RANGE = 800,000,000)
		//SetRange(0x3f,RANGE_MCX/100);  //(8000000/R)
	 if(r <= 1)	r=1;
	 //SetAcc(aix,(unsigned int)r);      //加速度  A*125	1~8000
	 //SetDec(aix,(unsigned int)r*0.2);      //减速度  A*125 1~8000
	 //SetStartV(aix,(unsigned int)r);      //初始速度   范围：1~8000
	 SetPulseFreq(aix,r);	//驱动速度 1~8000（脉冲输出频率）
}

/*****************************************
 * 功能：更新速度，和上面的函数功能不同，上面适用于初始化速度，
 * 		 而这个函数适用于机械手动作过程中调整速度,修改的是全局速度
 * 输入参数：ovrd，全局速度值，范围1~100
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

	persentage = (float)new_all_speed/current_all_speed;//速度增加的百分比
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
 * 打开IO口(与地导通)
 * 输入参数：num：将被打开的IO口,范围0~23，分别对应IO 0~23
 * 返回值： 0->OK 1->跟STM32通信异常 2->IO num超出范围
 */
u8 SetPoutState(unsigned int num)
{
	 u8 tx_cmd[5],rx_cmd[5],t=0;
	 if(num >= MAX_POUT_NUM) return 2;
	 s_PoutState = s_PoutState&(~((long)0x01<<(num+4)));//0~3预留给刹车


	 if(num<=11)
	 {
		 WriteMCX312_WR4(s_PoutState&0xffff);//bit0~bit15分别对应电路板上的EOUT1~EOUT16，
								  //低电平IO口与地导通，高电平IO口与地不导通
		 SetButtonValue(12,num+2,1);//按钮弹起来
	 }
	 else
	 {
		 //通过spi发送数据给stm32，控制IO，IO12~23由stm32控制的
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

		SetButtonValue(12,num+8,1);//按钮弹起来
	 }
	 return 0;
}
/*****************************************
 * 关闭IO口(与地不导通)
 * 输入参数：num：将被关闭的IO口,范围0~23，分别对应IO 0~23
 * 返回值： 0->OK 1->跟STM32通信异常 2->IO num超出范围
 */
u8 ResetPoutState(unsigned char num)
{
	 u8 tx_cmd[5],rx_cmd[5],t=0;
	 if(num>=MAX_POUT_NUM) return 2;
	 s_PoutState = s_PoutState|((long)0x01<<(num+4));//0~3预留给刹车


	 if(num<=11)
	 {
		 WriteMCX312_WR4(s_PoutState&0xffff);//bit0~bit23分别对应电路板上的EOUT1~EOUT24，
								  //低电平IO口与地导通，高电平IO口与地不导通
		 SetButtonValue(12,num+2,0);//按钮按下去
	 }
	 else
	 {
		 //通过spi发送数据给stm32，控制IO，IO12~23由stm32控制的
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

		 SetButtonValue(12,num+8,0);//按钮按下去
	 }
	 return 0;
}

/*****************************************
 * 发生报警的时候亮红灯，其余两种灯关闭
 * 输入参数：none
 * 返回值：  none
 */
void AlarmOutput(void)
{
	SetPoutState(9);	//亮红灯
	SetPoutState(10);	//亮红灯
	ResetPoutState(7);	//关闭绿灯
	ResetPoutState(8);	//关闭黄灯
//	delay_1ms(3);
}

/*****************************************
 * 待机的时候亮黄灯，其余两种灯关闭
 * 输入参数：none
 * 返回值：  none
 */
void StandbyOutput(void)
{
	SetPoutState(8);	//亮黄灯
	ResetPoutState(7);	//关闭绿灯
	ResetPoutState(9);	//关闭红灯
	ResetPoutState(10);	//关闭红灯
//	delay_1ms(3);
}

/*****************************************
 * 运行中的时候亮绿灯，其余两种灯关闭
 * 输入参数：none
 * 返回值：  none
 */
void RunningOutput(void)
{
	ResetPoutState(9);	//关闭红灯
	ResetPoutState(10);	//关闭红灯
	ResetPoutState(8);
//	SetPoutState(11);	//亮绿灯
//	ResetPoutState(10);	//关闭黄灯
//	delay_1ms(3);
}

/*****************************************
 * 打开刹车
 * 输入参数：num：将被打开的刹车,输入范围0~2，分别对应EOUT14~16,也就是1~3轴的电磁制动（刹车）
 * 返回值：  none
 */
void OpenBrake(unsigned char num)
{
	 if(num>=3) return;
	 s_PoutState = s_PoutState&(~(0x0001<<(num)));
	 WriteMCX312_WR4(s_PoutState&0xffff);//bit0~bit15分别对应电路板上的EOUT1~EOUT16，
							  //高电平IO口与地导通，低电平IO口与地不导通
	 SetButtonValue(12,num+15,1);//按钮弹起来
}

/*****************************************
 * 关闭刹车
 * 输入参数：num：将被关闭的刹车,输入范围0~2，分别对应EOUT14~16,也就是1~3轴的电磁制动（刹车）
 * 返回值：  none
 */
void CloseBrake(unsigned char num)
{
	 if(num>=3) return;
	 s_PoutState = s_PoutState|(0x0001<<(num));
	 WriteMCX312_WR4(s_PoutState&0xffff);//bit0~bit15分别对应电路板上的EOUT1~EOUT16，
							  //高电平IO口与地导通，低电平IO口与地不导通
	 SetButtonValue(12,num+15,0);//按钮按下去
}

/*****************************************
 * 函数功能：打开风扇
 * 输入参数：NONE
 * 输出参数：NONE
 */
void OpenFan(void)
{
	 RunningOutput();	//亮绿灯
	 s_PoutState = s_PoutState&(~(0x0001<<3));
	 WriteMCX312_WR4(s_PoutState&0xffff);//bit0~bit15分别对应电路板上的EOUT1~EOUT16，
								  //高电平IO口与地导通，低电平IO口与地不导通
}

/*****************************************
 * 函数功能：关闭风扇
 * 输入参数：NONE
 * 输出参数：NONE
 */
void CloseFan(void)
{
	StandbyOutput();	//亮黄灯
	s_PoutState = s_PoutState|(0x0001<<3);
	WriteMCX312_WR4(s_PoutState&0xffff);//bit0~bit15分别对应电路板上的EOUT1~EOUT16，
}

/*****************************************
 * 读输出IO状态
 * 返回值：s_PoutState:IO状态,bit0~bit23分别代表IO 0~23,0:开状态	1：IO处于关状态
 */
u32 ReadPoutState(void)
{
	 return s_PoutState>>4;//0000 0000 0000 1111 bit0~3刹车占用
}

/*****************************************
 * 函数功能：读刹车状态
 * 返回值：bit0~bit2，分别代表1~3轴的刹车
 * 		   1:刹车已关闭 	0:刹车已打开
 */
unsigned char ReadBrakeState(void)
{
	return s_PoutState&0x0f;//0000 0000 0000 1111
}
/*****************************************
 * 读输入IO状态(已接上拉电阻)
 * 返回值：输入IO状态（前18bits，1：有信号，0：没信号）
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
 * 功能：设定限位
 * 参数：plus:  正限位值
 * 		 minus：负限位值
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
 * 关节插补 Joint interpolation
 * 返回值： 0：正确
 * 			1：动作过程中发生报警
 * 			2：动作过程中松开了安全开关
 * 			3：动作过程中按下了stop按钮
 */
extern unsigned char THE_FOUR_AIXS_XUANZHUAN;
unsigned char JointInterpolation(long* next_pulse)
{
	 unsigned char i,err=0;
	 long current_pulse[6],differ_pulse[6];
	 //ReadCurrentPulseEp(current_pulse);
	 ReadCurrentPulseLp(current_pulse);//修正于//2018/06/01
	 //WriteWR2(0x3F,0x3003);//重新使能报警（因为直线插补后，这个寄存器会无端端被清零）
	 for(i=0;i<6;i++)
	 {
		  differ_pulse[i] = *(next_pulse+i) - current_pulse[i];
	 }
	 SetStartV(0x3F,(unsigned int)GetProgramRunSpeed()*6);//设置初速度
	 //SetStartV(0x3F,8000);//设置初速度
	 TranslationProccess(differ_pulse,0);//开始运动
	 while(ReadDriState())//等待驱动结束
	 {
//		  if((ROBOT_PARAMETER.AXIS_NUM == 4) && (THE_FOUR_AIXS_XUANZHUAN==1))
//		  {
//			  if(ReadPinState()&(0x0001<<9))//如果第9输入IO为有效，则停止运行
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
				   SetStartV(0x3F,8000);//初速度恢复到最大值
				   return 1;
			  }
		  }
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:上拉,使用安全开关的时候，松开(高电平)就退出
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
			   SetDriCmd(0x3F,0x26);	//减速停止
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
	  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:上拉,使用安全开关的时候，松开(高电平)就退出
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
		   SetDriCmd(0x3F,0x26);	//减速停止
		   return 3;
	  }
	 SetStartV(0x3F,8000);
	 return 0;
}
/*****************************************
 * 直线插补 Linear interpolation
 * 返回值： 0：直线插补成功
 * 			1：动作过程中发生报警
 * 			2：动作过程中松开了安全开关
 * 			3：动作过程中按下了stop按钮
 * 			4：超出动作范围
 * 			5：运行成功，但姿态不一致
 * 			6:奇异点
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
	 UpdateCurrentPositionGesture();			//更新当前位置姿态
	 PulseToAngle(next_pulse,next_angle);		//将下一个脉冲位置转换为角度
	 //ReadCurrentPulseEp(current_pulse);			//读当前脉冲位置
	 ReadCurrentPulseLp(current_pulse);//修正于//2018/06/01
	 PulseToAngle(current_pulse,current_angle);	//将当前脉冲位置转换为角度
	 next_ges_pos = GetPositionGesture(next_angle);// xxx 可以优化
	 current_ges_pos = GetPositionGesture(current_angle);
	 if(CompareTowGesture(next_ges_pos,current_ges_pos))
	 {
		 return 5;//把这个return注释掉,就可以姿态不一致时,边直线边均匀变化姿态方式运行

		 gestrue_flg = 1;//标记姿态不一致
		 if(ROBOT_PARAMETER.AXIS_NUM == 4)
		 {
			 return 5;
		 }
	 }

	 differ_p[0] =  next_ges_pos.px - current_ges_pos.px;//将要移动的距离
	 differ_p[1] =  next_ges_pos.py - current_ges_pos.py;
	 differ_p[2] =  next_ges_pos.pz - current_ges_pos.pz;

	 for(i=0;i<3;i++)
	 {
		  if(differ_p[i]<0)
		  {
			   differ_p[i] = -differ_p[i];
			   t |= 0x01<<i;//记录哪个是负数
		  }
	 }
	 speed = GetProgramRunSpeed();

	 if(gestrue_flg == 1)	//姿态不一致
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
	 else	//姿态一致
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
	 max = max>times[2]?max:times[2];		//移动的次数

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
//	 for(i=0;i<max;i++)	//移动的总距离=每次移动的距离*次数;
//	 {
//		  MovePxyz(delta_p[0],delta_p[1],delta_p[2]);
//		  err = BaseCoordSlineMinPrecisionRun();
//		  if(err) return err;
//	 }
	 if(gestrue_flg == 0)//姿态一致时
	 {
		 for(i=0;i<max;i++)	//移动的总距离=每次移动的距离*次数;
		 {
			  MovePxyz(delta_p[0],delta_p[1],delta_p[2]);
			  err = BaseCoordSlineMinPrecisionRun();
			  if(err) return err;
		 }
	 }
	 else//姿态不一致时
	 {
		 /*
		  * 功能：姿态不一致时，不需要通过STM32串口向电脑发送数据，这个功能调试时可以用
		  * 修正于//2018/06/01
		 //char str[]={'*','*','*','*','*'};
		 //STM32_USART_SentData(str,4);
		 */
		 ReadCurrentPulseLp(currentPulse);//获得当前的位置的脉冲数（逻辑值）
		 //4,5,6轴的每次行走脉冲数
		 delta_pulse_456[0] = (next_pulse[3] - currentPulse[3])/max;
		 delta_pulse_456[1] = (next_pulse[4] - currentPulse[4])/max;
		 delta_pulse_456[2] = (next_pulse[5] - currentPulse[5])/max;

		 for(i=0;i<max;i++)	//移动的总距离=每次移动的距离*次数;
		 {
			  MovePxyz(delta_p[0],delta_p[1],delta_p[2]);
			  if(GetBestSolution(movedAg))//获得最优解
				  return 4;	//超出动作范围
			  ReadCurrentPulseLp(currentPulse);//获得当前的位置的脉冲数（逻辑值）
			  AngleToPulse(movedAg,movedPulse);//计算逆解得到的最优解换算成脉冲
			  for(j=0;j<3;j++)
			  {
				  //获得平移后的位置的脉冲数（理论值）
				  delta_pulse[j]=movedPulse[j]-currentPulse[j];//需要移动的脉冲数，理论值与当前值的差值
			  }
			  delta_pulse[3] = delta_pulse_456[0];
			  delta_pulse[4] = delta_pulse_456[1];
			  delta_pulse[5] = delta_pulse_456[2];

			  err = MovePulse(delta_pulse);
			  if(err) return err;
		 }
	 }
	 while(ReadDriState())//等待驱动结束
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
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:上拉,使用安全开关的时候，松开(高电平)就退出
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
			   SetDriCmd(0x3F,0x26);	//减速停止
			   return 3;
		  }
	 }
	 if(gestrue_flg)
	 {
		 return 5;//运行成功，但姿态不一致
	 }
	 return 0;
}
/*****************************************
 * 半圆
 * 输入参数：next_pulse	下一个脉冲位置
 * 			 orientation 	画圆的方向	0：正方向	1：负方向
 * 返回值： 0：圆弧插补成功
 * 			1：动作过程中发生报警
 * 			2：动作过程中松开了安全开关
 * 			3：动作过程中按下了stop按钮
 * 			4：超出动作范围
 * 			5：圆弧插补失败
 * 			6: 奇异点
 * 			7: 姿态不一致
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
	 UpdateCurrentPositionGesture();			//更新当前位置姿态
	 PulseToAngle(next_pulse,next_angle);			//将下一个脉冲位置转换为角度
	 //ReadCurrentPulseEp(current_pulse);			//读当前脉冲位置
	 ReadCurrentPulseLp(current_pulse);//修正于//2018/06/01
	 PulseToAngle(current_pulse,current_angle);	//将当前脉冲位置转换为角度
	 next_ges_pos = GetPositionGesture(next_angle);
	 current_ges_pos = GetPositionGesture(current_angle);
	 if(CompareTowGesture(next_ges_pos,current_ges_pos))
	 {
		  return 7;
	 }

	 differ_p[0] =  (next_ges_pos.px - current_ges_pos.px)*0.5;
	 differ_p[1] =  (next_ges_pos.py - current_ges_pos.py)*0.5;
	 differ_p[2] =  (next_ges_pos.pz - current_ges_pos.pz);//两位置的垂直高度
	 //if(differ_p[2]>5) return 4;//
	 circle_parameter(differ_p[0],differ_p[1]);//圆半径
	 R = sqrt(differ_p[0]*differ_p[0]+differ_p[1]*differ_p[1]);//圆半径
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
	 r = t_p/R;//每次走动的弧度
	 n = PI/r; //总共要走的次数   n*r=PI,半圈
	 delta_height = differ_p[2]/n;//垂直高度每次走动的距离
	 if(orientation)	r = -r;//方向
	 for(i=0;i<n;i++)//
	 {
		  circle_xy(r,delta_height);////每次走动r弧度
		  err = BaseCoordSlineMinPrecisionRun();//走起
		  if(err) return err;
	 }
	 while(ReadDriState())//等待驱动结束
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
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:上拉,使用安全开关的时候，松开(高电平)就退出
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
			   SetDriCmd(0x3F,0x26);	//减速停止
			   return 3;
		  }
	 }
	 return 0;
}
/*****************************************
 * 插补运动
 * 输入参数：differpulse 需要移动的脉冲数
 * 			mode:0->关节插补		1->直线插补
 */
void TranslationProccess(long *differPulse,unsigned char mode)
{
	 unsigned char i,t=0,speed;
	 long freq[6],max;
	 float R[6],data_base;
	 //修正与2018/06/01
	 static long currentLPPulse[6];
	 static long currentEPPulse[6];
	 //char str[58];
	 //char newline[5]="\r\n";
	 //char distinguish[3]=" ";
/*
	 //delay_1ms(20);
	 //通过串口读入伺服编码器数据，并和LP的值做差
	 //ReadCurrentPulseEp(currentEPPulse);
 */
    if(Teach_mode==1)
    {
    	//读入LP的值，并放入一个暂存变量数组currentLPPulse中
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
		//修正与2018/06/01
		for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
		{
			memset(str,0,sizeof(str));
			longToString(Deviation[i],str,12);//浮点型转换为字符串，小数点后3位
			//longToString(currentEPPulse[i],str,12);//浮点型转换为字符串，小数点后3位
			strcat(str," ");
			STM32_USART_SentData(str,strlen(str)+1);
		}
		STM32_USART_SentData(newline,strlen(newline)+1);
	 }
	 STM32_USART_SentData(newline,strlen(newline)+1);
*/
	 //脉冲输出
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
		  if(*(differPulse+i) >= 0)
		  {
			   SetPulse(0x01<<i,*(differPulse+i));
		  }
		  else
		  {
			   *(differPulse+i) = -(*(differPulse+i));//取反加1，负变正
			   SetPulse(0x01<<i,*(differPulse+i));
			   t=t|(0x01<<i);//记录哪个轴是负的  bit0~5   1：负的  0: 正的
		  }
	 }
	 max=*differPulse;
	 for(i=1;i<ROBOT_PARAMETER.AXIS_NUM;i++)//获得最大值
	 {
		  if(*(differPulse+i) > max)
		  {
			   max = *(differPulse+i);
		  }
	 }
	 speed = GetProgramRunSpeed();

	 //直线运动中，如果运动到接近极限位置时，电机转速会过快，切换到关节插补

	 if(max > 5000)	mode = 0;
	 if(mode == 0)
	 {
		  for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
		  {
			 R[i]=(float)(*(differPulse+i))/max;//max最大值
		  }
		  for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
		  {
			  s_current_speed[i]= R[i]*80*speed;//根据脉冲数来设定驱动频率，让六轴联动起来，六个轴以相同时间走完相同的脉冲数
									 //跟下面的方法不一样，因为如果脉冲数过大的时候，在短时间内走完速度会过快
			 if(s_current_speed[i]<1)
			 {
				 s_current_speed[i]=1;
			 }else if(s_current_speed[i]>8000)
			 {
				 s_current_speed[i]=8000;//防止速度过大
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
			   if(freq[i]>8000)//防止过大或过小
			   {
					freq[i]=8000;
			   }
			   else if(freq[i]<1)
			   {
					freq[i]=1;
			   }
			   //根据脉冲数来设定速度，单次运动中以相同时间（例如20ms）走完相同的路程（例如0.2mm）
			   SetSpeed(0x01<<i,freq[i]);//设置速度
		  }
	 }
	 //开始驱动
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
		  if((t>>i)&0x01)//如果是负的 （J要逆时针转）
		  {
			   SetDriCmd(0x01<<i,0x21);//负方向定量驱动(电机逆时针旋转)
		  }
		  else//如果是正的 （J要顺时针转）
		  {
			   SetDriCmd(0x01<<i,0x20);//正方向定量驱动
		  }
	 }
}
/*****************************************
 * 功能：弧度转换成脉冲
 * 参数：
 * 		angle:角度
 * 		pulse:转换成的脉冲
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
 * 功能：脉冲转换成弧度
 * 参数：
 * 		pulse:传进去要转化的脉冲
 * 		angle:保存转换成的弧度
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
 * 函数功能：回原点
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
	 //ReadCurrentPulseEp(currentPulse);//获得当前的位置的脉冲数（实际值）
	 ReadCurrentPulseLp(currentPulse);//修正于//2018/06/01
	 for(i=0;i<6;i++)
	 {
		  differPulse[i] =  movedPulse[i] - currentPulse[i];
	 }
	 if(speed>15) speed =15;
	 ModifyAllSpeed(speed);
	 mode_status=Teach_mode;//修正于//2018/06/01
	 Teach_mode=0;//修正于//2018/06/01
	 TranslationProccess(differPulse,0);
	 Teach_mode=mode_status;//修正于//2018/06/01
}
/****************************************
 * 函数功能：原点清零/原点锁定
 * 输入参数：NONE
 * 输出参数：bit0:0->清零成功 1->清零失败
 * 			bit1:0->保存零点偏移失败 1->保存成功
 */
u8 ClearOrigin(void)
{
	 long pulse;
	 u8 i,err=0,err1;
	 //u16 times;

	 if(ROBOT_PARAMETER.MOTOR_TYPE == 1)//三菱电机原点清零
	 {
		 WriteWR4(0xFFC0);          //CR : U0=1
		 delay_1ms(200);
		 WriteWR4(0xEFC0);          //CR : U0=0
		 delay_200ns();delay_200ns();
	 }
	 else// if(ROBOT_PARAMETER.MOTOR_TYPE == 2)//三协电机原点清零
	 {
		 ServoOFF();	//关伺服
		 delay_1ms(200);
		 //发送对电机编码器清零的命令
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
//						GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485发送模式
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
//					 GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485发送模式
//					 err = 1;	//超时 300ms
//					 break;
//				 }
//				 delay_1us(100);
//				 times++;
//			 }
		 }
		 if(err) return 1;
		 //GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485发送模式
		 //三协伺服，对他进行编码器清零是需要重新启动才生效的
		 ROBOT_PARAMETER.ENCODER_LEFT_SAV = 1;	//标记需要保存原点
		 err1 = SaveRobotPara(ROBOT_PARAMETER);//保存机械手参数
		 err |= (err1<<1);
	 }

	 for(i=0;i<6;i++)
	 {
		 pulse = ROBOT_PARAMETER.RATIO[i]*ROBOT_PARAMETER.MOTOR_PRECISION*(ROBOT_PARAMETER.ORIGIN_POSITION[i]/360.0);
	     SetLp(0x01<<i,pulse);
	     SetEp(0x01<<i,pulse);
	 }

     if(ROBOT_PARAMETER.MOTOR_TYPE == 2)	//三协电机的原点清零需要重启生效
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
 * 函数功能：解除报警
 */
void ClearAlarm(void)
{
	 ClearServoAlarm();//清除伺服报警
	 SetDriCmd(0x3f,0x27);//清除急停报警
}
//清除报警错误信息
void ClearAlarmInfo(void)
{
	 G_CTRL_BUTTON.CTRL_BUTTON.EMG_FLG = 0;
	 G_CTRL_BUTTON.CTRL_BUTTON.NEG_LIMITED_FLG = 0;
	 G_CTRL_BUTTON.CTRL_BUTTON.POS_LIMITED_FLG = 0;
	 G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG=0;//标记无报警
}
/****************************************
 * 函数功能：获得伺服的脉冲
 * 输入值： mode=0,更新脉冲数据
 * 			mode=1,强制更新脉冲数据，不管读取数据是否正确
 * 返回值： 0：成功
 *  		非0情况下：bit0~bit5的1分别代表哪个伺服读取数据出错，可能是相应的伺服出错
 *  		bit6
 */
unsigned char UpdatePulseFromServo(void)
{
	 unsigned char i=0,err;
	 long pulse[6]={0};

	 err = ReadEncoderPulseAll(pulse);//编码器脉冲其实就是实际脉冲Ep（只要第二轴减上600000的话），
										 //所以得来的数据就直接赋给Ep，然后J3、J5取反就给Lp
	 if(err & ROBOT_PARAMETER.MOTOR_SELECT)
	 {
		  return err;//发生错误
	 }

	 if(ROBOT_PARAMETER.MOTOR_TYPE == 2)
	 {
		 //三协电机需要减去零点偏移量才是真正的零点
		 for(i=0;i<6;i++)
		 {
			 pulse[i] = pulse[i] - ROBOT_PARAMETER.ENCODER_LEFT[i];
		 }
	 }

	 //伺服放大器如果设置了反方向旋转的话，读回来的脉冲是相反数
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
	 //减去零点状态,得到的脉冲才是实际的位置
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
 * 函数功能：获得伺服的脉冲
 * 输入值： mode=0,更新脉冲数据
 * 			mode=1,强制更新脉冲数据，不管读取数据是否正确
 * 返回值： 0：成功
 *  		非0情况下：bit0~bit5的1分别代表哪个伺服读取数据出错，可能是相应的伺服出错
 *  		bit6
 */
unsigned char UpdatePulseFromServoForEp(long *pulse)
{
	 unsigned char i=0,err;
	 //long pulse[6]={0};

	 err = ReadEncoderPulseAll(pulse);//编码器脉冲其实就是实际脉冲Ep（只要第二轴减上600000的话），
										 //所以得来的数据就直接赋给Ep，然后J3、J5取反就给Lp
	 if(err & ROBOT_PARAMETER.MOTOR_SELECT)
	 {
		  return err;//发生错误
	 }

	 if(ROBOT_PARAMETER.MOTOR_TYPE == 2)
	 {
		 //三协电机需要减去零点偏移量才是真正的零点
		 for(i=0;i<6;i++)
		 {
			 pulse[i] = pulse[i] - ROBOT_PARAMETER.ENCODER_LEFT[i];
		 }
	 }

	 //伺服放大器如果设置了反方向旋转的话，读回来的脉冲是相反数
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
	 //减去零点状态,得到的脉冲才是实际的位置
	 for(i=0;i<6;i++)
	 {
		 pulse[i] = pulse[i] + ROBOT_PARAMETER.RATIO[i]*ROBOT_PARAMETER.MOTOR_PRECISION*
				 	(ROBOT_PARAMETER.ORIGIN_POSITION[i]/360.0);
	 }
	 return 0;
}
/********************************************
 * 伺服开
 * 输入参数：mode=0，更新脉冲数据正确后开伺服on，
 * 			 mode=1，强制开伺服on，不管脉冲数据读取数据是否正确(更新脉冲数据)
 * 返回值：	 err:  bit0~bit5为1，相应的轴读取脉冲错误
 */
unsigned char ServoON(unsigned char mode)
{
	 unsigned char err;
	 if(G_CTRL_BUTTON.CTRL_BUTTON.SERVO_ON == 0)//如果伺服已经打开
	 {
		 SetScreen(20);
		 SetTextValue(20, 4, "");
		 SetTextValue(20, 3, "Servo is turned on!");
		 delay_1ms(1100);
		 return 0;
	 }
	 ServoOFF();	//关掉伺服再读取
	 err = UpdatePulseFromServo();//读取编码器脉冲数据
	 G_CTRL_BUTTON.CTRL_BUTTON.ORIGIN_FLG = 0;
	 if(err)
	 {
		  if(mode==0)	//如果不是强制打开伺服
		  {
			   ServoON_OFF(0x3f);
			   delay_1ms(10);
			   G_CTRL_BUTTON.CTRL_BUTTON.ORIGIN_FLG = 1;
			   return err;
		  }
	 }
	 ServoON_OFF(0);	//开伺服
	 delay_1ms(100);
	 OpenBrake(0);	//打开1~3轴的刹车
	 OpenBrake(1);
	 OpenBrake(2);
	 delay_1ms(100);
	 G_CTRL_BUTTON.CTRL_BUTTON.SERVO_ON = 0;//伺服打开 标志位
	 SetButtonValue(2, 13, 1);//更新伺服开关按钮，设为打开状态
	 SetButtonValue(44, 13, 1);
	 return err;
}
/********************************************
 * 伺服关
 */
void ServoOFF(void)
{
	 CloseBrake(0);//关闭1~3轴的刹车
	 CloseBrake(1);
	 CloseBrake(2);
	 delay_1ms(200);
	 ServoON_OFF(0x3f);
	 delay_1ms(10);
	 G_CTRL_BUTTON.CTRL_BUTTON.SERVO_ON = 1;//伺服关闭标志位
	 SetButtonValue(2, 13, 0);//更新伺服开关按钮，设为关闭状态
	 SetButtonValue(44, 13, 0);
}
/********************************************
 * 控制伺服开关，bit0~bit5分别代表J1~J6,0开伺服，1关伺服
 */
void ServoON_OFF(unsigned int state)
{
	 state &= 0x3F;
	 s_WR4state = (s_WR4state&0xFFC0) | state;
	 WriteWR4(s_WR4state);
}
/********************************************
 * 控制单个伺服ON的开关，
 * Axis：输入范围1~6，代表J1~J6
 * state：0开伺服，1关伺服
 */
void ServoON_OFF_Single(unsigned char axis,unsigned char state)
{
	 s_WR4state = (s_WR4state&(~(0x0001<<(axis-1))));//清零第（axis-1）位
	 s_WR4state = s_WR4state | (state<<(axis-1));
	 WriteWR4(s_WR4state);
}
/*******************************************
 * 清除伺服报警
 */
void ClearServoAlarm(void)
{
	 s_WR4state = s_WR4state & 0xF03F;//清零bit6~bit11位
	 WriteWR4(s_WR4state);
	 delay_1ms(55);
	 s_WR4state = s_WR4state | 0x0FC0;
	 WriteWR4(s_WR4state);
	 delay_1ms(50);
}

/********************************************
 * 函数功能：检测触摸屏连接状态并修复
 * 输入参数：NONE
 * 输出参数：NONE
 */
void CheckScreenConnectState(void)
{
	 unsigned char screen_connect_state = 0;	// 0:触摸屏已连接	1:已断开
	 static unsigned char s_ScreenConnectState = 0;	// 0:触摸屏已连接	1:已断开
	 static unsigned int s_num=0;
	 static unsigned char s_flag=0;			//0:计时结束	1：计时开始
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
				 s_num = 8000;//程序正在运行
			 }
			 else
			 {
				 s_num = 16000;//程序停止运行
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
			 InitSci();//触摸屏重新连接后要对DSP的串口初始化，否则DSP无法接收数据，原因未知
			 delay_1us(50);
			 SetCurrentScreen();
		 }
	 }
	 s_num--;
}

/********************************************
 * 检查报警
 * 输入参数：screen_id,发生报警时，如果id=0则返回当前页面，其他值时返回其他值的页面
 * 返回值 err 0：无报警	其他值：有报警
 */
unsigned char ErrCheck(unsigned char screen_id)
{
	 static u8 s_ALM_STATE = 0;	//记录报警和急停状态
	 unsigned char i,err,err2=0;
	 CheckScreenConnectState();
	 delay_1us(7);
	 err=ReadErrState();//bit0~bit5:J1~6,读错误状态
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
			   s_ALM_STATE = 0;	//清零
		   }
	 }

	 if(err)//
	 {
		   SetDriCmd(0x3F,0x27);
		   ProgramRuningFlag(3);//如果是程序运行中就停止程序运行
		   for(i=0;i<6;i++)
		   {
				if(err&(0x01<<i))//检查哪个轴有错误
				{
					 delay_200ns();
					 err2 = ReadRR2(0x01<<i);//读详细错误信息

					 //if((err2&0x10) == 0) G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG=0;
					 //else G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG=1;

					 if((err2&0x20) == 0) G_CTRL_BUTTON.CTRL_BUTTON.EMG_FLG=0;//就算已经有报警或限位，按急停也能显示

					 if(G_CTRL_BUTTON.all&0x38)//如果已经有急停或限位
					 {
						  if(err2&0x10)//如果是伺服报警
							   break;//则退出
					 }
					 else
					 {
						  break;//如果之前没有急停限位报警，则直接退出
					 }
				}
		   }
		   if((err2 & 0x10) && !G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG)//如果之前没有报警
		   {
			   s_ALM_STATE = 1;	//标记急停
			    AlarmOutput();	//亮红灯
			    ServoON_OFF(0x3f);
				ServoOFF();
				G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG=1;//标记有报警
				G_CTRL_BUTTON.CTRL_BUTTON.EMG_FLG=1;
				display_servo_alarm(i+1,err2,screen_id);//弹出窗口哪轴发生什么错误
		   }
		   else if((err2&0x20) && (!G_CTRL_BUTTON.CTRL_BUTTON.EMG_FLG))//如果之前没有急停
		   {
			   s_ALM_STATE = 1;	//标记报警
			   	AlarmOutput();	//亮红灯
				G_CTRL_BUTTON.CTRL_BUTTON.EMG_FLG=1;
				err2 = err2 & (~0x10);//屏蔽掉报警，否则只会显示报警，不显示急停
				display_servo_alarm(i+1,err2,screen_id);
		   }
		   else if((err2&0x01) && (!G_CTRL_BUTTON.CTRL_BUTTON.POS_LIMITED_FLG))//如果之前没有限位
		   {
				G_CTRL_BUTTON.CTRL_BUTTON.POS_LIMITED_FLG=1;//正限位
				G_CTRL_BUTTON.CTRL_BUTTON.NEG_LIMITED_FLG=0;//清零负限位
				display_servo_alarm(i+1,err2,screen_id);
		   }
		   else if((err2&0x02) && (!G_CTRL_BUTTON.CTRL_BUTTON.NEG_LIMITED_FLG))//如果之前没有限位
		   {
				G_CTRL_BUTTON.CTRL_BUTTON.NEG_LIMITED_FLG=1;//负限位
				G_CTRL_BUTTON.CTRL_BUTTON.POS_LIMITED_FLG=0;//正限位
				display_servo_alarm(i+1,err2,screen_id);
		   }
		   SetDriCmd(0x3F,0x25);
	 }
	 return err;
}

/************************************
 * 函数功能：打开蜂鸣器
 */
void OpenBuzzer(void)
{
	ENABLE_SPEAKER;
}

/************************************
 * 函数功能：关闭蜂鸣器
 */
void CloseBuzzer(void)
{
	DISABLE_SPEAKER;
}

/**************************************
 * 函数功能：读安全开关状态
 * 返回值：	 0：安全开关已按下
 * 			 1：安全开关未按下
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
 * 函数功能：设置伺服报警和限位的有效/无效
 * 输入参数：flag
 * 			 bit0：	0:伺服报警功能无效		1:伺服报警功能有效
 * 			 bit1：	0:限位功能无效			1:限位功能有效
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
 * 函数功能：伺服报警功能无效
 */
void DisableServoAlarm(void)
{
	s_WR2state = s_WR2state & 0xcfff;
	WriteWR2(0x3F,s_WR2state);
	G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG = 0;//清除报警标记位
}

/************************************
 * 函数功能：伺服报警功能有效
 */
void EnableServoAlarm(void)
{
	s_WR2state = s_WR2state | 0x3000;
	WriteWR2(0x3F,s_WR2state);
}

/************************************
 * 函数功能：限位功能无效
 */
void DisableLimited(void)
{
	s_WR2state = s_WR2state & 0xfffc;
	WriteWR2(0x3F,s_WR2state);
}

/************************************
 * 函数功能：限位功能有效
 */
void EnableLimited(void)
{
	s_WR2state = s_WR2state | 0x0003;
	WriteWR2(0x3F,s_WR2state);
}

/************************************
 * 函数功能：设置为高速示教模式
 */
void SetSpeedMode_High(void)
{
	s_SpeedMode = 1;
}

/************************************
 * 函数功能：设置为低速示教模式
 */
void SetSpeedMode_Low(void)
{
	s_SpeedMode = 0;
}

/************************************
 * 函数功能：获得示教速度模式
 * 返回值：0->低速示教模式
 * 		   1->高速示教模式
 */
unsigned char GetSpeedMode(void)
{
	return s_SpeedMode;
}
/************************************
 * 函数功能：开发者模式：开
 */
void DeveloperMode_On(void)
{
	//RecoverSpeed();
}

/************************************
 * 函数功能：开发者模式：关
 */
void DeveloperMode_Off(void)
{
	SetSpeedMode_Low();
	EnableLimited();
	delay_1ms(2);
	EnableServoAlarm();
	Teach_mode=0;//修正于//2018/06/01;
}
/************************************
 * 函数功能：恢复MCX31x的WR2寄存器
 * 由于干扰或不稳定，导致WR2寄存器会被清零，导致报警和限位
 * 无效，所以，此函数用在动作机械手之前，先恢复WR2寄存器。
 */
void RecoverWR2(void)
{
	WriteWR2(0x3F,s_WR2state);
}
/************************************
 * 函数功能：获取WR2寄存器的值
 ********WR2模式寄存器*****
 *bit0,1 正负限位有效无效 0无效  1有效
 *bit5   COMP+/-寄存器比较对象  0：逻辑   1：实位
 *bit12  nALARM输入信号的逻辑电平，0：低电平有效  1：高电平有效
 *bit13  nALARM   0:无效  1：有效
 */
unsigned int GetWR2state(void)
{
	return s_WR2state;
}
/************************************
 * 函数功能：读当前警报/错误信息
 * 返回值：	bit0=1，伺服on没打开
 * 			bit1=1，有警报
 * 			bit2=1，无原点或原点出错
 * 			bit3=1，急停中
 * 			bit4=1, +限位中
 * 			bit5=1, -限位中
 * 			bit6=1, 安全开关未按下
 * 			bit7=1, 1轴刹车未打开
 * 			bit8=1, 2轴刹车未打开
 * 			bit9=1, 3轴刹车未打开
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
 * 函数功能：读机械手参数
 * 输入参数：指向robot_para的指针
 * 返回值：	0->读取成功	1->读取失败  2->需要重新初始化设定机械手参数
 */
unsigned char ReadRobotPara(RobotParaStru* robot_para)
{
	unsigned char addr;
	u16 crc16;
	RobotParaStru_CRC robot_pata_crc;
	addr = INFO_BLOCK_ADDR1;
	ReadNandFlashData(addr,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));
	if(robot_pata_crc.RobotPara.BAD == 0)//INFO_BLOCK_ADDR1是坏块
	{
		addr = INFO_BLOCK_ADDR2;
	}
	if(robot_pata_crc.RobotPara.FIRST == 1)
	{
		*robot_para = robot_pata_crc.RobotPara;
		return 2;	//参数还未设定
	}

	crc16 = crc_cal_by_bit((u8*)(&(robot_pata_crc.RobotPara)),sizeof(robot_pata_crc.RobotPara));//校验计算
	if(crc16 == robot_pata_crc.CRC16)
	{
		if(robot_pata_crc.RobotPara.HARDWARE_REV == 0)
			robot_pata_crc.RobotPara.HARDWARE_REV = 8;	//如果是之前老版本的，这个值默认是0，现在改为默认都是8
		*robot_para = robot_pata_crc.RobotPara;
		return 0;
	}
	//读取失败时再读取一次
	ReadNandFlashData(addr,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));
	crc16 = crc_cal_by_bit((u8*)(&(robot_pata_crc.RobotPara)),sizeof(robot_pata_crc.RobotPara));//校验计算

	if(robot_pata_crc.RobotPara.HARDWARE_REV == 0)
		robot_pata_crc.RobotPara.HARDWARE_REV = 8;	//如果是之前老版本的，这个值默认是0，现在改为默认都是8
	*robot_para = robot_pata_crc.RobotPara;

	if(crc16 == robot_pata_crc.CRC16)
	{
		//*robot_para = robot_pata_crc.RobotPara;
		return 0;
	}
	return 1;
}

/************************************
 * 函数功能：保存机械手参数
 * 返回值：	0->保存成功	1->保存失败
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

//	if(err == 0)//读取成功
//	{
		if(robot_pata_crc.RobotPara.BAD == 0)//如果INFO_BLOCK_ADDR1是坏块
		{
			addr = INFO_BLOCK_ADDR2;
		}
//	}
	//校验计算
	crc16 = crc_cal_by_bit((u8*)(&robot_para),sizeof(RobotParaStru));
	robot_pata_crc.CRC16 = crc16;
	robot_pata_crc.RobotPara = robot_para;
	robot_pata_crc.RobotPara.FIRST = 0;	//标记为已初始化过
	robot_pata_crc.RobotPara.BAD = 1;	//标记为好块
	err  = EraseNandFlashBlock(addr);	//擦除信息块
	err |= WriteNandFlashData(addr,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));
	//保存失败时，再保存一次
	if(err)
	{
		err  = EraseNandFlashBlock(addr);	//擦除信息块
		err |= WriteNandFlashData(addr,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));
		if(err)
		{
			if(addr == INFO_BLOCK_ADDR1)
			{

				err  = EraseNandFlashBlock(addr);	//擦除信息块1
				robot_pata_crc.RobotPara.BAD = 0;	//标记为坏块
				err |= WriteNandFlashData(addr,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));

				robot_pata_crc.RobotPara.BAD = 1;	//标记为坏块
				err  = EraseNandFlashBlock(INFO_BLOCK_ADDR2);	//擦除信息块2
				err |= WriteNandFlashData(INFO_BLOCK_ADDR2,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));
				if(err)
				{
					err  = EraseNandFlashBlock(INFO_BLOCK_ADDR2);	//擦除信息块2
					err |= WriteNandFlashData(INFO_BLOCK_ADDR2,0,0,sizeof(robot_pata_crc),1,(u8*)(&robot_pata_crc));
				}
			}
		}
	}
	return err;
}

/************************************
 * 函数功能：获得stm32版本号
 * 输入参数:data,保存数据的指针，数据长度4字节
 * 返回值： 0->OK 1->跟STM32通信异常
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
 * 函数功能：获得stm32串口接收数据状态
 * 输入参数:data->如果串口有数据，那么这个指针指向接收到的数据
 * 返回值： 0->串口有接收到数据 1->串口无接收到数据 2->通信异常
 */
u8 GetSTM32USART_STA(char* data)
{
	u8 tx_cmd[5],rx_cmd[5],t=0,i;
	Uint32 data_len;

	memset(tx_cmd,0,sizeof(tx_cmd));
	tx_cmd[0] = R_CMD_USART_RX_STA;

	while(1)
	{
		SpiSendCmd(tx_cmd);	//发送查询命令
		delay_1ms(1);
		if(SpiReceiveCmd(rx_cmd) == 0)
		{
			if(rx_cmd[0] == R_CMD_USART_RX_OK)
			{
				data_len = 0;
				for(i=0;i<4;i++)
				{
					data_len |= (Uint32)rx_cmd[i+1]<<(i*8);//数据长度
				}

				if(SpiReceiveData(data_len,(u8*)data,0))
				{
					delay_1ms(100);//等待stm32退出接收模式
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
 * 函数功能：读取通过STM32串口接收到的数据
 * 输入参数:data:最多32字节长度
 * 返回值： 0->接收成功 1->接收失败 2->通信异常
 */
//u8 Read_STM32_USART_Data(char* data)
//{
//	u8 tx_cmd[5],rx_cmd[5],i;
//	Uint32 data_len;
//
////	memset(tx_cmd,0,sizeof(tx_cmd));
////	tx_cmd[0] = R_CMD_USART;
////	SpiSendCmd(tx_cmd);	//发送读取数据命令
////	delay_1ms(1);
////	if(SpiReceiveCmd(rx_cmd))
////	{
////		delay_1ms(100);//等待stm32退出接收模式
////		return 2;
////	}
//	//开始接收数据
//	data_len = 0;
//	for(i=0;i<4;i++)
//	{
//		data_len |= (Uint32)rx_cmd[i+1]<<(i*8);//数据长度
//	}
//	if(SpiReceiveData(data_len,data,0))
//	{
//		delay_1ms(100);//等待stm32退出接收模式
//		return 2;
//	}
//	return 0;
//}

/************************************
 * 函数功能：通过STM32串口发送数据
 * 输入参数:NONE
 * 返回值： 0->发送数据成功 1->发送数据失败 2->通信异常
 */
u8 STM32_USART_SentData(char* data,u16 len)
{
	unsigned char rx_cmd[5],tx_cmd[5],i,t=0;
	//int dly=0;
	//dly = 1/(CURRENT_BAUD*0.1)*10000000;	//波特率越小，需要延时的时间越长    (延时增加 *10)
	memset(tx_cmd,0,sizeof(tx_cmd));
	tx_cmd[0] = T_CMD_USART;
	for(i=0;i<4;i++)
	{
		tx_cmd[i+1] = len>>(i*8)&0x000000ff;
	}
	while(1)
	{
		SpiSendCmd(tx_cmd);	//发送需要发送的数据的长度
		delay_1us(10);
		SpiSendData(len,(u8*)data,0);//发送需要发送的数据
		delay_1us(100);
		for(i=0;i<len;i++)	//发送数据越多，需要延时的时间越长
		{
			DELAY_US(900);
		}
		delay_1us(1000);                      //
		if(SpiReceiveCmd(rx_cmd) == 0)	//读对方接收状态
		{
			if(rx_cmd[0] == TR_CMD_OK)
			{
				return 0;
			}
			else
			{
				delay_1us(100);
				if(SpiReceiveCmd(rx_cmd) == 0)	//读对方接收状态
				{
					if(rx_cmd[0] == TR_CMD_OK)
					{
						return 0;
					}
				}
			}
			//return 0;
//			else	//对方接收出错
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
