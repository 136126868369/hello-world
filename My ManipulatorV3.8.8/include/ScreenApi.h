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
 * 功能：单轴运动
 * 参数：pressFlag  按钮标志位；  	1：表示按下   0：表示松开
 * 		 whichAxis	表示哪个轴	  	1~6分别表示1~6轴
 * 		 sign		表示驱动方向 	0：正方向	  1：负方向
 * 返回值：0：正确
 * 		   1：机械手动作过程中发生报警
 * 		   2：机械手动作过程中松开了安全开关（示教时）
 */
unsigned char AxisSingleRun(unsigned char pressFlag,unsigned char whichAxis,unsigned char sign);

/********************************************************
 * 功能：沿基坐标直线运动  Base coordinate straight line motion
 * 参数：pressFlag 按钮标志位， 1：表示按钮按下   0：按钮松开
 * 		 orientation  方向标志位  1：X-  2：X+
 * 		 						  3：Y-	 4：Y+
 * 		 						  5：Z-  6：Z+
 * 返回值：0：正确	1：发生错误
 */
unsigned char BaseCoordSlineRun(unsigned char pressFlag,unsigned char orientation);


/********************************************************
 * 功能：沿基坐标固定直线距离运动  Base coordinate straight line Fixed distance
 * 参数：movingDistance 移动距离， double类型，可正负，正负代表了方向,最高精度的倍数
 * 		 orientation  方向  		只能输入px，py，pz的指针
 */
void BaseCoordSlineFixedDistance(unsigned char changeGesture,double movingDistance,double *orientation);

/********************************************************
 * 功能：沿基坐标固定直线最小精度运动,使用前需要先改变px，py，pz的值，
 * 		 否则直接调用无意义
 * 返回值：0：正确		1:发生错误
 */
//#pragma CODE_SECTION(BaseCoordSlineMinPrecisionRun, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char BaseCoordSlineMinPrecisionRun(void);

/********************************************************
 * 功能：以当前位置，行走输入的脉冲数
 * 输入参数：delta_pulse 指向6个轴每个轴需要走的脉冲数的指针
 * 			//mdoe:0->关节插补		1->直线插补
 * 返回值：    0：正确
 * 			1:机械手动作过程中发生报警
 * 			2：机械手动作过程中松开了安全开关（示教时）
 * 			3：动作过程中按下了stop按钮
 */
//#pragma CODE_SECTION(BaseCoordSlineMinPrecisionRun, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char MovePulse(long* delta_pulse);

/*****************************************
 * 函数功能：回原点
 * 输入参数： speed 返回原点的速度	1~30（推荐8~10）
 */
void ReturnHome(unsigned char speed);

/****************************************
 * 函数功能：原点清零
 */
u8 ClearOrigin(void);

/****************************************
 * 函数功能：清除报警
 */
void ClearAlarm(void);

//清除报警错误信息
void ClearAlarmInfo(void);
/****************************************
 * 函数功能：获得伺服的脉冲
 * 返回值： 0：成功
 *  		非0情况下：bit0~bit5的1分别代表哪个伺服读取数据出错，可能是相应的伺服出错
 * 					   bit7为1代表读取回来的数据出错，可能是传输过程出错，可能是线路问题
 */

/********************************************
 * 函数功能：检测触摸屏连接状态并修复
 * 输入参数：NONE
 * 输出参数：NONE
 */
void CheckScreenConnectState(void);

/********************************************
 * 检查报警
 * 返回值：错误轴，bit0~bit5分别代表J1~J6
 */
unsigned char ErrCheck(unsigned char screen_id);

/********************************************
 * 伺服开
 */
unsigned char  ServoON(unsigned char mode);

/********************************************
 * 伺服关
 */
void ServoOFF(void);

/********************************************
 * 控制伺服开关，bit0~bit5分别代表J1~J6,0开伺服，1关伺服
 */
void ServoON_OFF(unsigned int state);

/********************************************
 * 控制单个伺服ON的开关，
 * Axis：输入范围1~6，代表J1~J6
 * state：0开伺服，1关伺服
 */
void ServoON_OFF_Single(unsigned char axis,unsigned char state);

/*******************************************
 * 清除伺服报警
 */
void ClearServoAlarm(void);

/************************************
 * 读当前警报/错误信息
 * 返回值	bit0=1，伺服on没打开
 * 			bit1=1，有警报
 * 			bit2=1，无原点或原点出错
 * 			bit3=1，急停中
 */
unsigned int ReadErrInfo(void);

/****************************************
 * 函数功能：获得伺服的脉冲
 * 输入值： mode=0,更新脉冲数据
 * 			mode=1,强制更新脉冲数据，不管读取数据是否正确
 * 返回值： 0：成功
 *  		非0情况下：bit0~bit5的1分别代表哪个伺服读取数据出错，可能是相应的伺服出错
 */
unsigned char UpdatePulseFromServo(void);
unsigned char UpdatePulseFromServoForEp(long *pulse);


//末端坐标直线运动
void XEndCoordSlineRunP(unsigned char pressFlag);
void XEndCoordSlineRunM(unsigned char pressFlag);
void YEndCoordSlineRunP(unsigned char pressFlag);
void YEndCoordSlineRunM(unsigned char pressFlag);
void ZEndCoordSlineRunP(unsigned char pressFlag);
void ZEndCoordSlineRunM(unsigned char pressFlag);

