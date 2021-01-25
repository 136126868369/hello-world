/**********************************************************************
 * 伺服采用14400个脉冲每圈
 * 频率=圈数*14400/60
 * 3000r/min对应720kHz  240HZ=1r/mis
 * 1C对应40P
 * 增加对绝对坐标的监控，点动除外，最快5ms判断出跟伺服的连接问题。
 **********************************************************************/
#include "EditProgram.h"
#include "My_Project.h"
#include "ReadEncoder.h"
#include "Man_MCX31xAS.h"
#include "Man_Nandflash.h"
#include "Man_DriverScreen.h"
#include "EditProgram.h"
#include "ScreenApi.h"
#include "kinematic_explain.h"
#include "KeyboardDriver.h"
#include "Touch_screen.h"
#include "Sanxie_Servo.h"
// ======================================================================
// 功    能： 子函数定义**
// ======================================================================
void InitMcu (void);
void InitData(void);
//PositionGesture pg1,pg2;
#define len	50
char str[9];
unsigned char Teach_mode=0;//修正于//2018/06/01
/*************************************
 * 函数功能：初始化机械手参数
 * 输入参数：axis_num 轴个数 5或6
 * 输出参数：NONE
 */
void InitRobotPara(unsigned char axis_num);

/*************************************
 * 函数功能：初始化机械手状态
 * 输入参数：NONE
 * 输出参数：NONE
 */
void InitRobotState(void);
/*======================================================================
* 名    称： void main(void)
* 功    能： 主函数
* 入口参数： 无
* 出口参数： 无
======================================================================*/
//long pulse[6]={100000,-200000,300000,-400000,500000,-600000};
//long pulse_tmp[6],i;

 void main(void)

 {
//	 u8 data[4],err;
//	 u16 tmp;
	/***********************
	 *
	 *
	 */
     InitMcu();

     //测试新指令
//      unsigned char code_err,err;
//      char data0[54]="part ( -0.222 , +123.123, +0 ,-456, -0.234, +55.432)";
//      code_err = CheckGrammar((PrgCode*)data0);//检查语法
//      err = ProgramStepRun_Code(code_err,((PrgCode*)data0));
//
//      char data[35]="part.px= -5.67";
//      code_err = CheckGrammar((PrgCode*)data);//检查语法
//      err = ProgramStepRun_Code(code_err,((PrgCode*)data));
//
//      char data1[35]="part.rz = +12.34";
//      code_err = CheckGrammar((PrgCode*)data1);//检查语法
//      err = ProgramStepRun_Code(code_err,((PrgCode*)data1));

     InitMcx31x();

	 MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);//修正于//2018/06/01 for Extend RAM
	 InitFlash();

     CloseBrake(1);//关闭刹车

//再次初始化串口，不然，重启会接收不了数据
     delay_1ms(300);
	 DINT;   				     //关CPU中断
	 InitSci();				     //初始化SCIB寄存器
	 EINT;   				     //Enable INTM 使能全局中断
	 ERTM;					     //Enable DBGM 使能实时中断


	 delay_1ms(5500);
	 SetScreen(1);
	 delay_1ms(100);
//
//	 err = GetServoPARAM(1,SX_UNLOCK_PARAM_ALL,0,data,0);
//	 tmp = data[0]<<8;
//	 tmp |= data[1];
//	 err = SetServoPARAM(1,SX_SAVE_PARAM_ALL,tmp,0);
     InitData();

