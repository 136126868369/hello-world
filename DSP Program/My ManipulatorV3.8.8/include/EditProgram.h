/*
 * PickUpTextCode.h
 *
 *  Created on: 2015-1-9
 *      Author: rd49
 */

#ifndef PICKUPTEXTCODE_H_
#define PICKUPTEXTCODE_H_

#include "DSP2833x_Device.h"
#include "kinematic_explain.h"

//DSP跟STM32通信的设计到的命令
#define TR_CMD_OK				1	//发送或接收 命令/数据正确
#define TR_CMD_REPEAT			2	//命令传输出错，请求重发
#define TR_CMD_PRG_NAME		    3	//回复程序名的指令
#define TR_CMD_CODE_T			4	//发送传输程序数据命令
#define TR_CMD_CODE_TF		    5	//发送传输程序数据命令
#define TR_CMD_CODE_R			6	//发送接收程序数据命令
#define TR_CMD_OVER			    7	//传输出错，（超过重复发送次数时）

#define T_CMD_U_STATE			8	//发送查询U盘状态命令
#define T_CMD_READ			    9	//发送读取程序指令(让stm32去U盘获取程序)
#define T_CMD_SAVE_STATE		10	//发送查询上一次是否保存成功
#define T_CMD_READ_STATE		11	//发送查询上一次是否读取成功

#define R_CMD_CONNECT			12	//U盘已连接 后面四个字节表示已存在的程序个数
#define R_CMD_DISCONNECT		13	//U盘未连接
#define R_CMD_PRG_FLOW		    14	//超出目前程序范围
#define R_CMD_SAVE_FAILED	    15	//保存失败
#define R_CMD_READ_FAILED	    16	//读取失败
#define R_CMD_EXIST			    17	//文件已存在
#define R_CMD_INEXIST			18	//文件不存在
#define R_CMD_IDLE			    19	//空闲状态
#define R_CMD_BUSY			    20	//忙状态

#define T_CMD_SET_IO		    21	//设置IO状态
#define T_CMD_VERSION		    22	//查询当前版本号

#define T_CMD_USART			    23	//dsp发送数据到stm32，让其用串口发送出去
#define R_CMD_USART			    24	//stm32串口接收到的数据发送给DSP
#define R_CMD_USART_RX_STA	    25	//查询串口是否有数据到来
#define R_CMD_USART_RX_OK	    26	//串口有数据到来
#define R_CMD_USART_RX_NO	    27	//串口无数据到来
#define TR_CMD_NG			    28	//接收到的数据校验出错

#define CMD_RS232_OK			29	//完成网络端口传输来的命令后回复的指令
#define CMD_RS232_BEGIN			30	//rs232 从第一个开始
#define CMD_RS232_OVER			31	//因运行中发生某些原因(伺服报警，限位，暂停等)而停止


#define CMD_ETH_OK				33	//完成网络端口传输来的命令后回复的指令
#define CMD_ERROR				34	//spi接收到错误的数据，请求重发
#define CMD_ETH_BEGIN			35	//eth 从第一个开始
#define CMD_ETH_OVER			36	//因运行中发生某些原因(伺服报警，限位，暂停等)而停止

#define MXA_REPEAT			5	//数据传输过程出错时，最多重复发送的次数

#define COLUMNS_IN_LINE		19//一行最多19个字符（最后一个给结束符'\0'）
#define ROWS_IN_PAGE  		15//一页的行数
#define PROGRAM_NAME_LENGTH	20//程序名字长度
#define ROWS_IN_PROGRAM	    750//程序最大行数,(对应触摸屏是50页)750
#define MAX_POSITION_NUM	100//最大P位置个数100
#define MAX_PROGRAM_NUM	    50//最多能保存的程序数量50
#define MAX_POUT_NUM		24//输出IO总数目
#define MAX_PIN_NUM			18//输入IO总数目
#define MAX_TRAY_NUM		20//最大托盘编号20
#define MAX_N_NUM			50//最大的变量编号
#define MAX_F_NUM			50//浮点型变量的最大编号
#define MAX_RETURN			20//return堆栈最大值
#define ASCII_0 			0x30//0的ASCII码
#define ASCII_9 			0x39//9的ASCII码
#define ASCII_ENTER		    0x0d//回车
#define ASCII_BACKSPACE  	0x08//backspace
#define ASCII_DEL			0x0c//删除一行
#define ASCII_ESC			0x1b//ESC退出
#define ASCII_SAVE		    0x11//保存
#define MAX_NUM_COMPONENT   65000//最大加工零件数
#define MAX_STATUS_VALUE    60000//机械手最大位置状态数

