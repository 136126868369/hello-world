#include "My_Project.h"
#include "Man_MCX31xAS.h"
#include "ScreenApi.h"//2018/5/25添加
#include "kinematic_explain.h"
#include "ReadEncoder.h"//添加于//2018/06/01
unsigned int *Ex6Start = (unsigned  int *)0x100000;
unsigned int *Ex7Start = (unsigned  int *)0x200000;
extern  unsigned char Teach_mode;//修正于//2018/06/01
//****************************WR1模式寄存器*********************************/
void WriteWR1(unsigned int axis,unsigned int wdata)
{
     if(axis&0x0F)
     {
          *(Ex7Start+WR0) = ((axis&0x0f)<<8)+0xf;
          *(Ex7Start+WR1) = wdata;
     }
     if(axis&0x30)
     {
          *(Ex6Start+WR0) = ((axis&0x30)<<4)+0xf;
          *(Ex6Start+WR1) = wdata;
     }
}
//****************************WR2模式寄存器**********************************/
//bit0,1 正负限位有效无效 0无效  1有效
//bit5   COMP+/-寄存器比较对象  0：逻辑   1：实位
//bit12  nALARM输入信号的逻辑电平，0：低电平有效  1：高电平有效
//bit13  nALARM   0:无效  1：有效
void WriteWR2(unsigned int axis,unsigned int wdata)
{
     if(axis&0x0F)
     {
			if(ROBOT_PARAMETER.SYS==FOUR_AXIS_B || ROBOT_PARAMETER.SYS==FOUR_AXIS_C)
			{
				  *(Ex7Start+WR0) = ((axis&0x0B)<<8)+0xf;
				  NOP;NOP;NOP;NOP;
				  *(Ex7Start+WR2) = wdata;
				  NOP;NOP;NOP;NOP;

				  *(Ex7Start+WR0) = ((axis&0x04)<<8)+0xf;
				  NOP;NOP;NOP;NOP;
				  *(Ex7Start+WR2) = wdata&0xfffc;	//第三轴不限位
				  NOP;NOP;NOP;NOP;

//				WriteWR2(0x3B,s_WR2state);
//				WriteWR2(0x04,s_WR2state&0xfffc);
			}
			else
			{
				  *(Ex7Start+WR0) = ((axis&0x0f)<<8)+0xf;
				  NOP;NOP;NOP;NOP;
				  *(Ex7Start+WR2) = wdata;
				  NOP;NOP;NOP;NOP;

			}
     }
     if(axis&0x30)
     {
    	  NOP;NOP;NOP;NOP;
          *(Ex6Start+WR0) = ((axis&0x30)<<4)+0xf;
          NOP;NOP;NOP;NOP;
          *(Ex6Start+WR2) = wdata;
     }
}
//****************************WR3模式寄存器********************************/
void WriteWR3(unsigned int axis,unsigned int wdata)
{
     if(axis&0x0F)
     {
          *(Ex7Start+WR0) = ((axis&0x0f)<<8)+0xf;
          *(Ex7Start+WR3) = wdata;
     }
     if(axis&0x30)
     {
          *(Ex6Start+WR0) = ((axis&0x30)<<4)+0xf;
          *(Ex6Start+WR3) = wdata;
     }
}
//***************************mcx314 WR4输出寄存器*********************************/
void WriteWR4(unsigned int wdata)
{
     *(Ex7Start+WR4) = wdata;
}
/****************************mcx312 WR4输出寄存器*******************************
* D15~D12:UOUT3~UOUT0,D11~D8:ZOUT3~ZOUT0,D7~D4:YOUT3~YOUT0,D3~D0:XOUT3~XOUT0
*/
void WriteMCX312_WR4(unsigned int wdata)
{
	NOP;NOP;
    *(Ex6Start+WR4) = wdata;
    NOP;NOP;
}
//***************************插补模式寄存器*********************************/
void WriteWR5(unsigned int axis,unsigned int wdata)
{
     if(axis&0x0F)
     {
          *(Ex7Start+WR0) = ((axis&0x0f)<<8)+0xf;
          *(Ex7Start+WR5) = wdata;
     }
     if(axis&0x30)
     {
          *(Ex6Start+WR0) = ((axis&0x30)<<4)+0xf;
          *(Ex6Start+WR5) = wdata;
     }
}
//******************************设置R倍率**************************/
void SetRange(unsigned int axis,long wdata)
{
	 if(axis&0x0F)
	 {
	      *(Ex7Start+WR7) = (wdata>>16)&0xFFFF;
	      *(Ex7Start+WR6) = wdata&0xFFFF;
	      *(Ex7Start+WR0) = ((axis&0x0F)<<8)+0x0;
	 }
	 if(axis&0x30)
	 {
	      *(Ex6Start+WR7) = (wdata>>16)&0xFFFF;
	      *(Ex6Start+WR6) = wdata&0xFFFF;
	      *(Ex6Start+WR0) = ((axis&0x30)<<4)+0x0;
	 }
}
//******************设置加/减速度变化率acac***************/
void SetAcac(unsigned int axis,unsigned int wdata)
{
	 if(axis&0x0F)
	 {
	      *(Ex7Start+WR6) = wdata;
	      *(Ex7Start+WR0) = ((axis&0x0F)<<8)+0x1;
	 }
	 if(axis&0x30)
	 {
	      *(Ex6Start+WR6) = wdata;
	      *(Ex6Start+WR0) = ((axis&0x30)<<4)+0x1;
	 }
}
//******************设置加速度acc***************************/
void SetAcc(unsigned int axis,unsigned int wdata)
{
	 if(axis&0x0F)
	 {
	      *(Ex7Start+WR6) = wdata;
	      *(Ex7Start+WR0) = ((axis&0x0F)<<8)+0x2;
	 }
	 if(axis&0x30)
	 {
	      *(Ex6Start+WR6) = wdata;
	      *(Ex6Start+WR0) = ((axis&0x30)<<4)+0x2;
	 }
}
//********************设置减速度dec**************************/
void SetDec(unsigned int axis,unsigned int wdata)
{
	 if(axis&0x0F)
	 {
	      *(Ex7Start+WR6) = wdata;
	      *(Ex7Start+WR0) = ((axis&0x0F)<<8)+0x3;
	 }
	 if(axis&0x30)
	 {
	      *(Ex6Start+WR6) = wdata;
	      *(Ex6Start+WR0) = ((axis&0x30)<<4)+0x3;
	 }
}
//******************设置初速度StartV*************************/
void SetStartV(unsigned int axis,unsigned int wdata)
{
	 if(axis&0x0F)
	 {
	      *(Ex7Start+WR6) = wdata;
	      *(Ex7Start+WR0) = ((axis&0x0F)<<8)+0x4;
	 }
	 if(axis&0x30)
	 {
	      *(Ex6Start+WR6) = wdata;
	      *(Ex6Start+WR0) = ((axis&0x30)<<4)+0x4;
	 }
}
/*******************设置速度脉冲输出频率*************************
*	驱动中可以随便变更驱动速度
*/
void SetPulseFreq(unsigned int axis,unsigned int wdata)
{
	 if(axis&0x0F)
	 {
	      *(Ex7Start+WR6) = wdata;
	      *(Ex7Start+WR0) = ((axis&0x0F)<<8)+0x5;
	 }
	 if(axis&0x30)
	 {
	      *(Ex6Start+WR6) = wdata;
	      *(Ex6Start+WR0) = ((axis&0x30)<<4)+0x5;
	 }

}
//********************设置输出脉冲个数****************************/
void SetPulse(unsigned int axis,long wdata)
{
	 if(axis&0x0F)
	 {
	      *(Ex7Start+WR7) = (wdata>>16)&0xFFFF;
	      NOP;NOP;
	      *(Ex7Start+WR6) = wdata&0xFFFF;
	      NOP;NOP;
	      *(Ex7Start+WR0) = ((axis&0x0F)<<8)+0x6;
	      NOP;NOP;
	 }
	 if(axis&0x30)
	 {
	      *(Ex6Start+WR7) = (wdata>>16)&0xFFFF;
	      NOP;NOP;
	      *(Ex6Start+WR6) = wdata&0xFFFF;
	      NOP;NOP;
	      *(Ex6Start+WR0) = ((axis&0x30)<<4)+0x6;
	 }
}
//************************设置逻辑位置计数器***********************/
void SetLp(unsigned int axis,long wdata)
{
	 if(axis&0x0F)
 	 {
 	      *(Ex7Start+WR7) = (wdata>>16)&0xFFFF;
 	      NOP;NOP;
 	      *(Ex7Start+WR6) = wdata&0xFFFF;
 	      NOP;NOP;
 	      *(Ex7Start+WR0) = ((axis&0x0F)<<8)+0x9;
 	      NOP;NOP;
 	 }
 	 if(axis&0x30)
 	 {
 	      *(Ex6Start+WR7) = (wdata>>16)&0xFFFF;
 	      NOP;NOP;
 	      *(Ex6Start+WR6) = wdata&0xFFFF;
 	      NOP;NOP;
 	      *(Ex6Start+WR0) = ((axis&0x30)<<4)+0x9;
 	 }
}
//*************************设置实际位置计数器***********************/
void SetEp(unsigned int axis,long wdata)
{
	 if(axis&0x0F)
 	 {
 	      *(Ex7Start+WR7) = (wdata>>16)&0xFFFF;
 	      NOP;NOP;
 	      *(Ex7Start+WR6) = wdata&0xFFFF;
 	      NOP;NOP;
 	      *(Ex7Start+WR0) = ((axis&0x0F)<<8)+0xa;
 	      NOP;NOP;
 	 }
 	 if(axis&0x30)
 	 {
 	      *(Ex6Start+WR7) = (wdata>>16)&0xFFFF;
 	      NOP;NOP;
 	      *(Ex6Start+WR6) = wdata&0xFFFF;
 	      NOP;NOP;
 	      *(Ex6Start+WR0) = ((axis&0x30)<<4)+0xa;
 	 }
}
//***********************设置Compp+比较寄存器***********************/
void SetCompp(unsigned int axis,long wdata)
{
	 if(axis&0x0F)
 	 {
 	      *(Ex7Start+WR7) = (wdata>>16)&0xFFFF;
 	      NOP;NOP;
 	      *(Ex7Start+WR6) = wdata&0xFFFF;
 	      NOP;NOP;
 	      *(Ex7Start+WR0) = ((axis&0x0F)<<8)+0xb;
 	      NOP;NOP;
 	 }
 	 if(axis&0x30)
 	 {
 	      *(Ex6Start+WR7) = (wdata>>16)&0xFFFF;
 	      NOP;NOP;
 	      *(Ex6Start+WR6) = wdata&0xFFFF;
 	      NOP;NOP;
 	      *(Ex6Start+WR0) = ((axis&0x30)<<4)+0xb;
 	 }
}
//***********************设置Compm-比较寄存器**********************/
void SetCompm(unsigned int axis,long wdata)
{
	 if(axis&0x0F)
 	 {
 	      *(Ex7Start+WR7) = (wdata>>16)&0xFFFF;
 	      NOP;NOP;
 	      *(Ex7Start+WR6) = wdata&0xFFFF;
 	      NOP;NOP;
 	      *(Ex7Start+WR0) = ((axis&0x0F)<<8)+0xc;
 	      NOP;NOP;
 	 }
 	 if(axis&0x30)
 	 {
 	      *(Ex6Start+WR7) = (wdata>>16)&0xFFFF;
 	      NOP;NOP;
 	      *(Ex6Start+WR6) = wdata&0xFFFF;
 	      NOP;NOP;
 	      *(Ex6Start+WR0) = ((axis&0x30)<<4)+0xc;
 	 }
}
//*******************设置加速计数器偏移Ao*******************************/
void SetAo(unsigned int axis,long wdata)
{
	 if(axis&0x0F)
 	 {
 	      *(Ex7Start+WR7) = (wdata>>16)&0xFFFF;
 	      *(Ex7Start+WR6) = wdata&0xFFFF;
 	      *(Ex7Start+WR0) = ((axis&0x0F)<<8)+0xd;
 	 }
 	 if(axis&0x30)
 	 {
 	      *(Ex6Start+WR7) = (wdata>>16)&0xFFFF;
 	      *(Ex6Start+WR6) = wdata&0xFFFF;
 	      *(Ex6Start+WR0) = ((axis&0x30)<<4)+0xd;
 	 }
}

