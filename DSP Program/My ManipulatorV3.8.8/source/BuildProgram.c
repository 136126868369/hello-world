/*
 * BuildProgram.c
 *
 *  Created on: 2015-1-21
 *      Author: rd49
 */
#include "BuildProgram.h"
#include "kinematic_explain.h"
#include "ScreenApi.h"
#include "EditProgram.h"
#include "My_Project.h"
#include "Man_DriverScreen.h"
static const unsigned char s_code_loop[]={"*"};//循环开头标志
static const unsigned char s_code_sub[]={"#"};//子程序开头标志
static const unsigned char s_code_tray[]={"t"};//托盘
static const unsigned char s_code_ft[]={"ft"};
static const unsigned char s_code_n[]={"n"};
static const unsigned char s_code_f[]={"f"};
static const unsigned char s_code_acc[]={"acc"};
static const unsigned char s_code_dly[]={"dly"};
static const unsigned char s_code_end[]={"end"};
//static const unsigned char s_code_for[]={"for "};
//static const unsigned char s_code_next[]={"next "};
static const unsigned char s_code_gosub[]={"gosub"};
static const unsigned char s_code_goto[]={"goto"};
static const unsigned char s_code_stop[]={"stop"};
static const unsigned char s_code_if[]={"if"};
static const unsigned char s_code_then[]={"then"};
static const unsigned char s_code_else[]={"else"};
static const unsigned char s_code_mov[]={"mov"};
static const unsigned char s_code_mvs[]={"mvs"};
static const unsigned char s_code_mvh[]={"mvh"};
static const unsigned char s_code_mvr[]={"mvr"};
static const unsigned char s_code_mvc[]={"mvc"};
static const unsigned char s_code_ovrd[]={"ovrd"};
static const unsigned char s_code_open[]={"open"};
static const unsigned char s_code_pin[]={"pin"};
static const unsigned char s_code_pout[]={"pout"};
static const unsigned char s_code_px[]={"p"};
static const unsigned char s_code_speed[]={"speed"};
static const unsigned char s_code_wait[]={"wait"};
static const unsigned char s_code_return[]={"return"};
static const unsigned char s_code_baud[]={"baud"};
static const unsigned char s_code_rstd[]={"rstd"};
static const unsigned char s_code_rsrd[]={"rsrd"};
static const unsigned char s_code_disp[]={"disp"};
static const unsigned char s_code_rs232[]={"rs232"};
static const unsigned char s_code_finish[]={"finish"};
static const unsigned char s_code_toollen[]={"toollen"};
static const unsigned char s_code_tool[]={"tool"};
static const unsigned char s_code_part[]={"part"};
static const unsigned char s_code_network[]={"network"};//修正于//2018/06/01
static const unsigned char s_code_teamwork[]={"teamwork"};//修正于//2018/06/01
//static const unsigned char s_code_[]={"rs"};


static unsigned char CheckAcc(PrgCode* codeStrings);
static unsigned char CheckGoto(PrgCode* codeStrings);
static unsigned char CheckGosub(PrgCode* codeStrings);
static unsigned char CheckLoop(PrgCode* codeStrings);
static unsigned char CheckSub(PrgCode* codeStrings);
static unsigned char CheckReturn(PrgCode* codeStrings);
static unsigned char CheckDly(PrgCode* codeStrings);
static unsigned char CheckEnd(PrgCode* codeStrings);
static unsigned char CheckMov(PrgCode* codeStrings);
static unsigned char CheckMvs(PrgCode* codeStrings);
static unsigned char CheckMvh(PrgCode* codeStrings);
static unsigned char CheckMvr(PrgCode* codeStrings);
static unsigned char CheckMvc(PrgCode* codeStrings);
static unsigned char CheckOvrd(PrgCode* codeStrings);
static unsigned char CheckOpen(PrgCode* codeStrings);
static unsigned char CheckPout(PrgCode* codeStrings);
static unsigned char CheckPin(PrgCode* codeStrings);
static unsigned char CheckPx(PrgCode* codeStrings);
static unsigned char CheckSpeed(PrgCode* codeStrings);
static unsigned char CheckStop(PrgCode* codeStrings);
//static unsigned char CheckM(PrgCode* codeStrings);
//static unsigned char CheckP(PrgCode* codeStrings);
static unsigned char CheckWait(PrgCode* codeStrings);
static unsigned char CheckTray(PrgCode* codeStrings);
static unsigned char CheckFtx(PrgCode* codeStrings);
static unsigned char CheckTx(unsigned char* codeStrings);
static unsigned char CheckIf(PrgCode* codeStrings);
static unsigned char CheckThen(PrgCode* codeStrings);
static unsigned char CheckElse(PrgCode* codeStrings);
static unsigned char CheckNx(unsigned char* codeStrings);
static unsigned char CheckFx(unsigned char* codeStrings);
static unsigned char CheckBaud(PrgCode* codeStrings);
static unsigned char CheckRstd(PrgCode* codeStrings);
static unsigned char CheckDisp(PrgCode* codeStrings);
static unsigned char CheckRS232(PrgCode* codeStrings);
static unsigned char CheckFinish(PrgCode* codeStrings);
static unsigned char CheckToolLen(PrgCode* codeStrings);
static unsigned char CheckTool(PrgCode* codeStrings);
static unsigned char CheckPart(PrgCode* codeStrings);
static unsigned char CheckNetwork(PrgCode* codeStrings);//修正于//2018/06/01
static unsigned char CheckTeamwork(PrgCode* codeStrings);//修正于//2018/06/01
//static unsigned char CheckE(PrgCode* codeStrings);
//static unsigned char CheckT(PrgCode* codeStrings);
extern struct TrayStru s_tray[MAX_TRAY_NUM];
extern struct EX_Position_stru EX_POSITION;
/***************************************
 * 语法检查
 * 参数：codeStrings  字符串
 * 		 row		  检查的行
 */
unsigned char CheckGrammar(PrgCode* codeStrings)
{
	 unsigned char i,j,code=0,code_flg,err;
	 long n_value;
	 double f_value;
	 int n_num;
	 char str1[20];
	 char str2[19];
	 for(i=0;i<COLUMNS_IN_LINE-3;i++)//-3可有可无（因为命令至少有3个字符）
	 {
		  if(codeStrings->codeStrings[0] == ' ')
		  {
			   for(j=0;j<COLUMNS_IN_LINE - 1;j++)
			   {
					codeStrings->codeStrings[j] = codeStrings->codeStrings[j+1];//左移一个单位，覆盖掉空格
			   }
			   codeStrings->codeStrings[COLUMNS_IN_LINE - 1] = 0;
		  }
		  else
		  {
			   break;
		  }
	 }

	 code_flg = GetCommand(codeStrings);

	 switch(code_flg)
	 {
	   case code_mov:		code = CheckMov(codeStrings);break;
	   case code_mvs:		code = CheckMvs(codeStrings);break;
	   case code_mvh:		code = CheckMvh(codeStrings);break;
	   case code_mvr:		code = CheckMvr(codeStrings);break;
	   case code_mvc:		code = CheckMvc(codeStrings);break;
	   case code_dly:	   	code = CheckDly(codeStrings);break;//dly
	   case code_else:		code = CheckElse(codeStrings);break;
	   case code_end:		code = CheckEnd(codeStrings);break;
	   case code_goto:		code = CheckGoto(codeStrings);break;
	   case code_gosub:		code = CheckGosub(codeStrings);break;
	   case code_loop:		code = CheckLoop(codeStrings);break;//*loop
	   case code_sub:		code = CheckSub(codeStrings);break;//#sub
	   case code_then:		code = CheckThen(codeStrings);break;
	   case code_tray:		code = CheckTray(codeStrings);
	   	   	   	   	   	    if(code == code_tray) ExecuteTray(*codeStrings);//build的时候定义托盘
	   	   	   	   	   	    break;
	   case code_ft_x:		code = CheckFtx(codeStrings);
	   	   	   	   	   	    if(code == code_ft_x) ExecuteFtx(*codeStrings);
	   	   	   	   	   	    break;
	   case code_if:		code = CheckIf(codeStrings);break;//If

	   case code_n_x:		code = CheckNx((unsigned char*)codeStrings);
	   	   	   	   	   	    if(code == code_n_x)
							{
								code = ExecuteNx((unsigned char*)codeStrings,&n_value);
								if(code == 0)
								{
									n_num = StringToUint((unsigned char*)codeStrings);//提取n变量
									ltoa(n_num,str2);
									str1[0] = 'n';
									str1[1] = '\0';
									strcat(str1,str2);
									strcat(str1," = ");
									ltoa(n_value,str2);
									strcat(str1,str2);
									SetTextValue(7, 87, str1);
									code = code_n_x;
								}
							}
							break;//Nx

	   case code_f_x:		code = CheckFx((unsigned char*)codeStrings);
	   	   	   	   	   	    if(code == code_f_x)
	   	   	   	   	   	    {
	   	   	   	   	   	    	code = ExecuteFx((unsigned char*)codeStrings,&f_value);
	   	   	   	   	   	    	if(code == 0)
	   	   	   	   	   	    	{
									n_num = StringToUint((unsigned char*)codeStrings);//提取n变量
									ltoa(n_num,str2);
									str1[0] = 'f';
									str1[1] = '\0';
									strcat(str1,str2);
									strcat(str1," = ");
									My_DoubleToString(f_value,str2,18,3,1);
									strcat(str1,str2);
									SetTextValue(7, 87, str1);
									code = code_f_x;
	   	   	   	   	   	    	}
	   	   	   	   	   	    }
	   	   	   	   	   	    break;//Fx

	   case code_ovrd:		code = CheckOvrd(codeStrings);
	   	   	   	   	   	    if(code == code_ovrd) ExecuteOvrd(*codeStrings);
	   	   	   	   	   	    break;//ovrd
	   case code_open:		code = CheckOpen(codeStrings);break;
	   case code_pout:		code = CheckPout(codeStrings);break;
	   case code_pin:		code = CheckPin(codeStrings);break;
	   case code_p_x:		code = CheckPx(codeStrings);
	   	   	   	   	   	    if(code == code_p_x)
	   	   	   	   	   	    {
	   	   	   	   	   	    	 err = ExecutePx(*codeStrings);//编译的时候执行
	   	   	   	   				 if(err == 0)
	   	   	   	   					 return code_p_x;
	   	   	   	   				 else
	   	   	   	   					 return err;
	   	   	   	   	   	    }
	   	   	   	   	   	    break;
	   case code_return:	code = CheckReturn(codeStrings);break;//return
	   case code_speed:	code = CheckSpeed(codeStrings);
	   	   	   	   	   	    if(code == code_speed) ExecuteSpeed(*codeStrings);
	   	   	   	   	   	    break;//speed
	   case code_stop:		code = CheckStop(codeStrings);break;//stop
	   case code_wait:		code = CheckWait(codeStrings);break;//wait
	   case code_acc:		code = CheckAcc(codeStrings);break;
	   case code_baud:		code = CheckBaud(codeStrings);
	   	   	   	   	   	    if(code == code_baud)
	   	   	   	   	   	    	ExecuteBaud(*codeStrings);
	   	   	   	   	   	   	break;
	   case code_rstd:		code = CheckRstd(codeStrings);break;
	   case code_disp:		code = CheckDisp(codeStrings);break;
	   case code_finish:	code = CheckFinish(codeStrings);break;
	   case code_rs232:		code = CheckRS232(codeStrings);break;
	   case code_toollen:	code = CheckToolLen(codeStrings);break;
	   case code_tool:		code = CheckTool(codeStrings);break;
	   case code_part:		code = CheckPart(codeStrings);break;
	   case code_network:   code= CheckNetwork(codeStrings);break;//修正于//2018/06/01
	   case code_teamwork:  code= CheckTeamwork(codeStrings);break;//修正于//2018/06/01

	   default :			code = ERR_CMD;
	 }
	 return code;
}