//
//	 PositionGesture pos_ges;
//	 PositionEuler tool_end = {
//			 -15.642,0,175.447,180,-45,0
//	 };
//	 PositionEuler part_rob = {
//			 0,449.998,122.504,0,0,0
//	 };
//	 PositionEuler part = {
//			 //0,0,132.503,90,0,0
//			 45.367,-69.718,127.504,0,0,83.2005
//	 };
//	 tool_end.Rx = tool_end.Rx*PI_DIVIDE_180;
//	 tool_end.Ry = tool_end.Ry*PI_DIVIDE_180;
//	 tool_end.Rz = tool_end.Rz*PI_DIVIDE_180;
//	 part_rob.Rx = part_rob.Rx*PI_DIVIDE_180;
//	 part_rob.Ry = part_rob.Ry*PI_DIVIDE_180;
//	 part_rob.Rz = part_rob.Rz*PI_DIVIDE_180;
//	 part.Rx = part.Rx*PI_DIVIDE_180;
//	 part.Ry = part.Ry*PI_DIVIDE_180;
//	 part.Rz = part.Rz*PI_DIVIDE_180;
//	 double length=89;
//	 pos_ges = PartPosToToolPos(tool_end,part_rob,part,length);
//
//	 double ag_tmp[6] = {90,-56.8,-8.66,0,20.52,0};
//	 double angle[6];
//	 int i;
//	  for(i=0;i<6;i++)
//	  {
//		  *(ag_tmp+i) = *(ag_tmp+i)*PI_DIVIDE_180;//
//	  }
//	  update_sin_cos_data(ag_tmp);			//更新库
//	  ModifyPositionGesture(pos_ges);			//更新正解
//	  GetBestSolution(angle);
//	  for(i=0;i<6;i++)
//	  {
//		  *(angle+i) = *(angle+i)*PI_MULTI_180_INVERSE;//弧度转角度
//	  }
//
//
/*****************************************/
     StandbyOutput();	//亮黄灯