//**********************************驱动命令*******************************/
/*axis:	轴指定
 *cmd: 0x20:正方向定量驱动		0x21:负方向定量驱动
 *     0x22:正方向连续驱动		0x23:负方向连续驱动
 * 	   0x24:暂停驱动			0x25:解除暂停驱动状态/清除结束状态
 * 	   0x26:驱动减速停止		0x27:驱动立即停止
 * 	   驱动命令的命令处理时间最多需250nSEC（CLK=16MHz），请在命令处理结束后写入下一个命令。
 */
void SetDriCmd(unsigned int axis,unsigned int cmd)
{
	 if((axis&0x0F))
	 *(Ex7Start+WR0) = ((axis&0x0F)<<8)+cmd;
	 if((axis&0x30))
	 *(Ex6Start+WR0) = ((axis&0x30)<<4)+cmd;
}
/*******************************************************************
 *                                                                 *
 *                             读								   *
 *                                                                 *
 *******************************************************************/

//***************************读RR1状态寄存器（轴状态信息）*****************************/
unsigned int ReadRR1(unsigned int axis)
{
	 unsigned int RDdata=0;
	 if(axis&0x0F)
	 {
		     *(Ex7Start+WR0) = ((axis&0x0f)<<8)+0xf;
			 MCX_DIR = 0;
			 NOP;NOP;
			 RDdata = *(Ex7Start+RR1);
	 }
	 else
	 {
		     *(Ex6Start+WR0) = ((axis&0x30)<<4)+0xf;
			 MCX_DIR = 0;
			 NOP;NOP;
			 RDdata = *(Ex6Start+RR1);
	 }
	 MCX_DIR = 1;
	 NOP;NOP;
	 return RDdata;
}
//**************************读RR2状态寄存器(轴出错信息)****************************/
/*
 *  D0  SLMT+  设定COMP+寄存器作为有效软件限制后，在正方向驱动中，逻辑/实位计数器大于COMP+寄存器值。
	D1  SLMT-  设定COMP-寄存器作为有效软件限制后，在负方向驱动中，逻辑/实位计数器小于COMP-寄存器值。
	D2  HLMT+  正方向限制信号（nLMTP）处于有效电平。
	D3  HLMT-  负方向限制信号（nLMTP）处于有效电平。
	D4  ALARM  设定成有效的伺服马达报警信号（nALARM）处于有效电平。
	D5  EMG   紧急停止信号（EMGN）处于低电平。
 */