/*************************************
 * 函数功能：检查命令
 * 输入参数：codeStrings  字符串
 * 返回值：	 命令代号
 */
unsigned char GetCommand(PrgCode* codeStrings)
{
	 unsigned char code_flg;
	 switch(codeStrings->codeStrings[0])
	 {
	 case 'm':
		 if		 (codeStrings->codeStrings[2] == 'v') 	code_flg = code_mov;
		 else if(codeStrings->codeStrings[2] == 's') 	code_flg = code_mvs;
		 else if(codeStrings->codeStrings[2] == 'h') 	code_flg = code_mvh;
		 else if(codeStrings->codeStrings[2] == 'r') 	code_flg = code_mvr;
		 else if(codeStrings->codeStrings[2] == 'c') 	code_flg = code_mvc;
		 else return ERR_CMD;
		 break;
	 case 'e':
		 if		 (codeStrings->codeStrings[1] == 'l') 	code_flg = code_else;
		 else if(codeStrings->codeStrings[1] == 'n') 	code_flg = code_end;
		 else return ERR_CMD;
		 break;
	 case 'g':
		 if		 (codeStrings->codeStrings[2] == 't') 	code_flg = code_goto;
		 else if(codeStrings->codeStrings[2] == 's') 	code_flg = code_gosub;
		 else return ERR_CMD;
		 break;
	 case 't':
		 if		 (codeStrings->codeStrings[1] == 'h') 	code_flg = code_then;
		 else if(codeStrings->codeStrings[1] == 'e')
		 {
			 code_flg = code_teamwork; //修正于//2018/06/01
		 }
		 else if(codeStrings->codeStrings[1] == 'o')
		 {
			 if(codeStrings->codeStrings[4] == 'l')
			 {
				 code_flg = code_toollen;
			 }
			 else
			 {
				 code_flg = code_tool;
			 }
		 }
		 else if(codeStrings->codeStrings[1]>='0' && codeStrings->codeStrings[1]<='9')
			 	 	 	 	 	 	 	 	 	 	 	code_flg = code_tray;
		 else return ERR_CMD;
		 break;
	 case 'p':
		 if		(codeStrings->codeStrings[1] == 'a') 	code_flg = code_part;
		 else if(codeStrings->codeStrings[1] == 'o') 	code_flg = code_pout;
		 else if(codeStrings->codeStrings[1] == 'i') 	code_flg = code_pin;
		 else if(codeStrings->codeStrings[1]>='0'&&codeStrings->codeStrings[1]<='9') code_flg = code_p_x;
		 else return ERR_CMD;
		 break;
	 case 's':
		 if		 (codeStrings->codeStrings[1] == 'p') 	code_flg = code_speed;
		 else if(codeStrings->codeStrings[1] == 't') 	code_flg = code_stop;
		 else return ERR_CMD;
		 break;
	 case 'd':
		 if(codeStrings->codeStrings[1] == 'i')	code_flg = code_disp;
		 else if(codeStrings->codeStrings[1] == 'l') code_flg = code_dly;
		 else return ERR_CMD;
		 break;
	 case '*':
		 code_flg = code_loop;
		 break;
	 case '#':
		 code_flg = code_sub;
		 break;
	 case 'i':
		 code_flg = code_if;
		 break;
	 case 'n':
		 if(codeStrings->codeStrings[1] == 'e') 	code_flg = code_network;//修正于//2018/06/01
		 else code_flg = code_n_x;
		 break;
	 case 'o':
		 if		 (codeStrings->codeStrings[1] == 'v') 	code_flg = code_ovrd;
		 else if(codeStrings->codeStrings[1] == 'p') 	code_flg = code_open;
		 else return ERR_CMD;
		 break;
	 case 'r':
		 if		 (codeStrings->codeStrings[3] == 'u') 	code_flg = code_return;
		 else if(codeStrings->codeStrings[3] == 'd') 	code_flg = code_rstd;
		 else if(codeStrings->codeStrings[3] == '3') 	code_flg = code_rs232;
		 else return ERR_CMD;
		 break;
	 case 'w':
		 code_flg = code_wait;
		 break;
	 case 'a':
		 code_flg = code_acc;
		 break;
	 case 'b':
		 code_flg = code_baud;
		 break;
	 case 'f':
		 if(codeStrings->codeStrings[1] == 'i' )	code_flg = code_finish;
		 else if(codeStrings->codeStrings[1]=='t')	code_flg = code_ft_x;
		 else code_flg = code_f_x;
		 break;
	 default:
		 code_flg = ERR_CMD;
		 break;
	 }
	 return code_flg;
}
/*************************************
 * 函数功能：编译程序
 * 输入参数：num 接收错误个数
 * 			 err 指向错误代号的指针
 * 			 line_of_err 指向出错的行的指针
 * 返回值：  此程序错误个数
 * 说明：	 Build之前务必先打开一个文件
 * 			 错误信息一次编译最多能够收集num个错误信息；参数输入前请清理
 * 			 如果没有错误信息，err将会全部是0；请确保Build完之后err指向
 * 			 的值全部为0才能运行程序
 * 			 （提示，善于运用sizeof）
 */
extern int RX_POSITION_STA;
extern int USART_RX_STA_B;
unsigned char BuildProgram(unsigned char num,unsigned char* err,unsigned int* line_of_err)
{
	 unsigned int i,j=0;
	 unsigned char max_lines,code[COLUMNS_IN_LINE], err_tmp;

	 //
	 //xxx 子程序不能调用自己
	 //xxx 一个If后接一个Then，else可有可无，但then和else各只能一个
	 ClearReturnStack();//清除return堆栈

	 InitSci();	//初始化串口
	 memset(&EX_POSITION,0,sizeof(EX_POSITION));
	 USART_RX_STA_B  = 0;
	 RX_POSITION_STA = 0;
	 max_lines = GetLinesCurrentProgram();
	 if(max_lines==0)return ERR_NONE_PROGRAM;
	 for(i=0;i<max_lines;i++)
	 {
		  GetRowCode(i,code);
		  err_tmp = CheckGrammar((PrgCode*)code);
		  if((err_tmp>=0x31))
		  {
			   if(j<num)
			   {
					*(err+j) = err_tmp;
					*(line_of_err+j) = i;
			   }
			   j++; //错误个数+1
		  }
	 }
	 return j;
}

/**************************************
 * 检测 acc 指令 acc 1~100
 */
