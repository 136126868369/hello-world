/*
 * PickUpTextCode.c
 *
 *  Created on: 2015-1-9
 *      Author: rd49
 */
#include "My_Project.h"
#include "My_USART.h"
#include "EditProgram.h"
#include "Man_DriverScreen.h"
#include "Man_MCX31xAS.h"
#include "Man_Nandflash.h"
#include "BuildProgram.h"
#include "ScreenApi.h"
#include "KeyboardDriver.h"
#include "DSP28335_Spi.h"
#include "Touch_screen.h"
#include "pulse number.h"
#define CRC_PAGE_ADDR			63	//保存校验位的页地址
#define CRC_BYTE_ADDR			0	//保存校验位的页地址
long CURRENT_BAUD = 115200;		    //当前波特率
//工具末端坐标相对于机械手末端的坐标关系的参数
ToolParameter TOOL_END={
		{0,0,0,0,0,0},
		0
};

//工件基坐标相对于机械手基坐标的位置关系的参数
PartParameter PART_ROBOT={
		{0,0,0,0,0,0}
};

struct program_name_num s_program_name_num = {0,0};

static unsigned int s_row=0;			//行
static unsigned char s_column=0;		//列
static unsigned char s_page=0;		//页
static unsigned int s_ACC = 0;		//加速度值
//static unsigned char s_run_flag=0;//0 没有全速运行  1：程序正在全速运行
//static unsigned char s_program_open_flg=0;//程序打开标志位，0：当前没有程序打开	1：已经打开了程序
struct SpeedStru
{
	 unsigned char ovrd:8;		//程序运行的全局速度
	 unsigned char speed:8;	//程序运行的局部速度
	 unsigned int	 all_speed;//用来记录程序运行的总速度，all_speed=ovrd*speed
};
static struct SpeedStru s_speed={20,50,10};

struct Position_Pulse  //sizeof为14
{
	unsigned char usedFlag:1; //P点位置是否被使用的标志位： 0:未使用  1：已被使用
	unsigned char reserved:7; //保留
	unsigned char position; //Pxx
	long pulse[6];
};

struct ProgramInfo{
	unsigned char blockAddr;//程序保存所在的块地址
	unsigned char prgName[PROGRAM_NAME_LENGTH];//程序名
};


struct TrayStru s_tray[MAX_TRAY_NUM];//托盘参数 (加个括号只能将第一个数初始化为0而已)
#pragma  DATA_SECTION (s_Nx_variable, ".MY_MEM0")
static long s_Nx_variable[MAX_N_NUM];//Nx变量
#pragma  DATA_SECTION (s_Sx_variable, ".MY_MEM0")
static long s_Sx_variable[MAX_N_NUM];//Sx变量
#pragma  DATA_SECTION (s_Fx_variable, ".MY_MEM0")
static double s_Fx_variable[MAX_F_NUM];	//Fx变量
#pragma  DATA_SECTION (s_program_code, ".MY_MEM0")
static PrgCode	   s_program_code[ROWS_IN_PROGRAM]={0};//程序行存储 750行
#pragma  DATA_SECTION (s_position_pulse, ".MY_MEM0")
static struct Position_Pulse s_position_pulse[MAX_POSITION_NUM]={0};

static unsigned int s_return_stack[MAX_RETURN]={0};//return的堆栈，进入子函数前，保存return后将会返回的行值
static unsigned int *s_return_stack_pointer=NULL;//指向return堆栈的底部
int RX_POSITION_STA=0;//外部位置数据接收标志位，1：有位置数据  0：无位置数据
extern int USART_RX_STA_B;
//void MoveCursorPosition(void);
unsigned char GetRowPositionValue(unsigned int row);

void AngleToStrings(double* angle,unsigned char* angle_strings);

void ClearTrayData(void);
void SingleAxisAngleChange(unsigned char axis,double angle,long* pulse);
void SetSingleAxisAngle(unsigned char axis,double angle,long* pulse);

//#pragma CODE_SECTION(ExecuteGosub, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char ExecuteGosub(PrgCode codeStrings,unsigned int* row);
//#pragma CODE_SECTION(ExecuteReturn, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void ExecuteReturn(PrgCode codeStrings,unsigned int* row);
//#pragma CODE_SECTION(ExecuteGoto, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void ExecuteGoto(PrgCode codeStrings,unsigned int* row);
unsigned char ExecuteDly(PrgCode codeStrings);
void ExecuteEnd(PrgCode codeStrings);
void ExecuteFinish(PrgCode codeStrings);
//#pragma CODE_SECTION(ExecuteMov, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char ExecuteMov(PrgCode codeStrings,char teach);
//#pragma CODE_SECTION(ExecuteMvs, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char ExecuteMvs(PrgCode codeStrings,char teach);
//#pragma CODE_SECTION(ExecuteMvr, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char ExecuteMvr(PrgCode codeStrings,char teach);
//#pragma CODE_SECTION(ExecuteMvc, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char ExecuteMvc(PrgCode codeStrings,char teach);
//#pragma CODE_SECTION(ExecuteMvh, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char ExecuteMvh(PrgCode codeStrings,char teach);
//void ExecuteOvrd(PrgCode codeStrings);
//void ExecuteSpeed(PrgCode codeStrings);
void ExecuteStop(PrgCode codeStrings);
//unsigned char ExecutePin(PrgCode codeStrings);
u8 ExecutePout(PrgCode codeStrings);
unsigned char ExecuteWait(PrgCode codeStrings);
//void ExecuteTray(PrgCode codeStrings);
unsigned char ExecuteTx(unsigned char* strings,long* pulse);
//void ExecuteNx(unsigned char* strings);
//unsigned char ExecutePx(PrgCode codeStrings);
//#pragma CODE_SECTION(ExecuteIf, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char ExecuteIf(PrgCode codeStrings,unsigned int* row);
//#pragma CODE_SECTION(ExecuteThen, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void ExecuteThen(PrgCode codeStrings,unsigned int* row);
//#pragma CODE_SECTION(ExecuteElse, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void ExecuteElse(PrgCode codeStrings,unsigned int* row);
void ExecuteAcc(PrgCode codeStrings);
u8 ExecuteRstd(PrgCode codeStrings);
unsigned char ExecuteDisp(PrgCode codeStrings);
unsigned char ExecuteRS232(PrgCode codeStrings);
void ExecuteToolLength(PrgCode codeStrings);
void ExecuteTool(const PrgCode* codeStrings);
void ExecutePart(const PrgCode* codeStrings);
void ExecuteNetwork(PrgCode codeStrings,unsigned int* row);//修正于//2018/06/01 for network
void ExecuteTeamwork(PrgCode codeStrings,unsigned int* row);//修正于//2018/06/01 for teamwork


PositionEuler StringToPositionEuler(char* str);
/***************************************
 * 函数功能：获得程序行的首指针
 * 输入参数：void
 * 输出参数：s_program_code的指针
 */
PrgCode* GetProgramCode(void)
{
	return s_program_code;
}

/***************************************
 * 函数功能：清零程序和脉冲
 * 输入参数：NONE
 * 输出参数：NONE
 */
void ClearProgramPulseData(void)
{
	   memset(s_program_code,0,sizeof(s_program_code));		//清零
	   memset(s_position_pulse,0,sizeof(s_position_pulse)); 	//清零
	   //memset(DISPLAY_BUF,0,sizeof(DISPLAY_BUF));
	   memset(s_Nx_variable,0,sizeof(s_Nx_variable));
	   memset(s_Fx_variable,0,sizeof(s_Fx_variable));
	   memset(s_tray,0,sizeof(s_tray));
}

/***************************************
 * 函数功能：获得n变量的值
 * 输入参数：n_num编号
 * 输出参数：n变量的值
 */
long get_n_value(int n_num)
{
	return s_Nx_variable[n_num];
}
/***************************************
 * 函数功能：获得s变量的值
 * 输入参数：s_num编号
 * 输出参数：s变量的值
 */
long get_s_value(int s_num)
{
	return s_Sx_variable[s_num];
}

/***************************************
 * 函数功能：获得n变量的值
 * 输入参数：n_num编号
 * 输出参数：n变量的值
 */
double get_f_value(int n_num)
{
	return s_Fx_variable[n_num];
}

/***************************************
 * 函数功能：让第row行获得文本
 * 输入参数：row	行
 * 			 strings	字符串首指针
 * 			 说明：字符串以\0作为结尾
 * 返回值：	 编译出错信息
 */
unsigned char GetCodeStrings(unsigned int row,unsigned char* strings)
{
	 unsigned char err_info=0,i=0;
	 s_row = row;
	 while(*strings)
	 {
		  if(i>=COLUMNS_IN_LINE)	break;
		  s_program_code[row].codeStrings[i] = *strings++;
		  i++;
	 }
	 for(;i<COLUMNS_IN_LINE;i++)
	 {
		  s_program_code[row].codeStrings[i] = 0;
	 }
	 //err_info = CheckGrammar(&s_program_code[row]);
	 return err_info;
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/*********************************************
 * 函数功能：	获得光标位置
 * 输入参数：	none
 * 返回值：		光标位置	bit0~7是行，bit8~15是列
 * 用法：		cursor = GetCursorPosition();
 * 		 		row = cursor&0xff；//行
 * 		 		column = (cursor>>8)0xff;//列
 */
unsigned int GetCursorPosition(void)
{
	 unsigned int cursor;
	 cursor = s_column;
	 cursor <<= 8;
	 cursor |= s_row;
	 return cursor;
}
/*********************************************
 * 函数功能：	获得当前页
 * 输入参数：	none
 * 返回值：		当前页码	从第0页开始
 */
unsigned char GetPage(void)
{
	 return s_page;
}
/*********************************************
 * 函数功能：	获得当前程序的代码总行数
 * 输入参数：	none
 * 返回值：		代码行数0~750,0代表一行都没有
 */
unsigned int GetLinesCurrentProgram(void)
{
	 unsigned int i;
	 for(i=ROWS_IN_PROGRAM;i>0;i--)
	 {
		  if(s_program_code[i-1].codeStrings[0] != 0 &&
				  s_program_code[i-1].codeStrings[0] != 0xff)
		  {
			   break;
		  }
	 }
	 return i;
}
/*********************************************
 * 函数功能：	获得第row行代码
 * 输入参数：	row	行 从0开始
 * 				code	代码文本首指针
 * 				注意：代码文本包含了最多COLUMNS_IN_LINE(18)个字符的代码,要获得全部数据，
 * 				请使用一个长度为18的unsigned char类型的数组，如code[18]；
 * 				然后将数组指针首地址作为参数输入：GetCodeTxt(row,code);
 */
void GetRowCode(unsigned int row,unsigned char* codeStrings)
{
	 unsigned int i;
	 for(i=0;i<COLUMNS_IN_LINE;i++)
	 {
		  *(codeStrings+i) = s_program_code[row].codeStrings[i];
	 }
}
/*********************************************
 * 函数功能：	获得已保存程序数量
 * 输入参数：	none
 * 返回值：		已保存程序的数量（范围0~MAX_PROGRAM_NUM）
 */
unsigned char GetProgramNum(void)
{
	 unsigned char num;
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//获得程序个数
	 if(num == 0xff) return 0;//0xff 表示第一次使用，Flash全部值为0xff，故返回0
	 return num;
}
/*********************************************
 * 函数功能：	获得对应编号的程序的名字
 * 输入参数：	num：	程序名编号（不能大于目前已保存的程序数量）
 * 				name：	程序名首指针（）
 * 				说明：	程序是按顺序连续排列的，从0开始。
 * 返回值：		0：成功		1：失败(无此程序名)
 */
unsigned char GetProgramName(unsigned char prg_num,unsigned char *name)
{
	 unsigned char i,num;
	 struct ProgramInfo prg_info;
	 memset(&prg_info,0,sizeof(prg_info));
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//获得程序个数
	 if(num == 0xff) return 1;
	 if(prg_num>num)
	 {
		  return 1;
	 }
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1+sizeof(prg_info)*(prg_num),sizeof(prg_info),0,(unsigned char*)&prg_info);
	 for(i=0;i<PROGRAM_NAME_LENGTH;i++)
	 {
		  *(name+i) = prg_info.prgName[i];
	 }
	 return 0;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/**************************************
 * 函数功能：提取当前行的位置，已经进行语法检查（如果此行存在有P点位置Pxx的话，示教使用）
 * 输入参数：row->行
 * 			 num->指向p_num的指针
 * 返回值：0->成功	0x91->此位置已被使用，询问是否覆盖  其他非0：错误(查阅错误代号)
 */
unsigned int GetPositionNumInRow(unsigned int row,unsigned int* num)
{
	 unsigned int i,sign,p_num;
	 sign = CheckGrammar(&s_program_code[row]);
	 if((sign==code_mov) || (sign == code_mvh) || (sign==code_mvs) ||
			 (sign==code_p_x) || (sign==ERR_P_NO_VALUE))
	 {
		  p_num = StringToUint((unsigned char*)&s_program_code[row]);//从row行中获取P值，P后面的值
		  *num = p_num;
		  i=0;
		  while(s_position_pulse[i].usedFlag)
		  {
			   if(s_position_pulse[i].position == p_num)//有重复的P位置将会覆盖掉
			   {
					return WARN_DUPLI_P;//  WARN_DUPLI_P
			   }
			   i++;
			   if(i>=MAX_POSITION_NUM)
			   {
					return ERR_NO_P;//一般情况下不会到这里，写这句只是预防进入死循环
			   }
		  }
		  sign =0;
	 }
	 return sign;
}


/**************************************
 * 函数功能：捕获当前位置脉冲数据到第row行的P位置中保存，已经进行语法检查（如果此行存在有P点位置Pxx的话，示教使用）
 * 			(P位置在数组中是按顺序存储的，读的时候可以按顺序读取。先读P位置个数n，再循环n次分别读出)
 * 输入参数：row：行
 * 返回值：0：成功	非0：错误(查阅错误代号)
 */
unsigned char CapturePositionDataInRow(unsigned int row)
{
	 unsigned int i,sign,p_num;
	 sign = CheckGrammar(&s_program_code[row]);
	 if((sign==code_mov) || (sign == code_mvh) || (sign==code_mvs) ||
			 (sign==code_p_x) || (sign==ERR_P_NO_VALUE))
	 {
		  p_num = StringToUint((unsigned char*)&s_program_code[row]);//从row行中获取P值，P后面的值
		  i=0;
		  while(s_position_pulse[i].usedFlag)
		  {
			   if(s_position_pulse[i].position == p_num)//有重复的P位置将会覆盖掉
			   {
					//ReadCurrentPulseEp(s_position_pulse[i].pulse);
					ReadCurrentPulseLp(s_position_pulse[i].pulse);//修正于//2018/06/01
					return 0;
					//return WARN_DUPLI_P;//  WARN_DUPLI_P
			   }
			   i++;
			   if(i>=MAX_POSITION_NUM)
			   {
					return ERR_NO_P;//一般情况下不会到这里，写这句只是预防进入死循环
			   }
		  }
		  s_position_pulse[i].usedFlag = 1;//标记此P位置已经被使用
		  s_position_pulse[i].position = p_num;//保存P位置值
		  //ReadCurrentPulseEp(s_position_pulse[i].pulse);
		  ReadCurrentPulseLp(s_position_pulse[i].pulse);//修正于//2018/06/01
		  sign =0;
	 }
	 return sign;
}

/******************************************
 * 函数功能：将脉冲值保存到p_num
 */
void SavePositionData(unsigned char p_num,long* pulse)
{
	unsigned char i=0,j=0;
	while(s_position_pulse[i].usedFlag)
	{
	   if(s_position_pulse[i].position == p_num)//有重复的P位置将会覆盖掉
	   {
		   break;
	   }
	   i++;
	}

	for(j=0;j<6;j++)
	{
		s_position_pulse[i].pulse[j] = *(pulse+j);
	}
	s_position_pulse[i].usedFlag = 1;//标记此P位置已经被使用
	s_position_pulse[i].position = p_num;//保存P位置值
}

/******************************************
 * 函数功能：将P位置标记为已使用
 */
void SavePositionState(unsigned char p_num)
{
	unsigned char i=0;
	while(s_position_pulse[i].usedFlag)
	{
	   if(s_position_pulse[i].position == p_num)//有重复的P位置将会覆盖掉
	   {
		   break;
	   }
	   i++;
	}
	s_position_pulse[i].usedFlag = 1;//标记此P位置已经被使用
	s_position_pulse[i].position = p_num;//保存P位置值
}

/**************************************
 * 函数功能：检测有没有P位置，已经进行语法检查
 * 输入参数：row  行
 * 返回值：	 0：没有发现错误
 * 			 0x91(WARN_DUPLI_P) 发现有重复P位置，用来判断是否覆盖
 * 			 其他：错误(查阅错误代号)
 */
unsigned char CheckPnum(unsigned int row)
{
	 unsigned int sign,i,p_num;
	 sign = CheckGrammar(&s_program_code[row]);
	 if((sign==code_mov) || (sign == code_mvh) || (sign==code_mvs))
	 {
		  p_num = StringToUint((unsigned char*)&s_program_code[row]);//从row行中获取P值，P后面的值
		  i=0;
		  while(s_position_pulse[i].usedFlag)
		  {
			   if(s_position_pulse[i].position == p_num) return WARN_DUPLI_P;//有重复的P位置
			   i++;
		  }
	 }
	 return sign;
}
/**************************************
 * 函数功能：删除Pxx的位置（如果Pxx存在的话）xxx
 * 			(P位置在数组中是按顺序存储的，读的时候可以按顺序读取。先读P位置个数n，再循环n次分别读出)
 * 输入参数：row：行
 * 返回值：0：成功	非0：错误
 */
unsigned char DeletePosition(unsigned char p_num)
{
	 //删除后，后面补上，保持在数组中按顺序保存
	 return 0;
}
/**************************************
 * 函数功能：获得当前每轴角度转换好的的字符串
 * 输入参数：angle_strings	字符串首指针
 * 			 字符串格式：angle_strings[6][8] 6行8列，每一行代表一个轴的数据，单位：度
 * 			 例如,20.123度，会返回"\0\020.123"
 * 			 	  -1.230读，会返回"\0\0-1.230"
 * 			 	  使用时，建议从右往左显示，遇到\0结束，能够提高效率
 * 返回值：  none
 */
void GetCurrentAngleStrings(unsigned char* angle_strings)
{
	 double angle[6];
	 GetCurrentAngle(angle);
	 AngleToStrings(angle,angle_strings);
}

/**************************************
 * 函数功能：获得Pxx点位置每轴角度（转换好的字符串）
 * 输入参数：p_num	p位置的值（如p11，就输入11，而不是p11)
 * 			 p_to_angle	指向字符串的指针
 * 			 字符串格式：angle_strings[6][8] 6行8列，每一行代表一个轴的数据，单位：度
 * 			 例如,20.123度，会返回"\0\020.123"
 * 			 	   -1.230读，会返回"\0\0-1.230"
 * 			 	   使用时，建议从右往左显示，遇到\0结束，能够提高效率
 * 返回值：	 0：成功	1：P点不存在
 */
unsigned char GetPnumAngleStrings(unsigned char p_num,unsigned char* angle_strings)
{
	 unsigned char i;
	 double angle[6];
	 for(i=0;i<MAX_POSITION_NUM;i++)
	 {
		  //进行(s_position_pulse[i].usedFlag == 1)判断是因为，s_position_pulse[i].position是肯定存在
		  //有值的，所以，不判断的话，会出现读到P位置空值的情况，例如s_position_pulse[0].position
		  if((s_position_pulse[i].position == p_num)
				&& (s_position_pulse[i].usedFlag == 1))//
		  {
					PulseToAngle(s_position_pulse[p_num].pulse,angle);
					AngleToStrings(angle,angle_strings);
					return 0;
		  }
	 }
	 return 1;
}

/**************************************
 * 函数功能：获得p位置数组的第num个成员的每轴的角度（转换好的字符串，P位置管理页面用）
 * 输入参数：num	将保存p位置数组的第num个数据读出
 * 			 p_name p的名字，如p01,p11,字符类型，长度为3
 * 			 p_to_angle	指向字符串的指针
 * 			 字符串格式：angle_strings[6][8] 6行8列，每一行代表一个轴的数据，单位：度
 * 			 例如,20.123度，会返回"\0\020.123"
 * 			 	   -1.230读，会返回"\0\0-1.230"
 * 			 	   使用时，建议从右往左显示，遇到\0结束，能够提高效率
 * 返回值：	 0：成功	1：P点不存在
 */
unsigned char GetPositionAngleStrings(unsigned char num,unsigned char* p_name,unsigned char* angle_strings)
{
	 double angle[6];
	 if((s_position_pulse[num].usedFlag == 1))//
	 {
		  *(p_name+0) = 'p';
		  *(p_name+1) = s_position_pulse[num].position / 10 + '0';
		  *(p_name+2) = s_position_pulse[num].position % 10 + '0';
		  PulseToAngle(s_position_pulse[num].pulse,angle);
		  AngleToStrings(angle,angle_strings);
		  return 0;
	 }
	 return 1;
}
/**************************************
 * 函数功能：获得Pxx点位置每轴的脉冲值
 * 输入参数：p_num	p位置的值（如p11，就输入11，而不是p11)
 * 			 pulse	指向脉冲值的指针，long类型
 * 返回值：	 0：成功	ERR_P_NO_VALUE：P点不存在
 */
unsigned char GetPxxPulseValue(unsigned char p_num,long* pulse)
{
	 unsigned char i,j;

	 for(i=0;i<MAX_POSITION_NUM;i++)
	 {
		  //进行(s_position_pulse[i].usedFlag == 1)判断是因为，s_position_pulse[i].position是肯定存在
		  //有值的，所以，不判断的话，会出现读到P位置空值的情况，例如s_position_pulse[0].position==0时
		  if(s_position_pulse[i].position == p_num)//
		  {
			    if(s_position_pulse[i].usedFlag == 1)
			    {
					for(j=0;j<6;j++)
					{
						 *(pulse+j) = s_position_pulse[i].pulse[j];
					}
					return 0;
			    }
			    else
			    {
			    	return ERR_P_NO_VALUE;//P点位置不存在
			    }
		  }
	 }
	 return ERR_P_NO_VALUE;//P点位置不存在
}
/*************************************
 * 函数功能：获得目前已被使用的P位置个数
 * 输入参数：none
 * 返回值：	 已被使用的P位置个数
 */
unsigned char GetPositionUsedNum(void)
{
	 unsigned char i=0;
	 while(s_position_pulse[i].usedFlag)//因为P位置是顺序存储的
	 {
		 i++;
	 }
	 return i;
}

/**************************************
 * 函数功能: 检查Pxx位置使用状态
 * 输入参数: p_num	P位置值（Pxx中的xx，如P11的11）
 * 返回值：	 1：P位置已被使用	0：P位置未被使用
 */
unsigned char CheckPositionState(unsigned char p_num)
{
	 unsigned char i;
	 for(i=0;i<MAX_POSITION_NUM;i++)
	 {
		  if(s_position_pulse[i].position == p_num)
		  {
			    if(s_position_pulse[i].usedFlag == 1)
			    {
			    	return 1;
			    }
			    else
			    {
			    	return 0;
			    }
		  }
	 }
	 return 0;
}

/**************************************
 * 函数功能：从第row行获得P位置值（如果此行存在有P点位置Pxx，就将xx的值提取出来，
 * 			 如P11就获得11，比较适合示教使用）
 * 输入参数：row：行
 * 返回值：	 P位置值
 */
unsigned char GetRowPositionValue(unsigned int row)
{
	 unsigned int i,n1,n2;
	 for(i=0;;i++)
	 {
		  if(s_program_code[row].codeStrings[i] == 'p')
		  {
			   break;
		  }
	 }
	 n1 = s_program_code[row].codeStrings[i+1]-'0';
	 n2 = s_program_code[row].codeStrings[i+2]-'0';
	 if((n2>='0') && (n2<='9'))
	 {
		  return n1*10 + n2;
	 }
	 return n1;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/***************************************
 * 函数功能：修改第row行第column列的代码
 * 输入参数：row	行	（范围0~255）
 * 			 column	列	 (范围0~14)
 * 			 Character	字符
 */
void IncreaseColumnChar(unsigned int row,unsigned char column,unsigned char character)
{
	 unsigned int i;
	 for(i=COLUMNS_IN_LINE-1;i<column;i--)
	 {
		  s_program_code[row].codeStrings[i] = s_program_code[row].codeStrings[i-1];
	 }
	 s_program_code[row].codeStrings[column] = character;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

/***************************************
 * 函数功能：在第row行增加一行空白行（也就是回车的时候,增加一行）
 * 输入参数：row	行	（范围0~749）
 * 返回值：	 0：成功	1：失败（>=749行）
 */
unsigned char IncreaseEmptyRow(unsigned int row)
{
	 unsigned int i;
	 if(row > (ROWS_IN_PROGRAM-1))return 1;			//判断有没有超过最大行

	 //if(s_program_code[ROWS_IN_PROGRAM-1].codeStrings[0]!=0)//判断最后一行有没有数据（非0）
	 if(s_program_code[ROWS_IN_PROGRAM-1].codeStrings[0]!=0 &&
		 s_program_code[ROWS_IN_PROGRAM-1].codeStrings[0]!=0xff	)
	 {
		  return 1;
	 }
	 for(i=ROWS_IN_PROGRAM-1;i>row;i--)
	 {
		  s_program_code[i] = s_program_code[i-1];
	 }
	 for(i=0;i<COLUMNS_IN_LINE;i++)
	 {
		  s_program_code[row].codeStrings[i] = 0;
	 }
	 return 0;
}
/***************************************
 * 函数功能：删除程序第row行第column列的字符
 * 输入参数：row		第row行（row范围：0~255）
 * 			 column		第column列（范围：0~13）
 */
void DeleteColumnChar(unsigned int row,unsigned char column)
{
	 unsigned int i;
	 if(s_column == 0)	return;
	 for(i=column;i<COLUMNS_IN_LINE-1;i++)
	 {
		  s_program_code[row].codeStrings[i] = s_program_code[row].codeStrings[i+1];
	 }
	 s_program_code[row].codeStrings[COLUMNS_IN_LINE-1] = 0;
	 s_column--;		//光标退一格
}
/***************************************
 * 函数功能：删除某一行的代码文本
 * 输入参数：row	将被删除的第row行代码（row范围：0~255）
 */
void ClearRowChar(unsigned int row)
{
	 unsigned int i;
	 for(i=0;i<COLUMNS_IN_LINE;i++)
	 {
		  s_program_code[row].codeStrings[i] = 0;
	 }
	 s_column = 0;		//光标回到第一列
}
/***************************************
 * 函数功能：删除程序的第row行
 * 输入参数：row	将被删除的第row行（row范围：0~255）
 * 			 和DeleteRowCodeTxt（）函数的区别：DeleteRowCodeTxt
 * 			 只是删除文本，行是保留的，只不过是空行；而DeleteRowCode（）是删除
 * 			 第row行，并且后面的行会向前平移1个单位
 */
void DeleteRow(unsigned int row)
{
	 unsigned int i;
	 //if(s_column == 0)	return;
	 for(i=row;i<ROWS_IN_PROGRAM-1;i++)
	 {
		  s_program_code[i] = s_program_code[i+1];
	 }
	 for(i=0;i<COLUMNS_IN_LINE;i++)
	 {
		  s_program_code[ROWS_IN_PROGRAM-1].codeStrings[i] = 0;//最后一行（第255行）清零
	 }
	 //memset(&s_program_code[ROWS_IN_PROGRAM-1],0,sizeof(&s_program_code[ROWS_IN_PROGRAM-1]));
	 //s_column = COLUMNS_IN_LINE-1;
	 s_row--;			//光标位置
	// ModifyCursorPosition(s_row,s_column);//移动光标
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/***************************************
 * 函数功能：修改光标位置（点击程序编辑区域时用）
 * 输入参数：row	行	（范围0~14）
 * 			 column	列	（范围0~13）
 */
void ModifyCursorPosition(unsigned int row,unsigned char column)
{
	int i;
	 unsigned int j;
	 for(j=row;;j--)
	 {
		  for(i=column;i>=0;i--)
		  {
			   if(s_program_code[s_row].codeStrings[i] != 0)
			   {
					//s_row = page*ROWS_IN_PAGE+j;
					s_row = j;
					s_column = i;
					return ;
			   }
		  }
		  if(j==0)
		  {
				s_row = 0;
				s_column = 0;
			    return ;
		  }
	 }
}
/***************************************
 * 函数功能：修改当前页码（点击翻页按钮时用）
 * 输入参数：page	页码
 */
void ModifyPage(unsigned char page)
{
	 if(page>(ROWS_IN_PROGRAM/ROWS_IN_PAGE+1))//255/15=17
	 {
		  s_page = 17;
	 }
	 else
	 {
		  s_page = page;
	 }
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/**************************************
 * 函数功能：判断此程序是否存在
 * 输入参数：prg_name	程序名首指针
 * 返回值：	 0：程序存在
 * 			 1：无此程序
 * 			 2：flash出错
 */
unsigned char IsProgramValid(unsigned char* prg_name)
{
	 unsigned char num,err;
	 int i;
	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));

	 //检查名字
	 err = CheckPrgName((char*)prg_name);
	 if(err) return 1;

	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//获得程序个数
	 if(num > MAX_PROGRAM_NUM) return 2;
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//将程序信息读出
	 for(i=0;i<num;i++)
	 {
		  if(!StringCmp(PROGRAM_NAME_LENGTH,prg_info[i].prgName,prg_name))
		  {
			   return 0;
		  }
	 }
	 return 1;
}


/**************************************
 * 新建程序
 * 参数		name	程序名首指针，名字最长19个字符
 * 			mdoe:	0->新建时清空存储区域	1->新建时不清空存储区域
 * 返回值	0x00：成功
 * 			0x01：失败(擦除信息块失败)
 * 			0x03：失败（名字重复）
 * 			0x04: 名字过长（19个字符以内）
 * 			0xff：失败（超过最大程序个数）
 */
unsigned char NewProgram(unsigned char* name,unsigned char mode)
{
	 unsigned char j;
	 int i;
	 unsigned char err=0,num=0,flag=0,byte=0;
	 long crc[2];
	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));

	 //检查名字
	 err = CheckPrgName((char*)name);
	 if(err) return 4;

	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//获得程序个数
	 if((num==0xff) || (num==0))//0xff表示这是第一次使用，还没有保存过,所以程序个数为0
	 {
		  num =0;
		  //memset(prg_info,0,sizeof(prg_info));
	 }
	 if(num >= MAX_PROGRAM_NUM) return 0xff;				//超过最大程序个数，不能再新建
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info[0])*num,0,(unsigned char*)prg_info);//将程序信息读出
	 for(i=0;i<num;i++)
	 {
		  if(!StringCmp(PROGRAM_NAME_LENGTH,prg_info[i].prgName,name))
		  {
			  return 3;//程序名重复
		  }
	 }

	 for(i=4;i<(MAX_PROGRAM_NUM*2+40);i++)	//从第5块开始存数据100*2=200（最多可以建MAX_PROGRAM_NUM个程序）
	 {
		  ReadNandFlashData(i,0,0,1,0,&byte);//获得块的page0的首个byte
		  if(byte == 0xff)					  	//0xff：未使用过  非0xff：被使用
		  {
			   if(!EraseNandFlashBlock(i))	//如果擦除成功
			   {
					flag++;
			   }
			   else
			   {
					flag = 0;				 	//重新开始
					WriteNandFlashData(i,0,0,1,0,0x00);	//第一页第一byte写入非0xff，标记这块已用(因为坏块不能再用）
			   }
		  }
		  else
		  {
			   flag = 0;					 	//重新开始
		  }
		  if(flag >= 2)//获得了连续两块的空块（一块保存代码，一块保存脉冲数据）
		  {
				/*if(!NandFlashCopyBack(INFO_BLOCK_ADDR*64,0,CACHE_BLOCK_ADDR*64,0))//备份第0块page0信息
				{
					 if(!NandFlashCopyBack(INFO_BLOCK_ADDR,0,CACHE_BLOCK_BACK_ADDR,0))
					 {
						  return 1;//内存出错
					 }
				}*/
			   prg_info[num].blockAddr = i-1;							//程序保存的所在块地址
			   for(j=0;j<PROGRAM_NAME_LENGTH;j++)
			   {
					prg_info[num].prgName[j] = *(name+j);				//程序的名字
					if(!*(name+j)) break;
			   }
			   if(mode == 0)
			   {
				   memset(s_program_code,0,sizeof(s_program_code));		//清零
				   memset(s_position_pulse,0,sizeof(s_position_pulse)); 	//清零
			   }
			   crc[0] = 0;
			   crc[1] = 0;

			   err = 0;
			   //保存校验位
			   err |= WriteNandFlashData(prg_info[num].blockAddr,CRC_PAGE_ADDR,CRC_BYTE_ADDR,sizeof(crc),1,(unsigned char*)&crc);
			   //确保新建程序保存所在块中，全部值为0；否则，打开程序后，未被保存的行的值全部为0xff
			   err  = WriteNandFlashData(prg_info[num].blockAddr,0,0,sizeof(s_program_code),0,(unsigned char*)s_program_code);
			   err |= WriteNandFlashData(prg_info[num].blockAddr+1,0,0,sizeof(s_position_pulse),1,(unsigned char*)s_position_pulse);
			   err |= EraseNandFlashBlock(INFO_BLOCK_ADDR);			//擦除信息块
			   num++;													//程序个数加1
			   err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);	//第一个byte保存的是程序个数
			   err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//保存
			   ClearReturnStack();//清除return堆栈
			   break;													//退出for循环
		  }
	 }
	 return err;
}