unsigned int ReadRR2(unsigned int axis)
{
	 u8 flg;
	 unsigned int RDdata=0;
	 double angle[6];

	 if(axis&0x0F)
	 {
		     *(Ex7Start+WR0) = ((axis&0x0f)<<8)+0xf;
		     NOP;NOP;NOP;NOP;NOP;NOP;
			 MCX_DIR = 0;
			 NOP;NOP;
			 RDdata = *(Ex7Start+RR2)&0x33;
			 NOP;NOP;
	 }
	 else
	 {
		     *(Ex6Start+WR0) = ((axis&0x30)<<4)+0xf;
		     NOP;NOP;NOP;NOP;NOP;NOP;
			 MCX_DIR = 0;
			 NOP;NOP;
			 RDdata = *(Ex6Start+RR2)&0x33;
			 NOP;NOP;
	 }
	 if(axis == 0x04)	//第三轴
	 {
		 if((ROBOT_PARAMETER.SYS==FOUR_AXIS_B) || (ROBOT_PARAMETER.SYS==FOUR_AXIS_C) &&
				 (GetWR2state()&0x0003))
		 {
			 delay_200ns();
			 GetCurrentRadian(angle);
			 flg = FourAxisRobot_GetZ_LimitedFlg(angle);
			 if(flg == 1)
			 {
				 RDdata |= 0x01;
			 }
			 else if(flg == 2)
			 {
				 RDdata |= 0x02;
			 }
		 }
	 }
	 MCX_DIR = 1;
	 return RDdata;
}
/*********************************************
 * 函数功能：读MCX312的RR4输入IO寄存器(已接上拉电阻)
 * 输入参数：none
 * 返回值：	 输出IO状态（bit0~5分别代表XIN0~6）
 */