//     DisableServoAlarm();
//     G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 0;
//     ExecuteRS232(*((PrgCode*)"rs232"));
//     double ag_tmp[6] = {
////    		 0.0001409623,
////    		 -47.97009,
////    		-6.465645,
////    		0.000001,
////    		57.60002,
////    		-179.9999
//    		 0,-47.9706,-6.4647,0.0001,57.5986,0
//     };
//     PositionGesture  p_g;
//     int i;
//	 for(i=0;i<6;i++)
//	 {
//		 ag_tmp[i] = ag_tmp[i]*0.01745329252;
//	 }
//     p_g = GetPositionGesture(ag_tmp);
//     G_CTRL_BUTTON.CTRL_BUTTON.SERVO_ON = 0;
     ReadAndSendProgramName();
	 while(1)
	 {
		 scan_touch_screen();//全盘扫描
	 }
}
//*******************************MUC初始化************************************/
void InitMcu(void)
{
	 InitSysCtrl();			     //初始化系统
	 //MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	 //InitFlash();

	 //ADC时钟
     //EALLOW;
     //SysCtrlRegs.HISPCP.all = 0x3;	//High-speed peripheral clock pre-scaler
     //EDIS;

	 DINT;   				     //关CPU中断

	 IER = 0x0000;
	 IFR = 0x0000;
	 InitPieCtrl();			     //初始化PIE中断
	 InitPieVectTable();         //初始化PIE中断矢量表
     InitGpio();                 //定义I/O口
     InitSci();				     //初始化SCIB寄存器
	 InitXintf();
	 InitSpiaGpio();
	 InitSpi();

//	 InitAdc();
//	 AdcRegs.ADCTRL1.bit.ACQ_PS = 0x01;	//ADC模块周期中的S/H宽度 = 2个ADC周期
//
//	 AdcRegs.ADCTRL3.bit.ADCCLKPS = 0;//ADC时钟模块=HSPCLK/1=25.5MHz/(1)=25MHz
//	 AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;	//1级联模式
//	 AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
//	 AdcRegs.ADCTRL1.bit.CONT_RUN = 1;	//设置连续运行
//	 AdcRegs.ADCTRL1.bit.SEQ_OVRD = 1;	//使能排序器覆盖功能
//	 AdcRegs.ADCCHSELSEQ1.all = 0x00;	//初始化所有的ADC通道到A0
//	 AdcRegs.ADCCHSELSEQ2.all = 0x00;
//	 AdcRegs.ADCCHSELSEQ3.all = 0x00;
//	 AdcRegs.ADCCHSELSEQ4.all = 0x00;
//	 AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0x7;	//转换并存储到8个结果寄存器

	 //InitXIntrupt();
	 //InitXintf16Gpio();
// Step 4. Initialize the Device Peripheral. This function can be
//         found in DSP2833x_CpuTimers.c
	 InitCpuTimers();   // For this example, only initialize the Cpu Timers


	 ConfigCpuTimer(&CpuTimer0, 15000, 10000);//1000,000us
	 //ConfigCpuTimer(&CpuTimer1, 150, 1000000);
	 //ConfigCpuTimer(&CpuTimer2, 150, 1000000);



// Enable CPU int1 which is connected to CPU-Timer 0, CPU int13
// which is connected to CPU-Timer 1, and CPU int 14, which is connected
// to CPU-Timer 2:
	 IER |= M_INT1;
	//IER |= M_INT13;
	//IER |= M_INT14;

// Enable TINT0 in the PIE: Group 1 interrupt 7 //开定时器0中断
	 PieCtrlRegs.PIEIER1.bit.INTx7 = 1;


	 IER |= M_INT8;//SCIC

	 IER |= M_INT9;//SCIB

// 外部中断配置
	 /*IER |= M_INT12;//外部中断
	 GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 2;		//采用6个采样周期宽度限制
	 GpioCtrlRegs.GPBCTRL.bit.QUALPRD0 = 30;	//采样周期 = 32 x TSYSCLKOUT
	 EALLOW;
	 GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 1;//外部中断7选择GPIO33引脚为中断源
	 EDIS;
	 XIntruptRegs.XINT3CR.bit.POLARITY = 0;	//0:下降沿触发中断 1:上升沿
	 XIntruptRegs.XINT3CR.bit.ENABLE = 1;		//外部中断7使能
	 PieCtrlRegs.PIEIER12.bit.INTx1 = 1; 		//Group12 INTx1使能
*/
	 // GPIO29 are inputs
/*	 EALLOW;
	 GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 0;         // GPIO
	 GpioCtrlRegs.GPADIR.bit.GPIO29 = 0;          // input
	 GpioCtrlRegs.GPAQSEL2.bit.GPIO29 = 2;        // Xint1 Synch to SYSCLKOUT only
	 GpioCtrlRegs.GPACTRL.bit.QUALPRD0 = 0xFF;   // Each sampling window is 510*SYSCLKOUT
	 GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL = 29;//外部中断2选择GPIO29引脚为中断源
	 EDIS;
	 XIntruptRegs.XINT2CR.bit.POLARITY = 0;	//0:下降沿触发中断 1:上升沿
	 XIntruptRegs.XINT2CR.bit.ENABLE = 1;		//外部中断2使能
	 PieCtrlRegs.PIEIER1.bit.INTx5 = 1; 		//Group1 INTx5使能
*/
	 EINT;   				     //Enable INTM 使能全局中断
	 ERTM;					     //Enable DBGM 使能实时中断
	 //ConfigCpuTimer(&CpuTimer0, 150, 50000);//设置CPU  (150M   50ms）
	 //StartCpuTimer0();
	 //SendStm32DataFlag = 1;
	 //ReadStm32DataFlag = 1;
	 //delay_1ms(500);             //延时500MS

	 //StartCpuTimer0();

	 GpioDataRegs.GPBDAT.bit.GPIO57=1;delay_200ns();//SPITEA

	 GpioDataRegs.GPADAT.bit.GPIO21=0;delay_200ns();//delay_200ns();//speaker

	 GpioDataRegs.GPADAT.bit.GPIO25=1;delay_200ns();//delay_1us(1);//DIR
	 GpioDataRegs.GPADAT.bit.GPIO24=1;delay_200ns();//delay_1us(1);//MCXRET

	 GpioDataRegs.GPBDAT.bit.GPIO48=1;delay_200ns();//delay_1us(1);//ABSM1
	 GpioDataRegs.GPBDAT.bit.GPIO49=1;delay_200ns();//delay_1us(1);
	 GpioDataRegs.GPBDAT.bit.GPIO50=1;delay_200ns();//delay_1us(1);
	 GpioDataRegs.GPBDAT.bit.GPIO51=1;delay_200ns();//delay_1us(1);
	 GpioDataRegs.GPBDAT.bit.GPIO52=1;delay_200ns();//delay_1us(1);
	 GpioDataRegs.GPBDAT.bit.GPIO53=1;delay_200ns();//delay_1us(1);//ABSM6
	 GpioDataRegs.GPADAT.bit.GPIO27=1;delay_200ns();//delay_1us(1);//ABSR

	 GpioDataRegs.GPBDAT.bit.GPIO59=1;delay_200ns();//delay_1us(1);//HC_LD
	 GpioDataRegs.GPBDAT.bit.GPIO60=0;delay_200ns();//HC_CLK

	 GpioDataRegs.GPADAT.bit.GPIO20=1;delay_200ns();//RS485DIR = 1;默认发送
}
/**************
 * 初始化数据
 */
