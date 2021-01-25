/*
 * BuildProgram.h
 *
 *  Created on: 2015-1-21
 *      Author: rd49
 */

#ifndef BUILDPROGRAM_H_
#define BUILDPROGRAM_H_
#include "EditProgram.h"
//编辑程序过程中会出现的正确的指令
#define code_start		0x01
#define code_dly		0x02
#define code_end		0x03
#define code_for		0x04
#define code_next		0x05
#define code_gosub		0x06
#define code_goto		0x07
#define code_stop		0x08
#define code_if			0x09
#define code_then		0x0a
#define code_else		0x0b
#define code_mov		0x0c
#define code_mvs		0x0d
#define code_mvh		0x0e
#define code_mvr		0x0f
#define code_mvc		0x10
#define code_ovrd		0x11
#define code_pin		0x12
#define code_pout		0x13
#define code_speed		0x14
#define code_wait		0x15
#define code_loop		0x16
#define code_sub		0x17
#define code_return		0x18
#define code_tray		0x19
#define code_t_x		0x1a
#define code_n_x		0x1b
#define code_p_x		0x1c
#define code_acc		0x1d
#define code_open		0x1e
#define code_baud		0x1f
#define code_rstd		0x20
#define code_disp		0x21
#define code_ft_x		0x22
#define code_finish		0x23
#define code_p_g		0x24	//表示位置+欧拉角的数据
#define code_rs232		0x25
#define code_toollen	0x26
#define code_tool		0x27
#define code_part		0x28
#define code_f_x		0x29	//fx浮点型变量
#define code_network    0x2a    //修正于//2018/06/01 for network
#define code_teamwork   0x2b    //修正于//2018/06/01 for network
#define CORRECT			0x00	//命令正确
//编辑程序过程中会出现的错误信息
#define ERR_CMD				0x31	//命令错误
#define ERR_START			0x32	//*start 重复
#define ERR_DLY				0x33	//延时范围 0.1~999.9秒
#define ERR_NO_END			0x34	//没有END结束
#define ERR_TOO_MUCH_END	0x35	//end重复
#define ERR_NO_P			0x36	//缺少目的位置
#define ERR_P_OVERFLOW		0x37	//P变量名范围P0~P99
#define ERR_ORIENTATION		0x38	//圆弧方向只能是‘-’或正‘+’
#define ERR_OVRD			0x39	//全局速度范围0~100
#define ERR_SPEED			0x3a	//局部速度范围0~100
#define ERR_POUT_OVERFLOW	0x3b	//超过输出IO总数目，范围0~11
#define ERR_PIN_OVERFLOW	0x3c	//超过输入IO总数目，范围0~11
#define ERR_WAIT			0x3d	//wait指令后接pin指令
#define ERR_STAR			0x3e	//缺少*开头的字符串
#define ERR_STR_NAME		0x3f	//*后面至少有一个字符，且在0~9或a~z范围内，中间不能有空格
#define ERR_NO_THIS_LOOP	0x40	//没有此跳转标志
#define ERR_POUND			0x41	//缺少#开头的字符串
#define ERR_NO_THIS_SUB		0x42	//没有此子程序
#define ERR_SUB_NAME		0x43	//#后面至少有一个字符，且在0~9或a~z范围内，中间不能有空格
#define ERR_GUSTURE			0x44	//姿态不一致
#define ERR_TRAY			0x45	//托盘指令使用示例：t1 p12p13p14 10 11
#define ERR_TRAY_OVERFLOW	0x46	//托盘编号范围0~9
#define ERR_TRAY_GRID_OVERFLOW	0x47//超过托盘的格子数量
#define ERR_TRAY_GRID_ZERO	0x48	//托盘的格子编号从1开始
#define ERR_TRAY_UNDEFINE	0x49	//未定义此托盘
#define ERR_N_OVERFLOW		0x4a	//变量n的编号范围0~49
#define ERR_IF_COMPARE		0x4b	//比较符号只能是 <,=,>,<=,>=
#define ERR_IF_OBJECT		0x4c	//比较对象只能是变量或正整数值
#define ERR_STAR_POUND		0x4d	//缺少跳转标志或子程序
#define ERR_P_NO_VALUE		0x4e	//此P位置值为空
#define ERR_ALARM			0x4f	//机械手动作过程中发生报警
#define ERR_SAVE_BTN		0x50	//机械手动作过程中松开安全开关（示教的时候）
#define ERR_STOP			0x51	//机械手动作过程中按下了stop按钮
#define ERR_CIRCLE1			0x52	//12点重合，三点不能成圆
#define ERR_CIRCLE2			0x53	//13点重合，三点不能成圆
#define ERR_CIRCLE3			0x54	//23点重合，三点不能成圆
#define ERR_CIRCLE4			0x55	//三点成直线，三点不能成圆
#define ERR_ACC				0x56	//加减速范围1~100,100对应最大加减速
#define ERR_PIN				0x57	//输入IO:pin+数字如pin1
#define ERR_REPEATED_LOOP	0x58	//重复的跳转标记
#define ERR_REPEATED_SUB	0x59	//重复的子程序
#define ERR_REPEATED_RETURN	0x5a	//重复的return
#define ERR_LACK_OF_RETURN	0x5b	//缺少return
#define ERR_STRING_NAME		0x5c	//只能由数字和字母组成
#define ERR_NO_PROGRAM		0x5d	//不存在此程序
#define ERR_FLASH			0x5e	//FLASH出错
#define ERR_BAUD			0x5f	//波特率设置不正确
#define ERR_RSRD			0x60	//px = rsrd? or px = p(y) + rsrd?
#define ERR_RSRD_1			0x61	//rsrd?
#define ERR_OUT_OF_RANGE	0x62	//超出机械手动作范围
#define ERR_RETURN_OVERFLOW 0x63	//return堆栈溢出
#define ERR_DISP			0x64	//显示指令出错
#define ERR_FT_X			0x65	//托盘修正指令
#define ERR_OUTPUT			0x66	//设置IO错误，通信异常
#define ERR_STM32COMM		0x67	//与stm32通信出错
#define ERR_RS_TIMEOUT		0x68	//等待rs232数据超时
#define ERR_SINGULARITY  	0x69	//出现奇异点
#define ERR_F_OVERFLOW		0x6a	//浮点型变量的编号范围0~49
#define ERR_J1_J4			0x6b	//最多4轴
#define ERR_J1_J5			0x6c	//最多5轴
#define ERR_J1_J6			0x6d	//最多6轴
#define ERR_CANT_ZERO		0x6e	//除数不能为零
#define ERR_NO_RS232		0x6f	//该版本没有RS232
#define ERR_S_OVERFLOW		0x70	//变量的编号s范围0~49
#define ERR_NONE_PROGRAM	0xff	//程序不能为空
//程序单步调试过程中会出现的错误信息

//警告信息
#define WARN_DUPLI_P		0x91	//示教时发现有重复的P位置,用来判断是否覆盖
/***************************************
 * 函数功能：语法检查
 * 参数：codeStrings  被检查的字符串
 */
unsigned char CheckGrammar(PrgCode* codeStrings);

unsigned char GetCommand(PrgCode* codeStrings);

/***************************************
 * 函数功能：编译程序
 * 输入参数：num 接收错误个数
 * 			 err 指向错误代号的指针
 * 			 line_of_err 指向出错的行的指针
 * 返回值：  此程序错误个数
 * 说明：	 Build之前务必先打开一个文件
 * 			 错误信息一次编译最多能够收集num个错误信息；参数输入前请清0
 * 			 如果没有错误信息，err将会全部是0；请确保Build完之后err指向
 * 			 的值全部为0才能运行程序
 * 			 （提示，善于运用sizeof）
 */
unsigned char BuildProgram(unsigned char num,unsigned char* err,unsigned int* line_of_err);

#endif /* BUILDPROGRAM_H_ */