struct ProgramCode{
	 //unsigned char num;
	 unsigned char codeStrings[COLUMNS_IN_LINE];
};
typedef struct ProgramCode PrgCode;

struct program_name_num{
	unsigned int num;//记录当前被打开第几个程序,0表示没有打开
	char program_name[PROGRAM_NAME_LENGTH];//记录当前打开的程序名
};

struct ThreeDimCoordStru{//三维坐标
	double x;
	double y;
	double z;
};

struct TrayStru{
	unsigned char def_flag;//标记是否被定义过：0：未定义	1：已定义
	unsigned int x_num;//托盘的宽将被分x_num份
	unsigned int y_num;//托盘的长将被分y_num份
	double width;//宽每份的长度
	double length;//长每份的长度
	double fix_x;	//修正x
	double fix_y;	//修正y
	double fix_z;	//修正z
	//double x,y;//长宽每份的长度
	//long pulse[6];//起始点脉冲位置
	PositionGesture pos_ges;//托盘起始点位姿
	struct ThreeDimCoordStru unit_vector_x;//x单位向量
	struct ThreeDimCoordStru unit_vector_y;//y单位向量
	double angle[6];	//保存第一个点的角度，用来求托盘最优解的参考点
};

unsigned char ExecuteOpen(PrgCode codeStrings);
void ExecuteOvrd(PrgCode codeStrings);
void ExecuteSpeed(PrgCode codeStrings);
unsigned char ExecuteNx(unsigned char* strings,long* nx);
unsigned char ExecuteFx(unsigned char* strings,double* fx);
unsigned char ExecutePx(PrgCode codeStrings);
void ExecuteTray(PrgCode codeStrings);
void ExecuteFtx(PrgCode codeStrings);
u8 ExecuteBaud(PrgCode codeStrings);
extern long CURRENT_BAUD;	//当前波特率
/***************************************
 * 函数功能：获得程序行的首指针
 * 输入参数：void
 * 输出参数：s_program_code的指针
 */
PrgCode* GetProgramCode(void);

/***************************************
 * 函数功能：清零程序和脉冲
 * 输入参数：NONE
 * 输出参数：NONE
 */
void ClearProgramPulseData(void);


/***************************************
 * 函数功能：让第row行获得文本
 * 输入参数：row	行
 * 			 strings	字符串首指针
 * 			 说明：获得的字符串以\0作为结尾
 * 返回值：	 编译出错信息
 */
unsigned char GetCodeStrings(unsigned int row,unsigned char* strings);


/*********************************************
 * 函数功能：	获得当前程序的代码总行数
 * 输入参数：	none
 * 返回值：		代码行数
 */
unsigned int GetLinesCurrentProgram(void);


/*********************************************
 * 函数功能：	获得第row行代码
 * 输入参数：	row	行
 * 				code	代码文本首指针
 * 				注意：代码文本包含了最多14个字符的代码,要获得全部数据，
 * 				请使用一个长度为19的unsigned char类型的数组，如code[19]；
 * 				然后将数组指针首地址作为参数输入：GetCodeTxt(row,code);
 */
void GetRowCode(unsigned int row,unsigned char* codeStrings);


/*********************************************
 * 函数功能：	获得已保存程序数量
 * 输入参数：	none
 * 返回值：		已保存程序的数量（范围0~99）
 */
unsigned char GetProgramNum(void);


/*********************************************
 * 函数功能：	获得对应编号的程序的名字
 * 输入参数：	num：	程序名编号（不能大于目前已保存的程序数量）
 * 				name：	程序名首指针（）
 * 				说明：	程序是按顺序连续排列的，从0开始。
 * 返回值：		0：成功		1：失败
 */