unsigned char CheckAcc(PrgCode* codeStrings)
{
	 unsigned char i;
	 unsigned long value=0;
	 for(i=0;i<3;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_acc[i])
		  {
			   return  ERR_CMD;//命令不正确
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_ACC;//加减速范围1~100,100 is 最大加减速
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_ACC;

	 value = StringToUint(&codeStrings->codeStrings[i]);
	 if(value == 0 || value >100)return ERR_ACC;

	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0') return code_acc;
	 else return ERR_ACC;
}
/**************************************
 * 检测 goto 指令
 */
unsigned char CheckGoto(PrgCode* codeStrings)
{
	 unsigned int i,j;
	 unsigned char str_tmp[COLUMNS_IN_LINE]={0};
	 unsigned char code_strings[COLUMNS_IN_LINE]={0};
	 unsigned int current_lines=0;//当前行数
	 for(i=0;i<4;i++)//
	 {
		  if(codeStrings->codeStrings[i]!=s_code_goto[i])
		  {
			   return  ERR_CMD;//命令错误
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_STAR;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != '*') return ERR_STAR;//缺少*开头的字符串

	 str_tmp[0] = '*';
	 j=1;
	 i++;
	 if(codeStrings->codeStrings[i] == '\0' || codeStrings->codeStrings[i] == ' ')
		 return ERR_STR_NAME;//至少要有一个非空格的字符
	 while(codeStrings->codeStrings[i] && codeStrings->codeStrings[i]!=' ')
	 {
		 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
				 && !(codeStrings->codeStrings[i]>='a' && codeStrings->codeStrings[i]<='z')
				 && (codeStrings->codeStrings[i] != '_'))
		 {
			 return ERR_STR_NAME;//*后面至少有一个字符，且在0~9或a~z范围内，中间不能有空格
		 }
		 str_tmp[j++] = codeStrings->codeStrings[i];
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_STR_NAME;

	 current_lines = GetLinesCurrentProgram();//当前代码总行数
	 for(i=0;i<current_lines;i++)
	 {
		 GetRowCode(i,code_strings);
		 if(code_strings[0]=='*')
		 {
			 if(!StringCmp(COLUMNS_IN_LINE,code_strings,str_tmp)) return code_goto;
		 }
	 }
	 return ERR_NO_THIS_LOOP;//无此跳转标志
}
/**************************************
 * 检测 gosub 指令
 */
unsigned char CheckGosub(PrgCode* codeStrings)
{
	 unsigned int i,j;
	 unsigned char str_tmp[COLUMNS_IN_LINE]={0};
	 unsigned char code_strings[COLUMNS_IN_LINE]={0};
	 unsigned int current_lines=0;//当前行数
	 for(i=0;i<5;i++)//
	 {
		  if(codeStrings->codeStrings[i]!=s_code_gosub[i])
		  {
			   return  ERR_CMD;//命令错误
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_POUND;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != '#') return ERR_POUND;//缺少#开头的字符串

	 str_tmp[0] = '#';
	 j=1;
	 i++;
	 if(codeStrings->codeStrings[i] == '\0' || codeStrings->codeStrings[i] == ' ')
		 return ERR_SUB_NAME;//至少要有一个非空格的字符
	 while(codeStrings->codeStrings[i] && codeStrings->codeStrings[i]!=' ')
	 {
		 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
				 && !(codeStrings->codeStrings[i]>='a' && codeStrings->codeStrings[i]<='z')
				 && (codeStrings->codeStrings[i] != '_'))
		 {
			 return ERR_SUB_NAME;//#后面至少有一个字符，且在0~9或a~z范围内，中间不能有空格
		 }
		 str_tmp[j++] = codeStrings->codeStrings[i];
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_SUB_NAME;

	 current_lines = GetLinesCurrentProgram();//当前代码总行数
	 for(i=0;i<current_lines;i++)
	 {
		 GetRowCode(i,code_strings);
		 if(code_strings[0]=='#')
		 {
			 if(!StringCmp(COLUMNS_IN_LINE,code_strings,str_tmp)) return code_gosub;
		 }
	 }
	 return ERR_NO_THIS_SUB;//无此子程序
}
/**************************************
 * 检测 * 开头的指令(跳转标志)
 */
unsigned char CheckLoop(PrgCode* codeStrings)
{
	 unsigned char i,cmp_flag=0;
	 unsigned int max_lines,j;
	 PrgCode* program_code;
	 for(i=0;i<1;i++)//
	 {
		  if(codeStrings->codeStrings[i]!=s_code_loop[i])
		  {
			   return  ERR_CMD;//命令错误
		  }
	 }
	 if(codeStrings->codeStrings[i]=='\0' || codeStrings->codeStrings[i]==' ')return ERR_STR_NAME;

	 while(codeStrings->codeStrings[i] && codeStrings->codeStrings[i]!=' ')
	 {
		 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
				 && !(codeStrings->codeStrings[i]>='a' && codeStrings->codeStrings[i]<='z')
				 && (codeStrings->codeStrings[i] != '_'))
		 {
			 return ERR_STR_NAME;//*后面至少有一个字符，且在0~9或a~z范围内，中间不能有空格
		 }
		 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_STR_NAME;

	 max_lines = GetLinesCurrentProgram();//当前代码总行数
	 program_code = GetProgramCode();
	 for(j=0;j<max_lines;j++)
	 {
		 if(program_code[j].codeStrings[0] == '*')
		 {
			  if(!StringCmp(COLUMNS_IN_LINE,program_code[j].codeStrings,codeStrings->codeStrings))
			  {
				  cmp_flag++;
			  }
			  if(cmp_flag >=2)
			  {
				  return ERR_REPEATED_LOOP;//重复的跳转标志
			  }
		 }
	 }
	 return code_loop;
}
/**************************************
 * 检测 # 开头的指令(子程序标志)
 */
unsigned char CheckSub(PrgCode* codeStrings)
{
	 unsigned char i,return_num=0;
	 unsigned int max_lines,j,cmp_flag=0,flag=0,sub_line;
	 PrgCode* program_code;
	 for(i=0;i<1;i++)//
	 {
		  if(codeStrings->codeStrings[i]!=s_code_sub[i])
		  {
			   return  ERR_CMD;//命令错误
		  }
	 }
	 if(codeStrings->codeStrings[i]=='\0' || codeStrings->codeStrings[i]==' ')return ERR_SUB_NAME;

	 while(codeStrings->codeStrings[i] && codeStrings->codeStrings[i]!=' ')
	 {
		 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
				 && !(codeStrings->codeStrings[i]>='a' && codeStrings->codeStrings[i]<='z')
				 && (codeStrings->codeStrings[i] != '_'))
		 {
			 return ERR_SUB_NAME;//#后面至少有一个字符，且在0~9或a~z范围内，中间不能有空格
		 }
		 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_SUB_NAME;

	 //检查是否有重复的子程序
	 max_lines = GetLinesCurrentProgram();//当前代码总行数
	 program_code = GetProgramCode();
	 for(j=0;j<max_lines;j++)
	 {
		 if(program_code[j].codeStrings[0] == '#')
		 {
			  if(!StringCmp(COLUMNS_IN_LINE,program_code[j].codeStrings,codeStrings->codeStrings))
			  {
				  cmp_flag++;
				  if(flag == 0)
				  {
					  sub_line = j;//记录子程序出现在哪一行(仅记录第一次出现的)，后面要用
					  flag = 1;
				  }
			  }
			  if(cmp_flag >=2)
			  {
				  return ERR_REPEATED_SUB;//重复的子程序
			  }
		 }
	 }

	 //检查此子程序有没有return或多余的return
	 for(j=sub_line+1;j<max_lines;j++)
	 {
		 if(program_code[j].codeStrings[0] == '#')
		 {
			 if(return_num ==0)
			 {
				 return ERR_LACK_OF_RETURN;//缺少return
			 }
			 else if(return_num ==1)
			 {
				 return code_sub;
			 }
			 else
			 {
				 return ERR_REPEATED_RETURN;//有重复的return
			 }
		 }
		 else if(program_code[j].codeStrings[0] == 'r')
		 {
			 if(CheckReturn(&program_code[j]) == code_return)
			 {
				 return_num++;//return+1
			 }
		 }
	 }
	 if(return_num ==0)
	 {
		 return ERR_LACK_OF_RETURN;//缺少return
	 }
	 else if(return_num ==1)
	 {
		 return code_sub;
	 }
	 else
	 {
		 return ERR_REPEATED_RETURN;//有重复的return
	 }
}
/**************************************
 * 检测 return 指令
 */
unsigned char CheckReturn(PrgCode* codeStrings)
{
	 unsigned char i;
	 for(i=0;i<6;i++)//
	 {
		  if(codeStrings->codeStrings[i] != s_code_return[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 //  xxx  检查:有没有重复的return
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0')return code_return;
	 else return ERR_CMD;
}
/**************************************
 * 检测 d 开头的指令 dly 0.1
 */
unsigned char CheckDly(PrgCode* codeStrings)
{
	 unsigned char i=0;
	 float f_value;
	 for(i=0;i<3;i++)
	 {
		  if(codeStrings->codeStrings[i]!=s_code_dly[i])
		  {
			  return  ERR_CMD;//命令错误
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_DLY;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_DLY;//缺少参数

	 f_value = StringToFloat(&codeStrings->codeStrings[i]);

	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '.')
	 {
		 i++;
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i] > '9') return ERR_DLY;
		 i++;
	 }
	 else if(codeStrings->codeStrings[i] == '\0')
	 {
		 if(f_value > 999.9) return ERR_DLY;
		 return code_dly;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_DLY;

	 if(f_value > 999.9) return ERR_DLY;//参数范围 0.0~999.9
	 return code_dly;
}
/**************************************
 * 检测 end 指令
 */
unsigned char CheckEnd(PrgCode* codeStrings)
{
	 unsigned char i;
	 for(i=0;i<3;i++)//
	 {
		  if(codeStrings->codeStrings[i] != s_code_end[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0')return code_end;
	 else return ERR_CMD;
}
/**************************************
 * 检测 hlt 指令
 */
unsigned char CheckStop(PrgCode* codeStrings)
{
	 unsigned char i;
	 for(i=0;i<4;i++)//
	 {
		  if(codeStrings->codeStrings[i] != s_code_stop[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0')return code_stop;
	 else return ERR_CMD;
}
/**************************************
 * 检测 mvs 指令 直线插补 mvs px.x/y/z +/- m/n
 */
unsigned char CheckMvs(PrgCode* codeStrings)
{
	 unsigned char i,err,p_num,n_num;
	 int speed;
	 for(i=0;i<3;i++)//
	 {
		  if(codeStrings->codeStrings[i] != s_code_mvs[i])
		  {
			   return  ERR_CMD;//命令错误
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_NO_P;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == 'p')
	 {
		  i++;
	 }
	 else if(codeStrings->codeStrings[i] == 't')
	 {
		 err = CheckTx(((unsigned char*)codeStrings)+i);
		 if(err != code_t_x)
		 {
			 return err;
		 }

		 else
		 {
			 return code_mvs;
		 }
	 }
	 else
	 {
		 return ERR_NO_P;//格式错误 位置变量名应该是p开头
	 }

	 if(codeStrings->codeStrings[i] == '.')//mvs p
	 {

	 }
	 else if( (codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')
			 && (codeStrings->codeStrings[i] != 'n') )
	 {
		 return ERR_NO_P;
	 }
	 else//mvs pxx || mvs pnx
	 {
		 if(codeStrings->codeStrings[i] == 'n')
		 {
			 i++;
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')
			 {
				 return ERR_NO_P;
			 }
			 n_num = StringToUint(((unsigned char*)codeStrings)+i);
			 if(n_num >= MAX_N_NUM) return ERR_N_OVERFLOW;//n0~n9
		 }
		 else
		 {
			 p_num = StringToUint(((unsigned char*)codeStrings)+i);
			 if(p_num>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;//显示错误 范围：P0~99
			 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//P位置值不能为空
		 }

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
	 }

	 if(codeStrings->codeStrings[i] == '.')
	 {
		 i++;
		 if(codeStrings->codeStrings[i] == 'x' || codeStrings->codeStrings[i] == 'y'||
			 codeStrings->codeStrings[i] == 'z')
		 {
			 i++;
		 }
		 else
		 {
			 return ERR_CMD;//mvs p1.?
		 }

		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] == '+' || codeStrings->codeStrings[i] == '-')
		 {
			 i++;
		 }
		 else
		 {
			 return ERR_CMD;//mvs p1.x ?
		 }

		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;//mvs p1.x + ?

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] == '.')
		 {
			 i++;
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;//mvs p1.x + 12.?
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i] == '\0')
			 {
				 return code_mvs;//mvs p1.x + 123.3
			 }
		 }
		 else if(codeStrings->codeStrings[i] == '\0')
		 {
			 return code_mvs;//mvs p1.x + 123
		 }
	 }
	 else if(codeStrings->codeStrings[i] == '\0')
	 {
		 return code_mvs;
	 }

	 if(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
	 {
		 speed = atoi(((char*)codeStrings)+i);
		 if(speed<=0 || speed>100)	return ERR_SPEED;
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] != '\0') return ERR_SPEED;
	 }
	 else if(codeStrings->codeStrings[i] =='\0')
	 {

	 }
	 else
	 {
		 return ERR_CMD;//p0 = p1.x + 123.3 ?
	 }

	 return code_mvs;
}
/**************************************
 * 检测 mov 指令 关节插补
 */
unsigned char CheckMov(PrgCode* codeStrings)
{
	 unsigned char i,err=0,p_num,n_num;
	 int speed;
	 for(i=0;i<3;i++)//
	 {
		  if(codeStrings->codeStrings[i] != s_code_mov[i])
		  {
			   return  ERR_CMD;//命令错误
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ')
	 {
		 if((codeStrings->codeStrings[i] == '(') || (codeStrings->codeStrings[i]=='@'))
		 {
			 return code_p_g;
		 }
		 else{
			 return ERR_NO_P;
		 }
	 }


	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == 'p')
	 {
		  i++;
	 }
	 else if(codeStrings->codeStrings[i] == 't')
	 {
		 err = CheckTx(((unsigned char*)codeStrings)+i);
		 if(err != code_t_x)
		 {
			 return err;
		 }
		 else
		 {
			 return code_mov;
		 }
	 }
	 else if((codeStrings->codeStrings[i]=='(') || (codeStrings->codeStrings[i]=='@'))
	 {
		 return code_p_g;
	 }
	 else
	 {
		 return ERR_NO_P;//格式错误 位置变量名应该是p开头
	 }

	 if(codeStrings->codeStrings[i] == '.')//mvs p
	 {

	 }
	 else if( (codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')
			 && (codeStrings->codeStrings[i] != 'n') )
	 {
		 return ERR_NO_P;
	 }
	 else//mvs pxx || mvs pnx
	 {
		 if(codeStrings->codeStrings[i] == 'n')
		 {
			 i++;
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')
			 {
				 return ERR_NO_P;
			 }
			 n_num = StringToUint(((unsigned char*)codeStrings)+i);
			 if(n_num >= MAX_N_NUM) return ERR_N_OVERFLOW;//n0~n9
		 }
		 else
		 {
			 p_num = StringToUint(((unsigned char*)codeStrings)+i);
			 if(p_num>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;//显示错误 范围：P0~99
			 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//P位置值不能为空
		 }

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
	 }

	 if(codeStrings->codeStrings[i] == '.')
	 {
		 i++;
		 if(codeStrings->codeStrings[i] == 'x' || codeStrings->codeStrings[i] == 'y'||
			 codeStrings->codeStrings[i] == 'z')
		 {
			 i++;
		 }
		 else if(codeStrings->codeStrings[i] == 'j')
		 {
			 i++;
			 if(codeStrings->codeStrings[i]<='0' || codeStrings->codeStrings[i]>'6') return ERR_CMD;//J1~6
			 i++;
		 }
		 else
		 {
			 return ERR_CMD;//p0 = p1.?
		 }

		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] == '+' || codeStrings->codeStrings[i] == '-')
		 {
			 i++;
		 }
		 else
		 {
			 return ERR_CMD;//p0 = p1.x ?
		 }

		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;//p0 = p1.x + ?

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] == '.')
		 {
			 i++;
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;//mov p1.x + 12.?
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
		 }
		 else if(codeStrings->codeStrings[i] == '\0')
		 {
			 return code_mov;//mov p1.x + 123
		 }
	 }
	 else if(codeStrings->codeStrings[i] == '\0')
	 {
		 return code_mov;	//mov p1
	 }

	 if(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
	 {
		 speed = atoi(((char*)codeStrings)+i);
		 if(speed<=0 || speed>100)	return ERR_SPEED;
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] != '\0') return ERR_SPEED;
	 }
	 else if(codeStrings->codeStrings[i] =='\0')
	 {

	 }
	 else
	 {
		 return ERR_CMD;//p0 = p1.x + 123.3 ?
	 }

	 return code_mov;
}
/**************************************
 * 检测 mvh 指令 螺旋半圆
 */
unsigned char CheckMvh(PrgCode* codeStrings)
{
	 unsigned char i,p_num=0;
	 int speed;
	 for(i=0;i<3;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_mvh[i])
		  {
				return  ERR_CMD;//命令错误
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_NO_P;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == 'p')
	 {
		  i++;
	 }
	 else
	 {
		 return ERR_NO_P;//格式错误 位置变量名应该是p开头
	 }

	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_NO_P;
	 p_num = StringToUint(((unsigned char*)codeStrings)+i);
	 if(p_num>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;//显示错误 范围：P0~49

	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i]=='\0')//遇到'\0'其实已经结束了
	 {
		  if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//P位置值不能为空
		  return code_mvh;
	 }
	 else if((codeStrings->codeStrings[i] == '+') || (codeStrings->codeStrings[i] == '-'))
	 {
	 	 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
	 {
		 speed = atoi(((char*)codeStrings)+i);
		 if(speed<=0 || speed>100)	return ERR_SPEED;
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] != '\0') return ERR_SPEED;
	 }
	 else if(codeStrings->codeStrings[i] == '\0')
	 {

	 }
	 else
	 {
		 return ERR_CMD;
	 }
	 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//P位置值不能为空
	 return code_mvh;
}

/**************************************
 * 检测 mvr 指令 圆弧
 */
unsigned char CheckMvr(PrgCode* codeStrings)
{
	 unsigned char i,p_num=0,j,flg;
	 int speed;
	 long pulse[6];
	 double angle[6];
	 PositionGesture p_g[3];
	 double position[9];
	 for(i=0;i<3;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_mvr[i])
		  {
				return  ERR_CMD;//命令错误
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_NO_P;


	 for(j=0;j<3;j++)
	 {
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] == 'p')
		 {
			  i++;
		 }
		 else
		 {
			 return ERR_NO_P;//格式错误 位置变量名应该是p开头
		 }
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_NO_P;

		 p_num = StringToUint(((unsigned char*)codeStrings)+i);
		 if(p_num>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;//显示错误 范围：P0~49

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//P位置值不能为空
		 GetPxxPulseValue(p_num,pulse);
		 PulseToAngle(pulse,angle);
		 p_g[j] = GetPositionGesture(angle);//获得正解
	 }

	 if(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
	 {
		 speed = atoi(((char*)codeStrings)+i);
		 if(speed<=0 || speed>100)	return ERR_SPEED;
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] != '\0') return ERR_SPEED;
	 }
	 else if(codeStrings->codeStrings[i] == '\0')
	 {

	 }
	 else
	 {
		 return ERR_CMD;
	 }

	 structrue_to_array(p_g, position);
	 flg = judge_three_point(position);
	 if(flg)
	 {
		 switch(flg)
		 {
		 case 2:	return ERR_CIRCLE1;
		 case 3:	return ERR_CIRCLE2;
		 case 4:	return ERR_CIRCLE3;
		 case 5:	return ERR_CIRCLE4;
		 default:	break;
		 }
	 }

	 return code_mvr;
}

/**************************************
 * 检测 mvc 指令 整圆
 */
unsigned char CheckMvc(PrgCode* codeStrings)
{
	 unsigned char i,p_num=0,j,flg;
	 int speed;
	 long pulse[6];
	 double angle[6];
	 PositionGesture p_g[3];
	 double position[9];
	 for(i=0;i<3;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_mvc[i])
		  {
				return  ERR_CMD;//命令错误
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_NO_P;


	 for(j=0;j<3;j++)
	 {
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] == 'p')
		 {
			  i++;
		 }
		 else
		 {
			 return ERR_NO_P;//格式错误 位置变量名应该是p开头
		 }
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_NO_P;

		 p_num = StringToUint(((unsigned char*)codeStrings)+i);
		 if(p_num>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;//显示错误 范围：P0~49

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//P位置值不能为空
		 GetPxxPulseValue(p_num,pulse);
		 PulseToAngle(pulse,angle);
		 p_g[j] = GetPositionGesture(angle);//获得正解
	 }

	 if(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
	 {
		 speed = atoi(((char*)codeStrings)+i);
		 if(speed<=0 || speed>100)	return ERR_SPEED;
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] != '\0') return ERR_SPEED;
	 }
	 else if(codeStrings->codeStrings[i] == '\0')
	 {

	 }
	 else
	 {
		 return ERR_CMD;
	 }

	 structrue_to_array(p_g, position);
	 flg = judge_three_point(position);
	 if(flg)
	 {
		 switch(flg)
		 {
		 case 2:	return ERR_CIRCLE1;
		 case 3:	return ERR_CIRCLE2;
		 case 4:	return ERR_CIRCLE3;
		 case 5:	return ERR_CIRCLE4;
		 default:	break;
		 }
	 }

	 return code_mvc;
}

/**************************************
 * 检测指令 ovrd
 */
unsigned char CheckOvrd(PrgCode* codeStrings)
{
	 unsigned char i;
	 unsigned long value=0;
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_ovrd[i])
		  {
			   return  ERR_CMD;//命令不正确
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_OVRD;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_OVRD;

	 value = StringToUint(&codeStrings->codeStrings[i]);
	 if(value == 0 || value >100)return ERR_OVRD;

	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0') return code_ovrd;
	 else return ERR_OVRD;
}



/**************************************
 * 检测指令 open
 */
unsigned char CheckOpen(PrgCode* codeStrings)
{
	 unsigned char i,err=0;

	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_open[i])
		  {
			   return  ERR_CMD;//命令不正确
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_CMD;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 err = IsProgramValid(&codeStrings->codeStrings[i]);

	 if(err == 1)
	 {
		 return ERR_NO_PROGRAM;
	 }
	 else if(err == 2)
	 {
		 return ERR_FLASH;
	 }
	 return code_open;
}


/**************************************
 * 检测Px指令 p0=p1, p0=p1.x-20.1,p0=p1.j2-12.3,p0=t1 n;
 * 			 p0=rsrd; p0=p+rsrd; p0=p1+rsrd;
 * 			 P0.j2 = (-/+)y/fy +/- (-)z/fz;
 */
unsigned char CheckPx(PrgCode* codeStrings)
{
	 unsigned char i,j,err_tmp,p_num1,p_num2,n_num,f_num;
	 for(i=0;i<1;i++)
	 {
		 if(codeStrings->codeStrings[i] != s_code_px[i])
		 {
			  return ERR_CMD;
		 }
	 }
	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_POUT_OVERFLOW;
	 p_num1 = StringToUint(&codeStrings->codeStrings[i]);
	 if(p_num1 >= MAX_POSITION_NUM) return ERR_P_OVERFLOW;//显示错误 范围：0~99
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 //Px.j2 = y/fy (-/+ z/fz);
	 if(codeStrings->codeStrings[i] == '.')
	 {
		 if(!CheckPositionState(p_num1)) return ERR_P_NO_VALUE;//P位置值不能为空
		 i++;	//跳过 .
		 if(codeStrings->codeStrings[i] != 'j')
		 {
			  return ERR_CMD;//缺少参数 px = ?
		 }
		 i++;	//跳过j
		 if(ROBOT_PARAMETER.AXIS_NUM == 6)
		 {
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'6') return ERR_J1_J6;//J1~6
		 }
		 else if(ROBOT_PARAMETER.AXIS_NUM == 4)
		 {
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'4') return ERR_J1_J4;//J1~6
		 }
		 else if(ROBOT_PARAMETER.AXIS_NUM == 5)
		 {
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'5') return ERR_J1_J5;//J1~6
		 }
		 else
			 return ERR_CMD;

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] != '=')
		 {
			  return ERR_CMD;//缺少参数 px.j1 ?
		 }
		 i++;	//跳过=
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }

		 if((codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9') ||
			 codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-' )
		 {
			 if(codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')
			 {
				 i++;	//跳过符号
				 //符号后面如果没有接数字就报错
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')
					 return ERR_CMD;
			 }
			 while((codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9'))//跳过数字
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i] == '.')
			 {
				 i++;
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;
				 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
				 {
					 i++;
				 }
			 }
		 }
		 else if(codeStrings->codeStrings[i] == 'f')
		 {
			 i++;
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_F_OVERFLOW;
			 f_num = StringToUint(&codeStrings->codeStrings[i]);
			 if(f_num >= MAX_F_NUM) return ERR_F_OVERFLOW;//显示错误 范围：f0~f49
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
			 {
				 i++;
			 }
		 }
		 else return ERR_CMD;//缺少参数 px.j1 = ?


		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] == '\0')
		 {
			 return code_p_x;
		 }
		 else if((codeStrings->codeStrings[i]=='+') || (codeStrings->codeStrings[i]=='-'))
		 {
			 i++;
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i] == 'f')
			 {
				 i++;
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_F_OVERFLOW;
				 f_num = StringToUint(&codeStrings->codeStrings[i]);
				 if(f_num >= MAX_F_NUM) return ERR_F_OVERFLOW;//显示错误 范围：f0~f49
				 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
				 {
					 i++;
				 }
				 while(codeStrings->codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 if(codeStrings->codeStrings[i] == '\0')
				 {
					 return code_p_x;
				 }
				 else return ERR_CMD;
			 }
			 else if((codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9') ||
					  codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')
			 {
				 if(codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')
				 {
					 i++;
					 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')
						 return ERR_CMD;
				 }
				 while((codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9'))//跳过数字
				 {
					 i++;
				 }
				 while(codeStrings->codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 if(codeStrings->codeStrings[i] == '.')
				 {
					 i++;
					 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;
					 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
					 {
						 i++;
					 }
					 while(codeStrings->codeStrings[i] == ' ')//跳过空格
					 {
						 i++;
					 }
					 if(codeStrings->codeStrings[i] == '\0') return code_p_x;//p0 = p1.x + 123.3 ?
					 else return ERR_CMD;
				 }

				 if(codeStrings->codeStrings[i] == '\0') return code_p_x;//p0.j1 = fx + 123 ?
				 else return ERR_CMD;
			 }
			 else return ERR_CMD;
		 }
		 else return ERR_CMD;
	 }
	 else if(codeStrings->codeStrings[i] != '=')	//p0 = ?
	 {
		  return ERR_CMD;//缺少参数 px = ?
	 }
	 i++;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] == 'p')
	 {
		 i++;
		 //px = p  的情况
		 if(codeStrings->codeStrings[i]==0)
		 {
			 return code_p_x;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]==0 ||
			codeStrings->codeStrings[i]=='-')
		 {
//			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
//			 {
//				 i++;
//			 }
			 if(codeStrings->codeStrings[i]==0)	//px = p
			 {
				 return code_p_x;
			 }
			 else if(codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')//px = p + rsrd的情况
			 {
				 i++;	//跳过+ or -
				 while(codeStrings->codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 for(j=0;j<4;j++,i++)
				 {
					  if(codeStrings->codeStrings[i] != s_code_rsrd[j])
					  {
						   return  ERR_RSRD;//命令不正确
					  }
				 }
				 while(codeStrings->codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 if(codeStrings->codeStrings[i] == '\0')
				 {
					 return code_p_x;
				 }
				 else
				 {
					 return  ERR_RSRD;//命令不正确
				 }
			 }
			 else
			 {
				 return ERR_RSRD;
			 }
		 }

		 //px = p(y).z的情况
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')
		 {
			 if(codeStrings->codeStrings[i] == '.')
			 {

			 }
			 else
			 {
				 return ERR_P_OVERFLOW;
			 }
		 }
		 else
		 {
			 p_num2 = StringToUint(&codeStrings->codeStrings[i]);
			 if(p_num2 >= MAX_POSITION_NUM) return ERR_P_OVERFLOW;//显示错误 范围：0~99
		 }
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] == '\0')
		 {
			 if(!CheckPositionState(p_num2)) return ERR_P_NO_VALUE;//P位置值不能为空
			 else
			 {
				 return code_p_x;
			 }
		 }
		 else if(codeStrings->codeStrings[i] == '.')
		 {
			 i++;
		 }
		 else if(codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')//px = py +/- rsrd的情况
		 {
			 i++;	//跳过 +/-
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 for(j=0;j<4;j++,i++)
			 {
				  if(codeStrings->codeStrings[i] != s_code_rsrd[j])
				  {
					   return  ERR_RSRD;//命令不正确
				  }
			 }
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i] == '\0')
			 {
				 return code_p_x;
			 }
			 else
			 {
				 return  ERR_RSRD;//命令不正确
			 }
		 }
		 else
		 {
			 return ERR_CMD;//
		 }

		 if(codeStrings->codeStrings[i] == 'x' || codeStrings->codeStrings[i] == 'y'||
			 codeStrings->codeStrings[i] == 'z')
		 {
			 i++;
		 }
		 else if(codeStrings->codeStrings[i] == 'j')
		 {
			 i++;//跳过j
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;
			 n_num = StringToUint(&codeStrings->codeStrings[i]);
			 if(ROBOT_PARAMETER.AXIS_NUM == 6)
			 {
				 if(n_num > 6) return ERR_J1_J6;//J1~6
			 }
			 else if(ROBOT_PARAMETER.AXIS_NUM == 4)
			 {
				 if(n_num > 4) return ERR_J1_J4;//J1~6
			 }
			 else if(ROBOT_PARAMETER.AXIS_NUM == 5)
			 {
				 if(n_num > 5) return ERR_J1_J5;//J1~6
			 }
			 else
				 return ERR_CMD;
			 i++;
		 }
		 else
		 {
			 return ERR_CMD;//p0 = p1.?
		 }

		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] == '+' || codeStrings->codeStrings[i] == '-')
		 {
			 i++;
		 }
		 else
		 {
			 return ERR_CMD;//p0 = p1.x ?
		 }

		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if((codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') &&
			 codeStrings->codeStrings[i]!='f')
			 return ERR_CMD;//p0 = p1.x + ?

		 //p0 = p1.x + f0
		 if(codeStrings->codeStrings[i]=='f')
		 {
			 i++;	//跳过f
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')
			 	 return ERR_CMD;//p0 = p1.x + f?
			 f_num = StringToUint(&codeStrings->codeStrings[i]);
			 if(f_num >= MAX_F_NUM) return ERR_F_OVERFLOW;//显示错误 范围：f0~f49
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i] != '\0') return ERR_CMD;//p0 = p1.x + fy?
			 return code_p_x;
		 }

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] == '.')
		 {
			 i++;
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i] != '\0') return ERR_CMD;//p0 = p1.x + 123.3 ?
		 }
		 else if(codeStrings->codeStrings[i] != '\0') return ERR_CMD;//p0 = p1.x + 123.3 ?

		 if(!CheckPositionState(p_num2)) return ERR_P_NO_VALUE;//P位置值不能为空
		 else
		 {
			 return code_p_x;
		 }
	 }
	 else if(codeStrings->codeStrings[i] == 't')
	 {
		 err_tmp = CheckTx(&codeStrings->codeStrings[i]);
		 if(err_tmp == code_t_x)
		 {
			 return code_p_x;
		 }
		 else return err_tmp;
	 }
	 else if(codeStrings->codeStrings[i] == 'r')
	 {
		 for(j=0;j<4;j++,i++)
		 {
			  if(codeStrings->codeStrings[i] != s_code_rsrd[j])
			  {
				   return  ERR_RSRD;//命令不正确
			  }
		 }
		 if(ROBOT_PARAMETER.HARDWARE_REV == 7)
		{
			return ERR_NO_RS232;	//07版没有RS232
		}
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] != '\0') return ERR_RSRD;//p0 = rsrd
		 else
		 {
			 if(!CheckPositionState(p_num1)) return ERR_P_NO_VALUE;//P位置值不能为空
			 else
			 {
				 return code_p_x;
			 }
		 }
	 }
	 else
	 {
		 return ERR_CMD;////缺少参数 px = ?
	 }
}