unsigned int ReadMCX312_RR4(void)
{
	 unsigned int Data;
	 MCX_DIR = 0;
	 NOP;NOP;NOP;NOP;
	 Data=*(Ex6Start+RR4);
	 MCX_DIR = 1;
	 return (Data&0x3F00)>>8;
}

/*********************************************
 * 函数功能：读MCX312的RR5输入IO寄存器(已接上拉电阻)
 * 输入参数：none
 * 返回值：	 输出IO状态（bit0~5分别代表YIN0~6）
 */
unsigned int ReadMCX312_RR5(void)
{
	 unsigned int Data;
	 MCX_DIR = 0;
	 NOP;NOP;NOP;NOP;
	 Data=*(Ex6Start+RR5);
	 MCX_DIR = 1;
	 return (Data&0x3F00)>>8;
}

/*********************************************
 * 函数功能：读MCX314的RR4输入IO寄存器(已接上拉电阻)
 * 输入参数：none
 * 返回值：	 输出IO状态（bit0~7分别代表IN0~7）
 */
unsigned int ReadMCX314_RR4(void)
{
	 unsigned int Data,tmp;
	 MCX_DIR = 0;
	 NOP;NOP;NOP;NOP;
	 Data=*(Ex7Start+RR4);
	 MCX_DIR = 1;
	 //RR4 bit0~3->XIN0~3   bit8~11->YIN0~3
	 tmp = (Data&0x0f00)>>4;
	 Data &= 0x000f;
	 Data |= tmp;
	 return Data&0x00ff;
}