unsigned char GetProgramName(unsigned char prg_num,unsigned char *name);


/***************************************************
 * 功能：获得当前有效位置个数
 * 参数：NONE
 * 返回值：有效位置个数
 */
Uint32 GetPositionNum(void);

/**************************************
 * 函数功能：提取当前行的位置，已经进行语法检查（如果此行存在有P点位置Pxx的话，示教使用）
 * 输入参数：row：行
 * 返回值：0->成功	0x91->此位置已被使用，询问是否覆盖  其他非0：错误(查阅错误代号)
 */
unsigned int GetPositionNumInRow(unsigned int row,unsigned int* num);

/**************************************
 * 函数名：	 CapturePositionDataInRow
 * 函数功能：捕获当前位置脉冲数据到第row行的P位置中保存（如果此行存在有P点位置Pxx的话，示教使用）
 * 输入参数：row：行
 * 返回值：0：成功	非0：错误
 */
unsigned char CapturePositionDataInRow(unsigned int row);


/******************************************
 * 函数功能：将脉冲值保存到p_num
 */
void SavePositionData(unsigned char p_num,long* pulse);


/******************************************
 * 函数功能：将P位置标记为已使用
 */
void SavePositionState(unsigned char p_num);


/**************************************
 * 函数功能：检测有没有P位置，已经进行语法检查
 * 输入参数：row  行
 * 返回值：	 0：没有发现错误
 * 			 0x91(WARN_DUPLI_P) 发现有重复P位置，用来判断是否覆盖
 * 			 其他：错误(查阅错误代号)
 */
unsigned char CheckPnum(unsigned int row);


/**************************************
 * 函数名：	 GetCurrentAngleStrings
 * 函数功能：获得当前每轴角度转换好的的字符串
 * 输入参数：angle_strings	字符串首指针
 * 			 字符串格式：angle_strings[6][8] 6行8列，每一行代表一个轴的数据，单位：度
 * 			 例如,20.123度，会返回"\0\020.123"
 * 			 	  -1.230读，会返回"\0\0-1.230"
 * 			 	  使用时，建议从右往左显示，遇到\0结束，能够提高效率
 * 返回值：  none
 */
void GetCurrentAngleStrings(unsigned char* angle_strings);


/**************************************
 * 函数功能：获得Pxx点位置每轴的脉冲值
 * 输入参数：p_num	p位置的值（如p11，就输入11，而不是p11)
 * 			 pulse	指向脉冲值的指针，long类型
 * 返回值：	 0：成功	1：P点不存在
 */
unsigned char GetPxxPulseValue(unsigned char p_num,long* pulse);


/*************************************
 * 函数功能：获得目前已被使用的P位置个数
 * 输入参数：none
 * 返回值：	 已被使用的P位置个数
 */
unsigned char GetPositionUsedNum(void);


/**************************************
 * 函数名：	 CheckPositionState
 * 函数功能: 检查P位置使用状态
 * 输入参数: p_num	P位置值（Pxx中的xx，如P11的11）
 * 返回值：	 0：P位置已被使用	1：P位置未被使用
 */
unsigned char CheckPositionState(unsigned char p_num);


void IncreaseColumnChar(unsigned int row,unsigned char column,unsigned char character);


/***************************************
 * 函数名：	 IncreaseEmptyRow
 * 函数功能：在第row行增加一行空白行（也就是回车的时候,增加一行）
 * 输入参数：row	行	（范围0~255）
 * 返回值：	 0：成功	1：失败（超过255行）
 */
unsigned char IncreaseEmptyRow(unsigned int row);


void DeleteColumnChar(unsigned int row,unsigned char column);


void ClearRowChar(unsigned int row);
/***************************************
 * 函数名：	 DeleteRow
 * 函数功能：删除程序的第row行
 * 输入参数：row	将被删除的第row行（row范围：0~255）
 * 			 和DeleteRowCodeTxt（）函数的区别：DeleteRowCodeTxt
 * 			 只是删除文本，行是保留的，只不过是空行；而DeleteRowCode（）是删除
 * 			 第row行，并且后面的行会向前平移1个单位
 */
void DeleteRow(unsigned int row);//