/******************
 * 函数功能：读取程序和位置
 * 返回值：0：成功
 * 		   1：读取出错，校验失败
 */
unsigned char ReadPrgPos(unsigned int block)
{
	long crc_old[2],crc_new[2];
	int j,k,tmp;

	memset(s_program_code,0,sizeof(s_program_code));		//清零
	memset(s_position_pulse,0,sizeof(s_position_pulse)); 	//清零
	ReadNandFlashData(block,CRC_PAGE_ADDR,CRC_BYTE_ADDR,sizeof(crc_old),1,(unsigned char*)crc_old);
	ReadNandFlashData(block,0,0,sizeof(s_program_code),0,(unsigned char*)s_program_code);
	ReadNandFlashData(block+1,0,0,sizeof(s_position_pulse),1,(unsigned char*)s_position_pulse);

	//校验计算，8bit数据全部相加
	crc_new[0] = 0;
	for(j=0;j<ROWS_IN_PROGRAM;j++)
	{
		 for(k=0;k<COLUMNS_IN_LINE;k++)
		 {
			 crc_new[0] += s_program_code[j].codeStrings[k];
		 }
	}
	if(crc_old[0] != crc_new[0])
	{
		 if(crc_old[0] != 0xffffffff)
		 {
				memset(s_program_code,0,sizeof(s_program_code));		//清零
				memset(s_position_pulse,0,sizeof(s_position_pulse)); 	//清零
				return 1;
		 }
	}

	tmp = sizeof(s_position_pulse[0]);
	crc_new[1] = 0;
	for(j=0;j<MAX_POSITION_NUM;j++)
	{
		 for(k=0;k<tmp;k++)
		 {
			 crc_new[1] += *(((unsigned int*)&s_position_pulse[j])+k);
		 }
	}
	if(crc_old[1] != crc_new[1])
	{
		 if(crc_old[1] != 0xffffffff)
		 {
			 memset(s_program_code,0,sizeof(s_program_code));		//清零
			 memset(s_position_pulse,0,sizeof(s_position_pulse)); 	//清零
			 return 1;
		 }
	}
	return 0;
}
/**************************************
 * 函数功能：打开程序文本和位置数据
 * 输入参数：name	程序名首指针
 * 		   ：num		指向第几个程序的指针
 * 返回值：	 0：打开成功
 * 			 1: 打开失败，读取出错（校验失败）
 * 			 2：打开失败（无此程序）
 */
unsigned char OpenProgram(unsigned char* name,unsigned int* num)
{
	 unsigned char n,err;
	 int i;

	 err = CheckPrgName((char*)name);
	 if(err) return 2;

	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&n);//获得程序个数
	 if(n > MAX_PROGRAM_NUM) return 1;
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//将程序信息读出
	 for(i=0;i<n;i++)
	 {
		  if(!StringCmp(PROGRAM_NAME_LENGTH,prg_info[i].prgName,name))
		  {
			   *num = i+1;
			   ClearTrayData();//清零托盘数据
			   ClearReturnStack();//清除return堆栈

			   if(ReadPrgPos(prg_info[i].blockAddr))
			   {
				   if(ReadPrgPos(prg_info[i].blockAddr))
				   {
					   if(ReadPrgPos(prg_info[i].blockAddr))
						   return 1;//读取出错，校验失败
				   }
			   }
			   return 0;
		  }
	 }
	 return 2;//无此程序
}


/**************************************
 * 保存程序文本和位置数据
 * 函数功能：保存程序和位置到相应文件夹
 * 输入参数：name 文件夹名字首指针
 * 返回值：	 0：保存成功
 * 			 1：保存失败(保存出错，可能Flash损坏)
 * 			 2: 保存失败(无此文件夹)
 */
unsigned char SaveProgram(unsigned char* name)
{
	 unsigned char i,err=0,num,block,flag=0,byte=0;;
	 int j,k,tmp,prg_num;
	 long crc[2];
	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//获得程序个数
	 if(num > MAX_PROGRAM_NUM) return 1;
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//将程序信息读出
	 for(i=0;i<num;i++)
	 {
		  if(!StringCmp(PROGRAM_NAME_LENGTH,prg_info[i].prgName,name))
		  {
			   block = prg_info[i].blockAddr;
			   prg_num = i;
			   break;
		  }
	 }
	 if(i==num) return 2;

	 //校验计算，8bit数据全部相加
	 crc[0] = 0;
	 for(j=0;j<ROWS_IN_PROGRAM;j++)
	 {
		 for(k=0;k<COLUMNS_IN_LINE;k++)
		 {
			 crc[0] += s_program_code[j].codeStrings[k];
		 }
	 }

	 tmp = sizeof(s_position_pulse[0]);
	 crc[1] = 0;
	 for(j=0;j<MAX_POSITION_NUM;j++)
	 {
		 for(k=0;k<tmp;k++)
		 {
			 crc[1] += *(((unsigned int*)&s_position_pulse[j])+k);
		 }
	 }
	 err  = EraseNandFlashBlock(block);
	 err |= EraseNandFlashBlock(block+1);

	 err |= WriteNandFlashData(block,CRC_PAGE_ADDR,CRC_BYTE_ADDR,sizeof(crc),1,(unsigned char*)&crc);//保存校验位

	 err |= WriteNandFlashData(block,0,0,sizeof(s_program_code),0,(unsigned char*)s_program_code);
	 err |= WriteNandFlashData(block+1,0,0,sizeof(s_position_pulse),1,(unsigned char*)s_position_pulse);

	 if(err)//如果保存出错，再来一次
	 {
		 err  = EraseNandFlashBlock(block);
		 err |= EraseNandFlashBlock(block+1);

		 err |= WriteNandFlashData(block,CRC_PAGE_ADDR,CRC_BYTE_ADDR,sizeof(crc),1,(unsigned char*)&crc);//保存校验位

		 err |= WriteNandFlashData(block,0,0,sizeof(s_program_code),0,(unsigned char*)s_program_code);
		 err |= WriteNandFlashData(block+1,0,0,sizeof(s_position_pulse),1,(unsigned char*)s_position_pulse);
	 }

	 if(err)//如果还出错证明块已经坏了
	 {
		 for(i=4;i<(MAX_PROGRAM_NUM*2+40);i++)	//从第5块开始存数据100*2=200（最多可以建MAX_PROGRAM_NUM个程序）
		 {
			  ReadNandFlashData(i,0,0,1,0,&byte);//获得块的page0的首个byte
			  if(byte == 0xff)					  	//0xff：未使用过  非0xff：被使用
			  {
				   if(!EraseNandFlashBlock(i))	//如果擦除成功
				   {
						flag++;
				   }
				   else
				   {
						flag = 0;				 	//重新开始
						WriteNandFlashData(i,0,0,1,0,0x00);	//第一页第一byte写入非0xff，标记这块已用(因为坏块不能再用）
				   }
			  }
			  else
			  {
				   flag = 0;					 	//重新开始
			  }
			  if(flag >= 2)//获得了连续两块的空块（一块保存代码，一块保存脉冲数据）
			  {
				   err = 0;
				   prg_info[prg_num].blockAddr = i-1;							//程序保存的所在块地址
				   err |= EraseNandFlashBlock(INFO_BLOCK_ADDR);			//擦除信息块												//程序个数加1
				   err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);	//第一个byte保存的是程序个数
				   err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//保存

				   err |= WriteNandFlashData(prg_info[prg_num].blockAddr,CRC_PAGE_ADDR,CRC_BYTE_ADDR,sizeof(crc),1,(unsigned char*)&crc);//保存校验位
				   err |= WriteNandFlashData(prg_info[prg_num].blockAddr,0,0,sizeof(s_program_code),0,(unsigned char*)s_program_code);
				   err |= WriteNandFlashData(prg_info[prg_num].blockAddr+1,0,0,sizeof(s_position_pulse),1,(unsigned char*)s_position_pulse);
				   break;//退出for循环
			  }
		 }
	 }
	 return err;
}
/***************************************
 * 函数功能：	删除程序
 * 输入参数：	name	将被删除的程序名字
 * 输出参数：	0：删除成功
 * 				1：删除失败（擦除信息块错误，可能flash损坏）
 * 				2: 删除失败 (无此程序)
 */
unsigned char DeleteProgram(unsigned char* prg_name)
{
	 unsigned char i,j,block;
	 unsigned char err=0,num=0;
	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//获得程序个数
	 if(num > MAX_PROGRAM_NUM) return 2;
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//将程序信息数据读出

	 for(j=0;j<num;j++)
	 {
		  if(!StringCmp(PROGRAM_NAME_LENGTH,prg_info[j].prgName,prg_name)) break;
	 }
	 if(j == num)	return 2;

	 block = prg_info[j].blockAddr;	//清零前暂存块地址
	 for(i=j;i<num-1;i++)			//被删除的位置将被后面替换(后面的整体向前移一个单位)
	 {
		  prg_info[i] = prg_info[i+1];
	 }
	 memset(&prg_info[num-1],0,sizeof(struct ProgramInfo));	//最后一个无用，清零
	 num--;											 	 		//程序个数减一
	 err = EraseNandFlashBlock(INFO_BLOCK_ADDR);	 		//擦除信息块
	 err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);		//第一个byte保存的是程序个数
	 err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//保存程序信息
	 err |= EraseNandFlashBlock(block);	 	//擦除被删除的块
	 err |= EraseNandFlashBlock(block+1);	//擦除被删除的块
	 return err;
}

/***************************************
 * 函数名：	 copy_program
 * 功能：   复制程序
 * 输入参数	prg_name	将被复制的程序名字
 * 			new_name	复制后的程序名字
 * 输出参数	0：复制成功
 * 			1：复制失败（擦除信息块错误，可能flash损坏）
 * 			2: 无此程序
 * 			3: 名字重复
 * 			4：名字过长，19个字符以内
 * 			0xff：失败（超过最大程序个数，不能再新建）
 */
unsigned char copy_program(unsigned char* prg_name,unsigned char* new_name)
{
	 unsigned char err=0;
	 unsigned int num;
	 err = NewProgram(new_name,0);
	 if(err != 0)
	 {
		 return err;
	 }
	 err = OpenProgram(prg_name,&num);
	 if(err!=0)
	 {
		 DeleteProgram(new_name);
		 return err;//无此程序
	 }
	 err = SaveProgram(new_name);
	 if(err != 0)
	 {
		 return 1;
	 }
	 return 0;
}

/***************************************
 * 函数名：	 CopyProgram
 * 功能：   复制程序，然后打开之前已经被打开的程序
 * 输入参数	prg_name	将被复制的程序名字
 * 			new_name	复制后的程序名字
 * 输出参数	0：复制成功
 * 			1：复制失败（擦除信息块错误，可能flash损坏）
 * 			2: 无此程序
 * 			3: 名字重复
 * 			4：名字过长，19个字符以内
 * 			0xff：失败（超过最大程序个数，不能再新建）
 */
unsigned char CopyProgram(unsigned char* prg_name,unsigned char* new_name)
{
	unsigned char err,err1;
	unsigned int num;
	err = copy_program(prg_name,new_name);
	if(prg_name[0]!=0 && StringCmp(PROGRAM_NAME_LENGTH,prg_name,(u8*)s_program_name_num.program_name))
	{
		err1 = OpenProgram((unsigned char*)s_program_name_num.program_name,&num);
		if(err1)
		{
			//打开失败则清空
			s_program_name_num.program_name[0] = 0;
			s_program_name_num.num = 0;
			SetTextValue(7, 82, "");
			SetTextValue(7, 83, "");
		}
	}
	return err;
}

/***************************************
 * 函数名：	 RenameProgram
 * 输入参数	old_name	被重命名的程序名字
 * 			new_name	新的名字
 * 输出参数	0：重命名成功
 * 			1：重命名失败（擦除信息块错误，可能flash损坏）
 * 			2: 重命名失败 (无此程序)
 * 			3: 名字重复
 * 			4：名字过长
 */
unsigned char RenameProgram(unsigned char* old_name,unsigned char* new_name)
{
	 unsigned char j=0,num,err=0;
	 int i;
	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));

	 err = CheckPrgName((char*)new_name);
	 if(err) return 4;

	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//获得程序个数
	 if(num > MAX_PROGRAM_NUM) return 1;//读取错误
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//将程序信息读出

	 for(i=0;i<num;i++)
	 {
		 if(!StringCmp(PROGRAM_NAME_LENGTH,prg_info[i].prgName,new_name))
		 {
			 return 3;//新建的名字重复
		 }
	 }
	 for(i=0;i<num;i++)
	 {
		  if(!StringCmp(PROGRAM_NAME_LENGTH,prg_info[i].prgName,old_name))
		  {
			   memset(prg_info[i].prgName,0,sizeof(prg_info[i].prgName));
			   while(*(new_name+j))
			   {
				   prg_info[i].prgName[j] = *(new_name+j);
				   j++;
			   }
			   err |= EraseNandFlashBlock(INFO_BLOCK_ADDR);				//擦除信息块
			   err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);	//第一个byte保存的是程序个数
			   err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//保存
			   return err;
		  }
	 }
	 return 2;
}

/*****************************************
 * 函数功能：SaveAxisNumber 保存机械手轴个数
 * 输入参数：axis_num	轴数量
 * 输出参数：0：保存成功	1：保存失败
 */
unsigned char SaveAxisNumber(unsigned char axis_num)
{
	unsigned char err=0;
	//err |= EraseNandFlashBlock(INFO_BLOCK_ADDR1);				//擦除信息块
	//err |= WriteNandFlashData(INFO_BLOCK_ADDR1,0,0,1,0,&axis_num);	//第一个byte保存的是轴个数
	return err;
}

/*****************************************
 * 函数功能：ReadAxisNumber 读机械手轴个数
 * 输入参数：NONE
 * 输出参数：机械手轴个数
 */