/**************************************
 * 检测 pout 指令    pout1 = 1
 */
unsigned char CheckPout(PrgCode* codeStrings)
{
	 unsigned char i;
	 unsigned int value=0;
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_pout[i])
		  {
			   return  ERR_CMD;//命令不正确
		  }
	 }
	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_POUT_OVERFLOW;
	 value = StringToUint(&codeStrings->codeStrings[i]);
	 if(value >= MAX_POUT_NUM) return ERR_POUT_OVERFLOW;//显示错误 范围：0~11
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != '=')
	 {
		  return ERR_CMD;//set or reset
	 }
	 i++;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if((codeStrings->codeStrings[i] != '0') && (codeStrings->codeStrings[i] != '1'))
	 {
		  return ERR_CMD;// 等于 0 或 1
	 }
	 i++;

	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0') return code_pout;
	 else return ERR_CMD;
}
/**************************************
 * 检测 pin 指令，IO从0开始
 */
unsigned char CheckPin(PrgCode* codeStrings)
{
	 unsigned char i;
	 unsigned int value=0;
	 for(i=0;i<3;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_pin[i])
		  {
			   return  ERR_CMD;//命令不正确
		  }
	 }

	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_PIN_OVERFLOW;
	 value = StringToUint(&codeStrings->codeStrings[i]);
	 if(value >= MAX_PIN_NUM) return ERR_PIN_OVERFLOW;//显示错误 范围：0~17
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != '=')
	 {
		  return ERR_CMD;//set or reset
	 }
	 i++;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if((codeStrings->codeStrings[i] != '0') && (codeStrings->codeStrings[i] != '1'))
	 {
		  return ERR_CMD;// 等于 0 或 1
	 }
	 i++;

	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0') return code_pin;
	 else return ERR_CMD;
}
/**************************************
 * 检测 p 开头的指令
 */