void ModifyCursorPosition(unsigned int row,unsigned char column);

/**************************************
 * 函数功能：判断此程序是否存在
 * 输入参数：prg_name	程序名首指针
 * 返回值：	 0：程序存在
 * 			 1：无此程序
 * 			 2：flash出错
 */
unsigned char IsProgramValid(unsigned char* prg_name);

/**************************************
 * 函数名：  NewProgram
 * 函数功能：新建程序
 * 参数		name	程序名首指针，名字最长19个字符
 * 			mdoe:	0->新建时清空存储区域	1->新建时不清空存储区域
 * 返回值	0x00：成功
 * 			0x01：失败(擦除信息块失败)
 * 			0x03：失败（名字重复）
 * 			0x04: 名字过长（19个字符以内）
 * 			0xff：失败（超过最大程序个数）
 */
unsigned char NewProgram(unsigned char* name,unsigned char mode);


/**************************************
 * 函数名：	 OpenProgram
 * 函数功能：打开程序文本和位置数据
 * 输入参数：name	程序名首指针
 * 		   ：num		指向第几个程序的指针
 * 返回值：	 0：打开成功
 * 			 1：打开失败（无此程序）
 */
unsigned char OpenProgram(unsigned char* prg_name,unsigned int* num);


/**************************************
 * 函数名：	 SaveProgram
 * 保存程序文本和位置数据
 * 函数功能：保存程序和位置到相应文件夹
 * 输入参数：name 文件夹名字首指针
 * 返回值：	 0：保存成功
 * 			 1：保存失败(保存出错，可能Flash损坏)
 * 			 2: 保存失败(无此文件夹)
 */
unsigned char SaveProgram(unsigned char* prg_name);


/***************************************
 * 函数名：	 DeleteProgram
 * 删除程序
 * 输入参数	name	将被删除的程序名字
 * 输出参数	0：删除成功
 * 			1：删除失败（擦除信息块错误，可能flash损坏）
 * 			2: 删除失败 (无此程序)
 */
unsigned char DeleteProgram(unsigned char* prg_name);


/***************************************
 * 函数名：	 CopyProgram
 * 删除程序
 * 输入参数	prg_name	将被复制的程序名字
 * 			new_name	复制后的程序名字
 * 输出参数	0：复制成功
 * 			1：复制失败（擦除信息块错误，可能flash损坏）
 * 			2: 复制失败 (无此程序)
 * 			3: 名字重复
 */
unsigned char CopyProgram(unsigned char* prg_name,unsigned char* new_name);


/***************************************
 * 函数名：	 RenameProgram
 * 删除程序
 * 输入参数	old_name	被重命名的程序名字
 * 			new_name	新的名字
 * 输出参数	0：重命名成功
 * 			1：重命名失败（擦除信息块错误，可能flash损坏）
 * 			2: 重命名失败 (无此程序)
 * 			3: 名字重复
 */
unsigned char RenameProgram(unsigned char* old_name,unsigned char* new_name);


/*****************************************
 * 函数功能：SaveAxisNumber 保存机械手轴个数
 * 输入参数：axis_num	轴数量
 * 输出参数：0：保存成功	1：保存失败
 */
unsigned char SaveAxisNumber(unsigned char axis_num);


/*****************************************
 * 函数功能：ReadAxisNumber 读机械手轴个数
 * 输入参数：NONE
 * 输出参数：机械手轴个数
 */
unsigned char ReadAxisNumber(void);

/*****************************************
 * 函数功能：程序单步运行
 * 输入参数：cmd:指令代码
 * 			codeStrings:代码文本
 * 返回值：	 0：成功	其他非0：错误（查阅）
 */
unsigned int ProgramStepRun_Code(unsigned char cmd,const PrgCode* codeStrings);

/*****************************************
 * 函数功能：程序单步运行
 * 输入参数：row：将被运行的行，
 * 			 注意：如果执行的行是跳转或子函数，那么这个值将被修改为跳到相应的行
 * 			 其他情况下是自加1，表示运行下一行
 * 返回值：	 0：成功	其他非0：错误（查阅）
 */
unsigned int ProgramStepRun(unsigned int* row,unsigned char check);