//*****************************读逻辑计数器***************************/
long ReadPulseLp(unsigned int axis)
{
	 long DA,D6,D7;
	 if(axis&0x0F)
	 {
			 *(Ex7Start+WR0) = ((axis&0x0f)<<8)+0x10;
			 NOP;NOP;
			 MCX_DIR = 0;
			 NOP;NOP;
			 D6 = *(Ex7Start+RR6);
			 NOP;NOP;
			 D7 = *(Ex7Start+RR7);
		     DA = D6+(D7 << 16);
	 }
	 else
	 {
			 *(Ex6Start+WR0) = ((axis&0x30)<<4)+0x10;
			 NOP;NOP;
			 MCX_DIR = 0;
			 NOP;NOP;
			 D6 = *(Ex6Start+RR6);
			 NOP;NOP;
			 D7 = *(Ex6Start+RR7);
		     DA = D6+(D7 << 16);
	 }
     MCX_DIR = 1;
     return(DA);
}
//*****************************读实际计数器**************************/
long ReadPulseEp(unsigned int axis)
{
	 long DB,D6,D7;
	 if(axis&0x0F)
	 {
			 *(Ex7Start+WR0) = ((axis&0x0f)<<8)+0x11;
			 NOP;NOP;
			 MCX_DIR = 0;
			 NOP;NOP;
			 D6 = *(Ex7Start+RR6);
			 NOP;NOP;
			 D7 = *(Ex7Start+RR7);
			 NOP;NOP;
		     DB = D6+(D7 << 16);
	 }
	 else
	 {
			 *(Ex6Start+WR0) = ((axis&0x30)<<4)+0x11;
			 NOP;NOP;
			 MCX_DIR = 0;
			 NOP;NOP;
			 D6 = *(Ex6Start+RR6);
			 NOP;NOP;
			 D7 = *(Ex6Start+RR7);
			 NOP;NOP;
		     DB = D6+(D7 << 16);
	 }
     MCX_DIR = 1;
     return(DB);
}
//*****************************读当前速度*************************/
unsigned int ReadCv(unsigned int axis)
{
	 unsigned int DA;
	 if(axis&0x0F)
	 {
			 *(Ex7Start+WR0) = ((axis&0x0f)<<8)+0x12;
			 NOP;NOP;NOP;NOP;
			 MCX_DIR = 0;
			 NOP;NOP;NOP;NOP;
			 DA = *(Ex7Start+RR6);
			 NOP;NOP;
	 }
	 else
	 {
			 *(Ex6Start+WR0) = ((axis&0x30)<<4)+0x12;
			 NOP;NOP;NOP;NOP;
			 MCX_DIR = 0;
			 NOP;NOP;NOP;NOP;
			 DA = *(Ex6Start+RR6);
			 NOP;NOP;
	 }
     MCX_DIR = 1;
     return(DA);
}
/****************************读轴驱动状态****************************/
unsigned int ReadDriState(void)
{
	 unsigned int RDdata=0,RDdata_Ser=0;
	 MCX_DIR = 0;       //5v to 3v
	 NOP;NOP;NOP;NOP;
     RDdata = *(Ex7Start+RR0)&0x0F;
     //delay_200ns();
     NOP;NOP;NOP;NOP;
     RDdata |= ((*(Ex6Start+RR0)&0x03)<<4);
     //delay_200ns();
     NOP;NOP;NOP;NOP;
	 MCX_DIR = 1;       //3v to 5v
	 NOP;NOP;
	 //添加于//2018/06/01
	 if(Teach_mode==1)
	 {
		 //添加于//2018/06/01
		 if((RDdata&0x3F)==0)
		 {
			 if(ROBOT_PARAMETER.AXIS_NUM == 4)
			 {
				 RDdata_Ser = J1_ABSB0<<0;
				 RDdata_Ser = RDdata+(J2_ABSB0<<1);
				 RDdata_Ser = RDdata+(J3_ABSB0<<2);
				 RDdata_Ser = RDdata+(J4_ABSB0<<3);
				 while(RDdata_Ser != 0)
				 {
					 RDdata_Ser = J1_ABSB0<<0;
					 RDdata_Ser = RDdata+(J2_ABSB0<<1);
					 RDdata_Ser = RDdata+(J3_ABSB0<<2);
					 RDdata_Ser = RDdata+(J4_ABSB0<<3);
				 }
			 }
			 if(ROBOT_PARAMETER.AXIS_NUM == 5)
			 {
				 RDdata_Ser = J1_ABSB0<<0;
				 RDdata_Ser = RDdata+(J2_ABSB0<<1);
				 RDdata_Ser = RDdata+(J3_ABSB0<<2);
				 RDdata_Ser = RDdata+(J4_ABSB0<<3);
				 RDdata_Ser = RDdata+(J5_ABSB0<<4);
				 while(RDdata_Ser != 0)
				 {
					 RDdata_Ser = J1_ABSB0<<0;
					 RDdata_Ser = RDdata+(J2_ABSB0<<1);
					 RDdata_Ser = RDdata+(J3_ABSB0<<2);
					 RDdata_Ser = RDdata+(J4_ABSB0<<3);
					 RDdata_Ser = RDdata+(J5_ABSB0<<4);
				 }
			 }
			 if(ROBOT_PARAMETER.AXIS_NUM == 6)
			 {
				 RDdata_Ser = J1_ABSB0<<0;
				 RDdata_Ser = RDdata+(J2_ABSB0<<1);
				 RDdata_Ser = RDdata+(J3_ABSB0<<2);
				 RDdata_Ser = RDdata+(J4_ABSB0<<3);
				 RDdata_Ser = RDdata+(J5_ABSB0<<4);
				 RDdata_Ser = RDdata+(J6_ABSB0<<5);
				 while(RDdata_Ser != 0)
				 {
					 RDdata_Ser = J1_ABSB0<<0;
					 RDdata_Ser = RDdata+(J2_ABSB0<<1);
					 RDdata_Ser = RDdata+(J3_ABSB0<<2);
					 RDdata_Ser = RDdata+(J4_ABSB0<<3);
					 RDdata_Ser = RDdata+(J5_ABSB0<<4);
					 RDdata_Ser = RDdata+(J6_ABSB0<<5);
				 }
			 }
		 }
	 }
	 return RDdata&0x3F;
}
/****************************读轴错误状态****************************/
unsigned int ReadErrState(void)
{
	 unsigned int Error;
	 double angle[6];
	 MCX_DIR = 0;
	 NOP;NOP;NOP;NOP;
	 Error = (*(Ex7Start+RR0)&0xF0)>>4;
	 //delay_200ns();
	 NOP;NOP;NOP;NOP;
	 Error |= (*(Ex6Start+RR0)&0x30);
	 //delay_200ns();
	 NOP;NOP;NOP;NOP;
	 MCX_DIR = 1;
	 NOP;NOP;
	 if((ROBOT_PARAMETER.SYS==FOUR_AXIS_B) || (ROBOT_PARAMETER.SYS==FOUR_AXIS_C) &&
			 (GetWR2state()&0x0003))
	 {
		 delay_200ns();
		 GetCurrentRadian(angle);
		 if(FourAxisRobot_GetZ_LimitedFlg(angle))
		 {
			 Error |= 0x04;
		 }
	 }
	 return Error&0x3F;
}
/******************************************************************
 *                          Read end                              *
*******************************************************************/