unsigned char ReadAxisNumber(void)
{
	unsigned char axis_num=0;
	ReadNandFlashData(INFO_BLOCK_ADDR1,0,0,1,0,&axis_num);//获得轴个数
	if((axis_num != 5) && (axis_num != 6))
	{
		axis_num = 6;
		//SaveAxisNumber(axis_num);
	}
	return axis_num;
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/*****************************************
 * 函数功能：程序单步运行
 * 输入参数：cmd:指令代码
 * 			codeStrings:代码文本
 * 返回值：	 0：成功	其他非0：错误（查阅）
 */
unsigned int ProgramStepRun_Code(unsigned char cmd,const PrgCode* codeStrings)
{
	 unsigned char err=0;
	 long nx;
	 double fx;
	 while(ProgramRuningFlag(0) == 0)
	 {
		 switch(cmd)
		 {
		 case code_dly:
			 err = ExecuteDly(*codeStrings);
			 break;
		 case code_end: ExecuteEnd(*codeStrings);break;

	//	 case code_gosub:
	//		 err = ExecuteGosub(codeStrings,row);
	//		 if(err)
	//		 {
	//			 return err;
	//		 }
	//		 else
	//		 {
	//			 return 0;
	//		 }
	//	 case code_sub:break;
	//	 case code_goto:ExecuteGoto(codeStrings,row);return 0;
	//	 case code_loop:break;
		 case code_stop:ExecuteStop(*codeStrings);break;
	//	 case code_if:ExecuteIf(codeStrings,row);return 0;
	//	 case code_then:ExecuteThen(codeStrings,row);return 0;
	//	 case code_else:ExecuteElse(codeStrings,row);return 0;
		 case code_mov:
			 err = ExecuteMov(*codeStrings,0);
			 display_sixangle(6);//全速运行时显示六个轴的角度
			 break;
		 case code_mvs:
			 err = ExecuteMvs(*codeStrings,0);
			 display_sixangle(6);//全速运行时显示六个轴的角度
			 break;
		 case code_mvr:
			 err = ExecuteMvr(*codeStrings,0);
			 display_sixangle(6);
			 err &= 0x10;
			 break;
//			 if((err&0x10))
//			 {
//				 //(*row)++;
//				 return code_mvr;
//			 }
//			 else
//			 {
//				 return err;
//			 }
		 case code_mvc:
			 err = ExecuteMvc(*codeStrings,0);
			 display_sixangle(6);
			 err &= 0x10;
			 break;
		 case code_mvh:
			 err = ExecuteMvh(*codeStrings,0);
			 display_sixangle(6);
			 break;
		 case code_n_x: 	err = ExecuteNx((unsigned char*)(codeStrings),&nx);break;
		 case code_f_x: 	err = ExecuteFx((unsigned char*)(codeStrings),&fx);break;
		 case code_ovrd: 	ExecuteOvrd(*codeStrings);break;
	//	 case code_open:
	//		 	 err = ExecuteOpen(codeStrings);
	//		 	 if(err)	return err;
	//		 	 else
	//		 	 {
	//		 		 //*row = 0;//程序打开后从第0行开始执行
	//		 		 return 0;
	//		 	 }

		 case code_pout: err = ExecutePout(*codeStrings);break;
		 case code_p_x:
				 err = ExecutePx(*codeStrings);
				  break;
		 case code_speed: ExecuteSpeed(*codeStrings);break;
		 case code_tray: 	ExecuteTray(*codeStrings);break;
		 case code_ft_x:	ExecuteFtx(*codeStrings);break;
	//	 case code_return:ExecuteReturn(codeStrings,row);return 0;
		 case code_wait:
			 err = ExecuteWait(*codeStrings);
			 break;
		 case code_acc:	ExecuteAcc(*codeStrings);break;
	//	 case code_baud: ExecuteBaud(codeStrings);break;
	//	 case code_eth: ExecuteEth(codeStrings);break;
		 case code_rstd: err = ExecuteRstd(*codeStrings); break;

		 case code_disp: err = ExecuteDisp(*codeStrings); break;

		 case code_toollen:	   ExecuteToolLength(*codeStrings); break;
		 case code_tool: ExecuteTool(codeStrings);break;
		 case code_part: ExecutePart(codeStrings);break;
		 default:return ERR_CMD;
		 }

		if(err == ERR_STOP)
		{
			if(ProgramRuningFlag(0) == 0)
			{
				ProgramRuningFlag(2);	//暂停
				SetButtonValue(6,2,1);	//“继续运行”
			}
		}
		else if(err == 0)
		{
			return 0;
		}
		else
		{
			return err;
		}
		while(ProgramRuningFlag(0) == 1)	//暂停
		{
			if(ErrCheck(6))
			{
			  G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//使用安全开关
			  ClearStopProgramFlag();//清除暂停标志位
			  //SPIA_RX_STA = 0;
			  return ERR_ALARM;
			}
			KeyBoard_StopSpeed();
		}
		if(ProgramRuningFlag(0)==2)	//退出
		{
			CloseFan();
			ClearStopProgramFlag();
			G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//使用安全开关
			//SPIA_RX_STA = 0;
			return 0;
		}
		else	//继续
		{
			ClearStopProgramFlag();
			continue;
		}
	 }
	 return 0;
}


/*****************************************
 * 函数功能：程序单步运行
 * 输入参数：row：被运行的行，从0开始
 * 			 注意：如果执行的行是跳转或子函数，那么这个值将被修改为跳到相应的行
 * 			 其他情况下是自加1，表示运行下一行
 * 			 check: 0:不带语法检查（全速）	1：带语法检查(示教)
 * 返回值：	 0：成功	其他非0：错误（查阅）
 */
unsigned int ProgramStepRun(unsigned int* row,unsigned char check)
{
	 unsigned char cmd,err;
	 long nx;
	 double fx;
	 PrgCode codeStrings;
	 GetRowCode(*row,(unsigned char*)(&codeStrings));

	 if(check)
	 {
		 cmd = CheckGrammar(&codeStrings);
		 //防止单步运行的时候ExecutePx ExecuteNx执行两次（语法检查的时候已经执行过一次了）
		 if(cmd == code_n_x)
		 {
			 (*row)++;
			 return code_n_x;
		 }

		 if(cmd == code_f_x)
		 {
			 (*row)++;
			 return code_f_x;
		 }

		 if(cmd == code_p_x)
		 {
			 (*row)++;
			 return 0;
		 }

		 if(cmd == code_baud)
		 {
			 (*row)++;
			 return 0;
		 }
	 }
	 else
	 {
		 cmd = GetCommand(&codeStrings);
	 }
	 switch(cmd)
	 {
	 case code_dly:
		 err = ExecuteDly(codeStrings);
		 if(!err)
		 {
			 (*row)++;
			 return 0;
		 }
		 else
		 {
			 return err;
		 }
	 case code_end: ExecuteEnd(codeStrings);break;

	 case code_gosub:
		 err = ExecuteGosub(codeStrings,row);
		 if(err)
		 {
			 return err;
		 }
		 else
		 {
			 return 0;
		 }
	 case code_sub:break;
	 case code_goto:ExecuteGoto(codeStrings,row);return 0;
	 case code_loop:break;
	 case code_stop:ExecuteStop(codeStrings);break;
	 case code_if:err = ExecuteIf(codeStrings,row);return err;
	 case code_then:ExecuteThen(codeStrings,row);return 0;
	 case code_else:ExecuteElse(codeStrings,row);return 0;
	 case code_mov:
		 err = ExecuteMov(codeStrings,check);
		 display_sixangle(6);//全速运行时显示六个轴的角度
		 if(!err)
		 {
			 (*row)++;
			 return code_mov;
		 }
		 else
		 {
			 return err;
		 }
	 case code_mvs:
		 err = ExecuteMvs(codeStrings,check);
		 display_sixangle(6);//全速运行时显示六个轴的角度
		 if(!err)
		 {
			 (*row)++;
			 return code_mvs;
		 }
		 else
		 {
			 return err;
		 }
	 case code_mvr:
		 err = ExecuteMvr(codeStrings,check);
		 display_sixangle(6);
		 if(!err)
		 {
			 (*row)++;
			 return code_mvr;
		 }
		 else
		 {
			 return err;
		 }
	 case code_mvc:
		 err = ExecuteMvc(codeStrings,check);
		 display_sixangle(6);
		 if(!err)
		 {
			 (*row)++;
			 return code_mvc;
		 }
		 else
		 {
			 return err;
		 }
	 case code_mvh:
		 err = ExecuteMvh(codeStrings,check);
		 display_sixangle(6);
		 if(!err)
		 {
			 (*row)++;
			 return code_mvh;
		 }
		 else
		 {
			 return err;
		 }
	 case code_n_x:
		 err = ExecuteNx((unsigned char*)(&codeStrings),&nx);
		 if(err) return err;
		 (*row)++;
		 return code_n_x;
	 case code_f_x:
		 err = ExecuteFx((unsigned char*)(&codeStrings),&fx);
		 if(err) return err;
		 (*row)++;
		 return code_f_x;
	 case code_ovrd: 	ExecuteOvrd(codeStrings);break;
	 case code_open:
		 	 err = ExecuteOpen(codeStrings);
		 	 if(err)	return err;
		 	 else
		 	 {
		 		 *row = 0;//程序打开后从第0行开始执行
		 		 return 0;
		 	 }

	 case code_pout:
		 	 err = ExecutePout(codeStrings);
		 	 if(err) return err;
		 	 break;
	 case code_p_x:
			 err = ExecutePx(codeStrings);
			 if(err) return err;
			 else  break;
	 case code_speed: ExecuteSpeed(codeStrings);break;
	 case code_tray: 	ExecuteTray(codeStrings);break;
	 case code_ft_x:	ExecuteFtx(codeStrings);break;
	 case code_return:ExecuteReturn(codeStrings,row);return 0;
	 case code_wait:
		 err = ExecuteWait(codeStrings);
		 if(!err)
		 {
			 (*row)++;
			 return 0;
		 }
		 else
		 {
			 return err;
		 }
	 case code_acc:	ExecuteAcc(codeStrings);break;
	 case code_baud: err = ExecuteBaud(codeStrings);
	 	 if(err)
	 	 {
	 		 return ERR_STM32COMM;
	 	 }
	 	 break;
	 case code_rstd: err = ExecuteRstd(codeStrings);
	 	 if(err)
	 	 {
	 		 return err;
	 	 }
	 	 break;
	 case code_disp:
		 err = ExecuteDisp(codeStrings);
		 if(err)
			 return err;
		 else
		 {
			 (*row)++;
			 return code_disp;
		 }
	 case code_rs232: err = ExecuteRS232(codeStrings);
	 	 display_sixangle(6);//全速运行时显示六个轴的角度
		 if(!err)
		 {
			 (*row)++;
			 return 0;
		 }
		 else
		 {
			 return err;
		 }
	 case code_finish: ExecuteFinish(codeStrings);break;
	 case code_toollen: ExecuteToolLength(codeStrings);break;
	 case code_tool: ExecuteTool(&codeStrings);break;
	 case code_part: ExecutePart(&codeStrings);break;
	 case code_network: ExecuteNetwork(codeStrings,row);break;//修正于//2018/06/01
	 case code_teamwork: ExecuteTeamwork(codeStrings,row);break;//修正于//2018/06/01
	 default:return cmd;
	 }
	 (*row)++;
	 return 0;
}
/*****************************************
* 函数功能：中断改变标志位，执行程序时取标志位返回值
* 输入参数：sign=0读执行状态，sign=1开始执行，sign=2，暂停运行，sign=3，退出运行
* 输出参数：0：开始运行	1：暂停运行 2：退出运行
*/
u8 ProgramRuningFlag(u8 sign)
{
	static u8 sign_pause=1;
	if(sign>0)
	{
		if(1 == sign)
		{
			StartCpuTimer0();
			sign_pause=0;//开始运行
		}
		if(2 == sign)
		{
			StopCpuTimer0();
			sign_pause=1;//暂停运行
		}
		if(3 == sign)
		{
			StopCpuTimer0();
			sign_pause=2;//退出运行
		}
	}
	return sign_pause;
}

/*****************************************
 * 函数功能：程序全速运行(带背景指示运行到哪一步)
 * 输入参数：f 显示指针 函数指针
 * 			 row 将被运行的行
 * 			 next_row  将被运行的下一行的指针
 * 返回值：  错误代码  0：正确		其他值：错误
 */
extern struct EX_Position_stru EX_POSITION;
u8 ProgramRunWithPointer(void (*f)(unsigned char),unsigned int* row)
{
	 unsigned int num,err,i;
	 *row = 0;
	 SetButtonValue(6,2,0);
	 InitSci();	//初始化串口
	 memset(&EX_POSITION,0,sizeof(EX_POSITION));
	 USART_RX_STA_B  = 0;
	 RX_POSITION_STA = 0;

	 num = GetLinesCurrentProgram();//获得程序行数
	 ProgramRuningFlag(1);
	 ClearStopProgramFlag();
	 ClearReturnStack();
	 StartCpuTimer0();	//开启定时器0，开始计时

	 //清空disp显示区域
	 for(i=0;i<6;i++)
	 {
		 SetTextValue(6, 28+i, " ");
	 }
	 G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 0;//不使用安全开关
	 if(num>0)
	 {
		 OpenFan();
		 for(;*row<num;)//死循环
		 {
			  if(ProgramRuningFlag(0) == 0)
			  {
				   f(*row);					//改变第i行背景色
				   err = ProgramStepRun(row,0);//执行第i行程序
				   if((err>=ERR_CMD)&&(err!=ERR_STOP))
				   {
					   G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//使用安全开关
					   CloseFan();
					   StopCpuTimer0();
					   ClearStopProgramFlag();
					   return err;
				   }
				   if( (err==ERR_STOP)&&(ProgramRuningFlag(0)!=2))
				   {
					   ClearStopProgramFlag();
					   ProgramRuningFlag(2);
					   delay_1ms(100);
					   StopCpuTimer0();
					   CloseFan();
					   err=0;
				   }
				   if(*row>=num) *row=0;		//到尽头，重新开始
			  }
			  else if(ProgramRuningFlag(0) == 2)//退出运行
			  {
				  CloseFan();
				  ClearStopProgramFlag();
				  StopCpuTimer0();
				 // SetButtonValue(6,2,0);//明天调试一下，解决第四个问题
				  G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//使用安全开关
				  return 0;
			  }
			  else//暂停状态
			  {
				  if(ErrCheck(6))
				  {
					  G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//使用安全开关
					  ClearStopProgramFlag();//清除暂停标志位
					  StopCpuTimer0();
					  return ERR_ALARM;
				  }
				  delay_1ms(100);//修订于//2018/06/01（前值为1）
				  KeyBoard_StopSpeed();
				  delay_1ms(100);//修订于//2018/06/01（前值为2）
				  Pause_network_function();//修正于//2018/06/01
			  }
		 }
	 }
	 G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//使用安全开关
	 ClearStopProgramFlag();
	 StopCpuTimer0();
	 return 0;
}

void Pause_network_function(void)
{
	static u16 value1=0;
	static u8 Re_Data[8]={0,0,0,0};
    static u8 Frame_Value=0;
	SendComponentNum(value1);
	delay_1ms(3);
	SpiReceiveNetData(8,Re_Data,0);
	Frame_Value=JudgeReceiveFrame(Re_Data);
	if(Frame_Value==3)
	{
		net_control_function(2,0);
		SpiSendNetdata(5,Continue_OK,0);//给STM32发送机械手继续工作命令
	}
	else if(Frame_Value==5)
	{
		net_control_function(18,0);
		SpiSendNetdata(5,Stop_Ok,0);//给STM32发送机械手停止工作命令
	}
}
/*****************************************
 * 函数功能：程序全速运行
 * 输入参数：none
 * 返回值：	 none
 */
void ProgramRun(void)
{
	//
}

/*****************************************
 * 函数功能：开始运行
 * 输入参数：row  从第row行开始
 * 返回值：
 */
/*void StartRun(void)
{
	 s_run_flag = 0;
}*/
/*****************************************
 * 函数功能：停止运行
 * 输入参数：none
 * 返回值:	 none
 */
/*void StopRun(void)
{
	 s_run_flag = 1;
}*/
/*****************************************
 * 函数功能：暂停运行
 * 输入参数：
 * 返回值
 */
/*void PuaseRun(void)
{
	 s_run_flag = 2;
}*/
/*****************************************
 * 函数功能：执行 goto指令
 * 输入参数：PrgCode类型的 代码
 */
void ExecuteGoto(PrgCode codeStrings,unsigned int* row)
{
	unsigned int i=0,j,current_lines;
	unsigned char str_tmp[COLUMNS_IN_LINE]={0};
	unsigned char code_strings[COLUMNS_IN_LINE]={0};
	while(codeStrings.codeStrings[i]!='*')
	{
		i++;
	}
	str_tmp[0] = '*';
	j=0;
	while(codeStrings.codeStrings[i]!=' ' && codeStrings.codeStrings[i]!='\0')
	{
		str_tmp[j++] = codeStrings.codeStrings[i++];
	}
	 current_lines = GetLinesCurrentProgram();//当前代码总行数
	 for(i=0;i<current_lines;i++)
	 {
		 GetRowCode(i,code_strings);
		 if(code_strings[0]=='*')
		 {
			 if(!StringCmp(COLUMNS_IN_LINE,code_strings,str_tmp))
			 {
				 *row = i;
			 }
		 }
	 }
}
/*****************************************
 * 函数功能：执行 gosub指令
 * 输入参数：PrgCode类型的 代码
 * 			 u16* row 行，将被跳去的行
 */
unsigned char ExecuteGosub(PrgCode codeStrings,unsigned int* row)
{
	unsigned int i=0,j,current_lines;
	unsigned char str_tmp[COLUMNS_IN_LINE]={0};
	unsigned char code_strings[COLUMNS_IN_LINE]={0};
	while(codeStrings.codeStrings[i]!='#')
	{
		i++;
	}
	//str_tmp[0] = '*';
	j=0;
	while(codeStrings.codeStrings[i]!=' ' && codeStrings.codeStrings[i]!='\0')
	{
		str_tmp[j++] = codeStrings.codeStrings[i++];
	}
	 current_lines = GetLinesCurrentProgram();//当前代码总行数
	 for(i=0;i<current_lines;i++)
	 {
		 GetRowCode(i,code_strings);
		 if(code_strings[0]=='#')
		 {
			 if(!StringCmp(COLUMNS_IN_LINE,code_strings,str_tmp))
			 {

				 if(s_return_stack_pointer==NULL)//如果是空指针
				 {
					 s_return_stack_pointer = s_return_stack;
				 }
				 else
				 {
					 s_return_stack_pointer++;
					 if(s_return_stack_pointer>=(s_return_stack+MAX_RETURN*sizeof(int)))
					 {
						 s_return_stack_pointer--;
						 return ERR_RETURN_OVERFLOW;
					 }
				 }
				 *s_return_stack_pointer = *row + 1;//压栈
				 *row = i;
				 break;
			 }
		 }
	 }
	 return 0;
}
/*****************************************
 * 函数功能：执行 return 指令
 * 输入参数：PrgCode类型的 代码
 */
void ExecuteReturn(PrgCode codeStrings,unsigned int* row)
{
	if(s_return_stack_pointer==NULL)//如果是空指针
	{
		*row = 0;
	}
	else
	{
		*row = *s_return_stack_pointer;
		if(s_return_stack_pointer == s_return_stack)//如果到达了堆栈的底部
		{
			s_return_stack_pointer = NULL;//设为空指针
		}
		else
		{
			s_return_stack_pointer--;
		}
	}
}
/*****************************************
 * 函数功能：清空return的堆栈
 */
void ClearReturnStack(void)
{
	s_return_stack_pointer = NULL;//设为空指针
}
/*****************************************
 * 函数功能：执行延时命令 dly x.x (秒)
 * 输入参数：PrgCode类型的 代码
 * 返回值：
 * 			0：正常运行
 * 			其他值：查阅出错代码
 */
unsigned char ExecuteDly(PrgCode codeStrings)
{
	 unsigned int i=0,j,k,err=0;
	 float value=0;
	 value = (StringToFloat((unsigned char*)&codeStrings));
	 value = value*10;
	 for(i=value;i>0;i--)
	 {
		  for(j=0;j<63;j++)
		  {
			  delay_1ms(1);
			  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
			  {
				  delay_1us(5);
				  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
				  {
					  for(k=0;k<6;k++)
					  {
					  		delay_200ns();
					  		err |= ReadRR2(0x01<<k);
					  }
					  if(err)
					  {
						   return ERR_ALARM;
					  }
				  }
			  }
			  if(KeyBoard_StopSpeed())
			  {
				   return ERR_STOP;
			  }
		  }
	 }
	 return 0;
}

/*****************************************
 * 函数功能：执行结束运行命令 end
 * 输入参数：PrgCode类型的 代码
 */
void ExecuteEnd(PrgCode codeStrings)
{
	ProgramRuningFlag(3);//停止全速运行
}

/*****************************************
 * 函数功能：执行结束运行命令 finish
 * 输入参数：PrgCode类型的 代码
 */
void ExecuteFinish(PrgCode codeStrings)
{
/*
	unsigned char i;
	unsigned short t;


	 for(t=0;t<3577;t++)
	 {

		 while(ReadDriState())//等待驱动结束
		 {
			  //delay_200ns();
			  NOP;NOP;
		 }

		 for(i=0;i<6;i++)
		 {
			 number[i]= pulse_num[t][i];
		 }
		 TranslationProccess(number,1);//平移数据处理，驱动输出脉冲//7000
	 }
*/
	 //ProgramRuningFlag(3);//停止全速运行
}

/*****************************************
 * 函数功能：执行暂停运行命令 stop
 * 输入参数：PrgCode类型的 代码
 */
void ExecuteStop(PrgCode codeStrings)
{
	ProgramRuningFlag(2);//停止全速运行
	SetButtonValue(6,2,1);//“继续运行”
}
/*****************************************
 * 函数功能：执行关节插补命令 mov
 * 输入参数：PrgCode类型的 代码
 * 			teach：0->全速	1->示教
 * 返回值：	 0：成功	 其他：查阅
 */
extern unsigned char THE_FOUR_AIXS_XUANZHUAN;
#pragma CODE_SECTION(ExecuteMov, "ramfuncs");
unsigned char ExecuteMov(PrgCode codeStrings,char teach)
{
	 unsigned char i=0,p_num,n_num,err=0,err1,i_value;
	 int speed;
	 long next_pulse[6];
	 double angle[6];
	 double f_value;
	 PositionGesture p_g;
	 while(codeStrings.codeStrings[i] != 'p' && codeStrings.codeStrings[i] != 't')
	 {
		 i++;
	 }
	 if(codeStrings.codeStrings[i] == 'p')//'p'
	 {
		 i++;
		 if( (codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')
				 || (codeStrings.codeStrings[i] == 'n') )//pxx的情况
		 {
			 if(codeStrings.codeStrings[i] == 'n')//pnx的情况
			 {
				 i++;
				 n_num = StringToUint((unsigned char*)&codeStrings);
				 p_num = s_Nx_variable[n_num];
				 if(p_num >= MAX_POSITION_NUM) return ERR_P_OVERFLOW;
				 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//P位置值不能为空
			 }
			 else
			 {
				 p_num = StringToUint((unsigned char*)&codeStrings);
			 }

			 err = GetPxxPulseValue(p_num,next_pulse);
			 PulseToAngle(next_pulse,angle);
			 while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//跳过数字
			 {
				 i++;
			 }
			 while(codeStrings.codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
		 }
		 else
		 {
			 //ReadCurrentPulseEp(next_pulse);
			 ReadCurrentPulseLp(next_pulse);//修正于//2018/06/01
		 }

		 if(codeStrings.codeStrings[i] == '.')
		 {
			 i++;	//跳过小数点
			 if(codeStrings.codeStrings[i] == 'j')//.jx
			 {
				 i++;//跳过j
				 i_value = StringToUint(&codeStrings.codeStrings[i]);//提取旋转轴
				 i++;//跳过x
				 while(codeStrings.codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 if(codeStrings.codeStrings[i] == '+')
				 {
					 f_value = StringToFloat(&codeStrings.codeStrings[i]);//提取旋转的角度
				 }
				 else// '-'
				 {
					 f_value = -StringToFloat(&codeStrings.codeStrings[i]);
				 }
				 SingleAxisAngleChange(i_value,f_value,next_pulse);//PulseToAngle_Single
			 }
			 else//.x/y/z
			 {
				 f_value = StringToFloat(&codeStrings.codeStrings[i]);
				 p_g = GetPositionGesture(angle);//获得正解
				 if(codeStrings.codeStrings[i] == 'x')
				 {
					 i++;	//跳过x
					 while(codeStrings.codeStrings[i] == ' ')//跳过空格
					 {
						 i++;
					 }
					 if(codeStrings.codeStrings[i++] == '+')	//顺便跳过+/-
					 {
						 p_g.px = p_g.px + f_value;
					 }
					 else// '-'
					 {
						 p_g.px = p_g.px - f_value;
					 }
				 }
				 else if(codeStrings.codeStrings[i] == 'y')
				 {
					 i++;
					 while(codeStrings.codeStrings[i] == ' ')//跳过空格
					 {
						 i++;
					 }
					 if(codeStrings.codeStrings[i++] == '+')
					 {
						 p_g.py = p_g.py + f_value;
					 }
					 else// '-'
					 {
						 p_g.py = p_g.py - f_value;
					 }
				 }
				 else//'z'
				 {
					 i++;
					 while(codeStrings.codeStrings[i] == ' ')//跳过空格
					 {
						 i++;
					 }
					 if(codeStrings.codeStrings[i++] == '+')
					 {
						 p_g.pz = p_g.pz + f_value;
					 }
					 else// '-'
					 {
						 p_g.pz = p_g.pz - f_value;
					 }
				 }
				 ModifyPositionGesture(p_g);//更新正解
				 if(GetBestSolution(angle))//获得逆解
					 return ERR_OUT_OF_RANGE;//超出动作范围
				 AngleToPulse(angle,next_pulse);
			 }

			 while(codeStrings.codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 while((codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9') ||
					 (codeStrings.codeStrings[i]=='.'))//跳过数字或小数点
			 {
			 	 i++;
			 }
			 while(codeStrings.codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//如果有speed
			 {
				 speed = atoi(((char*)&codeStrings)+i);
				 if(speed>0 && speed<=100)
				 {
					 if(teach == 0)//全速运行时才执行，单步示教不执行
					 {
						 ModifySpeed(speed);
					 }
				 }
			 }
		 }
		 else
		 {
			 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//如果有speed
			 {
				 speed = atoi(((char*)&codeStrings)+i);
				 if(speed>0 && speed<=100)
				 {
					 if(teach == 0)//全速运行时才执行，单步示教不执行
					 {
						 ModifySpeed(speed);
					 }
				 }
			 }
		 }
	 }
	 else//'t'
	 {
		 err = ExecuteTx(((unsigned char*)&codeStrings)+i,next_pulse);
	 }

	 if(!err)
	 {
		  err1 = JointInterpolation(next_pulse);
		  THE_FOUR_AIXS_XUANZHUAN = 0;//第四轴旋转标志为0
		  if(err1 == 0)//关节插补运动到目标位置
		  {
			  return 0;
		  }
		  else if(err1 == 1)
		  {
			  return ERR_ALARM;
		  }
		  else if(err1 == 2)
		  {
			  return ERR_SAVE_BTN;
		  }
		  else
		  {
			  return ERR_STOP;
		  }
	 }
	 else
	 {
		  return err;
	 }
}
/*****************************************
 * 函数功能：执行直线插补命令 Mvs p0.x + 10
 * 输入参数：PrgCode类型的 代码
 * 返回值：0：成功	其他值：失败
 */
#pragma CODE_SECTION(ExecuteMvs, "ramfuncs");
unsigned char ExecuteMvs(PrgCode codeStrings,char teach)
{
	 unsigned char i=0,p_num,err=0,err1,n_num;
	 unsigned char flg=0;//记录是否需要修正
	 int speed;
	 long next_pulse[6],current_pulse[6];
	 double angle[6];
	 float f_value;
	 PositionGesture p_g;
	 while(codeStrings.codeStrings[i] != 'p' && codeStrings.codeStrings[i] != 't')
	 {
		 i++;
	 }
	 if(codeStrings.codeStrings[i] == 'p')//'p'
	 {
		 i++;
		 if( (codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')
				 || (codeStrings.codeStrings[i] == 'n') )
		 {
			 if(codeStrings.codeStrings[i] == 'n')//pnx的情况
			 {
				 i++;
				 n_num = StringToUint((unsigned char*)&codeStrings);
				 p_num = s_Nx_variable[n_num];
				 if(p_num >= MAX_POSITION_NUM) return ERR_P_OVERFLOW;
				 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//P位置值不能为空
			 }
			 else//pxx的情况
			 {
				 p_num = StringToUint((unsigned char*)&codeStrings);

			 }

			 err = GetPxxPulseValue(p_num,next_pulse);

			 while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//跳过数字
			 {
				 i++;
			 }
			 while(codeStrings.codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
		 }
		 else//仅仅是p的情况 mvs p.x+n
		 {
			 //ReadCurrentPulseEp(next_pulse);
			 ReadCurrentPulseLp(next_pulse);//修正于//2018/06/01
		 }

		 PulseToAngle(next_pulse,angle);
		 p_g = GetPositionGesture(angle);//获得正解

		 if(codeStrings.codeStrings[i] == '.')
		 {
			 i++;	//跳过小数点
			 f_value = StringToFloat(&codeStrings.codeStrings[i]);

			 if(codeStrings.codeStrings[i] == 'x')
			 {
				 i++;
				 while(codeStrings.codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 if(codeStrings.codeStrings[i++] == '+')//调过+/-
				 {
					 p_g.px = p_g.px + f_value;
				 }
				 else// '-'
				 {
					 p_g.px = p_g.px - f_value;
				 }
			 }
			 else if(codeStrings.codeStrings[i] == 'y')
			 {
				 i++;
				 while(codeStrings.codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 if(codeStrings.codeStrings[i++] == '+')
				 {
					 p_g.py = p_g.py + f_value;
				 }
				 else// '-'
				 {
					 p_g.py = p_g.py - f_value;
				 }
			 }
			 else//'z'
			 {
				 i++;
				 while(codeStrings.codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 if(codeStrings.codeStrings[i++] == '+')
				 {
					 p_g.pz = p_g.pz + f_value;
				 }
				 else// '-'
				 {
					 p_g.pz = p_g.pz - f_value;
				 }
			 }
			 ModifyPositionGesture(p_g);	//更新正解
			 if(GetBestSolution(angle))		//获得逆解
				 return ERR_OUT_OF_RANGE;//超出动作范围
			 AngleToPulse(angle,next_pulse);

			 while(codeStrings.codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 while((codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9') ||
					 (codeStrings.codeStrings[i]=='.'))//跳过数字或小数点
			 {
			 	 i++;
			 }
			 while(codeStrings.codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//如果有speed
			 {
				 speed = atoi(((char*)&codeStrings)+i);
				 if(speed>0 && speed<=100)
				 {
					 if(teach == 0)//全速运行时才执行，单步示教不执行
					 {
						 ModifySpeed(speed);
					 }
				 }
			 }
		 }
		 else
		 {
			 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//如果有speed
			 {
				 speed = atoi(((char*)&codeStrings)+i);
				 if(speed>0 && speed<=100)
				 {
					 if(teach == 0)//全速运行时才执行，单步示教不执行
					 {
						 ModifySpeed(speed);
					 }
				 }
			 }
		 }
		 flg = 1;	//记录直线插补后需要关节插补修正
	 }
	 else//'t'
	 {
		 err = ExecuteTx(((unsigned char*)&codeStrings)+i,next_pulse);
		 flg = 1;	//记录直线插补后需要关节插补修正
	 }

	 if(!err)	//下一个脉冲位置
	 {
		  err1 = LinearInterpolation(next_pulse);
		  if(err1 == 3)
		  {
			   return ERR_STOP;
		  }
		  else if(err1 == 1)
		  {
			  return ERR_ALARM;
		  }
		  else if(err1 == 2)
		  {
			  return ERR_SAVE_BTN;
		  }
		  else if(err1 == 4)
		  {
			  return ERR_OUT_OF_RANGE;	//超出动作范围
		  }
		  else if(err1 == 6)
		  {
			  return ERR_SINGULARITY;
		  }
//		  else if(err1 == 5)
//		  {
//			  return ERR_GUSTURE;	//姿态不一致
//		  }

		  if(err1==0)	//直线插补成功
		  {
			  if(flg == 1)//用当前位置作为参考，重新计算最优解
			  {
				  //ReadCurrentPulseEp(current_pulse);	//读当前脉冲位置
				  ReadCurrentPulseLp(current_pulse);       //修正于//2018/06/01
				  PulseToAngle(current_pulse,angle);	//脉冲转换弧度
				  update_sin_cos_data(angle);			//更新库
				  ModifyPositionGesture(p_g);			//更新正解
				  if(GetBestSolution(angle))				//获得逆解
					  return ERR_OUT_OF_RANGE;//超出动作范围
				  AngleToPulse(angle,next_pulse);		//角度转脉冲
			  }
		  }

		  err1 = JointInterpolation(next_pulse);//则关节插补,调整
		  if(err1 == 0)	//关节插补运动到目标位置
		  {
			  return 0;
		  }
		  else if(err1 == 1)
		  {
			  return ERR_ALARM;
		  }
		  else if(err1 == 2)
		  {
			  return ERR_SAVE_BTN;
		  }
		  else
		  {
			  return ERR_STOP;
		  }
	 }
	 else
	 {
		  return err;//P点位置不存在
	 }
}

/*****************************************
 * 函数功能：执行圆弧插补命令 mvr
 * 输入参数：PrgCode类型的 代码
 * 输出参数：  0x00：正确
 * 			 0x01：机械手动作过程中发生报警
 * 			 0x02：机械手动作过程中松开了安全开关（示教时）
 * 			 0x03：动作过程中按下了stop按钮
 * 			 0x04: 超出动作范围
 * 			 0x05: 其他用途
 * 			 0x06: 奇异点
 * 			 0x10：结束执行
 * 			 0x20：起始点和中间点重合
 * 			 0x30：起始点和终点重合
 * 			 0x40：中间点和终点重合
 * 			 0x50：三点在同一直线
 */
#pragma CODE_SECTION(ExecuteMvr, "ramfuncs");
unsigned char ExecuteMvr(PrgCode codeStrings,char teach)
{
	unsigned char i=0,j,err=0,p_num;
	int speed;
	long pulse[6];
	double angle[6];
	PositionGesture p_g[3];

	for(j=0;j<3;j++)
	{
		while(codeStrings.codeStrings[i] != 'p')
		{
			i++;
		}
		i++;//跳过p
		p_num = StringToUint((unsigned char*)&codeStrings.codeStrings[i]);
		if(!GetPxxPulseValue(p_num,pulse))
		{
			PulseToAngle(pulse,angle);
			p_g[j] = GetPositionGesture(angle);
		}
		else
		{
			return ERR_P_NO_VALUE;
		}
	}

	while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//跳过数字
	{
	 i++;
	}
	while(codeStrings.codeStrings[i] == ' ')//跳过空格
	{
	 i++;
	}

	 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//如果有speed
	 {
		 speed = atoi(((char*)&codeStrings)+i);
		 if(speed>0 && speed<=100)
		 {
			 if(teach == 0)//全速运行时才执行，单步示教不执行
			 {
				 ModifySpeed(speed);
			 }
		 }
	 }

	err = drive_arc(p_g,0x02)<<4;//圆弧，从头开始
	//err |= BaseCoordSlineMinPrecisionRun();

	if(err) return err;
	while(1)
	{
		err = drive_arc(p_g,0x0)<<4;//圆弧，继续执行
		err |= BaseCoordSlineMinPrecisionRun();//走起
		if(err)break;
	}
	  if(err&0x10)//关节插补运动到目标位置
	  {
		  return 0;
	  }
	  else if(err == 1)
	  {
		  return ERR_ALARM;
	  }
	  else if(err == 2)
	  {
		  return ERR_SAVE_BTN;
	  }
	  else if(err == 3)
	  {
		  return ERR_STOP;
	  }
	  else if(err == 4)
	  {
		  return ERR_OUT_OF_RANGE;
	  }
	  else//if(err == 6)
	  {
		  return ERR_SINGULARITY;//奇异点singularity
	  }
}

/*****************************************
 * 函数功能：执行整圆插补命令 mvc
 * 输入参数：PrgCode类型的 代码
 * 输出参数：0x00：正确
 * 			 0x01：机械手动作过程中发生报警
 * 			 0x02：机械手动作过程中松开了安全开关（示教时）
 * 			 0x03：动作过程中按下了stop按钮
 * 			 0x04: 超出动作范围
 * 			 0x05: 其他用途
 * 			 0x06: 奇异点
 * 			 0x10：结束执行
 * 			 0x20：起始点和中间点重合
 * 			 0x30：起始点和终点重合
 * 			 0x40：中间点和终点重合
 * 			 0x50：三点在同一直线
 */
#pragma CODE_SECTION(ExecuteMvc, "ramfuncs");
unsigned char ExecuteMvc(PrgCode codeStrings,char teach)
{
	unsigned char i=0,j,err=0,p_num;
	int speed;
	long pulse[6];
	double angle[6];
	PositionGesture p_g[3];

	for(j=0;j<3;j++)
	{
		while(codeStrings.codeStrings[i] != 'p')
		{
			i++;
		}
		i++;//跳过p
		p_num = StringToUint((unsigned char*)&codeStrings.codeStrings[i]);
		if(!GetPxxPulseValue(p_num,pulse))
		{
			PulseToAngle(pulse,angle);
			p_g[j] = GetPositionGesture(angle);
		}
		else
		{
			return ERR_P_NO_VALUE;
		}
	}

	while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//跳过数字
	{
	 i++;
	}
	while(codeStrings.codeStrings[i] == ' ')//跳过空格
	{
	 i++;
	}

	 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//如果有speed
	 {
		 speed = atoi(((char*)&codeStrings)+i);
		 if(speed>0 && speed<=100)
		 {
			 if(teach == 0)//全速运行时才执行，单步示教不执行
			 {
				 ModifySpeed(speed);
			 }
		 }
	 }

	err = drive_arc(p_g,0x03)<<4;//整圆，从头开始
	if(err) return err;
	while(1)
	{
		err = drive_arc(p_g,0x01)<<4;//整圆，继续执行
		err |= BaseCoordSlineMinPrecisionRun();//走起
		if(err)break;
	}
	  if(err&0x10)//关节插补运动到目标位置
	  {
		  return 0;
	  }
	  else if(err == 1)
	  {
		  return ERR_ALARM;
	  }
	  else if(err == 2)
	  {
		  return ERR_SAVE_BTN;
	  }
	  else if(err == 3)
	  {
		  return ERR_STOP;
	  }
	  else if(err == 4)
	  {
		  return ERR_OUT_OF_RANGE;
	  }
	  else//if(err == 6)
	  {
		  return ERR_SINGULARITY;//奇异点singularity
	  }
}
/*****************************************
 * 函数功能：执行螺旋圆插补命令 mvh
 * 输入参数：PrgCode类型的 代码
 */
#pragma CODE_SECTION(ExecuteMvh, "ramfuncs");
unsigned char ExecuteMvh(PrgCode codeStrings,char teach)
{
	 unsigned char p_num,i=0,orientation=0,err1;
	 long next_pulse[6];
	 int speed;
	 while(codeStrings.codeStrings[i] != 'p')
	 {
		 i++;
	 }
	 i++;//跳过p
	 p_num = StringToUint(((unsigned char*)&codeStrings) + i);
	 while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }
	 while(codeStrings.codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	if((codeStrings.codeStrings[i] == '+')||(codeStrings.codeStrings[i]=='-'))
	{
		if(codeStrings.codeStrings[i]=='-')
		{
			orientation = 1;
		}else{
			orientation = 0;//默认是0
		}
		i++;
		while(codeStrings.codeStrings[i] == ' ')//跳过空格
		{
			i++;
		}
	}

	 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//如果有speed
	 {
		 speed = atoi(((char*)&codeStrings)+i);
		 if(speed>0 && speed<=100)
		 {
			 if(teach == 0)//全速运行时才执行，单步示教不执行
			 {
				 ModifySpeed(speed);
			 }
		 }
	 }

	 if(!GetPxxPulseValue(p_num,next_pulse))	//下一个脉冲位置
	 {

		  err1 = CircleInterpolation(next_pulse,orientation);
		  if(err1 == 3) return ERR_STOP;
		  else if(err1 == 4) return ERR_OUT_OF_RANGE;
		  else if(err1 == 6) return ERR_SINGULARITY;
		  else if(err1 == 7) return ERR_GUSTURE;
		  err1 = JointInterpolation(next_pulse);		//则关节插补,调整
		  if(err1 == 0)//关节插补运动到目标位置
		  {
			  return 0;
		  }
		  else if(err1 == 1)
		  {
			  return ERR_ALARM;
		  }
		  else if(err1 == 2)
		  {
			  return ERR_SAVE_BTN;
		  }
		  else
		  {
			  return ERR_STOP;
		  }
	 }
	 else
	 {
		  return ERR_P_NO_VALUE;//P点位置不存在
	 }
}
/*********************************************
 * 函数功能：执行Px指令 	p0=p,p0=p1, p0=p1.x-20.1;
 * 						p0=t1 n;
 * 						p0 = (p1+)rsrd
 * 					  	P0.j2 = (-/+)y/fy +/- (-)z/fz;
 * 返回值：0：成功	其他值：失败
 */
unsigned char ExecutePx(PrgCode codeStrings)
{
	 char type,sign;
	 unsigned char i=0,err,i_value,j;
	 unsigned char p_num1,p_num2,j_num,f_num1,f_num2;
	 double f_value,position[6];
	 double f_value1,f_value2;
	 double angle[6];
	 long pulse[6];
	 PositionGesture p_g;
	 i++;//跳过p
	 p_num1 = StringToUint(&codeStrings.codeStrings[i]);//提取p变量

	 while(codeStrings.codeStrings[i] != 'p' && codeStrings.codeStrings[i] != 't'&&
			codeStrings.codeStrings[i] != 'r'&& codeStrings.codeStrings[i] != 'j')
	 {
		 i++;
	 }

	 //P0.j2 = y/fy (- z/fz);
	 if(codeStrings.codeStrings[i] == 'j')
	 {
		 i++;	//跳过j
		 j_num = StringToUint(&codeStrings.codeStrings[i]);//提取j变量
		 GetPxxPulseValue(p_num1,pulse);
		 while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings.codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 i++;	//跳过 =
		 while(codeStrings.codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if((codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9') ||
				 codeStrings.codeStrings[i]=='+' || codeStrings.codeStrings[i]=='-')// +/-y
		 {
			 f_value1 = atof((char*)(&codeStrings.codeStrings[i]));
			 while((codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9') ||
				   codeStrings.codeStrings[i]=='+' || codeStrings.codeStrings[i]=='-' ||
				   codeStrings.codeStrings[i] == '.')//跳过数字,+,-,.
			 {
				 i++;
			 }
		 }
		 else	//fy
		 {
			 i++;	//跳过f
			 f_num1 = StringToUint(&codeStrings.codeStrings[i]);//提取f变量
			 f_value1 = s_Fx_variable[f_num1];
			 while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//跳过数字
			 {
				 i++;
			 }

		 }
		 while(codeStrings.codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings.codeStrings[i] == '\0')	//p0.j1 = x/fy
		 {
			 SetSingleAxisAngle(j_num,f_value1,pulse);
			 SavePositionData(p_num1,pulse);//保存
			 return 0;
		 }
		 j = codeStrings.codeStrings[i];	//记录了 +/-符号

		 i++;	//跳过符号
		 while(codeStrings.codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }

		 if((codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9') ||
				 codeStrings.codeStrings[i]=='+' || codeStrings.codeStrings[i]=='-')// +/-y
		 {
			 f_value2 = atof((char*)(&codeStrings.codeStrings[i]));
		 }
		 else	//fy
		 {
			 i++;	//跳过f
			 f_num2 = StringToUint(&codeStrings.codeStrings[i]);//提取f变量
			 f_value2 = s_Fx_variable[f_num2];
		 }
		 switch(j)
		 {
		 case '+':	f_value = f_value1 + f_value2;
			 break;
		 case '-':	f_value = f_value1 - f_value2;
			 break;
		 default:
			 break;
		 }
		 SetSingleAxisAngle(j_num,f_value,pulse);
		 SavePositionData(p_num1,pulse);//保存
		 return 0;
	 }else
	 if(codeStrings.codeStrings[i] == 'p')
	 {
		 i++;
		 while(codeStrings.codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 //px = p or px = p +/- rsrd的情况
		 if(codeStrings.codeStrings[i]=='\0' || codeStrings.codeStrings[i]=='+' ||
				 codeStrings.codeStrings[i]=='-')
		 {
			 if(codeStrings.codeStrings[i] == '+' || codeStrings.codeStrings[i]=='-')//px = p + rsrd
			 {
				 if(ROBOT_PARAMETER.HARDWARE_REV >= 8)	//三协 REV08版本
				 {
					 if((RX_POSITION_STA == 0))
					 {
						 delay_1ms(20);
						 //ReadCurrentPulseEp(pulse);
						 ReadCurrentPulseLp(pulse);//修正于//2018/06/01
						 SavePositionData(p_num1,pulse);//保存
						 return 0;
					 }
				 }
				 else if(ROBOT_PARAMETER.HARDWARE_REV == 7)
				 {
					 return ERR_NO_RS232;
				 }
				 else
				 {
					 j=0;
					 while(RX_POSITION_STA == 0)
					 {
						 delay_1ms(20);
						 j++;
						 if(j>=25)
						 {
							 //ReadCurrentPulseEp(pulse);
							 ReadCurrentPulseLp(pulse);//修正于//2018/06/01
							 SavePositionData(p_num1,pulse);//保存
							 return 0;
						 }
					 }
				 }
				 type = GetPosition_SCI(position);
				 //ReadCurrentPulseEp(pulse);	//获得当前位置
				 ReadCurrentPulseLp(pulse);//修正于//2018/06/01
				 PulseToAngle(pulse,angle);
				 p_g = GetPositionGesture(angle);//获得正解
				 if(codeStrings.codeStrings[i] == '+')
				 {
					 p_g.px = p_g.px + position[0];
					 p_g.py = p_g.py + position[1];
				 }
				 else
				 {
					 p_g.px = p_g.px - position[0];
					 p_g.py = p_g.py - position[1];
				 }

				 if(type == 3)
				 {
					 if(ROBOT_PARAMETER.AXIS_NUM == 4)
					 {
						 p_g.r11 = cos(position[2]/360*2*PI);
						 p_g.r21 = sin(position[2]/360*2*PI);
					 }
				 }

				 ModifyPositionGesture(p_g);//更新正解
				 if(GetBestSolution(angle))	//获得逆解
					 return ERR_OUT_OF_RANGE;//超出动作范围
				 AngleToPulse(angle,pulse);
				 SavePositionData(p_num1,pulse);//保存
				 RX_POSITION_STA = 0;
				 return 0;
			 }
			 else	// if(codeStrings.codeStrings[i] == '\0')//px = p
			 {
				  //ReadCurrentPulseEp(pulse);	//获得当前位置
				  ReadCurrentPulseLp(pulse);//修正于//2018/06/01
				  SavePositionData(p_num1,pulse);//px = p
				  return 0;
			 }
		 }

		 if(codeStrings.codeStrings[i] == '.')
		 {
			 //ReadCurrentPulseEp(pulse);	//获得当前位置
			 ReadCurrentPulseLp(pulse);     //修正于//2018/06/01
			 //i++;	//跳过 .
		 }
		 else
		 {
			 //px = p(y)(.z + n)  or  px = py + rsrd
			 p_num2 = StringToUint(&codeStrings.codeStrings[i]);//提取p变量
			 GetPxxPulseValue(p_num2,pulse);
			 while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//跳过数字
			 {
			 	 i++;
			 }
		 }


		 while(codeStrings.codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings.codeStrings[i] == '\0')//p0 = p1
		 {
			  SavePositionData(p_num1,pulse);//p0 = p1
			  return 0;
		 }
		 else if(codeStrings.codeStrings[i]=='+' || codeStrings.codeStrings[i] == '-')//px = py +/- rsrd
		 {
			 if(ROBOT_PARAMETER.HARDWARE_REV == 8)	//三协 REV08版本
			 {
				 if((RX_POSITION_STA == 0))
				 {
					 delay_1ms(20);
					 //ReadCurrentPulseEp(pulse);
					 ReadCurrentPulseLp(pulse);//修正于//2018/06/01
					 SavePositionData(p_num1,pulse);//保存
					 return 0;
				 }
			 }
			 else if(ROBOT_PARAMETER.HARDWARE_REV >= 8)
			 {
				 return ERR_NO_RS232;
			 }
			 else
			 {
				 while(RX_POSITION_STA == 0)
				 {
					 delay_1ms(20);
					 i++;
					 if(i>=25)
					 {
						 //ReadCurrentPulseEp(pulse);
						 ReadCurrentPulseLp(pulse);//修正于//2018/06/01
						 SavePositionData(p_num1,pulse);//保存
						 return 0;
					 }
				 }
			 }
			 type = GetPosition_SCI(position);
			 PulseToAngle(pulse,angle);
			 p_g = GetPositionGesture(angle);//获得正解
			 if(codeStrings.codeStrings[i] == '+')
			 {
				 p_g.px = p_g.px + position[0];
				 p_g.py = p_g.py + position[1];
			 }
			 else
			 {
				 p_g.px = p_g.px - position[0];
				 p_g.py = p_g.py - position[1];
			 }
			 if(type == 3)
			 {
				 if(ROBOT_PARAMETER.AXIS_NUM == 4)
				 {
					 p_g.r11 = cos(position[2]/360*2*PI);
					 p_g.r21 = sin(position[2]/360*2*PI);
				 }
			 }

			 ModifyPositionGesture(p_g);//更新正解
			 if(GetBestSolution(angle))	//获得逆解
			 	 return ERR_OUT_OF_RANGE;//超出动作范围
			 AngleToPulse(angle,pulse);
			 SavePositionData(p_num1,pulse);//保存
			 RX_POSITION_STA = 0;
			 return 0;
		 }
		 else if(codeStrings.codeStrings[i] == '.')//p0 = p(1).
		 {
			 i++;
			 if(codeStrings.codeStrings[i] == 'j')
			 {
				 i++;//跳过j
				 i_value = StringToUint(&codeStrings.codeStrings[i]);
				 i++;//跳过x
				 while(codeStrings.codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 sign = codeStrings.codeStrings[i];
				 i++;	//跳过符号
				 while(codeStrings.codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 if(codeStrings.codeStrings[i] == 'f')
				 {
					 i++;	//跳过f
					 f_num1 = StringToUint(&codeStrings.codeStrings[i]);//提取f变量
					 f_value = s_Fx_variable[f_num1];
				 }
				 else
				 {
					 f_value = atof((char*)(&codeStrings.codeStrings[i]));
				 }
				 if(sign == '-')
				 {
					 f_value = -f_value;
				 }
				 SingleAxisAngleChange(i_value,f_value,pulse);
			 }
			 else
			 {
				 char c;
				 c =  codeStrings.codeStrings[i];	//保存x,y,z
				 i++;
				 while(codeStrings.codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 sign = codeStrings.codeStrings[i];	//保存+，-号
				 i++;	//跳过符号
				 while(codeStrings.codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }

				 if(codeStrings.codeStrings[i] == 'f')
				 {
					 i++;	//跳过f
					 f_num1 = StringToUint(&codeStrings.codeStrings[i]);//提取f变量
					 f_value = s_Fx_variable[f_num1];
				 }
				 else
				 {
					 f_value = atof((char*)(&codeStrings.codeStrings[i]));
				 }

				 PulseToAngle(pulse,angle);
				 p_g = GetPositionGesture(angle);//获得正解

				 if(sign == '+')
				 {
					 switch(c)
					 {
					 case 'x':	p_g.px = p_g.px + f_value;
						 break;
					 case 'y':	p_g.py = p_g.py + f_value;
						 break;
					 case 'z':	p_g.pz = p_g.pz + f_value;
						 break;
					 }
				 }
				 else
				 {
					 switch(c)
					 {
					 case 'x':	p_g.px = p_g.px - f_value;
						 break;
					 case 'y':	p_g.py = p_g.py - f_value;
						 break;
					 case 'z':	p_g.pz = p_g.pz - f_value;
						 break;
					 }
				 }
				 ModifyPositionGesture(p_g);//更新正解
				 if(GetBestSolution(angle))	//获得逆解
					 return ERR_OUT_OF_RANGE;//超出动作范围
				 AngleToPulse(angle,pulse);
			 }
			 SavePositionData(p_num1,pulse);//保存
			 return 0;
		 }
	 }
	 else if(codeStrings.codeStrings[i] == 't')//'t'
	 {
		 err = ExecuteTx(&codeStrings.codeStrings[i],pulse);
		 if(err == 0)
		 {
			 SavePositionData(p_num1,pulse);//保存
		 }
		 return err;
	 }
	 else
	 {
		 i=0;

		 if(ROBOT_PARAMETER.MOTOR_TYPE == 2)	//三协 REV08版本
		 {
			 if((RX_POSITION_STA == 0))
			 {
				 delay_1ms(20);
				 //ReadCurrentPulseEp(pulse);
				 ReadCurrentPulseLp(pulse);//修正于//2018/06/01
				 SavePositionData(p_num1,pulse);//保存
				 return 0;
			 }
		 }
		 else
		 {
			 while(RX_POSITION_STA == 0)
			 {
				 delay_1ms(20);
				 i++;
				 if(i>=25)
				 {
					 //ReadCurrentPulseEp(pulse);
					 ReadCurrentPulseLp(pulse);//修正于//2018/06/01
					 SavePositionData(p_num1,pulse);//保存
					 return 0;
				 }
			 }
		 }

		 type = GetPosition_SCI(position);

		 if(type == 2)
		 {
			 GetPxxPulseValue(p_num1,pulse);
			 PulseToAngle(pulse,angle);
			 p_g = GetPositionGesture(angle);//获得正解
			 p_g.px = position[0];
			 p_g.py = position[1];
		 }
		 else if(type == 3)
		 {
			 GetPxxPulseValue(p_num1,pulse);
			 PulseToAngle(pulse,angle);
			 p_g = GetPositionGesture(angle);//获得正解
			 p_g.px = position[0];
			 p_g.py = position[1];
			 if(ROBOT_PARAMETER.AXIS_NUM == 4)
			 {
				 p_g.r11 = cos(position[2]/360*2*PI);
				 p_g.r21 = sin(position[2]/360*2*PI);
			 }
		 }
		 else if(type == 6)
		 {
			 //ReadCurrentPulseEp(pulse);	//获得当前位置
			 ReadCurrentPulseLp(pulse);//修正于//2018/06/01
			 PulseToAngle(pulse,angle);
			 //p_g = GetPositionGesture(angle);//获得正解
			 update_sin_cos_data(angle);	//更新库(涉及最优解)
	//xxx
	//		 ToolPointToEndPoint(*((PositionEuler*)position),&p_g,ToolPara.ToolLength);
		 }
		 else{
			 //ReadCurrentPulseEp(pulse);	//获得当前位置
			 ReadCurrentPulseLp(pulse);//修正于//2018/06/01
			 PulseToAngle(pulse,angle);
			 p_g = GetPositionGesture(angle);//获得正解
		 }

		 ModifyPositionGesture(p_g);//更新正解
		 if(GetBestSolution(angle))	//获得逆解
		 	 return ERR_OUT_OF_RANGE;//超出动作范围
		 AngleToPulse(angle,pulse);
		 SavePositionData(p_num1,pulse);//保存
		 RX_POSITION_STA = 0;
	 }
	 return 0;
}
/*****************************************
 * 函数功能：执行全局速度设定命令 ovrd
 * 输入参数：PrgCode类型的 代码
 */
void ExecuteOvrd(PrgCode codeStrings)
{
	 char str[4];
	 unsigned int value;

	 value = StringToUint((unsigned char*)&codeStrings);
	 s_speed.ovrd = value;
	 ltoa(value,str);
	 SetTextValue(6, 19, str);//更新显示ovrd

	 s_speed.all_speed = (float)s_speed.ovrd*s_speed.speed/100;
	 if(s_speed.all_speed <= 0)
	 {
		 s_speed.all_speed  = 1;
	 }
}

/*****************************************
 * 函数功能：执行打开程序指令
 * 输入参数：PrgCode类型的 代码
 * 返回值：0：打开成功
 */
unsigned char ExecuteOpen(PrgCode codeStrings)
{
	 int i=0;
	 unsigned int num;
	 unsigned char err;
	 while(codeStrings.codeStrings[i] != ' ')
	 {
		 i++;
	 }
	 while(codeStrings.codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 err = OpenProgram(&codeStrings.codeStrings[i],&num);
	 if(err == 0)
	 {
		 Fresh_program_name_num(num,&codeStrings.codeStrings[i]);
		 return 0;
	 }
	 return ERR_NO_PROGRAM;
}

/*****************************************
 * 函数功能：执行局部速度命令 speed
 * 输入参数：PrgCode类型的 代码
 */
void ExecuteSpeed(PrgCode codeStrings)
{
	 unsigned int value;
	 value = StringToUint((unsigned char*)&codeStrings);
	 s_speed.speed = value;
	 s_speed.all_speed = (float)s_speed.ovrd*s_speed.speed*0.01;
	 if(s_speed.all_speed <= 0)
	 {
		 s_speed.all_speed  = 1;
	 }
}

/********************
 * 获得总速度
 */
unsigned char GetProgramRunSpeed(void)
{
	 return s_speed.all_speed;
}
/******************
 * 修改总速度
 */
void ModifyAllSpeed(unsigned char speed)
{
	 if(speed>100)
	 {
		  speed =100;
	 }else if(speed<=0)
	 {
		  speed =1;
	 }
	 s_speed.all_speed = speed;//总速度
}

/*****************************
 * 函数功能：获得全局速度ovrd
 */
unsigned char GetOvrd(void)
{
	return s_speed.ovrd;
}

/******************
 * 修改全局速度ovrd
 * 返回值：总速度
 */
unsigned int ModifyOvrd(unsigned char ovrd)
{
	 if(ovrd>100)
	 {
		 ovrd =100;
	 }else if(ovrd<=0)
	 {
		 ovrd =1;
	 }
	 s_speed.ovrd = ovrd;
	 s_speed.all_speed = (float)s_speed.ovrd*s_speed.speed*0.01;
	 if(s_speed.all_speed <= 0)
	 {
		 s_speed.all_speed  = 1;
	 }
	 return s_speed.all_speed;
}

/******************
 * 修改局部速度speed
 * 返回值：总速度
 */
unsigned int ModifySpeed(unsigned char speed)
{
	 if(speed>100)
	 {
		 speed =100;
	 }else if(speed<=0)
	 {
		 speed =1;
	 }
	 s_speed.speed = speed;
	 s_speed.all_speed = (float)s_speed.ovrd*s_speed.speed*0.01;
	 if(s_speed.all_speed <= 0)
	 {
		 s_speed.all_speed  = 1;
	 }
	 return s_speed.all_speed;
}
/*****************************************
 * 函数功能：执行输入IO命令(应该配合wait 指令一起使用)
 * 输入参数：PrgCode类型的 代码
 * 返回值:	 IO口状态，用来判断输入
 */
/*unsigned char ExecutePin(PrgCode codeStrings)
{
	 unsigned char pin_value=0,io_state;
	 unsigned char i=0;
	 pin_value = StringToUint((unsigned char*)&codeStrings);//获得是哪个IO口
	 while(codeStrings.codeStrings[i] != '=')//直到遇上'='就跳出循环
	 {
		  i++;
	 }

	 while((codeStrings.codeStrings[i]<'0') || (codeStrings.codeStrings[i]>'9'))//直到遇上数字就跳出循环
	 {
		  i++;
	 }
	 io_state = codeStrings.codeStrings[i]-'0';
	 return io_state;
}*/
/*****************************************
 * 函数功能：执行控制IO命令
 * 输入参数：PrgCode类型的 代码
 * 返回值：0->OK 其他->出错
 */
u8 ExecutePout(PrgCode codeStrings)
{
	 unsigned char pout_value=0,io_state;
	 unsigned char i=0,err;
	 pout_value = StringToUint((unsigned char*)&codeStrings);//获得是哪个IO口
	 while(codeStrings.codeStrings[i] != '=')//直到遇上'='就跳出循环
	 {
		  i++;
	 }

	 while((codeStrings.codeStrings[i]<'0') || (codeStrings.codeStrings[i]>'9'))//直到遇上数字就跳出循环
	 {
		  i++;
	 }

	io_state = codeStrings.codeStrings[i]-'0';
	if(io_state==0)
	{
			err = ResetPoutState(pout_value);
	}
	else
	{
			err = SetPoutState(pout_value);
	}
	if(err)
		return ERR_OUTPUT;
	return 0;
}
/*****************************************
 * 函数功能：执行wait命令(wait指令后应该接pin指令一起使用，
 * 			 如wait pin=1，意义为：等待直到输入IO1脚变为有效，即有电压输入)
 * 输入参数：PrgCode类型的 代码
 * 返回值：
 * 			0：正常运行
 * 			其他值：查阅出错代码
 */
unsigned char ExecuteWait(PrgCode codeStrings)
{
	unsigned char pin_value=0,sta,err=0;
	unsigned char i=0;
	u32 io_state;
	pin_value = StringToUint((unsigned char*)&codeStrings);//获得是哪个IO口,输入IO范围0~11
	while(codeStrings.codeStrings[i] != '=')//直到遇上'='就跳出循环
	{
	  i++;
	}
	while((codeStrings.codeStrings[i]<'0') || (codeStrings.codeStrings[i]>'9'))//直到遇上数字就跳出循环
	{
	  i++;
	}
	sta = codeStrings.codeStrings[i]-'0';//获得需要判断的IO状态

	do
	{
		io_state = ReadPinState()&((long)0x01<<pin_value);
		io_state = io_state>>pin_value;
		  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
		  {
			  delay_200ns();
			  if(ReadErrState()&ROBOT_PARAMETER.MOTOR_SELECT)
			  {
				  i=0;
				  for(i=0;i<6;i++)
				  {
				  		delay_200ns();
				  		err |= ReadRR2(0x01<<i);
				  }
				  if(err) return ERR_ALARM;
			  }
		  }
		  if(KeyBoard_StopSpeed())
		  {
			   return ERR_STOP;
		  }
	}
	while(sta != io_state);
	return 0;
}

/********************************************
 * 函数功能：修正托盘   用法：ft1 p0
 */
void ExecuteFtx(PrgCode codeStrings)
{
	unsigned int t_num,p_num;
	int i=0;
	long pulse[6];
	double angle[6];
	struct ThreeDimCoordStru p_g;
	PositionGesture p_g_fix;

	t_num = StringToUint((unsigned char*)&codeStrings);//提取托盘编号
	while(codeStrings.codeStrings[i++] != 'p');//直到遇上'p'就跳出循环
	p_num = StringToUint((unsigned char*)&codeStrings+i);//提取P
	GetPxxPulseValue(p_num,pulse);//获得脉冲数据
	PulseToAngle(pulse,angle);	//将脉冲转换为角度
	p_g_fix = GetPositionGesture(angle);//获得位姿

	//第s_tray[tray_num].y_num个点的位置
	p_g.x = s_tray[t_num].pos_ges.px +
			((s_tray[t_num].y_num-1)*s_tray[t_num].length*s_tray[t_num].unit_vector_y.x);
	p_g.y = s_tray[t_num].pos_ges.py +
			((s_tray[t_num].y_num-1)*s_tray[t_num].length*s_tray[t_num].unit_vector_y.y);
	p_g.z = s_tray[t_num].pos_ges.pz +
			((s_tray[t_num].y_num-1)*s_tray[t_num].length*s_tray[t_num].unit_vector_y.z);

	s_tray[t_num].fix_x = p_g_fix.px - p_g.x;
	s_tray[t_num].fix_y = p_g_fix.py - p_g.y;
	s_tray[t_num].fix_z = p_g_fix.pz - p_g.z;
}

/********************************************
 * 函数功能：执行tray指令 初始化托盘   用法：t1 p0p1p2 10 11
 * 			 注意：p0和p2必须为对角
 */
void ExecuteTray(PrgCode codeStrings)
{
	unsigned char i=0,j;
	unsigned char p_num[3],tray_num;
	unsigned int x_num,y_num;
	double width,length;//托盘的长宽
	long pulse[3][6];
	double angle[3][6];

	PositionGesture p_g[3];//位姿
	tray_num = StringToUint((unsigned char*)&codeStrings);//提取托盘编号
	for(j=0;j<3;j++)
	{
		while(codeStrings.codeStrings[i++] != 'p');//直到遇上'p'就跳出循环

		p_num[j] = StringToUint(((unsigned char*)&codeStrings)+i);//提取3个p位置
		GetPxxPulseValue(p_num[j],pulse[j]);//获得脉冲数据
		PulseToAngle(pulse[j],angle[j]);	//将脉冲转换为角度
		p_g[j] = GetPositionGesture(angle[j]);//获得位姿
	}
	for(j=0;j<6;j++)
	{
		s_tray[tray_num].angle[j] = angle[0][j];	//保存第一个点的角度
	}

	s_tray[tray_num].pos_ges = p_g[0];//托盘起始点
	while(codeStrings.codeStrings[i++] != ' ');//直到遇上' '就跳出循环
	x_num = StringToUint(((unsigned char*)&codeStrings)+i);//提取x(矩阵的宽将会分x份)
	while(codeStrings.codeStrings[i++] != ' ');//直到遇上' '就跳出循环
	y_num = StringToUint(((unsigned char*)&codeStrings)+i);//提取y(矩阵的长将会分y份)
	width = sqrt((p_g[0].px - p_g[1].px)*(p_g[0].px - p_g[1].px) +
					 (p_g[0].py - p_g[1].py)*(p_g[0].py - p_g[1].py) +
					 (p_g[0].pz - p_g[1].pz)*(p_g[0].pz - p_g[1].pz));
	length = sqrt((p_g[1].px - p_g[2].px)*(p_g[1].px - p_g[2].px) +
					 (p_g[1].py - p_g[2].py)*(p_g[1].py - p_g[2].py) +
					 (p_g[1].pz - p_g[2].pz)*(p_g[1].pz - p_g[2].pz));
	if(x_num==1)//防止出现除以0的情况
	{
		s_tray[tray_num].width = 0;
	}
	else{
		s_tray[tray_num].width = width/(x_num-1);//宽分成x_num-1份(10个点只需分成9份就有10个点，所以减1)
	}
	if(y_num==1)//防止出现除以0的情况
	{
		s_tray[tray_num].length = 0;
	}
	else{
		s_tray[tray_num].length = length/(y_num-1);//长分成y_num份
	}
	s_tray[tray_num].x_num = x_num;//托盘的宽将被分x_num份
	s_tray[tray_num].y_num = y_num;//托盘的长将被分y_num份

	s_tray[tray_num].unit_vector_x.x = (p_g[1].px - p_g[0].px)/width;//宽的单位向量的x
	s_tray[tray_num].unit_vector_x.y = (p_g[1].py - p_g[0].py)/width;//宽的单位向量的y
	s_tray[tray_num].unit_vector_x.z = (p_g[1].pz - p_g[0].pz)/width;//宽的单位向量的z

	s_tray[tray_num].unit_vector_y.x = (p_g[2].px - p_g[1].px)/length;//长的单位向量的x
	s_tray[tray_num].unit_vector_y.y = (p_g[2].py - p_g[1].py)/length;
	s_tray[tray_num].unit_vector_y.z = (p_g[2].pz - p_g[1].pz)/length;

	s_tray[tray_num].fix_x = 0;
	s_tray[tray_num].fix_y = 0;
	s_tray[tray_num].fix_z = 0;

	s_tray[tray_num].def_flag = 1;//标记此托盘为已被定义
}

/********************************************
 * 函数功能：执行tx指令 运用托盘运算  例如：mov t1 10中的t1 10
 * 			 注意：p0和p2必须为对角
 * 返回值：0：成功	其他值：查阅
 */
unsigned char ExecuteTx(unsigned char* strings,long* pulse)
{
	unsigned char i=0,k,tray_num,n_num;
	unsigned long num;//托盘总格数
	unsigned int x_num,y_num;
	int speed;
	double angle[6];
	struct ThreeDimCoordStru fix;
	PositionGesture next_p_g;

	while(*(strings+i) != 't')//直到遇上't'就跳出循环
	{
		i++;
	}
	i++;//跳过t
	tray_num = StringToUint(strings+i);//提取托盘编号
	if(tray_num>MAX_TRAY_NUM) return ERR_CMD;
	if(!GetTrayDefState(tray_num)) return ERR_TRAY_UNDEFINE;//未定义此托盘

	 while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
	 {
		 i++;
	 }
	while(*(strings+i) == ' ')//跳过空格
	{
	  i++;
	}

	if(*(strings+i) == 'n')
	{
		i++;
		n_num = StringToUint(strings+i);
		num = s_Nx_variable[n_num];
	}
	else
	{
		num = StringToUint(strings+i);//提取格子编号(值从1开始的)
	}
	 while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
	 {
		 i++;
	 }

	if(num==0) return ERR_TRAY_GRID_ZERO;
	if(num>(s_tray[tray_num].x_num*s_tray[tray_num].y_num)) return ERR_TRAY_GRID_OVERFLOW;//超过格子总数

	 while(*(strings+i) == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(*(strings+i)>='0' && *(strings+i)<='9')//如果有speed
	 {
		 speed = atoi((char*)strings+i);
		 if(speed>0 && speed<=100)
		 {
			 ModifySpeed(speed);
		 }
	 }

	num = num -1;//计算的时候格子编号是从0开始的
	/*if(tray_num<=9)
	{
		x_num = num/s_tray[tray_num].y_num;	//第几行
		if(x_num%2 == 0)
		{
			y_num = num%s_tray[tray_num].y_num;	//第几列
		}
		else
		{
			y_num = s_tray[tray_num].y_num-1-num%s_tray[tray_num].y_num;	//第几列
		}
	}
	else
	{*/
		y_num = num%s_tray[tray_num].y_num;	//第几列
		x_num = num/s_tray[tray_num].y_num;	//第几行
	//}
	next_p_g = s_tray[tray_num].pos_ges;
	for(k=0;k<6;k++)
	{
		angle[k] = s_tray[tray_num].angle[k];
	}

	for(k=0;k<=x_num;k++)
	{
		update_sin_cos_data(angle);
		next_p_g.px = s_tray[tray_num].pos_ges.px +
					(k*s_tray[tray_num].width*s_tray[tray_num].unit_vector_x.x);
		next_p_g.py = s_tray[tray_num].pos_ges.py +
					(k*s_tray[tray_num].width*s_tray[tray_num].unit_vector_x.y);
		next_p_g.pz = s_tray[tray_num].pos_ges.pz +
					(k*s_tray[tray_num].width*s_tray[tray_num].unit_vector_x.z);
		ModifyPositionGesture(next_p_g);//更新一下姿态
		if(GetBestSolution(angle))	//获得逆解
			return ERR_OUT_OF_RANGE;//超出动作范围
	}
	fix.x = (s_tray[tray_num].fix_x/(s_tray[tray_num].y_num-1) *	//修正长度，每一份的长度
			y_num /	//需要修补多少份
			(s_tray[tray_num].x_num-1) *	//平摊
			(s_tray[tray_num].x_num-x_num-1) );//累积

	fix.y = (s_tray[tray_num].fix_y/(s_tray[tray_num].y_num-1))*//修正长度，每一份的长度
			y_num /		//需要修补多少份
			(s_tray[tray_num].x_num-1) *		//平摊
			(s_tray[tray_num].x_num-x_num-1);	//累积

	fix.z = (s_tray[tray_num].fix_z/(s_tray[tray_num].y_num-1))*//修正长度，每一份的长度
			y_num /		//需要修补多少份
			(s_tray[tray_num].x_num-1) *		//平摊
			(s_tray[tray_num].x_num-x_num-1);	//累积

	for(k=0;k<=y_num;k++)
	{
		update_sin_cos_data(angle);
		next_p_g.px = s_tray[tray_num].pos_ges.px + (x_num*s_tray[tray_num].width*s_tray[tray_num].unit_vector_x.x) +
				(k*s_tray[tray_num].length*s_tray[tray_num].unit_vector_y.x) + fix.x;

		next_p_g.py = s_tray[tray_num].pos_ges.py + (x_num*s_tray[tray_num].width*s_tray[tray_num].unit_vector_x.y) +
				(k*s_tray[tray_num].length*s_tray[tray_num].unit_vector_y.y) + fix.y;

		next_p_g.pz = s_tray[tray_num].pos_ges.pz + (x_num*s_tray[tray_num].width*s_tray[tray_num].unit_vector_x.z) +
				(k*s_tray[tray_num].length*s_tray[tray_num].unit_vector_y.z) + fix.z;

		ModifyPositionGesture(next_p_g);//更新一下姿态
		if(GetBestSolution(angle))	//获得逆解
			return ERR_OUT_OF_RANGE;//超出动作范围
	}
	if(GetBestSolution(angle))	//获得逆解
		return ERR_OUT_OF_RANGE;//超出动作范围
	AngleToPulse(angle,pulse);
	return 0;
}

/********************************************
 * 函数功能：执行Nx指令 整数变量  例如：nx++;nx--;//nx = ny;nx = z;nx = ny - z;
 * 				(mov t1 n0;Px = tx nx;if n0>100 )
 * 			 返回值：n_value 变量的值
 * 返回值：0：成功	其他值：查阅
 */
unsigned char ExecuteNx(unsigned char* strings,long* nx)
{
	unsigned char i=0,n_num,n_num1,n_num2;
	long value;

	while(*(strings+i) != 'n')//直到遇上'n'就跳出循环
	{
		i++;
	}
	i++;//跳过n
	n_num = StringToUint(strings+i);//提取n变量

	while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
	{
		i++;
	}
	while(*(strings+i) == ' ')//跳过空格
	{
	  i++;
	}

	if(*(strings+i) == '+')//n++
	{
		s_Nx_variable[n_num]++;
	}
	else if(*(strings+i) == '-')//n--
	{
		s_Nx_variable[n_num]--;
	}
	else if(*(strings+i) == '=')//nx = z;nx = ny;nx = ny +(-,*) z;
	{
		i++;
		while(*(strings+i) == ' ')//跳过空格
		{
		  i++;
		}
		if(*(strings+i)>='0' && *(strings+i)<='9')//nx = z;
		{
			value = StringToUint(strings+i);
			s_Nx_variable[n_num] = value;
		}
		else if(*(strings+i) == 'n')
		{
			i++;
			n_num1 = StringToUint(strings+i);
			while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
			{
				i++;
			}
			while(*(strings+i) == ' ')//跳过空格
			{
			  i++;
			}
			if(*(strings+i) == '\0')//nx = ny;
			{
				s_Nx_variable[n_num] = s_Nx_variable[n_num1];
			}
			else if(*(strings+i) == '+')//nx = ny + z;
			{
				i++;
				while(*(strings+i) == ' ')//跳过空格
				{
				  i++;
				}
				if(*(strings+i) == 'n')
				{
					i++;
					n_num2 = StringToUint(strings+i);
					s_Nx_variable[n_num] = s_Nx_variable[n_num1] + s_Nx_variable[n_num2];
				}
				else
				{
					value = StringToUint(strings+i);
					s_Nx_variable[n_num] = s_Nx_variable[n_num1] + value;
				}

			}
			else if(*(strings+i) == '-')//nx = ny - z;
			{
				i++;
				while(*(strings+i) == ' ')//跳过空格
				{
				  i++;
				}
				if(*(strings+i) == 'n')
				{
					i++;
					n_num2 = StringToUint(strings+i);
					s_Nx_variable[n_num] = s_Nx_variable[n_num1] - s_Nx_variable[n_num2];
				}
				else
				{
					value = StringToUint(strings+i);
					s_Nx_variable[n_num] = s_Nx_variable[n_num1] - value;
				}
			}
			else if(*(strings+i) == '*')//nx = ny * z;
			{
				i++;
				while(*(strings+i) == ' ')//跳过空格
				{
				  i++;
				}
				if(*(strings+i) == 'n')
				{
					i++;
					n_num2 = StringToUint(strings+i);
					s_Nx_variable[n_num] = s_Nx_variable[n_num1] * s_Nx_variable[n_num2];
				}
				else
				{
					value = StringToUint(strings+i);
					s_Nx_variable[n_num] = s_Nx_variable[n_num1] * value;
				}
			}
			else if(*(strings+i) == '/')//nx = ny / z;
			{
				i++;
				while(*(strings+i) == ' ')//跳过空格
				{
				  i++;
				}
				if(*(strings+i) == 'n')
				{
					i++;
					n_num2 = StringToUint(strings+i);
					s_Nx_variable[n_num] = s_Nx_variable[n_num1] / s_Nx_variable[n_num2];
				}
				else
				{
					value = StringToUint(strings+i);
					s_Nx_variable[n_num] = s_Nx_variable[n_num1] / value;
				}
			}
			else if(*(strings+i) == 'm')//nx = ny mod z;
			{
				i++;	//m
				i++;	//o
				i++;	//d
				while(*(strings+i) == ' ')//跳过空格
				{
				  i++;
				}
				if(*(strings+i) == 'n')
				{
					i++;
					n_num2 = StringToUint(strings+i);
					s_Nx_variable[n_num] = s_Nx_variable[n_num1] % s_Nx_variable[n_num2];
				}
				else
				{
					value = StringToUint(strings+i);
					s_Nx_variable[n_num] = s_Nx_variable[n_num1] % value;
				}
			}
		}
	}
	*nx = s_Nx_variable[n_num];
	return 0;
}
/********************************************
 * 函数功能：执行Fx指令 浮点型变量  例如：
 * 			fx = fy;fx = +/-z;fx = fy - +/-z;fx = fy - fz;f0 = j1;
 * 			(Px.j2 = y/fy (- z/fz); )
 * 传入参数：strings:指向字符串指针
 * 			fx: double类型指针，保存fx的值
 * 返回值：0：成功	其他值：查阅
 */
unsigned char ExecuteFx(unsigned char* strings,double* fx)
{
	unsigned char i=0,f_num,f_num1,f_num2;
	double f_value1;

	while(*(strings+i) != 'f')//直到遇上'n'就跳出循环
	{
		i++;
	}
	i++;//跳过f
	f_num = StringToUint(strings+i);//提取f变量

	while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
	{
		i++;
	}
	while(*(strings+i) == ' ')//跳过空格
	{
	  i++;
	}

	//fx = +/-z;
	//fx = j1;
	//fx = fy;fx = fy - +/-z;fx = fy - fz;
	if(*(strings+i) == '=')
	{
		i++;
		while(*(strings+i) == ' ')//跳过空格
		{
		  i++;
		}
		if((*(strings+i)>='0' && *(strings+i)<='9') ||
			*(strings+i)=='+' || *(strings+i)=='-')	//fx = +/-z;
		{
			f_value1 = atof((char*)(strings+i));
			s_Fx_variable[f_num] = f_value1;
		}
		else if(*(strings+i) == 'j')	//fx = j1;
		{
			i++;	//跳过j
			f_num1 = atol((char*)(strings+i));	//获得哪个轴

			double angle[6];
			GetCurrentAngle(angle);//获得每轴当前角度
			 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)	//转换符号
			 {
				if(ROBOT_PARAMETER.AXIS_ROTATE.all & (0x0001<<i))
				{
					angle[i] = -angle[i];
				}
			 }
			f_value1 = angle[f_num1 - 1];
			s_Fx_variable[f_num] = f_value1;
		}
		else if(*(strings+i) == 'f')	//fx = fy;fx = fy - +/-z;fx = fy - fz;
		{
			i++;	//跳过f
			f_num1 = StringToUint(strings+i);
			while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
			{
				i++;
			}
			while(*(strings+i) == ' ')//跳过空格
			{
			  i++;
			}
			if(*(strings+i) == '\0')//fx = fy;
			{
				s_Fx_variable[f_num] = s_Fx_variable[f_num1];
			}
			else if(*(strings+i) == '+')//fx = fy + fz/z;
			{
				i++;
				while(*(strings+i) == ' ')//跳过空格
				{
				  i++;
				}
				if(*(strings+i) == 'f')
				{
					i++;
					f_num2 = StringToUint(strings+i);
					s_Fx_variable[f_num] = s_Fx_variable[f_num1] + s_Fx_variable[f_num2];
				}
				else
				{
					f_value1 = atof((char*)(strings+i));
					s_Fx_variable[f_num] = s_Fx_variable[f_num1] + f_value1;
				}

			}
			else if(*(strings+i) == '-')//fx = fy - fz/z;
			{
				i++;
				while(*(strings+i) == ' ')//跳过空格
				{
				  i++;
				}
				if(*(strings+i) == 'f')
				{
					i++;
					f_num2 = StringToUint(strings+i);
					s_Fx_variable[f_num] = s_Fx_variable[f_num1] - s_Fx_variable[f_num2];
				}
				else
				{
					f_value1 = atof((char*)(strings+i));
					s_Fx_variable[f_num] = s_Fx_variable[f_num1] - f_value1;
				}
			}
			else if(*(strings+i) == '*')//nx = ny * fz/z;
			{
				i++;
				while(*(strings+i) == ' ')//跳过空格
				{
				  i++;
				}
				if(*(strings+i) == 'f')
				{
					i++;
					f_num2 = StringToUint(strings+i);
					s_Fx_variable[f_num] = s_Fx_variable[f_num1] * s_Fx_variable[f_num2];
				}
				else
				{
					f_value1 = atof((char*)(strings+i));
					s_Fx_variable[f_num] = s_Fx_variable[f_num1] * f_value1;
				}
			}
			else if(*(strings+i) == '/')//nx = ny / z;
			{
				i++;
				while(*(strings+i) == ' ')//跳过空格
				{
				  i++;
				}
				if(*(strings+i) == 'f')
				{
					i++;
					f_num2 = StringToUint(strings+i);
					if(fabs(s_Fx_variable[f_num2]) < 1.0e-8) return ERR_CANT_ZERO;
					s_Fx_variable[f_num] = s_Fx_variable[f_num1] / s_Fx_variable[f_num2];
				}
				else
				{
					f_value1 = atof((char*)(strings+i));
					if(fabs(f_value1) < 1.0e-8) return ERR_CANT_ZERO;
					s_Fx_variable[f_num] = s_Fx_variable[f_num1] / f_value1;
				}
			}
		}
	}
	*(fx) = s_Fx_variable[f_num];
	return 0;
}
/*********************************************
 * 函数功能：执行If指令 Nx/x （>、<、= ）Nx/x；Nx x xx；
 * 			if x
 * 			then *loop / #sub
 * 			else *loop / #sub
 */
unsigned char ExecuteIf(PrgCode codeStrings,unsigned int* row)
{
	unsigned int i=0,n_num1,n_num2,flg,pin_value;
	long value1,value2;
	u8 tmp;
	char data[STM32_USART_LEN];
	memset(data,0,STM32_USART_LEN);
	while(codeStrings.codeStrings[i] != 'f')//直到遇上'f'就跳出循环
	{
		i++;
	}
	i++;
	while(codeStrings.codeStrings[i] == ' ')//跳过空格
	{
	  i++;
	}

	if(codeStrings.codeStrings[i] == 'n')//如果是N变量
	{
		i++;
		n_num1 = StringToUint(&codeStrings.codeStrings[i]);//提取n变量
		value1 = s_Nx_variable[n_num1];
	}
	else if(codeStrings.codeStrings[i] == 'p')//如果是输入IO
	{
		pin_value = StringToUint(&codeStrings.codeStrings[i]);//提取输入IO
		i = i+3;//跳过pin
		value1 = ReadPinState()&((long)0x01<<pin_value);
		value1 = value1>>pin_value;//获得IO状态
	}
	else if(codeStrings.codeStrings[i] == 'r')//如果是rsrd外部数据接收标志位
	{

		if(ROBOT_PARAMETER.HARDWARE_REV >= 8)
		{
			tmp = GetSTM32USART_STA(data);
			if(tmp == 0)
			{
				Data_To_Position(data);
			}
			else if(tmp == 1)
			{
				value1 = 0;
			}
			else
			{
				return ERR_STM32COMM;	//通信出错
			}
		}
		else if(ROBOT_PARAMETER.HARDWARE_REV == 7)
		{
			return ERR_NO_RS232;	//07版没有RS232
		}
		value1 = RX_POSITION_STA;
		i=i+4;
	}
	else//数值
	{
		value1 = StringToUint(&codeStrings.codeStrings[i]);
	}

	while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//跳过数字
	{
		i++;
	}
	while(codeStrings.codeStrings[i] == ' ')//跳过空格
	{
	  i++;
	}
	flg = codeStrings.codeStrings[i];//记录是 <、> 或 =
	i++;
	if(codeStrings.codeStrings[i] == '=')
	{
		flg += '=';//'<'==60;'='==61;'>'==62;
		i++;
	}
	while(codeStrings.codeStrings[i] == ' ')//跳过空格
	{
	  i++;
	}
	if(codeStrings.codeStrings[i] == 'n')
	{
		n_num2 = StringToUint(&codeStrings.codeStrings[i]);//提取n变量
		value2 = s_Nx_variable[n_num2];
	}
	else if(codeStrings.codeStrings[i] == 'p')//如果是输入IO
	{
		pin_value = StringToUint(&codeStrings.codeStrings[i]);//提取输入IO
		//i = i+3;//跳过pin
		value2 = ReadPinState()&((long)0x01<<pin_value);
		value2 = value2>>pin_value;//获得IO状态
	}
	else if(codeStrings.codeStrings[i] == 'r')//如果是rsrd外部数据接收标志位
	{
		if(ROBOT_PARAMETER.HARDWARE_REV == 8)
		{
			tmp = GetSTM32USART_STA(data);
			if(tmp == 0)
			{
				Data_To_Position(data);
			}
			else if(tmp == 1)
			{
				value1 = 0;
			}
			else
			{
				return ERR_STM32COMM;	//通信出错
			}
		}
		else if(ROBOT_PARAMETER.HARDWARE_REV == 7)
		{
			return ERR_NO_RS232;	//07版没有RS232
		}
		value2 = RX_POSITION_STA;
	}
	else
	{
		value2 = StringToUint(&codeStrings.codeStrings[i]);//提取值
	}
	switch(flg)
	{
	case  '<':
		if(value1 < value2){
			*row = *row + 1;
		}
		else{
			*row = *row + 2;
		}
		break;
	case '>':
		if(value1 > value2){
			*row = *row + 1;
		}
		else{
			*row = *row + 2;
		}
		break;
	case '=':
		if(value1 == value2){
			*row = *row + 1;
		}
		else{
			*row = *row + 2;
		}
		break;
	case '='+'=':
		if(value1 == value2){
			*row = *row + 1;
		}
		else{
			*row = *row + 2;
		}
		break;
	case ('<'+'='):
		if(value1 <= value2){
			*row = *row + 1;
		}
		else{
			*row = *row + 2;
		}
		break;
	case ('>'+'='):
		if(value1 >= value2){
			*row = *row + 1;
		}
		else{
			*row = *row + 2;
		}
		break;
	}
	return 0;
}
/*******************************************
 * 函数功能：执行then指令
 * 			if x
 * 			then *loop / #sub
 * 			else *loop / #sub
 */
void ExecuteThen(PrgCode codeStrings,unsigned int* row)
{
	unsigned int i=0,j,current_lines;
	unsigned char str_tmp[COLUMNS_IN_LINE]={0};
	unsigned char code_strings[COLUMNS_IN_LINE]={0};
	while(codeStrings.codeStrings[i]!='*' && codeStrings.codeStrings[i]!='#')
	{
		i++;
	}
	//str_tmp[0] = codeStrings.codeStrings[i];//'*';
	j=0;
	while(codeStrings.codeStrings[i]!=' ' && codeStrings.codeStrings[i]!='\0')
	{
		str_tmp[j++] = codeStrings.codeStrings[i++];
	}
	 current_lines = GetLinesCurrentProgram();//当前代码总行数

	 if(str_tmp[0] == '*')
	 {
		 for(i=0;i<current_lines;i++)
		 {
			 GetRowCode(i,code_strings);
			 if(code_strings[0]=='*')
			 {
				 if(!StringCmp(COLUMNS_IN_LINE,code_strings,str_tmp))
				 {
					 *row = i;
				 }
			 }
		 }
	 }
	 else// if(str_tmp[0] == '#')
	 {
		for(i=0;i<current_lines;i++)
		{
			GetRowCode(i,code_strings);
			if(code_strings[0]=='#')
			{
				if(!StringCmp(COLUMNS_IN_LINE,code_strings,str_tmp))
				{

					 if(s_return_stack_pointer==NULL)//如果是空指针
					 {
						 s_return_stack_pointer = s_return_stack;
					 }
					 else
					 {
						 s_return_stack_pointer++;
					 }
					 *s_return_stack_pointer = *row + 2;//跳两行，跳过then和else
					 *row = i;
				}
			}
		}
	 }
}

/*******************************************
 * 函数功能：执行else指令
 * 			if x
 * 			then *loop / #sub
 * 			else *loop / #sub
 */
void ExecuteElse(PrgCode codeStrings,unsigned int* row)
{
	unsigned int i=0,j,current_lines;
	unsigned char str_tmp[COLUMNS_IN_LINE]={0};
	unsigned char code_strings[COLUMNS_IN_LINE]={0};
	while(codeStrings.codeStrings[i]!='*' && codeStrings.codeStrings[i]!='#')
	{
		i++;
	}

	j=0;
	while(codeStrings.codeStrings[i]!=' ' && codeStrings.codeStrings[i]!='\0')
	{
		str_tmp[j++] = codeStrings.codeStrings[i++];
	}
	 current_lines = GetLinesCurrentProgram();//当前代码总行数

	 if(str_tmp[0] == '*')
	 {
		 for(i=0;i<current_lines;i++)
		 {
			 GetRowCode(i,code_strings);
			 if(code_strings[0]=='*')
			 {
				 if(!StringCmp(COLUMNS_IN_LINE,code_strings,str_tmp))
				 {
					 *row = i;
				 }
			 }
		 }
	 }
	 else// if(str_tmp[0] == '#')
	 {
		for(i=0;i<current_lines;i++)
		{
			GetRowCode(i,code_strings);
			if(code_strings[0]=='#')
			{
				if(!StringCmp(COLUMNS_IN_LINE,code_strings,str_tmp))
				{

					 if(s_return_stack_pointer==NULL)//如果是空指针
					 {
						 s_return_stack_pointer = s_return_stack;
					 }
					 else
					 {
						 s_return_stack_pointer++;
					 }
					 *s_return_stack_pointer = *row + 1;//跳两行，跳过else
					 *row = i;
				}
			}
		}
	 }
}

/*******************************************
 * 函数功能：执行acc指令 acc 1~100
 */
void ExecuteAcc(PrgCode codeStrings)
{
	 s_ACC = StringToUint((unsigned char*)&codeStrings);
	 if(s_ACC >=100)
	 {
		 s_ACC = 100;
		 SetAcc(0x3f,8000);      //加速度  A*125	 1~8000
	 }
	 else
	 {
		 //SetStartV(0x3F,(unsigned int)speed*6);      //初始速度   范围：1~8000
		 //SetAcac(0x3f,80*value);      //加速度变化率 1~65535  (62.5*10^6)/K*(8000000/R)
		 SetAcc(0x3f,10*s_ACC);      //加速度  A*125	 1~8000
	 }
}

/*****************************
 * 函数功能：获得当前加速度
 */
unsigned int GetAcc(void)
{
	return s_ACC;
}

/*****************************
 * 函数功能：设置外置串口波特率
 * 返回值:0->成功	1->失败
 */
u8 ExecuteBaud(PrgCode codeStrings)
{
	long value;
	char str[20];
	char str_tmp[20];
	value = StringToUint((unsigned char*)&codeStrings);
	CURRENT_BAUD = value;
	if(ROBOT_PARAMETER.MOTOR_TYPE == 2)
	{
		memset(str,0,20);
		strcpy(str,"baud ");
		ltoa(value,str_tmp);
		strcat(str,str_tmp);
		if(STM32_USART_SentData(str,strlen(str)+1) == 2)
		{
			return 1;
		}
	}
	else
	{

		SetScibBaud(value);
	}
	return 0;
}

u8 ExecuteRstd(PrgCode codeStrings)
{
	int i=0;
	u8 err;
	char str[32];
	while(codeStrings.codeStrings[i] != ' ')
	{
		i++;
	}
	while(codeStrings.codeStrings[i] == ' ')//跳过空格
	{
		i++;
	}
	if(ROBOT_PARAMETER.MOTOR_TYPE == 2)
	{
		str[0] = 0;
		strcpy(str,(char*)&codeStrings.codeStrings[i]);
		strcat(str,"\r\n");	//自动添加回车
		delay_1ms(2);
		err = STM32_USART_SentData(str,strlen(str));
		delay_1ms(2);
		if(err)
		{
			return ERR_STM32COMM;
		}
	}
	else
	{
		SendStringB((char*)&codeStrings.codeStrings[i]);//发送数据
		SendStringB("\r\n");//发送数据
		USART_RX_STA_B = 0;//发送完数据之后，把标志位清零，然后重新接收
	}
	return 0;
}

unsigned char ExecuteDisp(PrgCode codeStrings)
{
	char str[30];
	char str_tmp1[30],sign;
	int i=0,j,n_num;
	int line_num;
	while((codeStrings.codeStrings[i]<'1') || (codeStrings.codeStrings[i]>'5'))
	{
		i++;
	}
	line_num = atoi((char*)(codeStrings.codeStrings+i));	//获得在第几行显示
	if(line_num<=0 || line_num>5) return ERR_DISP;
	i++;	//跳过数字
	while(codeStrings.codeStrings[i] == ' ')//跳过空格
	{
		i++;
	}
	j=0;
	str[0] = '\0';
	while(codeStrings.codeStrings[i] != '\0')
	{
		if( (codeStrings.codeStrings[i] == '>') &&
		    (codeStrings.codeStrings[i+1] == '>') &&
			((codeStrings.codeStrings[i+2] == 'n') || (codeStrings.codeStrings[i+2] == 'f')))
		{
			if((codeStrings.codeStrings[i+3]>='0') && (codeStrings.codeStrings[i+3]<='9'))
			{
				sign = codeStrings.codeStrings[i+2];	//记录n还是f
				i=i+3;
				n_num = atoi((char*)(codeStrings.codeStrings+i));
				if(sign == 'n')
				{
					if(n_num >= MAX_N_NUM) return ERR_N_OVERFLOW;
				}
				else
				{
					if(n_num >= MAX_F_NUM) return ERR_F_OVERFLOW;
				}
				//跳过数字
				while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')
				{
					i++;
				}

				if(sign == 'n')
				{
					ltoa(get_n_value(n_num),str_tmp1);
				}
				else
				{
					My_DoubleToString(get_f_value(n_num),str_tmp1,16,3,1);
				}
				//j++;
				str[j] = '\0';
				strcat(str,str_tmp1);
				//将n_num后面的字符继续处理
				j = 0;
				do{
					str_tmp1[j++] = codeStrings.codeStrings[i];
				}
				while(codeStrings.codeStrings[i++] != '\0');

				strcat(str,str_tmp1);
				break;
			}
		}
		str[j] = codeStrings.codeStrings[i];
		j++;
		i++;
		if(codeStrings.codeStrings[i] == '\0')
		{
			str[j] = codeStrings.codeStrings[i];
		}
	}
	//显示

	//页面7显示，在successful下面一行开始显示，如果是disp指令则不刷新（任何按键会被刷新?）
	SetTextValue(7, 87, str);

	//全速运行时页面6显示disp的内容
	for(i=0;i<5;i++)
	{
		SetTextValue(6, 28+line_num, str);
	}
	return 0;
}


PositionEuler StringToPositionEuler(char* str)
{
	char i;
	double* angle;
	PositionEuler p_e;
	angle = (double*)(&p_e);
	while(((*str)<'0' || (*str)>'9')&&(*str)!='+'&&(*str)!='-')
	{
		str++;
	}
	for(i=0;i<6;i++)
	{
		angle[i] = atof(str);
		if(i<5)//第六次不需要了
		{
			while(((*str)>='0'&&(*str)<='9')||(*str)=='.'||(*str)=='+'||(*str)=='-')
			{
				str++;
			}
			while(((*str)<'0' || (*str)>'9')&&(*str)!='+'&&(*str)!='-')
			{
				str++;
			}
		}
	}
	p_e.Rx = p_e.Rx*PI_DIVIDE_180;	//角度转换为弧度
	p_e.Ry = p_e.Ry*PI_DIVIDE_180;
	p_e.Rz = p_e.Rz*PI_DIVIDE_180;
	return p_e;
}


/********************************************
 * 函数功能：执行rs232指令，执行后就进入死循环，从串口接收数据命令，
 * 			 直到执行结束命令或发生错误时退出
 * 返回值：0：成功	其他值：查阅
 */
unsigned char ExecuteRS232(PrgCode codeStrings)
{
	u8 i,t,err_t;
	//double angle[6];
	//long pulse[6];
	unsigned char tx_cmd[CMD_LEN-1],rx_cmd[CMD_LEN-1],code_err,err;
	//PositionEuler p_e;//2018/05/22注释掉
	PositionGesture p_g;
	char data[STM32_USART_LEN];
	Uint32 data_len;

//	 PositionGesture pos_ges;
//
//	 PositionEuler tool_end = {
//			 -15.642,0,110.447,180,-45,0
//	 };
//	 PositionEuler part_rob = {
//			 0,-350,140.504,0,0,0
//	 };

	 PositionEuler part;
//	 tool_end.Rx = tool_end.Rx*PI_DIVIDE_180;
//	 tool_end.Ry = tool_end.Ry*PI_DIVIDE_180;
//	 tool_end.Rz = tool_end.Rz*PI_DIVIDE_180;
//	 part_rob.Rx = part_rob.Rx*PI_DIVIDE_180;
//	 part_rob.Ry = part_rob.Ry*PI_DIVIDE_180;
//	 part_rob.Rz = part_rob.Rz*PI_DIVIDE_180;


	UpdateCurrentPositionGesture();			//更新当前位置姿态

//	unsigned char rx_cmd[5],tx_cmd[5],i,t=0;
	//tmp = GetSTM32USART_STA(data);
	memset(tx_cmd,0,sizeof(tx_cmd));
	tx_cmd[0] = CMD_RS232_BEGIN;	//让stm32通过串口发送字符串begin的命令
	SpiSendCmd(tx_cmd);	//发送命令
	//delay_1us(200);
	while(ProgramRuningFlag(0) == 0)
	{
		t=0;
		err_t = 0;
		//查询数据是否到来
		while(1)
		{
			if(t>250)	//等到超时
			{
				return ERR_RS_TIMEOUT;
			}
			delay_1ms(2);
			tx_cmd[0] = R_CMD_USART_RX_STA;
			SpiSendCmd(tx_cmd);	//发送查询命令
			delay_1us(20);
			if(SpiReceiveCmd(rx_cmd) == 0)
			{
				if(rx_cmd[0] == R_CMD_USART_RX_OK)
				{
					data_len = 0;
					for(i=0;i<4;i++)
					{
						data_len |= (Uint32)rx_cmd[i+1]<<(i*8);//数据长度
					}
					break;
				}
//				else if(rx_cmd[0] == R_CMD_USART_RX_NO)
//				{
//					return 1;
//				}
				else
				{
					t++;
					continue;
				}
			}
			err_t++;
			delay_1us(300);
			if(err_t >MXA_REPEAT) return ERR_STM32COMM;
		}

		memset(data,0,STM32_USART_LEN);
		if(SpiReceiveData(data_len,(u8*)data,0))
		{
			delay_1ms(100);//等待stm32退出接收模式
			return ERR_STM32COMM;
		}
		else
		//if(err)
		{
			for(i=0;i<7;i++)
			{
				if(data[i]>='A' && data[i]<='Z')
				{
					data[i] += 32;
				}
			}
			code_err = CheckGrammar((PrgCode*)data);//检查语法
			if(code_err>=ERR_CMD)
			{
				//SPIA_RX_STA = 0;
				return code_err;
			}

			if(code_err == code_p_g)
			{
				//StringToAngle(SPIA_RX_DATA_BUF,angle);
				//p_e = StringToPositionEuler(data);
				//ToolPointToEndPoint(p_e,&p_g,ToolPara.ToolLength);
				//ModifyPositionGesture(p_g);

				part = StringToPositionEuler(data);
				p_g = PartPosToToolPos(TOOL_END,PART_ROBOT,part,89);
				ModifyPositionGesture(p_g);
				while(1)
				{
					err = BaseCoordSlineMinPrecisionRun();	//这里要改 移动距离太长的情况下，姿态可能会发生改变 xxx
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
									   err = 1;	//报警
									   break;
								  }
							  }
						  }
						  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:上拉,使用安全开关的时候，松开(高电平)就退出
						  {
							  delay_1us(1);
							  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)
							  {
								  SetDriCmd(0x3F,0x27);
								  err = 2;	//安全开关
								  break;
							  }
						  }
						  if(KeyBoard_StopSpeed())
						  {
							   SetDriCmd(0x3F,0x26);	//减速停止
							   err = 3;
							   break;
						  }
						  NOP;NOP;
					 }
					//if(err) return err;

					//AngleToPulse(angle,pulse);
					//err = JointInterpolation(pulse);
					if(err == 0)//关节插补运动到目标位置
					{
						//return 0;
						break;
					}
					else if(err == 1)
					{
						return ERR_ALARM;
					}
					else if(err == 2)
					{
						return ERR_SAVE_BTN;
					}
				    else if(err == 4)
				    {
					    return ERR_OUT_OF_RANGE;
				    }
				    else if(err == 6)
				    {
					    return ERR_SINGULARITY;//奇异点singularity
				    }
					else if(err == 3)
					{
						ProgramRuningFlag(2);	//暂停
						SetButtonValue(6,2,1);	//“继续运行”
						while(ProgramRuningFlag(0) == 1)	//暂停
						{
							if(ErrCheck(6))
							{
							  G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//使用安全开关
							  ClearStopProgramFlag();//清除暂停标志位
							  //SPIA_RX_STA = 0;
							  return ERR_ALARM;
							}
							KeyBoard_StopSpeed();
						}
						if(ProgramRuningFlag(0)==2)	//退出
						{
							CloseFan();
							ClearStopProgramFlag();
							G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//使用安全开关
							//SPIA_RX_STA = 0;
							return 0;
						}
						else	//继续
						{
							ClearStopProgramFlag();
							continue;
						}
					}
				}
			}
			else if(code_err == code_finish)
			{
				//SPIA_RX_STA = 0;
				err = 0;
				break;
			}
			else if(code_err == code_stop)
			{
				ProgramRuningFlag(2);	//暂停
				SetButtonValue(6,2,1);	//“继续运行”
				while(ProgramRuningFlag(0) == 1)	//暂停
				{
					if(ErrCheck(6))
					{
					  G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//使用安全开关
					  ClearStopProgramFlag();//清除暂停标志位
					  //SPIA_RX_STA = 0;
					  return ERR_ALARM;
					}
					KeyBoard_StopSpeed();
				}
				if(ProgramRuningFlag(0)==2)	//退出
				{
					CloseFan();
					ClearStopProgramFlag();
					G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//使用安全开关
					//SPIA_RX_STA = 0;
					return 0;
				}
				else	//继续
				{
					ClearStopProgramFlag();
					err = 0;
				}
			}
			else if(code_err == code_end)
			{
				ProgramRuningFlag(3);	//退出全速运行
				CloseFan();
				ClearStopProgramFlag();
				G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//使用安全开关
				return 0;
			}
			else
			{
				err = ProgramStepRun_Code(code_err,((PrgCode*)data));
			}

			if(err >= ERR_CMD)
			{
				//memset(tx_cmd,0,sizeof(tx_cmd));
				//tx_cmd[0] = CMD_RS232_OVER;	//结束停止
				//SpiSendCmd(tx_cmd);
				//SPIA_RX_STA = 0;
				break;
			}
			else
			{
				if(ProgramRuningFlag(0) == 0)
				{
					memset(tx_cmd,0,sizeof(tx_cmd));
					tx_cmd[0] = CMD_RS232_OK;	//指令完成
					SpiSendCmd(tx_cmd);
				}
			}
			//SPIA_RX_STA = 0;
		}
	}
	//SPIA_RX_STA = 0;
	while(ReadDriState())//等待驱动结束
	 {
		  delay_200ns();
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
					   return ERR_ALARM;
				  }
			  }
		  }
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:上拉,使用安全开关的时候，松开(高电平)就退出
		  {
			  delay_1us(1);
			  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)
			  {
				  SetDriCmd(0x3F,0x27);
				  return ERR_SAVE_BTN;
			  }
		  }
		  if(KeyBoard_StopSpeed())
		  {
			   SetDriCmd(0x3F,0x26);
			   return ERR_STOP;
		  }
		  NOP;NOP;
	 }
	return err;
}
/********************************************
 * 函数功能：执行 ExecuteNetwork指令，上传加工零件数，接收
 * 		    上位机的控制指令
 * 返回值：0：成功	其他值：查阅
 * //修正于//2018/06/01 for network
 */
void ExecuteNetwork(PrgCode codeStrings,unsigned int* row)
{
	unsigned int i=0,n_num1;
	unsigned int value1;
	long component_value=0;
	u8 Re_Data[8]={0,0,0,0};
	u8 Frame_value=0;
	while(codeStrings.codeStrings[i] != 'k')//直到遇上'k'就跳出循环
	{
		i++;
	}
	i++;
	while(codeStrings.codeStrings[i] == ' ')//跳过空格
	{
		i++;
	}

	if(codeStrings.codeStrings[i] == 'n')//如果是N变量
	{
		i++;
		n_num1 = StringToUint(&codeStrings.codeStrings[i]);//提取n变量
		component_value = s_Nx_variable[n_num1];
	}
	if(component_value>65000)
	{
		value1=0;
	}
	else
	{
		value1=(unsigned int)(component_value&0xFFFF);
	}
	//发送零件数
	SendComponentNum(value1);
	delay_1ms(3);
	SpiReceiveNetData(8,Re_Data,0);

	Frame_value=JudgeReceiveFrame(Re_Data);
	switch(Frame_value)
	{
	case 1:
		break;//上位机没有发送命令帧给STM32
	case 2:
		//SetButtonValue(6,2,1);//给触摸屏发送暂停命令
		net_control_function(2,1);
		SpiSendNetdata(5,Pause_Ok,0);//给STM32发送机械手继续工作命令
		break;
	case 3:
		//SetButtonValue(6,2,1);//给触摸屏发送继续工作命令
		//SpiSendNetdata(5,Continue_OK,0);//给STM32发送机械手继续工作命令
		net_control_function(2,0);
		SpiSendNetdata(5,Continue_OK,0);//给STM32发送机械手继续工作命令
		break;
	case 4:
		//SetScreen(2);//切换页面
		//SetButtonValue(2,9,0);//给触摸屏发送返回原点命令
		//SetScreen(17);//切换页面
		//SetButtonValue(17,4,0);//给触摸屏发送返回原点命令
		//SpiSendNetdata(5,origin_OK,0);//给STM32发送机械手返回原点命令
		//SetScreen(7);//切换页面
		return_home_network();
		break;
	case 5:
		//SetButtonValue(6,18,0);//给触摸屏发送停止工作命令
		//SpiSendNetdata(5,Stop_Ok,0);//给STM32发送机械手返回停止命令
		net_control_function(18,0);
		SpiSendNetdata(5,Stop_Ok,0);//给STM32发送机械手返回停止命令
		break;
	default:break;
	}
	//接收一帧数据

/*
	发送零件数
	接收一帧数据,解析数据，并判断STM32是否收到DSP控制指令
	若无收到给主机回复一帧数据
	若收到数据
	接收STM32发过来的命令，
	解析执行
	把执行情况回复给STM32
*/

}

/********************************************
 * 函数功能：执行rs232指令，执行后就进入死循环，从串口接收数据命令，
 * 			 直到执行结束命令或发生错误时退出
 * 返回值：0：成功	其他值：查阅
 * //修正于//2018/06/01 for network
 */
void ExecuteTeamwork(PrgCode codeStrings,unsigned int* row)
{
	unsigned int i=0,n_num1;
	unsigned int value1;
	long component_value=0;
	while(codeStrings.codeStrings[i] != 'k')//直到遇上'k'就跳出循环
	{
		i++;
	}
	i++;
	while(codeStrings.codeStrings[i] == ' ')//跳过空格
	{
		i++;
	}

	if(codeStrings.codeStrings[i] == 'n')//如果是n变量
	{
		i++;
		n_num1 = StringToUint(&codeStrings.codeStrings[i]);//提取n变量
		component_value = s_Nx_variable[n_num1];
	}
	if(component_value>65000)
	{
		value1=0;
	}
	else
	{
		value1=(unsigned int)(component_value&0xFFFF);
	}
	//发送目前状态值
	SendManipulatorPositionValue(value1);
	delay_1ms(3);
}
/********************************************
 * 函数功能：执行tl指令，
 * 返回值：0：成功	其他值：查阅
 */
void ExecuteToolLength(PrgCode codeStrings)
{
	unsigned char j=0;
	while( ((codeStrings.codeStrings[j]<'0') || (codeStrings.codeStrings[j]>'9')) &&
	        (codeStrings.codeStrings[j]!='+') && (codeStrings.codeStrings[j]!='-') )//直到遇上数字或'+' '-'
	{
	  j++;
	}
	TOOL_END.ToolLength = atof((char*)&codeStrings.codeStrings[j]);
}

/********************************************
 * 函数功能：执行tool指令，工具末端与机械手第六轴末端的位置关系
 * 返回值：0：成功	其他值：查阅
 */
void ExecuteTool(const PrgCode* codeStrings)
{
	unsigned char i=0,j;
	PositionEuler p_e;
	double data;
	while((codeStrings->codeStrings[i]!='.') && (codeStrings->codeStrings[i]!='('))
	{
		i++;
	}
	if(codeStrings->codeStrings[i]=='(')
	{
		p_e = StringToPositionEuler((char*)(&codeStrings->codeStrings[i]));	//已将角度转换为弧度
		TOOL_END.PosEuler = p_e;//数据的处理 xxx
	}
	else //tool.px = -12.34
	{
		i++;	//跳过 '.'
		while(codeStrings->codeStrings[i] == ' ')//跳过空格
		{
			i++;
		}

		j = i;
		while( ((codeStrings->codeStrings[j]<'0') || (codeStrings->codeStrings[j]>'9')) &&
		        (codeStrings->codeStrings[j]!='+') && (codeStrings->codeStrings[j]!='-') )//直到遇上数字或'+' '-'
		{
		  j++;
		}
		data = atof((char*)(&codeStrings->codeStrings[j]));

		if(codeStrings->codeStrings[i++] == 'p')	//px/py/pz
		{
			switch(codeStrings->codeStrings[i++])	//xxx
			{
			case 'x':	TOOL_END.PosEuler.Px = data;
				break;
			case 'y':	TOOL_END.PosEuler.Py = data;
				break;
			case 'z':	TOOL_END.PosEuler.Pz = data;
				break;
			}
		}
		else	//rx/ry/rz
		{
			switch(codeStrings->codeStrings[i++])
			{
			case 'x':	TOOL_END.PosEuler.Rx = data*PI_DIVIDE_180;	//角度转换为弧度
				break;
			case 'y':	TOOL_END.PosEuler.Ry = data*PI_DIVIDE_180;	//角度转换为弧度
				break;
			case 'z':	TOOL_END.PosEuler.Rz = data*PI_DIVIDE_180;	//角度转换为弧度
				break;
			}
		}
	}
}

/********************************************
 * 函数功能：执行Part指令，零件基坐标与机械手基坐标的位置关系
 * 返回值：0：成功	其他值：查阅
 */
void ExecutePart(const PrgCode* codeStrings)
{
	unsigned char i=0,j;
	PositionEuler p_e;
	double data;
	while((codeStrings->codeStrings[i]!='.') && (codeStrings->codeStrings[i]!='('))
	{
		i++;
	}
	if(codeStrings->codeStrings[i]=='(')
	{
		p_e = StringToPositionEuler((char*)(&codeStrings->codeStrings[i]));	//已将角度转换为弧度
		PART_ROBOT.PosEuler = p_e;//数据的处理 xxx
	}
	else
	{
		i++;	//跳过 '.'
		while(codeStrings->codeStrings[i] == ' ')//跳过空格
		{
			i++;
		}

		j = i;
		while( ((codeStrings->codeStrings[j]<'0') || (codeStrings->codeStrings[j]>'9')) &&
		        (codeStrings->codeStrings[j]!='+') && (codeStrings->codeStrings[j]!='-') )//直到遇上数字或'+' '-'
		{
		  j++;
		}
		data = atof((char*)(&codeStrings->codeStrings[j]));

		if(codeStrings->codeStrings[i++] == 'p')	//px/py/pz
		{
			switch(codeStrings->codeStrings[i++])	//xxx
			{
			case 'x':	PART_ROBOT.PosEuler.Px = data;
				break;
			case 'y':	PART_ROBOT.PosEuler.Py = data;
				break;
			case 'z':	PART_ROBOT.PosEuler.Pz = data;
				break;
			}
		}
		else	//rx/ry/rz
		{
			switch(codeStrings->codeStrings[i++])
			{
			case 'x':	PART_ROBOT.PosEuler.Rx = data*PI_DIVIDE_180;	//角度转换为弧度
				break;
			case 'y':	PART_ROBOT.PosEuler.Ry = data*PI_DIVIDE_180;	//角度转换为弧度
				break;
			case 'z':	PART_ROBOT.PosEuler.Rz = data*PI_DIVIDE_180;	//角度转换为弧度
				break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/**************************************
 * 函数功能：字符比较,遇到\0结束,忽略字母大小写
 * 输入参数：length		需要比较的长度
 * 			 s1:字符串1
 * 			 s2：字符串2
 * 返回值：	 0：相等	1：不相等
 */
unsigned char StringCmp(unsigned char length,unsigned char* s1,unsigned char* s2)
{
	 unsigned char ch1,ch2;
	 while (*s1)
	 {
		  if(*s1>= 'A' && *s1<='Z')
			  ch1 = *s1+32;
		  else ch1 = *s1;

		  if(*s2>= 'A' && *s2<='Z')
			  ch2 = *s2+32;
		  else ch2 = *s2;

		  if(ch1 != ch2)
		  {
			 return 1;
		  }
		  s1++;
		  s2++;
	 }
	 if(*s2==0)
	 {
		  return 0;
	 }
	 else
	 {
		  return 1;
	 }
}

/**************************************
 * 函数功能：将double类型的角度转换为字符串，精度是0.001，一次转换6轴
 * 输入参数：angle	指向六轴角度的指针
 * 			 angle_strings 指向字符串的指针
 * 字符串格式：angle_strings[6][8] 6行8列，每一行代表一个轴的数据，单位：度
 * 			   例如,20.123度，会返回"\0\020.123"
 * 			 	    -1.230读，会返回"\0\0-1.230"
 * 			   使用时，建议从右往左显示，遇到\0结束，能够提高效率
 */
void AngleToStrings(double* angle,unsigned char* angle_strings)
{
	 long	 angle_tmp[6];
	 unsigned char i,j,k,sign_flag=0;
	 for(i=0;i<6;i++)
	 {
		  angle_tmp[i] = (long)(((*angle+i)*1000)*PI_MULTI_180_INVERSE);//弧度转换成角度再*1000
		  if(angle_tmp[i]<0)
		  {
			   angle_tmp[i] = -angle_tmp[i];
			   sign_flag |= (0x01<<i);//记录负数
		  }
	 }
	 for(i=0;i<6;i++)
	 {
		  //*(angle_strings+i*7+0)//存放 '-'号
		  *(angle_strings+i*8+1) = angle_tmp[i]/100000;//888.888
		  *(angle_strings+i*8+2) = angle_tmp[i]/10000%10;
		  *(angle_strings+i*8+3) = angle_tmp[i]/1000%10+ '0';
		  *(angle_strings+i*8+4) = '.';			//小数点
		  *(angle_strings+i*8+5) = angle_tmp[i]/100%10+ '0';
		  *(angle_strings+i*8+6) = angle_tmp[i]/10%10+ '0';
		  *(angle_strings+i*8+7) = angle_tmp[i]%10+ '0';

	 }
	 for(i=0;i<6;i++)
	 {
		  for(j=1;j<3;j++)
		  {
			   if(*(angle_strings+i*8+j) == 0)
			   {
					continue;
			   }
			   if(sign_flag&(0x01<<i)) *(angle_strings+i*8+j-1) = '-';
			   break;
		  }
		  if(j==3) *(angle_strings+i*8+j-1) = '-'; //没有这句的时候，-x.xxx会没有‘-’
		  for(k=j;k<3;k++)
		  {
			   *(angle_strings+i*8+k) = *(angle_strings+i*8+k)+ '0';
		  }
	 }

}

/**************************************
 * 函数功能：将浮点型数据转换为字符串
 * 输入参数：value	要转换的数值
 * 			 str	转换后指向字符串的指针（请预留足够的长度，包括+-号,小数点和结束符\0）
 * 			 len	str的长度(包括结束符)
 * 			 f_len	小数点部分的精度
 * 			 zero	0->前面部分用0填充，有正号
 * 			 		1->前面部分不填充,无正号
 * 返回值：	指向string的指针
 */
char *My_DoubleToString(double value,char* str,int len,int f_len,char zero)
{
	int i,j;
	long long l_value,tmp,tmp1,tmp_zhengshu;
	double value_tmp;
	value_tmp = value;
	if(f_len<1) f_len = 1;
	tmp_zhengshu = value_tmp;
	value_tmp = value - tmp_zhengshu;
	for(i=0;i<f_len;i++)	//小数跟整数部分分开乘
	{
		value_tmp = value_tmp*10;
		tmp_zhengshu *= 10;
	}
	l_value = (long)(value_tmp+0.5) + tmp_zhengshu;//强制转换为整型(+0.5是四舍五入)

	if(value>=0.0)
	{
		str[0] = '+';
	}
	else
	{
		str[0] = '-';
		l_value = -l_value;
	}


	for(i=len-2,j=0;i>0;i--,j++)
	{
		if(j == f_len)
		{
			str[i] = '.';
			continue;
		}
		tmp1 = ((double)l_value)*0.1;	//
		tmp = tmp1*10;
		str[i] = l_value-tmp+'0';
		l_value = tmp1;
	}
	str[len-1] = '\0';

	if(zero)
	{
		j=0;	//记录有多少个'0'
		for(i=1; ;i++)
		{
			if(str[i] == '0')
			{
				if((str[i+1] == '.') || (str[i+1] == '\0'))	//如果下一个是.或\0  0.1 1.2
				{
					break;
				}
				else
				{
					j++;
				}
			}
			else
			{
				break;
			}
		}
		if(str[0] == '+')
		{
			for(i=0;i<(len-j-1);i++)
			{
				str[i] = str[i+j+1];//+000123.10	//正号不需要留
			}
			str[i] = 0;
		}
		else
		{
			for(i=0;i<(len-j-1);i++)
			{
				str[i+1] = str[i+j+1];//-000123.10	//负号要留
			}
			str[i+1] = 0;
		}
	}

	str[len-1] = 0;
	return str;
}

/**************************************
 * 函数功能：将浮点型数据转换为字符串
 * 输入参数：value	要转换的数值
 * 			 str	转换后指向字符串的指针（请预留足够的长度，包括+-号,小数点和结束符\0）
 * 			 len	str的长度(包括结束符)
 * 返回值：	指向string的指针
 */
char *longToString(long value,char* str,int len)
{
	int i,j;
	long tmp,tmp1;
	if(value>=0.0)
	{
		str[0] = '+';
	}
	else
	{
		str[0] = '-';
		value = -value;
	}

	for(i=len-2,j=0;i>0;i--,j++)
	{
		tmp1 = ((double)value)*0.1;	//
		tmp = tmp1*10;
		str[i] = value-tmp+'0';
		value = tmp1;
	}
	str[len-1] = '\0';

	j=0;	//记录有多少个'0'
	for(i=1; ;i++)
	{
		if(str[i] == '0')
		{
			if((str[i+1] == '.') || (str[i+1] == '\0'))	//如果下一个是.或\0  0.1 1.2
			{
				break;
			}
			else
			{
				j++;
			}
		}
		else
		{
			break;
		}
	}
	if(str[0] == '+')
	{
		for(i=0;i<(len-j-1);i++)
		{
			str[i] = str[i+j+1];//+000123.10	//正号不需要留
		}
		str[i] = 0;
	}
	else
	{
		for(i=0;i<(len-j-1);i++)
		{
			str[i+1] = str[i+j+1];//-000123.10	//负号要留
		}
		str[i+1] = 0;
	}

	str[len-1] = 0;
	return str;
}

/**************************************
 * 函数功能：将字符串转换为int类型数字
 * 			 如"123"转换为123，
 * 输入参数：指向字符串的指针
 * 			 注意：遇到数字开始转换，然后遇到非数字转换结束
 * 返回值：  unsigned long 类型的值
 */
unsigned long StringToUint(unsigned char* string)
{
	 unsigned long value=0;
	 unsigned char i=0;
	 while((*(string+i)<'0') || (*(string+i)>'9'))//直到遇上数字
	 {
		  i++;
	 }
	 while((*(string+i)>='0') && (*(string+i)<='9'))
	 {
		  value = value * 10 + *(string+i)-'0';
		  i++;
	 }
	 return value;
}
/**************************************
 * 函数功能：将字符串转换为float类型数字(注意，不对正负(+,-)进行处理,
 * 			因为有些指令，符号和数字是分开(中间有空格)的情况下无法处理,需人工单独处理正负)
 * 			 如"123.1"转换为123.1，
 * 输入参数：指向字符串的指针
 * 			 注意：遇到数字开始转换，然后遇到非数字转换结束
 * 返回值：  float 类型的值
 */
float StringToFloat(unsigned char* string)
{
	 float f_value=0;
	 float f_tmp=1;
	 int i_value=0;
	 unsigned char i=0;

	 while((*(string+i)<'0') || (*(string+i)>'9'))//直到遇上数字或'+' '-'
	 {
		  i++;
	 }

//	 while((*(string+i)<'0') || (*(string+i)>'9') ||
//		   (*(string+i)=='+')|| (*(string+i)=='-'))//直到遇上数字或'+' '-'
//	 {
//		  i++;
//	 }

//	 if(*(string+i)=='-')
//	 {
//		 sign = -1;
//	 }
//	 else
//	 {
//		 sign = 1;
//	 }

//	 while(((*(string+i)>='0') && (*(string+i)<='9')) ||
//			(*(string+i)=='+') || (*(string+i)=='-'))
	 while((*(string+i)>='0') && (*(string+i)<='9'))
	 {
		  i_value = i_value * 10 + string[i]-'0';
		  i++;
	 }
	 if(*(string+i) == '.')
	 {
		 	i++;
			while((*(string+i)>='0') && (*(string+i)<='9'))
			{
				 f_tmp = f_tmp * 0.1;
				 f_value = f_value + (float)(*(string+i)-'0') * f_tmp;
				 i++;
			}
//			return (((float)i_value + f_value)*sign);
			return ((float)i_value + f_value);
	 }
	 else
	 {
//		   return (float)(i_value*sign);
		   return (float)i_value;
	 }
}

/***************************************************
 * 功能：检查名字，名字前面和最后面都不能出现空格，一旦发现都会去掉空格
 *
 * 参数：prg_name:被导入的程序编号
 * 		 save_name:以什么名字保存
 * 		 prg_num:要将U盘中第几个程序复制到DSP这边(从0开始)
 *		 mode:保存模式，0->不覆盖保存(如果已存在重复的程序，则不覆盖，且返回文件已存在）
 *		 				1->覆盖保存(如果已存在重复的程序，则覆盖保存）
 * 返回值：0->符合规范的名字
 * 		   6->名字不能为空
 * 		   7->名字过长
 */
u8 CheckPrgName(char* name)
{
	int i,j;

	 i=0;
	 if(*(name+0) == 0)
	 {
		 return 6;//名字不能为空
	 }

	 for(i=0;i<PROGRAM_NAME_LENGTH;i++)
	 {
		 if(*(name) == ' ')
		 {
			 for(j=0;j<PROGRAM_NAME_LENGTH-1;j++)
			 {
				 *(name+j) = *(name+j+1);
				 if(*(name+j+1) == 0) break;
			 }
		 }
		 else break;
	 }

	 if(*(name) == 0) return 6;

	 i=0;
	 while(*(name+i))
	 {
		 i++;
	 }
	 i--;
	 while(*(name+i) == ' ')//名字结尾不能出现空格
	 {
		 *(name+i) = 0;
		 i--;
		 if(i<0) return 6;//名字不能为空
	 }

	 i++;
	 if(i>=PROGRAM_NAME_LENGTH) return 7;//名字过长

	 return 0;
}

/***************************************
 * 函数功能：清零tray(托盘)数据 （打开程序的时候清除）
 */
void ClearTrayData(void)
{
	memset(s_tray,0,sizeof(s_tray));
}
/***************************************
 * 函数功能：获得托盘的总格子数量
 * 输入参数：t_num:托盘编号
 * 返回值：格子总数
 */
long GetTrayGridNum(unsigned char t_num)
{
	return s_tray[t_num].x_num*s_tray[t_num].y_num;
}
/***************************************
 * 函数功能：获得托盘是否有定义过的标志
 * 输入参数：t_num:托盘编号
 * 返回值：0：未定义	1：已定义
 */
unsigned char GetTrayDefState(unsigned char t_num)
{
	return s_tray[t_num].def_flag;
}
/***************************************
 * 修改位置，测试用
 */
void ModifyPosition(unsigned char p_num,long* pulse)
{
	unsigned char i;
	s_position_pulse[p_num].position = p_num;
	s_position_pulse[p_num].usedFlag = 1;
	for(i=0;i<6;i++)
	{
		s_position_pulse[p_num].pulse[i] = *(pulse+i);
	}
}

/***************************************
 * 函数功能：设置某位置的某轴的角度
 * 输入参数：axis:第几轴,1~6
 * 			 angle：设置为多少度（单位是度）
 * 			 pulse：六个轴的脉冲位置的指针
 * 输出参数：none
 */
//执行了px = py.j4+a 指令，即旋转了第四轴时，标志为1，用于配合输入信号9有效是，停止旋转！(仅限四轴系统)
unsigned char THE_FOUR_AIXS_XUANZHUAN=0;
void SetSingleAxisAngle(unsigned char axis,double angle,long* pulse)//PulseToAngle_Single
{
	double tmp,ag_current,ag_diff;
	if(axis>0 && axis<=6)
	{
		if(ROBOT_PARAMETER.AXIS_ROTATE.all & (0x0001<<(axis-1)))
		{
			tmp = -1;
		}
		else
		{
			tmp = 1;
		}
		switch(axis)
		{
		case 1:*(pulse+axis-1) = tmp*angle*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[0]/360.0;break;
		case 2:*(pulse+axis-1) = tmp*angle*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[1]/360.0;break;
		case 3:*(pulse+axis-1) = tmp*angle*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[2]/360.0;break;
		case 4:
			//旋转第四轴，为了保持位置不变，第三轴需要移动
			if(ROBOT_PARAMETER.AXIS_NUM == 4)
			{
				THE_FOUR_AIXS_XUANZHUAN = 1;
			}
			if(ROBOT_PARAMETER.SYS==FOUR_AXIS_B || ROBOT_PARAMETER.SYS==FOUR_AXIS_C)//(angle/(2*PI)*螺距)/螺杆长度 *ROBOT_PARAMETER.RATIO[3]*ROBOT_PARAMETER.MOTOR_PRECISION
			{
				//先计算当前第三轴的角度
				ag_current = *(pulse+axis-1)/ROBOT_PARAMETER.MOTOR_PRECISION/ROBOT_PARAMETER.RATIO[3]/360.0;
				//得到第四轴变化量,第三轴变化量根据第三轴变化量变化而变化，以保持机械手末端高度不变
				ag_diff = ag_current - tmp;
				*(pulse+axis-2) = *(pulse+axis-2) + ag_diff*(angle/360.0*ROBOT_PARAMETER.ROBOT_PARA[3])/ROBOT_PARAMETER.ROBOT_PARA[2]*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[2];
			}
			*(pulse+axis-1) = tmp*angle*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[3]/360.0;
			break;
		case 5:*(pulse+axis-1) = tmp*angle*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[4]/360.0;break;
		case 6:*(pulse+axis-1) = tmp*angle*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[5]/360.0;break;
		default:break;
		}
	}
}

/***************************************
 * 函数功能：更改某位置的某轴的角度
 * 输入参数：axis:第几轴,1~6
 * 			 angle：更改多少度（单位是度）
 * 			 pulse：六个轴的脉冲位置的指针
 * 输出参数：none
 */
//执行了px = py.j4+a 指令，即旋转了第四轴时，标志为1，用于配合输入信号9有效是，停止旋转！(仅限四轴系统)
void SingleAxisAngleChange(unsigned char axis,double angle,long* pulse)//PulseToAngle_Single
{
	double tmp;
	if(axis>0 && axis<=6)
	{
		if(ROBOT_PARAMETER.AXIS_ROTATE.all & (0x0001<<(axis-1)))
		{
			tmp = -1;
		}
		else
		{
			tmp = 1;
		}
		switch(axis)
		{
		case 1:*(pulse+axis-1) = *(pulse+axis-1) + tmp*angle*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[0]/360.0;break;
		case 2:*(pulse+axis-1) = *(pulse+axis-1) + tmp*angle*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[1]/360.0;break;
		case 3:*(pulse+axis-1) = *(pulse+axis-1) + tmp*angle*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[2]/360.0;break;
		case 4:
			//旋转第四轴，为了保持位置不变，第三轴需要移动
			if(ROBOT_PARAMETER.AXIS_NUM == 4)
			{
				THE_FOUR_AIXS_XUANZHUAN = 1;
			}
			if(ROBOT_PARAMETER.SYS==FOUR_AXIS_B || ROBOT_PARAMETER.SYS==FOUR_AXIS_C)//(angle/(2*PI)*螺距)/螺杆长度 *ROBOT_PARAMETER.RATIO[3]*ROBOT_PARAMETER.MOTOR_PRECISION
			{
				*(pulse+axis-2) = *(pulse+axis-2) + tmp*(angle/360.0*ROBOT_PARAMETER.ROBOT_PARA[3])/ROBOT_PARAMETER.ROBOT_PARA[2]*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[2];
			}
			*(pulse+axis-1) = *(pulse+axis-1) + tmp*angle*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[3]/360.0;
			break;
		case 5:*(pulse+axis-1) = *(pulse+axis-1) + tmp*angle*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[4]/360.0;break;
		case 6:*(pulse+axis-1) = *(pulse+axis-1) + tmp*angle*ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[5]/360.0;break;
		default:break;
		}
	}
}

/***************************************************
 * 功能：查询U盘连接状态
 * 参数：prg_num:程序个数的指针
 * 返回值：0->U盘已连接	1->U盘未连接 5->DSP与STM32通信出错
 */
unsigned char GetExStorageState(long* prg_num)
{
	unsigned char tx_cmd[5],rx_cmd[5],t=0,i;
	memset(tx_cmd,0,sizeof(tx_cmd));
	tx_cmd[0] = T_CMD_U_STATE;
	while(1)
	{
		SpiSendCmd(tx_cmd);
		delay_1ms(1);
		if(SpiReceiveCmd(rx_cmd) == 0)
		{
			if(rx_cmd[0] == R_CMD_CONNECT)
			{
				*prg_num = 0;
				for(i=0;i<2;i++)
				{
					*prg_num |= (long)rx_cmd[i+1]<<(i*8);
				}
				return 0;
			}
			else if(rx_cmd[0] == R_CMD_DISCONNECT)
			{
				return 1;
			}
		}
		t++;
		delay_1ms(1);
		if(t >5)break;
	}
	return 5;
}

/***************************************************
 * 功能：获得程序名字，每次一个
 * 参数：num：要获得第几个程序的名字,从0开始
 * 		 prg_name:程序名字的指针
 * 返回值：0->成功	1->失败,num超出范围，没有此程序 5->DSP与STM32通信出错
 */
unsigned char GetExPrgName(Uint32 num,char* prg_name)
{
	unsigned char rx_cmd[5],tx_cmd[5],i,t=0,err=0;
	Uint32 rx_len=0;

	memset(tx_cmd,0,sizeof(tx_cmd));
	tx_cmd[0] = TR_CMD_PRG_NAME;
	for(i=0;i<4;i++)
	{
		tx_cmd[i+1] = num>>(i*8)&0x000000ff;
	}

	while(1)
	{
		SpiSendCmd(tx_cmd);
		delay_1ms(1);
		if(SpiReceiveCmd(rx_cmd) == 0)
		{
			if(rx_cmd[0] == TR_CMD_PRG_NAME)
			{
				for(i=0;i<2;i++)
				{
					rx_len |= (Uint32)rx_cmd[i+1]<<(i*8);//计算程序名字的长度
				}
				err = SpiReceiveData(rx_len,(u8*)prg_name,0);
				if(err == 0)
				{
					return 0;
				}
			}
			else if(rx_cmd[0] == R_CMD_PRG_FLOW)
			{
				return 1;
			}
		}
		t++;
		delay_1ms(1);
		if(t>5)break;
	}
	return 5;
}

/***************************************************
 * 功能：获得当前有效位置个数
 * 参数：NONE
 * 返回值：有效位置个数
 */
Uint32 GetPositionNum(void)
{
	Uint32 i=0;
	while(s_position_pulse[i].usedFlag)
	{
		i++;
	}
	return i;
}
/***************************************************
 * 功能：将程序从内部存储导出到U盘
 * 参数：prg_name:被导出的程序
 * 		 new_name:以什么名字保存
 *		 mode:保存模式，0->不覆盖保存(如果已存在重复的程序，则不覆盖，且返回文件已存在）
 *		 				1->覆盖保存(如果已存在重复的程序，则覆盖保存）
 * 返回值：0->保存成功
 * 		   1: 程序读取失败，读取出错（校验失败）
 * 		   2：无此程序
 * 		   3：程序重复
 * 		   4：保存失败，请重试
 * 		   5: DSP与STM32通信出错
 * 		   6：程序名不能为空
 * 		   7：程序名过长
 */
unsigned char export_program(char* prg_name,char* new_name,u8 mode)
{
	u8 rx_cmd[5],tx_cmd[5],err,t=0,t1=0,t2=0;
	int i;
	unsigned int num=0,lines_num=0;
	long data_len=0,name_len=0,p_data_len;
	memset(tx_cmd,0,sizeof(tx_cmd));

	//名字检查
	err = CheckPrgName(prg_name);
	if(err) return err;

	err = CheckPrgName(new_name);
	if(err) return err;

	//计算程序长度
	err = OpenProgram((u8*)prg_name,&num);
	if(err) return err;

	lines_num = GetLinesCurrentProgram();
	data_len = lines_num * COLUMNS_IN_LINE;//每行COLUMNS_IN_LINE个字符，包括最后一位的0

	p_data_len =GetPositionNum()*sizeof(s_position_pulse[0]);//sizeof(s_position_pulse[0])=14

	name_len = strlen(new_name)+1;

	for(i=0;i<4;i++)
	{
		tx_cmd[i+1] = name_len>>(i*8)&0x000000FF;
	}

	if(mode) tx_cmd[0] = TR_CMD_CODE_TF;//覆盖保存
	else tx_cmd[0] = TR_CMD_CODE_T;//不覆盖保存

	while(1)
	{
		err = 0;
		SpiSendCmd(tx_cmd);//先发送复制的程序名字的长度（复制到U盘后保存的名字）
		delay_1us(100);
		SpiSendData(name_len,(u8*)new_name,0);//发送复制的程序名字
		delay_1ms(20);
		if(SpiReceiveCmd(rx_cmd) == 0)//等回复
		{
			if(rx_cmd[0] == TR_CMD_OK)
			{

			}
			else if(rx_cmd[0] == R_CMD_EXIST)//如果文件已存在
			{
				return 3;//程序重复
			}
			else
			{
				t++;
				if(t>=5) break;
				else continue;
			}

			for(i=0;i<4;i++)
			{
				tx_cmd[i+1] = data_len>>(i*8)&0x000000FF;
			}
			SpiSendCmd(tx_cmd);//发送数据长度
			delay_1us(100);
			SpiSendData(data_len,(u8*)s_program_code,0);//发送程序文本
			delay_1us(100);
			for(i=0;i<4;i++)
			{
				//*2是因为DSP这边以16bits发送，STM32那边以8bits接收
				tx_cmd[i+1] = (p_data_len*2)>>(i*8)&0x000000FF;
			}
			SpiSendCmd(tx_cmd);//发送数据长度
			delay_1us(100);
			SpiSendData(p_data_len,(u8*)s_position_pulse,1);//发送P点位置
			delay_1us(100);

			t1 = 0;
			t2 = 0;
			while(1)//查询保存是否成功
			{
				err = 0;
				delay_1ms(500);
				tx_cmd[0] = T_CMD_SAVE_STATE;
				SpiSendCmd(tx_cmd);//发送查询命令
				delay_1us(100);
				err = SpiReceiveCmd(rx_cmd);//接收
				if(err)//连续通信错误5次就退出
				{
					t1++;
					if(t1>5)
						return 5;
				}
				if(rx_cmd[0] == R_CMD_SAVE_FAILED)
				{
					return 4;
				}
				else if(rx_cmd[0] == TR_CMD_OK)
				{
					return 0;
				}
				t1=0;
				t2++;
				if(t2>=6)
				{
					return 4;
				}
			}
		}
		t++;
		delay_1ms(1);
		if(t>5)break;
	}
	return 5;
}

/***************************************************
 * 功能：将程序从内部存储导出到U盘,之后打开之前已经被打开的程序
 * 参数：prg_name:被导出的程序
 * 		 new_name:以什么名字保存
 *		 mode:保存模式，0->不覆盖保存(如果已存在重复的程序，则不覆盖，且返回文件已存在）
 *		 				1->覆盖保存(如果已存在重复的程序，则覆盖保存）
 * 返回值：0->保存成功
 * 		   1: 程序读取失败，读取出错（校验失败）
 * 		   2：无此程序
 * 		   3：程序重复
 * 		   4：保存失败，请重试
 * 		   5: DSP与STM32通信出错
 * 		   6：程序名不能为空
 * 		   7：程序名过长
 */
unsigned char ExportProgram(char* prg_name,char* new_name,u8 mode)
{
	unsigned char err,err1;
	unsigned int num;
	err = export_program(prg_name,new_name,mode);//导出后，内存会被刷新，所以要重新打开之前被打开的程序
	if(StringCmp(PROGRAM_NAME_LENGTH,(unsigned char*)prg_name,(unsigned char*)s_program_name_num.program_name))
	{
		if(err != 6 || err != 7)
		{
			err1 = OpenProgram((unsigned char*)s_program_name_num.program_name,&num);
			if(err1)
			{
				//打开失败则清空
				s_program_name_num.program_name[0] = 0;
				s_program_name_num.num = 0;
				SetTextValue(7, 82, "");
				SetTextValue(7, 83, "");
			}
		}
	}
	return err;
}

/***************************************************
 * 功能：将程序从U盘导入内部存储
 * 参数：prg_name:被导入的程序编号
 * 		 save_name:以什么名字保存
 * 		 prg_num:要将U盘中第几个程序复制到DSP这边(从0开始)
 *		 mode:保存模式，0->不覆盖保存(如果已存在重复的程序，则不覆盖，且返回文件已存在）
 *		 				1->覆盖保存(如果已存在重复的程序，则覆盖保存）
 * 返回值：0->保存成功
 * 		   1->保存到flash失败
 * 		   2->保存失败
 * 		   3->程序重复
 * 		   4->读取U盘出错
 * 		   5->DSP与STM32通信出错
 * 		   6->名字不能为空
 * 		   7->名字过长
 * 		   0xff->超过程序最大数量，不能再新建
 */
unsigned char import_program(u32 read_num,char* save_name,u8 mode)
{
	 u8 tx_cmd[5],rx_cmd[5],t1=0,err=0,num;
	 int i;
	 u8 dupli_flg=0;//标记程序是否重复 0->没有重复	1->重复
	 u32 prg_len=0,pos_len=0;
	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));

	 //检查名字
	 err = CheckPrgName(save_name);
	 if(err) return err;

	//判断prg_name是否重复
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);		//获得内部存储中程序个数
	 if((num==0xff) || (num==0))	//0xff表示这是第一次使用，还没有保存过,所以程序个数为0
	 {
		  num =0;
	 }
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info[0])*num,0,(unsigned char*)prg_info);//将程序信息读出
	 for(i=0;i<num;i++)
	 {
		  if(!StringCmp(PROGRAM_NAME_LENGTH,prg_info[i].prgName,(u8*)save_name))
		  {
			  dupli_flg = 1;//标记程序重复
			  if(mode)
			  {
				  //
			  }
			  else return 3;//程序名重复
			  break;
		  }
	 }
	 if(dupli_flg == 0)//如果不是覆盖，将会新建程序，此时应该检查是否超过程序最大个数
	 {
		 if(num >= MAX_PROGRAM_NUM) return 0xff;				//超过最大程序个数，不能再新建
	 }

	//发送让stm32从U盘读取程序
	tx_cmd[0] = T_CMD_READ;
	for(i=0;i<4;i++)
	{
		tx_cmd[i+1] = (read_num>>(i*8)) & 0x000000ff;
	}
	SpiSendCmd(tx_cmd);

	memset(tx_cmd,0,sizeof(tx_cmd));
	while(1)
	{
		//等待stm32从U盘读取数据完成
		delay_1ms(500);
		tx_cmd[0] = T_CMD_READ_STATE;
		SpiSendCmd(tx_cmd);
		delay_1us(100);
		if(SpiReceiveCmd(rx_cmd))
		{
			t1++;
			if(t1>5) return 5;
			continue;
		}

		if(rx_cmd[0] == R_CMD_READ_FAILED) return 4;
		if(rx_cmd[0] == TR_CMD_OK)//stm32读取U盘成功
		{
			while(1)
			{
				//发送让stm32传输数据的命令
				tx_cmd[0] = TR_CMD_CODE_R;
				SpiSendCmd(tx_cmd);
				delay_1us(100);
				if(SpiReceiveCmd(rx_cmd))
				{
					delay_1ms(300);//等待stm32退出接收模式
					return 5;
				}

				//开始接收数据
				prg_len=0;
				for(i=0;i<4;i++)
				{
					prg_len |= (Uint32)rx_cmd[i+1]<<(i*8);
				}
				memset(s_program_code,0,sizeof(s_program_code));		//清零
				if(SpiReceiveData(prg_len,(u8*)s_program_code,0))
				{
					delay_1ms(200);//等待stm32退出接收模式
					return 5;
				}
				delay_1us(100);
				if(SpiReceiveCmd(rx_cmd))
				{
					delay_1ms(100);//等待stm32退出接收模式
					return 5;
				}
				pos_len=0;
				for(i=0;i<4;i++)
				{
					pos_len |= (Uint32)rx_cmd[i+1]<<(i*8);
				}
				memset(s_position_pulse,0,sizeof(s_position_pulse)); 	//清零
				if(SpiReceiveData(pos_len/2,(u8*)s_position_pulse,1))
				{
					return 5;
				}

				//保存到flash
				err = 0;
				if(dupli_flg==1)//如果程序重复则覆盖
				{
					err = SaveProgram((u8*)save_name);
					if(err)	return err;
					else	return 0;
				}
				else//否则新建再保存
				{
					err = NewProgram((u8*)save_name,1);//不清空新建
					if(err)	return err;
					err = SaveProgram((u8*)save_name);
					if(err)	return err;
					else 	return 0;
				}
			}
		}
		t1++;
		if(t1>5) return 5;
	}
	//return 0;//2018/05/22注释掉；
}

/***************************************************
 * 功能：将程序从U盘导入内部存储,并打开之前已经被打开的程序
 * 参数：prg_name:被导入的程序编号
 * 		 save_name:以什么名字保存
 * 		 prg_num:要将U盘中第几个程序复制到DSP这边(从0开始)
 *		 mode:保存模式，0->不覆盖保存(如果已存在重复的程序，则不覆盖，且返回文件已存在）
 *		 				1->覆盖保存(如果已存在重复的程序，则覆盖保存）
 * 返回值：0->保存成功
 * 		   1->保存到flash失败
 * 		   2->保存失败
 * 		   3->程序重复
 * 		   4->读取U盘出错
 * 		   5->DSP与STM32通信出错
 * 		   6->名字不能为空
 * 		   7->名字过长
 * 		   0xff->超过程序最大数量，不能再新建
 */
unsigned char ImportProgram(u32 read_num,char* save_name,u8 mode)
{
	unsigned char err,err1;
	unsigned int num;
	err = import_program(read_num,save_name,mode);//导出后，内存会被刷新，所以要重新打开之前被打开的程序
	if(StringCmp(PROGRAM_NAME_LENGTH,(unsigned char*)save_name,(unsigned char*)s_program_name_num.program_name))
	{
		if(err != 6 || err != 7 )
		{
			err1 = OpenProgram((unsigned char*)s_program_name_num.program_name,&num);
			if(err1)
			{
				//打开失败则清空
				s_program_name_num.program_name[0] = 0;
				s_program_name_num.num = 0;
				SetTextValue(7, 82, "");
				SetTextValue(7, 83, "");
			}
		}
	}
	return err;
}
//===========================================================================
// No more.
//===========================================================================