//基坐标旋转		Revolve  旋转
void XBaseCoordRevolRunP(unsigned char pressFlag);
void XBaseCoordRevolRunM(unsigned char pressFlag);
void YBaseCoordRevolRunP(unsigned char pressFlag);
void YBaseCoordRevolRunM(unsigned char pressFlag);
void ZBaseCoordRevolRunP(unsigned char pressFlag);
void ZBaseCoordRevolRunM(unsigned char pressFlag);

//末端坐标旋转
void XEndCoordRevolRunP(unsigned char pressFlag);
void XEndCoordRevolRunM(unsigned char pressFlag);
void YEndCoordRevolRunP(unsigned char pressFlag);
void YEndCoordRevolRunM(unsigned char pressFlag);
void ZEndCoordRevolRunP(unsigned char pressFlag);
void ZEndCoordRevolRunM(unsigned char pressFlag);

void OpenBuzzer(void);//打开蜂鸣器
void CloseBuzzer(void);//关闭蜂鸣器
void ReadCurrentPulseEp(long *pulse);
void ReadCurrentPulseLp(long *pulse);
void SetSpeed(unsigned char aix,unsigned int r);


/**************************************
 * 函数功能：读安全开关状态
 * 返回值：	 0：安全开关已按下
 * 			 1：安全开关未按下
 */
unsigned char ReadSaveButtonState(void);

/************************************
 * 函数功能：设置伺服报警和限位的有效/无效
 * 输入参数：flag
 * 			 bit0：	0:伺服报警无效		1:伺服报警有效
 * 			 bit1：	0:限位无效			1:限位有效
 */
void SetAlarmLimitedState(unsigned char flag);

void DisableServoAlarm(void);//伺服报警无效
void EnableServoAlarm(void);//伺服报警有效
void DisableLimited(void);//限位无效
void EnableLimited(void);//限位有效
//#pragma CODE_SECTION(GetWR2state, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned int GetWR2state(void);//获取WR2寄存器的值
void RecoverWR2(void);//恢复WR2寄存器
void SetSpeedMode_High(void);//设置为高速示教模式
void SetSpeedMode_Low(void);//设置为低速示教模式
unsigned char GetSpeedMode(void);//获得当前示教模式
void DeveloperMode_On(void);//设置开发者模式为：开
void DeveloperMode_Off(void);//设置开发者模式为：关


/*****************************************
 * 功能：更新速度，和上面的函数功能不同，上面适用于初始化速度，
 * 		 而这个函数适用于机械手动作过程中调整速度，修改的是全局速度
 * 输入参数：speed，速度值，范围1~100
 */
void UpdateSpeed(unsigned char speed);
u8 SetPoutState(unsigned int data);//打开输出IO
u8 ResetPoutState(unsigned char num);//关闭输出IO
void OpenBrake(unsigned char num);//打开刹车
void CloseBrake(unsigned char num);//关闭刹车
void AlarmOutput(void);		//只亮红灯
void StandbyOutput(void);	//只亮黄灯
void RunningOutput(void);	//只亮绿灯
void OpenFan(void);//打开风扇
void CloseFan(void);//关闭风扇
u32 ReadPoutState(void);//读输出IO状态
unsigned char ReadBrakeState(void);//读刹车状态
u32 ReadPinState(void);//读输入IO状态
void SetLimitedPosition(long *plus,long *minus);//限位设置

//#pragma CODE_SECTION(UpdateCurrentPositionGesture, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void UpdateCurrentPositionGesture(void);	//更新位姿
//#pragma CODE_SECTION(JointInterpolation, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char JointInterpolation(long* next_pulse);	//关节插补
//#pragma CODE_SECTION(LinearInterpolation, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char  LinearInterpolation(long* next_pulse);	//直线插补
//#pragma CODE_SECTION(CircleInterpolation, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char CircleInterpolation(long* next_pulse,unsigned char orientation);//圆弧插补
//#pragma CODE_SECTION(TranslationProccess, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void TranslationProccess(long *differPulse,unsigned char mode);//插补运动

//#pragma CODE_SECTION(AngleToPulse, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void AngleToPulse(double *angle,long *pulse);//弧度转换成脉冲
//#pragma CODE_SECTION(PulseToAngle, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void PulseToAngle(long *pulse,double *angle);//脉冲转换成弧度
//#pragma CODE_SECTION(GetCurrentAngle, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void GetCurrentAngle(double* angle);//获得当前角度
//#pragma CODE_SECTION(GetCurrentRadian, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void GetCurrentRadian(double* angle);//获得当前弧度

char GetPosition_SCI(double* position);//从外部(串口)获得位置数据
char Data_To_Position(char *data);
unsigned char ReadRobotPara(RobotParaStru* robot_para);
unsigned char SaveRobotPara(RobotParaStru robot_para);

u8 GetStm32Version(u8 *data);

u8 GetSTM32USART_STA(char *data);
//u8 Read_STM32_USART_Data(char* data);
u8 STM32_USART_SentData(char* data,u16 len);
#endif /* SCREENAPI_H_ */