/*****************************************
 * 函数功能：程序全速运行
 * 输入参数：none
 * 返回值：	 none
 */
void ProgramRun(void);


/*****************************************
* 函数功能：中断改变标志位，执行程序时取标志位返回值
* 输入参数：
*
*/
unsigned char ProgramRuningFlag(unsigned char sign);


/*****************************************
 * 函数功能：程序全速运行(带背景指示运行到哪一步)
 * 输入参数：f,更改运行行背景色函数的指针
 * 			 row,从第几行开始运行
 * 返回值：  程序运行到第几行
 */
unsigned char ProgramRunWithPointer(void (*f)(unsigned char),unsigned int* row);


/*****************************************
 * 函数功能：开始运行
 * 输入参数：row  从第row行开始
 * 返回值：
 */
void StartRun(void);


/*****************************************
 * 函数功能：停止运行
 * 输入参数：none
 * 返回值:	 none
 */
void StopRun(void);


/*****************************************
 * 函数功能：暂停运行
 * 输入参数：
 * 返回值
 */
void PuaseRun(void);


/*****************************************
 * 函数功能：清空return的堆栈
 */
void ClearReturnStack(void);


unsigned char GetProgramRunSpeed(void);
void ModifyAllSpeed(unsigned char speed);

/*****************************
 * 函数功能：获得全局速度ovrd
 */
unsigned char GetOvrd(void);

/*****************************
 * 函数功能：获得当前加速度
 */
unsigned int GetAcc(void);

/***************************************
 * 函数功能：获得n变量的值
 * 输入参数：n_num编号
 * 输出参数：n变量的值
 */
long get_n_value(int n_num);

/***************************************
 * 函数功能：获得n变量的值
 * 输入参数：n_num编号
 * 输出参数：n变量的值
 */
double get_f_value(int n_num);

/******************
 * 修改全局速度
 */
unsigned int ModifyOvrd(unsigned char speed);
/******************
 * 修改局部速度speed
 * 返回值：总速度
 */
unsigned int ModifySpeed(unsigned char speed);

unsigned char StringCmp(unsigned char length,unsigned char* s1,unsigned char* s2);
long GetTrayGridNum(unsigned char t_num);
unsigned char GetTrayDefState(unsigned char t_num);


/**************************************
 * 函数功能：将字符串转换为int类型数字
 * 			 如"123"转换为123，
 * 输入参数：指向字符串的指针
 * 			 注意：遇到数字开始转换，然后遇到非数字转换结束
 * 返回值：  unsigned long 类型的值
 */
unsigned long StringToUint(unsigned char* string);


/**************************************
 * 函数功能：将字符串转换为float类型数字
 * 			 如"123.1"转换为123.1，
 * 输入参数：指向字符串的指针
 * 			 注意：遇到数字开始转换，然后遇到非数字转换结束
 * 返回值：  float 类型的值
 */
float StringToFloat(unsigned char* string);

/**************************************
 * 函数功能：将浮点型数据转换为字符串
 * 输入参数：value	要转换的数值
 * 			 str	转换后指向字符串的指针
 * 			 str	转换后指向字符串的指针（请预留足够的长度，包括+-号,小数点和结束符\0）
 * 			 len	str的长度(包括结束符)
 * 			 zero	0->前面部分用0填充，有正号
 * 			 1->前面部分不填充,无正号
 * 返回值：	指向string的指针
 */
char *My_DoubleToString(double value,char* str,int len,int f_len,char zero);

char *longToString(long value,char* str,int len);

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
unsigned char CheckPrgName(char* name);

unsigned char GetExStorageState(long* prg_num);
unsigned char GetExPrgName(Uint32 num,char* prg_name);
unsigned char ExportProgram(char* prg_name,char* new_name,unsigned char mode);
unsigned char ImportProgram(Uint32 read_num,char* save_name,unsigned char mode);


/**************************************
 * 函数功能：处理暂停状态下程序的网络通信功能
 * 输入参数：无
 * 返回值：   无
 */
void Pause_network_function(void);//修正于//2018/06/01
#endif /* PICKUPTEXTCODE_H_ */