void InitData(void)
{
	 SetTextValue(43,2,"Initializing system...");
	 SetScreen(43);
	 delay_1ms(100);
	 SetScreen(37);
	 delay_1ms(100);
	 InitRobotPara(6);//初始化机械手参数
	 ClearProgramPulseData();//清零程序和脉冲内存
}

/*************************************
 * 函数功能：初始化机械手参数
 * 输入参数：axis_num 轴个数 5或6
 * 输出参数：NONE
 */
void InitRobotPara(unsigned char axis_num)
{
	long LimitedPositionPlus[]={0,0,0,0,0,0};
	long LimitedPositionMinus[]={0,0,0,0,0,0};
	long pulse[6],range;
	unsigned char err,err1,i;
	err = ReadRobotPara(&ROBOT_PARAMETER);
	if(err == 1)	//读取失败
	{
		SetScreen(20);
		SetTextValue(20, 4, "");
		SetTextValue(20, 3, "Read system parameter failed!");
		while(1)
		{
			if(USART_RX_STA&0x8000)
			{
				USART_RX_STA = 0;
				break;
			}
		}
		setting_robot_para();
		ReadRobotPara(&ROBOT_PARAMETER);
	}
	if(err == 2)	//参数未设置
	{
		SetScreen(20);
		SetTextValue(20, 4, "");
		SetTextValue(20, 3, "Please Set system parameter!");
		while(1)
		{
			if(USART_RX_STA&0x8000)
			{
				USART_RX_STA = 0;
				break;
			}
		}
		setting_robot_para();
		ReadRobotPara(&ROBOT_PARAMETER);
	}
	 if((ROBOT_PARAMETER.ENCODER_LEFT_SAV==1) &&
			 (ROBOT_PARAMETER.MOTOR_TYPE==2))//判断是否需要保存编码器原点偏移数据
	 {
		 if(ReadEncoderPulseAll(pulse))//保存编码器原点偏移数据
		 {
			 	//读取失败
				SetScreen(20);
				SetTextValue(20, 4, "");
				SetTextValue(20, 3, "Read encoder data failed!");
				while(1)
				{
					if(USART_RX_STA&0x8000)
					{
						USART_RX_STA = 0;
						break;
					}
				}
		 }
		 else//读取成功
		 {
			 for(i=0;i<6;i++)
			 {
				 ROBOT_PARAMETER.ENCODER_LEFT[i] = pulse[i];
			 }
			 ROBOT_PARAMETER.ENCODER_LEFT_SAV = 0;
			 err1 = SaveRobotPara(ROBOT_PARAMETER);//保存机械手参数
			 if(err1)//保存失败
			 {
					SetScreen(20);
					SetTextValue(20, 4, "");
					SetTextValue(20, 3, "Save encoder data failed!");
					while(1)
					{
						if(USART_RX_STA&0x8000)
						{
							USART_RX_STA = 0;
							break;
						}
					}
			 }
		 }
	 }

	PulseOfCircle_INVERSE = (double)1/ROBOT_PARAMETER.MOTOR_PRECISION;
	range = (double)8000000/(ROBOT_PARAMETER.MOTOR_SPEED/60.0*ROBOT_PARAMETER.MOTOR_PRECISION/8000.0);
	SetRange(0x3f,range);//55000-3500转  32000-6000转
	SetAcac(0x3f,300);      //加速度变化率 1~65535  (62.5*10^6)/K*(8000000/R)
	SetAcc(0x3f,400);      //加速度  A*125	 1~8000
	//SetDec(0x3f,80);      //减速度  A*125 1~8000
	SetStartV(0x3f,8000);      //初始速度   范围：1~8000
	SetSpeed(0x3f,400);//设置初始速度

	if(ROBOT_PARAMETER.SOUND == 0)
	{
		SetTouchScreen(0x21);//关闭声音
		delay_1ms(10);
		SetButtonValue(44, 10, 1);
		SetButtonValue(2, 10, 1);
		delay_1ms(10);
		SetButtonValue(2, 10, 1);
	}
	else
	{
		SetTouchScreen(0x23);//打开声音
		delay_1ms(10);
		SetButtonValue(44, 10, 0);
		delay_1ms(10);
		SetButtonValue(2, 10, 0);
	}

	LimitedPositionPlus[0]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[0]*ROBOT_PARAMETER.LIMITED_P[0]/360.0;	//第一轴正限位在165度 	(四轴：165-5)
	LimitedPositionPlus[1]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[1]*ROBOT_PARAMETER.LIMITED_P[1]/360.0;	//第二轴正限位在10度	(四轴：144-4)
	LimitedPositionPlus[2]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[2]*ROBOT_PARAMETER.LIMITED_P[2]/360.0;	//第三轴正限位在81度	(四轴：0)
	LimitedPositionPlus[3]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[3]*ROBOT_PARAMETER.LIMITED_P[3]/360.0;	//第四轴正限位在160度	(四轴：360)
	LimitedPositionPlus[4]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[4]*ROBOT_PARAMETER.LIMITED_P[4]/360.0;	//第五轴正限位在90度
	LimitedPositionPlus[5]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[5]*ROBOT_PARAMETER.LIMITED_P[5]/360.0;	//第六轴正限位在360度

	LimitedPositionMinus[0]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[0]*ROBOT_PARAMETER.LIMITED_M[0]/360.0;//第一轴负限位在-165度	(四轴：-165+5)
	LimitedPositionMinus[1]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[1]*ROBOT_PARAMETER.LIMITED_M[1]/360.0;//第二轴负限位在-180度	(四轴：-144+4)
	LimitedPositionMinus[2]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[2]*ROBOT_PARAMETER.LIMITED_M[2]/360.0;//第三轴负限位在-85度	(四轴：-300)
	LimitedPositionMinus[3]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[3]*ROBOT_PARAMETER.LIMITED_M[3]/360.0;//第四轴负限位在-160度	(四轴：360)
	LimitedPositionMinus[4]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[4]*ROBOT_PARAMETER.LIMITED_M[4]/360.0;//第五轴负限位在-90度
	LimitedPositionMinus[5]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[5]*ROBOT_PARAMETER.LIMITED_M[5]/360.0;//第六轴负限位在-360度

	SetLimitedPosition(LimitedPositionPlus,LimitedPositionMinus); //限位设置
	update_kinematic_parameter(axis_num);
}

/*************************************
 * 函数功能：初始化机械手状态
 * 输入参数：NONE
 * 输出参数：NONE
 */
void InitRobotState(void)
{

}
//******************************THE END*************************************/