//***************************初始化Mcx31x**************************/
void InitMcx31x(void) //SON:X0,X1,X2
{                      //RET:
     /*MCX_DIR=1;        //CR :U0
     NOP;NOP;NOP;NOP;
     MCX_RET=0;		//硬件复位
     delay_1us(10);
     MCX_RET=1;
     delay_1us(1);*/

     *(Ex7Start+WR0) = 0x8000;//软复位一下
     NOP;NOP;
     *(Ex6Start+WR0) = 0x8000;
     delay_1us(1);

     WriteMCX312_WR4(0xFFFF);//mcx312 IO口全部不导通

     WriteWR1(0x3F,0x0000);

     //(0x0020比较寄存器比较对象设定为实际）
     //(0x0003 COMP+-比较寄存器有效);
//     if(ROBOT_PARAMETER.SYS == FOUR_AXIS_B ||
//    		 ROBOT_PARAMETER.SYS == FOUR_AXIS_C)
//     {
//    	 WriteWR2(0x3B,0x3003);//3003 第三轴不限位
//    	 WriteWR2(0x04,0x3000);//3003
//     }
//     else
    	 WriteWR2(0x3F,0x3003);//3003

     WriteWR3(0x3F,0x5400);//0X0004曲线加减速  Bit10:Alarm滤波功能启动(512us) Bit12:输入滤波功能启用（512us）
     WriteWR4(0x0Fff);//0FC0 关CR(bit12)，关RES(bit6~bit11)，关伺服on(bit0~bit5)
     WriteWR5(0x3F,0x0000);

     WriteMCX312_WR4(0xFFFF);//mcx312 IO口全部不导通

     SetAo(0x3F,0);
     SetEp(0x3F,0);
     SetLp(0x3F,0);

}
//*******************************The End*********************************/

