/*unsigned char CheckP(PrgCode* codeStrings)
{
	 unsigned char code;
	 code = CheckPout(codeStrings);
	 if(code == code_pout)
	 {
		  return code;
	 }
	 code = CheckPin(codeStrings);
	 if(code == code_pin)
	 {
		  return code;
	 }
	 return code;
}*/
/**************************************
 * 检测 S 开头的指令
 */
unsigned char CheckSpeed(PrgCode* codeStrings)
{
	 unsigned char i=0;
	 unsigned long value=0;
	 for(i=0;i<5;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_speed[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_SPEED;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_SPEED;

	 value = StringToUint(&codeStrings->codeStrings[i]);
	 if(value == 0 || value >100)return ERR_SPEED;

	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0') return code_speed;
	 else return ERR_SPEED;

}
/**************************************
 * 检测 wait指令(配合pin指令一起使用，IO从0开始，0~11)
 */
unsigned char CheckWait(PrgCode* codeStrings)
{
	 unsigned char i;
	 unsigned char state=0;
	 PrgCode str={0};
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_wait[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_WAIT;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 for(i=0;i<COLUMNS_IN_LINE-5;i++)
	 {
		 str.codeStrings[i] = codeStrings->codeStrings[i+5];
	 }
	 state = CheckPin(&str);
	 if(state == code_pin)
	 {
		 return code_wait;
	 }
	 else if(state == ERR_PIN_OVERFLOW)
	 {
		 return ERR_PIN_OVERFLOW;
	 }
	 else
	 {
		 return ERR_WAIT;
	 }
}
/***************************************
 * 检测t指令 托盘运算 用法：t1 p12p13p14 10 11 (已具备完整的语法检查)
 */
unsigned char CheckTray(PrgCode* codeStrings)
{
	 unsigned char i,j;
	 unsigned int value;
	 long pulse[3][6];
	 double angle[3][6];
	 for(i=0;i<1;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_tray[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9'))
	 {
		 return ERR_CMD;
	 }
	 value = StringToUint(((unsigned char*)codeStrings) + i);
	 if(value>=MAX_TRAY_NUM)return ERR_TRAY_OVERFLOW;//托盘号范围：0~5
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != ' ') return ERR_TRAY;//后接至少一个空格 参考用法

	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 for(j=0;j<3;j++)//检测连续的3个P位置
	 {
		 if(codeStrings->codeStrings[i]!='p')//第一个P
		 {
			 return ERR_NO_P;//后接P位置
		 }
		 i++;
		 value= StringToUint(((unsigned char*)codeStrings) + i);//提取P后面的值
		 if(value>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;

		 if(GetPxxPulseValue(value,pulse[j])) return ERR_P_NO_VALUE;//无效的P位置
		 PulseToAngle(pulse[j],angle[j]);
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
	 }
	 if(CompareGestureByTowAngle(angle[0],angle[1])) return ERR_GUSTURE;//判断姿态是否一致
	 if(CompareGestureByTowAngle(angle[0],angle[2])) return ERR_GUSTURE;//判断姿态是否一致
	 //if(CompareGestureByTowAngle(angle[1],angle[2])) return ERR_GUSTURE;//判断姿态是否一致
	 if(codeStrings->codeStrings[i] != ' ') return ERR_TRAY;//后接至少一个空格 参考用法

	 while(codeStrings->codeStrings[i] == ' ')//跳过空格 t1 p12p13p14 10 11
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_TRAY;//分多少份？
	 value= StringToUint(((unsigned char*)codeStrings) + i);
	 if(value==0) return ERR_TRAY_GRID_ZERO;//不能分0份
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')return ERR_TRAY;//空格后接数字 参考用法
	 value= StringToUint(((unsigned char*)codeStrings) + i);
	 if(value==0) return ERR_TRAY_GRID_ZERO;//不能分0份
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0')
	 {
		   return code_tray;
	 }
	 return ERR_TRAY;//参考用法 t1 p12p13p14 10 11
}

/***************************************
 * 检测ft指令 托盘修正  用法：ft1 px
 */
unsigned char CheckFtx(PrgCode* codeStrings)
{
	unsigned int t_num,p_num;
	int i;
	long pulse[6];
	double angle[6];
	 for(i=0;i<2;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_ft[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9'))
	 {
		 return ERR_CMD;
	 }
	 t_num = StringToUint(((unsigned char*)codeStrings) + i);
	 if(t_num>=MAX_TRAY_NUM)return ERR_TRAY_OVERFLOW;//托盘号范围：0~19
	 if(!GetTrayDefState(t_num)) return ERR_TRAY_UNDEFINE;//未定义此托盘
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_CMD;//后接至少一个空格
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i]!='p')
	 {
		 return ERR_NO_P;//后接P位置
	 }
	 i++;
	 p_num= StringToUint(((unsigned char*)codeStrings) + i);//提取P后面的值
	 if(p_num>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;
	 if(GetPxxPulseValue(p_num,pulse)) return ERR_P_NO_VALUE;//无效的P位置
	 PulseToAngle(pulse,angle);

	 if(CompareGestureByTowAngle(angle,s_tray[t_num].angle))
		 return ERR_GUSTURE;//判断姿态是否一致

	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0')
	 {
		   return code_ft_x;
	 }
	 return ERR_FT_X;
}
/***************************************
 * 检测t指令 托盘运算 用法：t1 10（格子编号从1开始）
 */
unsigned char CheckTx(unsigned char* strings)
{
	unsigned char i=0,t_num,n_num;
	unsigned int value;
	int speed;
	if(*(strings+i) != s_code_tray[i])
	{
	   return  ERR_CMD;
	}
	i++;
	 if(!(*(strings+i)>='0' && *(strings+i)<='9'))
	 {
		 return ERR_CMD;
	 }
	 t_num = StringToUint(strings + i);
	 if(t_num>=MAX_TRAY_NUM)return ERR_TRAY_OVERFLOW;//托盘号范围：0~5
	 if(!GetTrayDefState(t_num)) return ERR_TRAY_UNDEFINE;//未定义此托盘
	 while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
	 {
		 i++;
	 }
	 if(*(strings+i) != ' ') return ERR_CMD;//后接至少一个空格
	 while(*(strings+i) == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(*(strings+i) == 'n')
	 {
		 i++;
		 if(*(strings+i)<'0' || *(strings+i)>'9') return ERR_N_OVERFLOW;
		 n_num = StringToUint(strings+i);
		 if(n_num >= MAX_N_NUM) return ERR_N_OVERFLOW;
	 }
	 else if(*(strings+i)>='0' && *(strings+i)<='9')
	 {
		 value = StringToUint(strings + i);
		 if(value == 0) return ERR_TRAY_GRID_ZERO;//编号从1开始,且只能为数字
		 if(value>GetTrayGridNum(t_num)) return ERR_TRAY_GRID_OVERFLOW;//编号溢出
	 }
	 else
	 {
		 return ERR_TRAY_GRID_ZERO;//编号从1开始,且只能为数字
	 }

	 while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
	 {
		 i++;
	 }
	 while(*(strings+i) == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(*(strings+i)>='0' && *(strings+i)<='9')
	 {
		 speed = atoi(((char*)strings)+i);
		 if(speed<=0 || speed>100)	return ERR_SPEED;
		 while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(*(strings+i) == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(*(strings+i) != '\0') return ERR_SPEED;
	 }
	 else if(*(strings+i) =='\0')
	 {

	 }
	 else
	 {
		 return ERR_TRAY_GRID_ZERO;//p0 = p1.x + 123.3 ?
	 }

	 //if(*(strings+i) != '\0') return ERR_TRAY_GRID_ZERO;//编号从1开始,且只能为数字
	 return code_t_x;
}
/*****************************************
 * 函数功能：检查If指令的语法
 * 			if Nx <,= ,>,<=,>= Nx/x
 * 			then *loop / #sub
 * 			else *loop / #sub
 */
unsigned char CheckIf(PrgCode* codeStrings)
{
	 unsigned char i=0,n_num,pin_value;
	 for(i=0;i<2;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_if[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_IF_OBJECT;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] == 'n')
	 {
		 i++;
		 if((codeStrings->codeStrings[i]<'0') || (codeStrings->codeStrings[i]>'9'))
			 return ERR_N_OVERFLOW;
		 n_num = StringToUint(&codeStrings->codeStrings[i]);
		 if(n_num>=MAX_N_NUM) return ERR_N_OVERFLOW;
	 }
	 else if(codeStrings->codeStrings[i] == 'p')
	 {
		 i++;
		 if(codeStrings->codeStrings[i++] != 'i') return ERR_PIN;
		 if(codeStrings->codeStrings[i++] != 'n') return ERR_PIN;
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')return ERR_PIN;
		 pin_value = StringToUint(&codeStrings->codeStrings[i]);//提取输入IO
		 if(pin_value >= MAX_PIN_NUM)return ERR_PIN_OVERFLOW;
	 }
	 else if(codeStrings->codeStrings[i] == 'r')
	 {
		 i++;
		 if(codeStrings->codeStrings[i++] != 's') return ERR_RSRD_1;
		 if(codeStrings->codeStrings[i++] != 'r') return ERR_RSRD_1;
		 if(codeStrings->codeStrings[i++] != 'd') return ERR_RSRD_1;
		 if((codeStrings->codeStrings[i]!=' ') && (codeStrings->codeStrings[i]!='>') &&
			(codeStrings->codeStrings[i]!='<') && (codeStrings->codeStrings[i]!='='))
			 return ERR_RSRD_1;
		 if(ROBOT_PARAMETER.HARDWARE_REV == 7)
		{
			return ERR_NO_RS232;	//07版没有RS232
		}
	 }
	 else if(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
	 {

	 }
	 else
	 {
		 return ERR_IF_OBJECT;
	 }
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '<' && codeStrings->codeStrings[i] != '=' &&
		codeStrings->codeStrings[i] != '>')
	 {
		 return ERR_IF_COMPARE;
	 }
	 i++;
	 if(codeStrings->codeStrings[i] == '=' || codeStrings->codeStrings[i] == ' ')
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] == 'n')
	 {
		 i++;
		 if((codeStrings->codeStrings[i]<'0') || (codeStrings->codeStrings[i]>'9'))
			 return ERR_N_OVERFLOW;
		 n_num = StringToUint(&codeStrings->codeStrings[i]);
		 if(n_num>=MAX_N_NUM) return ERR_N_OVERFLOW;
	 }
	 else if(codeStrings->codeStrings[i] == 'p')
	 {
		 i++;
		 if(codeStrings->codeStrings[i++] != 'i') return ERR_PIN;
		 if(codeStrings->codeStrings[i++] != 'n') return ERR_PIN;
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')return ERR_PIN;
		 pin_value = StringToUint(&codeStrings->codeStrings[i]);//提取输入IO
		 if(pin_value >= MAX_PIN_NUM)return ERR_PIN_OVERFLOW;
	 }
	 else if(codeStrings->codeStrings[i] == 'r')
	 {
		 i++;
		 if(codeStrings->codeStrings[i++] != 's') return ERR_RSRD_1;
		 if(codeStrings->codeStrings[i++] != 'r') return ERR_RSRD_1;
		 if(codeStrings->codeStrings[i++] != 'd') return ERR_RSRD_1;
		 if((codeStrings->codeStrings[i]!=' ') && (codeStrings->codeStrings[i]!=0))
			 return ERR_RSRD_1;
		 if(ROBOT_PARAMETER.HARDWARE_REV == 7)
		{
			return ERR_NO_RS232;	//07版没有RS232
		}
	 }
	 else if(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
	 {

	 }
	 else
	 {
		 return ERR_IF_OBJECT;
	 }
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0')
	 {
		   return ERR_IF_OBJECT;
	 }
	 return code_if;
}

/*****************************************
 * 函数功能：检查then指令的语法
 * 			if Nx <,= ,>,<=,>= Nx/x
 * 			then *loop / #sub
 * 			else *loop / #sub
 */
unsigned char CheckThen(PrgCode* codeStrings)
{
	 unsigned char i=0,j;
	 unsigned char str_tmp[COLUMNS_IN_LINE]={0};
	 unsigned char code_strings[COLUMNS_IN_LINE]={0};
	 unsigned int current_lines=0;//当前代码总行数
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_then[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_STAR_POUND;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != '*' && codeStrings->codeStrings[i] != '#')
		 return ERR_STAR_POUND;//缺少*/#开头的字符串

	 str_tmp[0] = codeStrings->codeStrings[i];
	 j=1;
	 i++;
	 if(codeStrings->codeStrings[i] == '\0' || codeStrings->codeStrings[i] == ' ')
		 return ERR_STRING_NAME;//至少要有一个非空格的字符

	 while(codeStrings->codeStrings[i] && codeStrings->codeStrings[i]!=' ')
	 {
		 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
				 && !(codeStrings->codeStrings[i]>='a' && codeStrings->codeStrings[i]<='z')
				 && (codeStrings->codeStrings[i] != '_'))
		 {
			 return ERR_STRING_NAME;//*/#后面至少有一个字符，且在0~9或a~z范围内，中间不能有空格
		 }
		 str_tmp[j++] = codeStrings->codeStrings[i];
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_STRING_NAME;

	 current_lines = GetLinesCurrentProgram();//当前代码总行数
	 for(i=0;i<current_lines;i++)
	 {
		 GetRowCode(i,code_strings);
		 if(code_strings[0] == str_tmp[0])
		 {
			 if(!StringCmp(COLUMNS_IN_LINE,code_strings,str_tmp)) return code_then;
		 }
	 }
	 if(str_tmp[0] == '*')
	 {
		 return ERR_NO_THIS_LOOP;//无此跳转标志
	 }
	 else
	 {
		 return ERR_NO_THIS_SUB;//无此子程序
	 }
}
/*****************************************
 * 函数功能：检查else指令的语法
 * 			if   Nx <,= ,>,<=,>= Nx/x
 * 			then *loop / #sub
 * 			else *loop / #sub
 */
unsigned char CheckElse(PrgCode* codeStrings)
{
	 unsigned char i=0,j;
	 unsigned char str_tmp[COLUMNS_IN_LINE]={0};
	 unsigned char code_strings[COLUMNS_IN_LINE]={0};
	 unsigned int current_lines=0;//当前代码总行数
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_else[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_STAR_POUND;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != '*' && codeStrings->codeStrings[i] != '#')
		 return ERR_STAR_POUND;//缺少*/#开头的字符串

	 str_tmp[0] = codeStrings->codeStrings[i];
	 j=1;
	 i++;
	 if(codeStrings->codeStrings[i] == '\0' || codeStrings->codeStrings[i] == ' ')
		 return ERR_STRING_NAME;//至少要有一个非空格的字符

	 while(codeStrings->codeStrings[i] && codeStrings->codeStrings[i]!=' ')
	 {
		 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
				 && !(codeStrings->codeStrings[i]>='a' && codeStrings->codeStrings[i]<='z')
				 && (codeStrings->codeStrings[i] != '_'))
		 {
			 return ERR_STRING_NAME;//*/#后面至少有一个字符，且在0~9或a~z范围内，中间不能有空格
		 }
		 str_tmp[j++] = codeStrings->codeStrings[i];
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_STRING_NAME;

	 current_lines = GetLinesCurrentProgram();//当前代码总行数
	 for(i=0;i<current_lines;i++)
	 {
		 GetRowCode(i,code_strings);
		 if(code_strings[0] == str_tmp[0])
		 {
			 if(!StringCmp(COLUMNS_IN_LINE,code_strings,str_tmp)) return code_else;
		 }
	 }
	 if(str_tmp[0] == '*')
	 {
		 return ERR_NO_THIS_LOOP;//无此跳转标志
	 }
	 else
	 {
		 return ERR_NO_THIS_SUB;//无此子程序
	 }
}
/******************************************
 * 函数功能：检查n 变量 指令 n0~n49  nx++;nx--;nx = nx;nx = xx;
 * 						加减乘除求余
 * 						nx = nx + x/nx;	 nx = nx - x/nx
 * 						nx = nx * x/nx;	 nx = nx / x/nx;
 * 						nx = nx mod x/nx;
 */
unsigned char CheckNx(unsigned char* strings)
{
	 unsigned char i=0,n_num,sign;
	 long n_value;
	 for(i=0;i<1;i++)
	 {
		  if(*(strings+i) != s_code_n[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 if(*(strings+i)<'0' || *(strings+i)>'9') return ERR_CMD;
	 n_num = StringToUint(strings+i);
	 if(n_num >= MAX_N_NUM) return ERR_N_OVERFLOW;//n0~n9
	 while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
	 {
		 i++;
	 }
	 while(*(strings+i) == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(*(strings+i) == '+')//++
	 {
		 i++;
		 if(*(strings+i) != '+') return ERR_CMD;//nx++;
		 i++;
		 while(*(strings+i) == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(*(strings+i) == '\0') return code_n_x;
		 else return ERR_CMD;//nx++;
	 }
	 else if(*(strings+i) == '-')//--
	 {
		 i++;
		 if(*(strings+i) != '-') return ERR_CMD;//nx--;
		 i++;
		 while(*(strings+i) == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(*(strings+i) == '\0') return code_n_x;
		 else return ERR_CMD;//nx--;
	 }
	 else if(*(strings+i) == '=')// =
	 {
		 i++;
		 while(*(strings+i) == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(*(strings+i)>='0' && *(strings+i)<='9')//nx = x;
		 {
			 while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
			 {
				 i++;
			 }
			 while(*(strings+i) == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(*(strings+i) == '\0') return code_n_x;
			 else return ERR_CMD;//nx--;
		 }
		 else if(*(strings+i) == 'n')//nx = ny; nx = nx +-*/% x/nx;
		 {
			 i++;
			 if(*(strings+i)<'0' || *(strings+i)>'9') return ERR_CMD;
			 n_num = StringToUint(strings+i);
			 if(n_num >= MAX_N_NUM) return ERR_N_OVERFLOW;//n0~n9
			 while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
			 {
				 i++;
			 }
			 while(*(strings+i) == ' ')//跳过空格
			 {
				 i++;
			 }

			 if(*(strings+i) == '\0') return code_n_x;
			 else if(*(strings+i) == '+' || *(strings+i) == '-' ||
					 *(strings+i) == '*' || *(strings+i) == '/' || *(strings+i) == 'm')
			 {
				 if(*(strings+i) == 'm')
				 {
					 i++;
					 if(*(strings+i) == 'o')
					 {
						 i++;
						 if(*(strings+i) != 'd')
						 {
							 return ERR_CMD;
						 }
					 }
					 else return ERR_CMD;
				 }
				 sign = strings[i];	//记录符号，后面用到
				 i++;
				 while(*(strings+i) == ' ')//跳过空格
				 {
					 i++;
				 }
				 if(*(strings+i)>='0' && *(strings+i)<='9' )
				 {
					 if(sign == '/')
					 {
						 n_value = atol((char*)(strings+i));
						 if(n_value == 0) return ERR_CANT_ZERO;//除数不能为零
					 }
					 while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
					 {
						 i++;
					 }
					 while(*(strings+i) == ' ')//跳过空格
					 {
						 i++;
					 }
					 if(*(strings+i) == '\0') return code_n_x;
					 else return ERR_CMD;
				 }
				 else if(*(strings+i) == 'n')
				 {
					 i++;
					 n_num = StringToUint(strings+i);
					 if(n_num >= MAX_N_NUM) return ERR_N_OVERFLOW;//n0~n9
					 if(sign == '/')
					 {
						 if(get_n_value(n_num) == 0) return ERR_CANT_ZERO;//除数不能为零
					 }
					 while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
					 {
						 i++;
					 }
					 while(*(strings+i) == ' ')//跳过空格
					 {
						 i++;
					 }
					 if(*(strings+i) == '\0') return code_n_x;
					 else return ERR_CMD;
				 }
				 else
				 {
					 return ERR_CMD;
				 }

			 }
			 else return ERR_CMD;
		 }
		 else
		 {
			 return ERR_CMD;
		 }
	 }
	 else
	 {
		 return ERR_CMD;
	 }
}
/******************************************
 * 函数功能：检查f 变量 指令 f0~n49
 * 			直接赋值：fx = fy;fx = z;
 * 			获得某轴角度：f0 = j1;
 * 			加减乘除：
 * 			fx = fx + x/fx;	 fx = fx - x/fx;
 * 			fx = fx * x/fx;	 fx = fx / x/fx;
 */
unsigned char CheckFx(unsigned char* strings)
{
	 unsigned char i=0,n_num,sign;
	 double f_value;
	 for(i=0;i<1;i++)
	 {
		  if(*(strings+i) != s_code_f[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 if(*(strings+i)<'0' || *(strings+i)>'9') return ERR_CMD;
	 n_num = StringToUint(strings+i);
	 if(n_num >= MAX_F_NUM) return ERR_F_OVERFLOW;//f0~f49
	 while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
	 {
		 i++;
	 }
	 while(*(strings+i) == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(*(strings+i) == '=')// =
	 {
		 i++;	//跳过 =
		 while(*(strings+i) == ' ')//跳过空格
		 {
			 i++;
		 }

		 if(*(strings+i)=='j')
		 {
			 i++;
			 if(strings[i]<'0' || strings[i]>'9') return ERR_CMD;
			 n_num = StringToUint(strings+i);
			 if(ROBOT_PARAMETER.AXIS_NUM == 6)
			 {
				 if(n_num > 6) return ERR_J1_J6;//J1~6
			 }
			 else if(ROBOT_PARAMETER.AXIS_NUM == 4)
			 {
				 if(n_num > 4) return ERR_J1_J4;//J1~4
			 }
			 else if(ROBOT_PARAMETER.AXIS_NUM == 5)
			 {
				 if(n_num > 5) return ERR_J1_J5;//J1~5
			 }
			 else
				 return ERR_CMD;
			 while(strings[i] >= '0' && strings[i] <= '9')//跳过数字
			 {
				 i++;
			 }
			 while(strings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(*(strings+i) == '\0') return code_f_x;
			 else return ERR_CMD;
		 }
		 else if((*(strings+i)>='0' && *(strings+i)<='9') ||
				  *(strings+i)=='+' || *(strings+i)=='-')//fx = x;
		 {
			 if(*(strings+i)=='+' || *(strings+i)=='-')
			 {
				 i++;	//如果有符号，先跳过符号，然后再判断后面接的是不是数字
				 //如果后面不是接数字就报错
				 if(strings[i]<'0' || strings[i]>'9') return ERR_CMD;
			 }
			 while((*(strings+i)>='0' && *(strings+i)<='9'))//跳过数字
			 {
				 i++;
			 }
			 while(strings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(strings[i] == '.')
			 {
				 i++;
				 if(strings[i]<'0' || strings[i]>'9') return ERR_CMD;//fx = 12.?
				 while(strings[i] >= '0' && strings[i] <= '9')//跳过数字
				 {
					 i++;
				 }
				 while(strings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 if(*(strings+i) == '\0') return code_f_x;
				 //else return ERR_CMD;
			 }
			 else if(strings[i] == '\0')
			 {
				 return code_f_x;//f.x = 12.34
			 }
			 else return ERR_CMD;
		 }
		 else if(*(strings+i) == 'f')//fx = fy; fx = fx +-*/ x/fx;
		 {
			 i++;	//跳过f
			 if(*(strings+i)<'0' || *(strings+i)>'9') return ERR_CMD;
			 n_num = StringToUint(strings+i);
			 if(n_num >= MAX_F_NUM) return ERR_F_OVERFLOW;//f0~f49
			 while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
			 {
				 i++;
			 }
			 while(*(strings+i) == ' ')//跳过空格
			 {
				 i++;
			 }

			 if(*(strings+i) == '\0') return code_f_x;
			 else if(*(strings+i) == '+' || *(strings+i) == '-' ||
					 *(strings+i) == '*' || *(strings+i) == '/')
			 {
				 sign = strings[i];	//记录符号，后面用
				 i++;//跳过+-*/
				 while(*(strings+i) == ' ')//跳过空格
				 {
					 i++;
				 }
				 if((*(strings+i)>='0' && *(strings+i)<='9') ||
						  *(strings+i)=='+' || *(strings+i)=='-')//fx = x;
				 {
					 if(*(strings+i)=='+' || *(strings+i)=='-')
					 {
						 i++;	//如果有符号，先跳过符号，然后再判断后面接的是不是数字
					 }
					 //如果后面不是接数字就报错
					 if(strings[i]<'0' || strings[i]>'9') return ERR_CMD;
					 if(sign == '/')
					 {
						 f_value = atof((char*)(strings+i));
						 if(fabs(f_value) < 1.0e-8) return ERR_CANT_ZERO;
					 }
					 while((*(strings+i)>='0' && *(strings+i)<='9'))//跳过数字
					 {
						 i++;
					 }
					 while(*(strings+i) == ' ')//跳过空格
					 {
						 i++;
					 }
					 if(strings[i] == '.')
					 {
						 i++;
						 if(strings[i]<'0' || strings[i]>'9') return ERR_CMD;//fx = 12.?
						 while(strings[i] >= '0' && strings[i] <= '9')//跳过数字
						 {
							 i++;
						 }
						 while(strings[i] == ' ')//跳过空格
						 {
							 i++;
						 }
						 if(*(strings+i) == '\0') return code_f_x;
						 else return ERR_CMD;
					 }
					 else if(strings[i] == '\0')
					 {
						 return code_f_x;//f.x = 12.34
					 }
					 else return ERR_CMD;
				 }
				 else if(*(strings+i) == 'f')
				 {
					 i++;
					 n_num = StringToUint(strings+i);
					 if(n_num >= MAX_F_NUM) return ERR_F_OVERFLOW;//n0~n9
					 if(sign == '/')
					 {
						 if(fabs(get_f_value(n_num)) < 1.0e-8) return ERR_CANT_ZERO;
					 }
					 while(*(strings+i) >= '0' && *(strings+i) <= '9')//跳过数字
					 {
						 i++;
					 }
					 while(*(strings+i) == ' ')//跳过空格
					 {
						 i++;
					 }
					 if(*(strings+i) == '\0') return code_f_x;
					 else return ERR_CMD;
				 }
				 else
				 {
					 return ERR_CMD;
				 }

			 }
			 else return ERR_CMD;
		 }
		 else
		 {
			 return ERR_CMD;
		 }
	 }
	 else
	 {
		 return ERR_CMD;
	 }
	 return  ERR_CMD;
}

unsigned char CheckBaud(PrgCode* codeStrings)
{
	 unsigned char i;
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_baud[i])
		  {
			   return  ERR_CMD;//命令不正确
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_CMD;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] < '0' || codeStrings->codeStrings[i] > '9')
		 return ERR_BAUD;//波特率设置不正确，只能为数字
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0') return code_baud;
	 else return ERR_BAUD;
}

unsigned char CheckRstd(PrgCode* codeStrings)
{
	 unsigned char i;
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_rstd[i])
		  {
			   return  ERR_CMD;//命令不正确
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_CMD;
	 return code_rstd;
}

unsigned char CheckDisp(PrgCode* codeStrings)
{
	 unsigned char i;
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_disp[i])
		  {
			   return  ERR_CMD;//命令不正确
		  }
	 }
	 if(codeStrings->codeStrings[i] < '1' || codeStrings->codeStrings[i] > '5') return ERR_DISP;
	 if(codeStrings->codeStrings[++i] != ' ') return ERR_DISP;
	 return code_disp;
}

/**************************************
 * 检测 rs232 指令
 */
unsigned char CheckRS232(PrgCode* codeStrings)
{
	 unsigned char i;
	 for(i=0;i<5;i++)//
	 {
		  if(codeStrings->codeStrings[i] != s_code_rs232[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(ROBOT_PARAMETER.HARDWARE_REV == 7) return ERR_NO_RS232;
	 if(codeStrings->codeStrings[i] == '\0')return code_rs232;
	 else return ERR_CMD;
}

unsigned char CheckFinish(PrgCode* codeStrings)
{
	 unsigned char i;
	 for(i=0;i<6;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_finish[i])
		  {
			   return  ERR_CMD;//命令不正确
		  }
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i]=='\0'||codeStrings->codeStrings[i]=='\r'||codeStrings->codeStrings[i]=='\n')
		 return code_finish;
	 else return ERR_CMD;
}

/**************************************
 * 检测 acc 指令 acc 1~100
 */
unsigned char CheckToolLen(PrgCode* codeStrings)
{
	 unsigned char i;
	 for(i=0;i<7;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_toollen[i])
		  {
			   return  ERR_CMD;//命令不正确
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_CMD;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if((codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') &&
			 codeStrings->codeStrings[i]!='+' && codeStrings->codeStrings[i]!='-')
		 return ERR_CMD;

	 while((codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9') ||
			 codeStrings->codeStrings[i] == '+' || codeStrings->codeStrings[i] == '-')//跳过数字
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] == '.')
	 {
		 i++;
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;//mov p1.x + 12.?
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] == '\0')
		 {
			 return code_toollen;//
		 }
	 }
	 else if(codeStrings->codeStrings[i] == '\0')
	 {
		 return code_toollen;//
	 }

	 return ERR_CMD;
}

/**************************************
 * 检测 tool 指令 tool(Px,Py,Pz,Rx,Ry,Rz)/tool.px = n
 */
unsigned char CheckTool(PrgCode* codeStrings)
{
	 unsigned char i,j;
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_tool[i])
		  {
			   return  ERR_CMD;//命令不正确
		  }
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 //tool(Px,Py,Pz,Rx,Ry,Rz) 形式
	 if(codeStrings->codeStrings[i]=='(') //|| codeStrings->codeStrings[i]>'9') return ERR_CMD;
	 {
		 i++;	//跳过 '('
		 for(j=0;j<6;j++)
		 {
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if((codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') &&
				 codeStrings->codeStrings[i]!='+' && codeStrings->codeStrings[i]!='-')
				 return ERR_CMD;
			 while((codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')||
					codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')//跳过数字
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i++] == '.')	//如果遇到小数点，跳过(逗号,括号也跳过)
			 {
				 //小数点后没有数字，出错
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;
				 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
				 {
					 i++;
				 }
				 while(codeStrings->codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')	//遇到非数字结束
				 {
					 i++;
				 }
			 }
		 }
		 i--;
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] != ')')
		 {
			 return  ERR_CMD;
		 }
	 }
	 else if(codeStrings->codeStrings[i]=='.')	//tool.px/py/px/rx/ry/rz = n
	 {
		 i++;	//跳过 '.'
		 if(codeStrings->codeStrings[i]!='p' && codeStrings->codeStrings[i]!='r')
		 {
			 return ERR_CMD;
		 }
		 i++;
		 if(codeStrings->codeStrings[i]!='x' && codeStrings->codeStrings[i]!='y' &&
				 codeStrings->codeStrings[i]!='z')
		 {
			 return ERR_CMD;
		 }
		 i++;

		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] != '=') return ERR_CMD;
		 i++;
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if((codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') &&
			 codeStrings->codeStrings[i]!='+' && codeStrings->codeStrings[i]!='-')
			 return ERR_CMD;
		 while((codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')||
				codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] == '.')
		 {
			 i++;
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i] != '\0')
			 {
				 return ERR_CMD;
			 }
		 }
		 else if(codeStrings->codeStrings[i] != '\0')
		 {
			 return ERR_CMD;
		 }

	 }
	 else
	 {
		 return  ERR_CMD;//命令不正确
	 }
	 return code_tool;
}

/**************************************
 * 检测part 指令 part(Px,Py,Pz,Rx,Ry,Rz)/part.px = n
 */
unsigned char CheckPart(PrgCode* codeStrings)
{
	 unsigned char i,j;
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_part[i])
		  {
			   return  ERR_CMD;//命令不正确
		  }
	 }
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 //part(Px,Py,Pz,Rx,Ry,Rz) 形式
	 if(codeStrings->codeStrings[i]=='(')
	 {
		 i++;	//跳过 '('
		 for(j=0;j<6;j++)
		 {
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if((codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') &&
				 codeStrings->codeStrings[i]!='+' && codeStrings->codeStrings[i]!='-')
				 return ERR_CMD;
			 while((codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')||
					codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')//跳过数字
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i++] == '.')	//如果遇到小数点，跳过(逗号,括号也跳过)
			 {
				 //小数点后没有数字，出错
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;
				 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
				 {
					 i++;
				 }
				 while(codeStrings->codeStrings[i] == ' ')//跳过空格
				 {
					 i++;
				 }
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')	//遇到非数字结束
				 {
					 i++;
				 }
			 }
		 }
		 i--;
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] != ')')
		 {
			 return  ERR_CMD;
		 }
	 }
	 else if(codeStrings->codeStrings[i]=='.')	//part.px = n
	 {
		 i++;	//跳过 '.'
		 if(codeStrings->codeStrings[i]!='p' && codeStrings->codeStrings[i]!='r')
		 {
			 return ERR_CMD;
		 }
		 i++;
		 if(codeStrings->codeStrings[i]!='x' && codeStrings->codeStrings[i]!='y' &&
				 codeStrings->codeStrings[i]!='z')
		 {
			 return ERR_CMD;
		 }
		 i++;
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] != '=') return ERR_CMD;
		 i++;
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }
		 if((codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') &&
			 codeStrings->codeStrings[i]!='+' && codeStrings->codeStrings[i]!='-')
			 return ERR_CMD;
		 while((codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')||
				codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')//跳过数字
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//跳过空格
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] == '.')
		 {
			 i++;
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//跳过数字
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//跳过空格
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i] != '\0')
			 {
				 return ERR_CMD;
			 }
		 }
		 else if(codeStrings->codeStrings[i] != '\0')
		 {
			 return ERR_CMD;
		 }
	 }
	 else
	 {
		 return  ERR_CMD;//命令不正确
	 }
	 return code_part;
}
/**************************************
 * 检测network 指令 network n
 * 修正于//2018/06/01
 */
unsigned char CheckNetwork(PrgCode* codeStrings)
{
unsigned char i=0,n_num;
	 for(i=0;i<7;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_network[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_IF_OBJECT;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] == 'n')
	 {
		 i++;
		 if((codeStrings->codeStrings[i]<'0') || (codeStrings->codeStrings[i]>'9'))
			 return ERR_N_OVERFLOW;
		 n_num = StringToUint(&codeStrings->codeStrings[i]);
		 if(n_num>=MAX_NUM_COMPONENT) return ERR_N_OVERFLOW;
	 }
		 return code_network;
}
/**************************************
 * 检测teamwork 指令 teamwork n
 * 修正于//2018/06/01
 */
unsigned char CheckTeamwork(PrgCode* codeStrings)
{
unsigned char i=0,n_num;
	 for(i=0;i<8;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_teamwork[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_IF_OBJECT;
	 while(codeStrings->codeStrings[i] == ' ')//跳过空格
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] == 'n')
	 {
		 i++;
		 if((codeStrings->codeStrings[i]<'0') || (codeStrings->codeStrings[i]>'9'))
			 return ERR_N_OVERFLOW;
		 n_num = StringToUint(&codeStrings->codeStrings[i]);
		 if(n_num>=MAX_STATUS_VALUE) return ERR_N_OVERFLOW;
	 }
		 return code_teamwork;
}
//===========================================================================
// No more.
//===========================================================================
