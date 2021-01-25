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
static const unsigned char s_code_loop[]={"*"};//ѭ����ͷ��־
static const unsigned char s_code_sub[]={"#"};//�ӳ���ͷ��־
static const unsigned char s_code_tray[]={"t"};//����
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
static const unsigned char s_code_network[]={"network"};//������//2018/06/01
static const unsigned char s_code_teamwork[]={"teamwork"};//������//2018/06/01
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
static unsigned char CheckNetwork(PrgCode* codeStrings);//������//2018/06/01
static unsigned char CheckTeamwork(PrgCode* codeStrings);//������//2018/06/01
//static unsigned char CheckE(PrgCode* codeStrings);
//static unsigned char CheckT(PrgCode* codeStrings);
extern struct TrayStru s_tray[MAX_TRAY_NUM];
extern struct EX_Position_stru EX_POSITION;
/***************************************
 * �﷨���
 * ������codeStrings  �ַ���
 * 		 row		  ������
 */
unsigned char CheckGrammar(PrgCode* codeStrings)
{
	 unsigned char i,j,code=0,code_flg,err;
	 long n_value;
	 double f_value;
	 int n_num;
	 char str1[20];
	 char str2[19];
	 for(i=0;i<COLUMNS_IN_LINE-3;i++)//-3���п��ޣ���Ϊ����������3���ַ���
	 {
		  if(codeStrings->codeStrings[0] == ' ')
		  {
			   for(j=0;j<COLUMNS_IN_LINE - 1;j++)
			   {
					codeStrings->codeStrings[j] = codeStrings->codeStrings[j+1];//����һ����λ�����ǵ��ո�
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
	   	   	   	   	   	    if(code == code_tray) ExecuteTray(*codeStrings);//build��ʱ��������
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
									n_num = StringToUint((unsigned char*)codeStrings);//��ȡn����
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
									n_num = StringToUint((unsigned char*)codeStrings);//��ȡn����
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
	   	   	   	   	   	    	 err = ExecutePx(*codeStrings);//�����ʱ��ִ��
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
	   case code_network:   code= CheckNetwork(codeStrings);break;//������//2018/06/01
	   case code_teamwork:  code= CheckTeamwork(codeStrings);break;//������//2018/06/01

	   default :			code = ERR_CMD;
	 }
	 return code;
}

/*************************************
 * �������ܣ��������
 * ���������codeStrings  �ַ���
 * ����ֵ��	 �������
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
			 code_flg = code_teamwork; //������//2018/06/01
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
		 if(codeStrings->codeStrings[1] == 'e') 	code_flg = code_network;//������//2018/06/01
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
 * �������ܣ��������
 * ���������num ���մ������
 * 			 err ָ�������ŵ�ָ��
 * 			 line_of_err ָ�������е�ָ��
 * ����ֵ��  �˳���������
 * ˵����	 Build֮ǰ����ȴ�һ���ļ�
 * 			 ������Ϣһ�α�������ܹ��ռ�num��������Ϣ����������ǰ������
 * 			 ���û�д�����Ϣ��err����ȫ����0����ȷ��Build��֮��errָ��
 * 			 ��ֵȫ��Ϊ0�������г���
 * 			 ����ʾ����������sizeof��
 */
extern int RX_POSITION_STA;
extern int USART_RX_STA_B;
unsigned char BuildProgram(unsigned char num,unsigned char* err,unsigned int* line_of_err)
{
	 unsigned int i,j=0;
	 unsigned char max_lines,code[COLUMNS_IN_LINE], err_tmp;

	 //
	 //xxx �ӳ����ܵ����Լ�
	 //xxx һ��If���һ��Then��else���п��ޣ���then��else��ֻ��һ��
	 ClearReturnStack();//���return��ջ

	 InitSci();	//��ʼ������
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
			   j++; //�������+1
		  }
	 }
	 return j;
}

/**************************************
 * ��� acc ָ�� acc 1~100
 */
unsigned char CheckAcc(PrgCode* codeStrings)
{
	 unsigned char i;
	 unsigned long value=0;
	 for(i=0;i<3;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_acc[i])
		  {
			   return  ERR_CMD;//�����ȷ
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_ACC;//�Ӽ��ٷ�Χ1~100,100 is ���Ӽ���
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_ACC;

	 value = StringToUint(&codeStrings->codeStrings[i]);
	 if(value == 0 || value >100)return ERR_ACC;

	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0') return code_acc;
	 else return ERR_ACC;
}
/**************************************
 * ��� goto ָ��
 */
unsigned char CheckGoto(PrgCode* codeStrings)
{
	 unsigned int i,j;
	 unsigned char str_tmp[COLUMNS_IN_LINE]={0};
	 unsigned char code_strings[COLUMNS_IN_LINE]={0};
	 unsigned int current_lines=0;//��ǰ����
	 for(i=0;i<4;i++)//
	 {
		  if(codeStrings->codeStrings[i]!=s_code_goto[i])
		  {
			   return  ERR_CMD;//�������
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_STAR;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != '*') return ERR_STAR;//ȱ��*��ͷ���ַ���

	 str_tmp[0] = '*';
	 j=1;
	 i++;
	 if(codeStrings->codeStrings[i] == '\0' || codeStrings->codeStrings[i] == ' ')
		 return ERR_STR_NAME;//����Ҫ��һ���ǿո���ַ�
	 while(codeStrings->codeStrings[i] && codeStrings->codeStrings[i]!=' ')
	 {
		 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
				 && !(codeStrings->codeStrings[i]>='a' && codeStrings->codeStrings[i]<='z')
				 && (codeStrings->codeStrings[i] != '_'))
		 {
			 return ERR_STR_NAME;//*����������һ���ַ�������0~9��a~z��Χ�ڣ��м䲻���пո�
		 }
		 str_tmp[j++] = codeStrings->codeStrings[i];
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_STR_NAME;

	 current_lines = GetLinesCurrentProgram();//��ǰ����������
	 for(i=0;i<current_lines;i++)
	 {
		 GetRowCode(i,code_strings);
		 if(code_strings[0]=='*')
		 {
			 if(!StringCmp(COLUMNS_IN_LINE,code_strings,str_tmp)) return code_goto;
		 }
	 }
	 return ERR_NO_THIS_LOOP;//�޴���ת��־
}
/**************************************
 * ��� gosub ָ��
 */
unsigned char CheckGosub(PrgCode* codeStrings)
{
	 unsigned int i,j;
	 unsigned char str_tmp[COLUMNS_IN_LINE]={0};
	 unsigned char code_strings[COLUMNS_IN_LINE]={0};
	 unsigned int current_lines=0;//��ǰ����
	 for(i=0;i<5;i++)//
	 {
		  if(codeStrings->codeStrings[i]!=s_code_gosub[i])
		  {
			   return  ERR_CMD;//�������
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_POUND;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != '#') return ERR_POUND;//ȱ��#��ͷ���ַ���

	 str_tmp[0] = '#';
	 j=1;
	 i++;
	 if(codeStrings->codeStrings[i] == '\0' || codeStrings->codeStrings[i] == ' ')
		 return ERR_SUB_NAME;//����Ҫ��һ���ǿո���ַ�
	 while(codeStrings->codeStrings[i] && codeStrings->codeStrings[i]!=' ')
	 {
		 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
				 && !(codeStrings->codeStrings[i]>='a' && codeStrings->codeStrings[i]<='z')
				 && (codeStrings->codeStrings[i] != '_'))
		 {
			 return ERR_SUB_NAME;//#����������һ���ַ�������0~9��a~z��Χ�ڣ��м䲻���пո�
		 }
		 str_tmp[j++] = codeStrings->codeStrings[i];
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_SUB_NAME;

	 current_lines = GetLinesCurrentProgram();//��ǰ����������
	 for(i=0;i<current_lines;i++)
	 {
		 GetRowCode(i,code_strings);
		 if(code_strings[0]=='#')
		 {
			 if(!StringCmp(COLUMNS_IN_LINE,code_strings,str_tmp)) return code_gosub;
		 }
	 }
	 return ERR_NO_THIS_SUB;//�޴��ӳ���
}
/**************************************
 * ��� * ��ͷ��ָ��(��ת��־)
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
			   return  ERR_CMD;//�������
		  }
	 }
	 if(codeStrings->codeStrings[i]=='\0' || codeStrings->codeStrings[i]==' ')return ERR_STR_NAME;

	 while(codeStrings->codeStrings[i] && codeStrings->codeStrings[i]!=' ')
	 {
		 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
				 && !(codeStrings->codeStrings[i]>='a' && codeStrings->codeStrings[i]<='z')
				 && (codeStrings->codeStrings[i] != '_'))
		 {
			 return ERR_STR_NAME;//*����������һ���ַ�������0~9��a~z��Χ�ڣ��м䲻���пո�
		 }
		 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_STR_NAME;

	 max_lines = GetLinesCurrentProgram();//��ǰ����������
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
				  return ERR_REPEATED_LOOP;//�ظ�����ת��־
			  }
		 }
	 }
	 return code_loop;
}
/**************************************
 * ��� # ��ͷ��ָ��(�ӳ����־)
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
			   return  ERR_CMD;//�������
		  }
	 }
	 if(codeStrings->codeStrings[i]=='\0' || codeStrings->codeStrings[i]==' ')return ERR_SUB_NAME;

	 while(codeStrings->codeStrings[i] && codeStrings->codeStrings[i]!=' ')
	 {
		 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
				 && !(codeStrings->codeStrings[i]>='a' && codeStrings->codeStrings[i]<='z')
				 && (codeStrings->codeStrings[i] != '_'))
		 {
			 return ERR_SUB_NAME;//#����������һ���ַ�������0~9��a~z��Χ�ڣ��м䲻���пո�
		 }
		 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_SUB_NAME;

	 //����Ƿ����ظ����ӳ���
	 max_lines = GetLinesCurrentProgram();//��ǰ����������
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
					  sub_line = j;//��¼�ӳ����������һ��(����¼��һ�γ��ֵ�)������Ҫ��
					  flag = 1;
				  }
			  }
			  if(cmp_flag >=2)
			  {
				  return ERR_REPEATED_SUB;//�ظ����ӳ���
			  }
		 }
	 }

	 //�����ӳ�����û��return������return
	 for(j=sub_line+1;j<max_lines;j++)
	 {
		 if(program_code[j].codeStrings[0] == '#')
		 {
			 if(return_num ==0)
			 {
				 return ERR_LACK_OF_RETURN;//ȱ��return
			 }
			 else if(return_num ==1)
			 {
				 return code_sub;
			 }
			 else
			 {
				 return ERR_REPEATED_RETURN;//���ظ���return
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
		 return ERR_LACK_OF_RETURN;//ȱ��return
	 }
	 else if(return_num ==1)
	 {
		 return code_sub;
	 }
	 else
	 {
		 return ERR_REPEATED_RETURN;//���ظ���return
	 }
}
/**************************************
 * ��� return ָ��
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
	 //  xxx  ���:��û���ظ���return
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0')return code_return;
	 else return ERR_CMD;
}
/**************************************
 * ��� d ��ͷ��ָ�� dly 0.1
 */
unsigned char CheckDly(PrgCode* codeStrings)
{
	 unsigned char i=0;
	 float f_value;
	 for(i=0;i<3;i++)
	 {
		  if(codeStrings->codeStrings[i]!=s_code_dly[i])
		  {
			  return  ERR_CMD;//�������
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_DLY;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_DLY;//ȱ�ٲ���

	 f_value = StringToFloat(&codeStrings->codeStrings[i]);

	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
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

	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_DLY;

	 if(f_value > 999.9) return ERR_DLY;//������Χ 0.0~999.9
	 return code_dly;
}
/**************************************
 * ��� end ָ��
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
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0')return code_end;
	 else return ERR_CMD;
}
/**************************************
 * ��� hlt ָ��
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
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0')return code_stop;
	 else return ERR_CMD;
}
/**************************************
 * ��� mvs ָ�� ֱ�߲岹 mvs px.x/y/z +/- m/n
 */
unsigned char CheckMvs(PrgCode* codeStrings)
{
	 unsigned char i,err,p_num,n_num;
	 int speed;
	 for(i=0;i<3;i++)//
	 {
		  if(codeStrings->codeStrings[i] != s_code_mvs[i])
		  {
			   return  ERR_CMD;//�������
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_NO_P;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
		 return ERR_NO_P;//��ʽ���� λ�ñ�����Ӧ����p��ͷ
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
			 if(p_num>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;//��ʾ���� ��Χ��P0~99
			 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//Pλ��ֵ����Ϊ��
		 }

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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

		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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

		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;//mvs p1.x + ?

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] == '.')
		 {
			 i++;
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;//mvs p1.x + 12.?
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
 * ��� mov ָ�� �ؽڲ岹
 */
unsigned char CheckMov(PrgCode* codeStrings)
{
	 unsigned char i,err=0,p_num,n_num;
	 int speed;
	 for(i=0;i<3;i++)//
	 {
		  if(codeStrings->codeStrings[i] != s_code_mov[i])
		  {
			   return  ERR_CMD;//�������
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


	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
		 return ERR_NO_P;//��ʽ���� λ�ñ�����Ӧ����p��ͷ
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
			 if(p_num>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;//��ʾ���� ��Χ��P0~99
			 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//Pλ��ֵ����Ϊ��
		 }

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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

		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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

		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;//p0 = p1.x + ?

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] == '.')
		 {
			 i++;
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;//mov p1.x + 12.?
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
 * ��� mvh ָ�� ������Բ
 */
unsigned char CheckMvh(PrgCode* codeStrings)
{
	 unsigned char i,p_num=0;
	 int speed;
	 for(i=0;i<3;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_mvh[i])
		  {
				return  ERR_CMD;//�������
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_NO_P;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == 'p')
	 {
		  i++;
	 }
	 else
	 {
		 return ERR_NO_P;//��ʽ���� λ�ñ�����Ӧ����p��ͷ
	 }

	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_NO_P;
	 p_num = StringToUint(((unsigned char*)codeStrings)+i);
	 if(p_num>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;//��ʾ���� ��Χ��P0~49

	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i]=='\0')//����'\0'��ʵ�Ѿ�������
	 {
		  if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//Pλ��ֵ����Ϊ��
		  return code_mvh;
	 }
	 else if((codeStrings->codeStrings[i] == '+') || (codeStrings->codeStrings[i] == '-'))
	 {
	 	 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
	 {
		 speed = atoi(((char*)codeStrings)+i);
		 if(speed<=0 || speed>100)	return ERR_SPEED;
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
	 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//Pλ��ֵ����Ϊ��
	 return code_mvh;
}

/**************************************
 * ��� mvr ָ�� Բ��
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
				return  ERR_CMD;//�������
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_NO_P;


	 for(j=0;j<3;j++)
	 {
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] == 'p')
		 {
			  i++;
		 }
		 else
		 {
			 return ERR_NO_P;//��ʽ���� λ�ñ�����Ӧ����p��ͷ
		 }
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_NO_P;

		 p_num = StringToUint(((unsigned char*)codeStrings)+i);
		 if(p_num>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;//��ʾ���� ��Χ��P0~49

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//Pλ��ֵ����Ϊ��
		 GetPxxPulseValue(p_num,pulse);
		 PulseToAngle(pulse,angle);
		 p_g[j] = GetPositionGesture(angle);//�������
	 }

	 if(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
	 {
		 speed = atoi(((char*)codeStrings)+i);
		 if(speed<=0 || speed>100)	return ERR_SPEED;
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
 * ��� mvc ָ�� ��Բ
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
				return  ERR_CMD;//�������
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_NO_P;


	 for(j=0;j<3;j++)
	 {
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] == 'p')
		 {
			  i++;
		 }
		 else
		 {
			 return ERR_NO_P;//��ʽ���� λ�ñ�����Ӧ����p��ͷ
		 }
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_NO_P;

		 p_num = StringToUint(((unsigned char*)codeStrings)+i);
		 if(p_num>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;//��ʾ���� ��Χ��P0~49

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//Pλ��ֵ����Ϊ��
		 GetPxxPulseValue(p_num,pulse);
		 PulseToAngle(pulse,angle);
		 p_g[j] = GetPositionGesture(angle);//�������
	 }

	 if(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
	 {
		 speed = atoi(((char*)codeStrings)+i);
		 if(speed<=0 || speed>100)	return ERR_SPEED;
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
 * ���ָ�� ovrd
 */
unsigned char CheckOvrd(PrgCode* codeStrings)
{
	 unsigned char i;
	 unsigned long value=0;
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_ovrd[i])
		  {
			   return  ERR_CMD;//�����ȷ
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_OVRD;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_OVRD;

	 value = StringToUint(&codeStrings->codeStrings[i]);
	 if(value == 0 || value >100)return ERR_OVRD;

	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0') return code_ovrd;
	 else return ERR_OVRD;
}



/**************************************
 * ���ָ�� open
 */
unsigned char CheckOpen(PrgCode* codeStrings)
{
	 unsigned char i,err=0;

	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_open[i])
		  {
			   return  ERR_CMD;//�����ȷ
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_CMD;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
 * ���Pxָ�� p0=p1, p0=p1.x-20.1,p0=p1.j2-12.3,p0=t1 n;
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
	 if(p_num1 >= MAX_POSITION_NUM) return ERR_P_OVERFLOW;//��ʾ���� ��Χ��0~99
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 //Px.j2 = y/fy (-/+ z/fz);
	 if(codeStrings->codeStrings[i] == '.')
	 {
		 if(!CheckPositionState(p_num1)) return ERR_P_NO_VALUE;//Pλ��ֵ����Ϊ��
		 i++;	//���� .
		 if(codeStrings->codeStrings[i] != 'j')
		 {
			  return ERR_CMD;//ȱ�ٲ��� px = ?
		 }
		 i++;	//����j
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

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] != '=')
		 {
			  return ERR_CMD;//ȱ�ٲ��� px.j1 ?
		 }
		 i++;	//����=
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }

		 if((codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9') ||
			 codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-' )
		 {
			 if(codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')
			 {
				 i++;	//��������
				 //���ź������û�н����־ͱ���
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')
					 return ERR_CMD;
			 }
			 while((codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9'))//��������
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i] == '.')
			 {
				 i++;
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;
				 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
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
			 if(f_num >= MAX_F_NUM) return ERR_F_OVERFLOW;//��ʾ���� ��Χ��f0~f49
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
			 {
				 i++;
			 }
		 }
		 else return ERR_CMD;//ȱ�ٲ��� px.j1 = ?


		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i] == 'f')
			 {
				 i++;
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_F_OVERFLOW;
				 f_num = StringToUint(&codeStrings->codeStrings[i]);
				 if(f_num >= MAX_F_NUM) return ERR_F_OVERFLOW;//��ʾ���� ��Χ��f0~f49
				 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
				 {
					 i++;
				 }
				 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
				 while((codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9'))//��������
				 {
					 i++;
				 }
				 while(codeStrings->codeStrings[i] == ' ')//�����ո�
				 {
					 i++;
				 }
				 if(codeStrings->codeStrings[i] == '.')
				 {
					 i++;
					 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;
					 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
					 {
						 i++;
					 }
					 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
		  return ERR_CMD;//ȱ�ٲ��� px = ?
	 }
	 i++;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] == 'p')
	 {
		 i++;
		 //px = p  �����
		 if(codeStrings->codeStrings[i]==0)
		 {
			 return code_p_x;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]==0 ||
			codeStrings->codeStrings[i]=='-')
		 {
//			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
//			 {
//				 i++;
//			 }
			 if(codeStrings->codeStrings[i]==0)	//px = p
			 {
				 return code_p_x;
			 }
			 else if(codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')//px = p + rsrd�����
			 {
				 i++;	//����+ or -
				 while(codeStrings->codeStrings[i] == ' ')//�����ո�
				 {
					 i++;
				 }
				 for(j=0;j<4;j++,i++)
				 {
					  if(codeStrings->codeStrings[i] != s_code_rsrd[j])
					  {
						   return  ERR_RSRD;//�����ȷ
					  }
				 }
				 while(codeStrings->codeStrings[i] == ' ')//�����ո�
				 {
					 i++;
				 }
				 if(codeStrings->codeStrings[i] == '\0')
				 {
					 return code_p_x;
				 }
				 else
				 {
					 return  ERR_RSRD;//�����ȷ
				 }
			 }
			 else
			 {
				 return ERR_RSRD;
			 }
		 }

		 //px = p(y).z�����
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
			 if(p_num2 >= MAX_POSITION_NUM) return ERR_P_OVERFLOW;//��ʾ���� ��Χ��0~99
		 }
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] == '\0')
		 {
			 if(!CheckPositionState(p_num2)) return ERR_P_NO_VALUE;//Pλ��ֵ����Ϊ��
			 else
			 {
				 return code_p_x;
			 }
		 }
		 else if(codeStrings->codeStrings[i] == '.')
		 {
			 i++;
		 }
		 else if(codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')//px = py +/- rsrd�����
		 {
			 i++;	//���� +/-
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 for(j=0;j<4;j++,i++)
			 {
				  if(codeStrings->codeStrings[i] != s_code_rsrd[j])
				  {
					   return  ERR_RSRD;//�����ȷ
				  }
			 }
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i] == '\0')
			 {
				 return code_p_x;
			 }
			 else
			 {
				 return  ERR_RSRD;//�����ȷ
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
			 i++;//����j
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

		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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

		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if((codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') &&
			 codeStrings->codeStrings[i]!='f')
			 return ERR_CMD;//p0 = p1.x + ?

		 //p0 = p1.x + f0
		 if(codeStrings->codeStrings[i]=='f')
		 {
			 i++;	//����f
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')
			 	 return ERR_CMD;//p0 = p1.x + f?
			 f_num = StringToUint(&codeStrings->codeStrings[i]);
			 if(f_num >= MAX_F_NUM) return ERR_F_OVERFLOW;//��ʾ���� ��Χ��f0~f49
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i] != '\0') return ERR_CMD;//p0 = p1.x + fy?
			 return code_p_x;
		 }

		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] == '.')
		 {
			 i++;
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i] != '\0') return ERR_CMD;//p0 = p1.x + 123.3 ?
		 }
		 else if(codeStrings->codeStrings[i] != '\0') return ERR_CMD;//p0 = p1.x + 123.3 ?

		 if(!CheckPositionState(p_num2)) return ERR_P_NO_VALUE;//Pλ��ֵ����Ϊ��
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
				   return  ERR_RSRD;//�����ȷ
			  }
		 }
		 if(ROBOT_PARAMETER.HARDWARE_REV == 7)
		{
			return ERR_NO_RS232;	//07��û��RS232
		}
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if(codeStrings->codeStrings[i] != '\0') return ERR_RSRD;//p0 = rsrd
		 else
		 {
			 if(!CheckPositionState(p_num1)) return ERR_P_NO_VALUE;//Pλ��ֵ����Ϊ��
			 else
			 {
				 return code_p_x;
			 }
		 }
	 }
	 else
	 {
		 return ERR_CMD;////ȱ�ٲ��� px = ?
	 }
}

/**************************************
 * ��� pout ָ��    pout1 = 1
 */
unsigned char CheckPout(PrgCode* codeStrings)
{
	 unsigned char i;
	 unsigned int value=0;
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_pout[i])
		  {
			   return  ERR_CMD;//�����ȷ
		  }
	 }
	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_POUT_OVERFLOW;
	 value = StringToUint(&codeStrings->codeStrings[i]);
	 if(value >= MAX_POUT_NUM) return ERR_POUT_OVERFLOW;//��ʾ���� ��Χ��0~11
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != '=')
	 {
		  return ERR_CMD;//set or reset
	 }
	 i++;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if((codeStrings->codeStrings[i] != '0') && (codeStrings->codeStrings[i] != '1'))
	 {
		  return ERR_CMD;// ���� 0 �� 1
	 }
	 i++;

	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0') return code_pout;
	 else return ERR_CMD;
}
/**************************************
 * ��� pin ָ�IO��0��ʼ
 */
unsigned char CheckPin(PrgCode* codeStrings)
{
	 unsigned char i;
	 unsigned int value=0;
	 for(i=0;i<3;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_pin[i])
		  {
			   return  ERR_CMD;//�����ȷ
		  }
	 }

	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_PIN_OVERFLOW;
	 value = StringToUint(&codeStrings->codeStrings[i]);
	 if(value >= MAX_PIN_NUM) return ERR_PIN_OVERFLOW;//��ʾ���� ��Χ��0~17
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != '=')
	 {
		  return ERR_CMD;//set or reset
	 }
	 i++;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if((codeStrings->codeStrings[i] != '0') && (codeStrings->codeStrings[i] != '1'))
	 {
		  return ERR_CMD;// ���� 0 �� 1
	 }
	 i++;

	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0') return code_pin;
	 else return ERR_CMD;
}
/**************************************
 * ��� p ��ͷ��ָ��
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
 * ��� S ��ͷ��ָ��
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
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_SPEED;

	 value = StringToUint(&codeStrings->codeStrings[i]);
	 if(value == 0 || value >100)return ERR_SPEED;

	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0') return code_speed;
	 else return ERR_SPEED;

}
/**************************************
 * ��� waitָ��(���pinָ��һ��ʹ�ã�IO��0��ʼ��0~11)
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
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
 * ���tָ�� �������� �÷���t1 p12p13p14 10 11 (�Ѿ߱��������﷨���)
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
	 if(value>=MAX_TRAY_NUM)return ERR_TRAY_OVERFLOW;//���̺ŷ�Χ��0~5
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != ' ') return ERR_TRAY;//�������һ���ո� �ο��÷�

	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 for(j=0;j<3;j++)//���������3��Pλ��
	 {
		 if(codeStrings->codeStrings[i]!='p')//��һ��P
		 {
			 return ERR_NO_P;//���Pλ��
		 }
		 i++;
		 value= StringToUint(((unsigned char*)codeStrings) + i);//��ȡP�����ֵ
		 if(value>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;

		 if(GetPxxPulseValue(value,pulse[j])) return ERR_P_NO_VALUE;//��Ч��Pλ��
		 PulseToAngle(pulse[j],angle[j]);
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
	 }
	 if(CompareGestureByTowAngle(angle[0],angle[1])) return ERR_GUSTURE;//�ж���̬�Ƿ�һ��
	 if(CompareGestureByTowAngle(angle[0],angle[2])) return ERR_GUSTURE;//�ж���̬�Ƿ�һ��
	 //if(CompareGestureByTowAngle(angle[1],angle[2])) return ERR_GUSTURE;//�ж���̬�Ƿ�һ��
	 if(codeStrings->codeStrings[i] != ' ') return ERR_TRAY;//�������һ���ո� �ο��÷�

	 while(codeStrings->codeStrings[i] == ' ')//�����ո� t1 p12p13p14 10 11
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_TRAY;//�ֶ��ٷݣ�
	 value= StringToUint(((unsigned char*)codeStrings) + i);
	 if(value==0) return ERR_TRAY_GRID_ZERO;//���ܷ�0��
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')return ERR_TRAY;//�ո������� �ο��÷�
	 value= StringToUint(((unsigned char*)codeStrings) + i);
	 if(value==0) return ERR_TRAY_GRID_ZERO;//���ܷ�0��
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] == '\0')
	 {
		   return code_tray;
	 }
	 return ERR_TRAY;//�ο��÷� t1 p12p13p14 10 11
}

/***************************************
 * ���ftָ�� ��������  �÷���ft1 px
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
	 if(t_num>=MAX_TRAY_NUM)return ERR_TRAY_OVERFLOW;//���̺ŷ�Χ��0~19
	 if(!GetTrayDefState(t_num)) return ERR_TRAY_UNDEFINE;//δ���������
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_CMD;//�������һ���ո�
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i]!='p')
	 {
		 return ERR_NO_P;//���Pλ��
	 }
	 i++;
	 p_num= StringToUint(((unsigned char*)codeStrings) + i);//��ȡP�����ֵ
	 if(p_num>=MAX_POSITION_NUM) return ERR_P_OVERFLOW;
	 if(GetPxxPulseValue(p_num,pulse)) return ERR_P_NO_VALUE;//��Ч��Pλ��
	 PulseToAngle(pulse,angle);

	 if(CompareGestureByTowAngle(angle,s_tray[t_num].angle))
		 return ERR_GUSTURE;//�ж���̬�Ƿ�һ��

	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
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
 * ���tָ�� �������� �÷���t1 10�����ӱ�Ŵ�1��ʼ��
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
	 if(t_num>=MAX_TRAY_NUM)return ERR_TRAY_OVERFLOW;//���̺ŷ�Χ��0~5
	 if(!GetTrayDefState(t_num)) return ERR_TRAY_UNDEFINE;//δ���������
	 while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
	 {
		 i++;
	 }
	 if(*(strings+i) != ' ') return ERR_CMD;//�������һ���ո�
	 while(*(strings+i) == ' ')//�����ո�
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
		 if(value == 0) return ERR_TRAY_GRID_ZERO;//��Ŵ�1��ʼ,��ֻ��Ϊ����
		 if(value>GetTrayGridNum(t_num)) return ERR_TRAY_GRID_OVERFLOW;//������
	 }
	 else
	 {
		 return ERR_TRAY_GRID_ZERO;//��Ŵ�1��ʼ,��ֻ��Ϊ����
	 }

	 while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
	 {
		 i++;
	 }
	 while(*(strings+i) == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(*(strings+i)>='0' && *(strings+i)<='9')
	 {
		 speed = atoi(((char*)strings)+i);
		 if(speed<=0 || speed>100)	return ERR_SPEED;
		 while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
		 {
			 i++;
		 }
		 while(*(strings+i) == ' ')//�����ո�
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

	 //if(*(strings+i) != '\0') return ERR_TRAY_GRID_ZERO;//��Ŵ�1��ʼ,��ֻ��Ϊ����
	 return code_t_x;
}
/*****************************************
 * �������ܣ����Ifָ����﷨
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
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
		 pin_value = StringToUint(&codeStrings->codeStrings[i]);//��ȡ����IO
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
			return ERR_NO_RS232;	//07��û��RS232
		}
	 }
	 else if(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
	 {

	 }
	 else
	 {
		 return ERR_IF_OBJECT;
	 }
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
		 pin_value = StringToUint(&codeStrings->codeStrings[i]);//��ȡ����IO
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
			return ERR_NO_RS232;	//07��û��RS232
		}
	 }
	 else if(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
	 {

	 }
	 else
	 {
		 return ERR_IF_OBJECT;
	 }
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }

	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
 * �������ܣ����thenָ����﷨
 * 			if Nx <,= ,>,<=,>= Nx/x
 * 			then *loop / #sub
 * 			else *loop / #sub
 */
unsigned char CheckThen(PrgCode* codeStrings)
{
	 unsigned char i=0,j;
	 unsigned char str_tmp[COLUMNS_IN_LINE]={0};
	 unsigned char code_strings[COLUMNS_IN_LINE]={0};
	 unsigned int current_lines=0;//��ǰ����������
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_then[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_STAR_POUND;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != '*' && codeStrings->codeStrings[i] != '#')
		 return ERR_STAR_POUND;//ȱ��*/#��ͷ���ַ���

	 str_tmp[0] = codeStrings->codeStrings[i];
	 j=1;
	 i++;
	 if(codeStrings->codeStrings[i] == '\0' || codeStrings->codeStrings[i] == ' ')
		 return ERR_STRING_NAME;//����Ҫ��һ���ǿո���ַ�

	 while(codeStrings->codeStrings[i] && codeStrings->codeStrings[i]!=' ')
	 {
		 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
				 && !(codeStrings->codeStrings[i]>='a' && codeStrings->codeStrings[i]<='z')
				 && (codeStrings->codeStrings[i] != '_'))
		 {
			 return ERR_STRING_NAME;//*/#����������һ���ַ�������0~9��a~z��Χ�ڣ��м䲻���пո�
		 }
		 str_tmp[j++] = codeStrings->codeStrings[i];
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_STRING_NAME;

	 current_lines = GetLinesCurrentProgram();//��ǰ����������
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
		 return ERR_NO_THIS_LOOP;//�޴���ת��־
	 }
	 else
	 {
		 return ERR_NO_THIS_SUB;//�޴��ӳ���
	 }
}
/*****************************************
 * �������ܣ����elseָ����﷨
 * 			if   Nx <,= ,>,<=,>= Nx/x
 * 			then *loop / #sub
 * 			else *loop / #sub
 */
unsigned char CheckElse(PrgCode* codeStrings)
{
	 unsigned char i=0,j;
	 unsigned char str_tmp[COLUMNS_IN_LINE]={0};
	 unsigned char code_strings[COLUMNS_IN_LINE]={0};
	 unsigned int current_lines=0;//��ǰ����������
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_else[i])
		  {
			   return  ERR_CMD;
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_STAR_POUND;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] != '*' && codeStrings->codeStrings[i] != '#')
		 return ERR_STAR_POUND;//ȱ��*/#��ͷ���ַ���

	 str_tmp[0] = codeStrings->codeStrings[i];
	 j=1;
	 i++;
	 if(codeStrings->codeStrings[i] == '\0' || codeStrings->codeStrings[i] == ' ')
		 return ERR_STRING_NAME;//����Ҫ��һ���ǿո���ַ�

	 while(codeStrings->codeStrings[i] && codeStrings->codeStrings[i]!=' ')
	 {
		 if(!(codeStrings->codeStrings[i]>='0' && codeStrings->codeStrings[i]<='9')
				 && !(codeStrings->codeStrings[i]>='a' && codeStrings->codeStrings[i]<='z')
				 && (codeStrings->codeStrings[i] != '_'))
		 {
			 return ERR_STRING_NAME;//*/#����������һ���ַ�������0~9��a~z��Χ�ڣ��м䲻���пո�
		 }
		 str_tmp[j++] = codeStrings->codeStrings[i];
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] != '\0') return ERR_STRING_NAME;

	 current_lines = GetLinesCurrentProgram();//��ǰ����������
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
		 return ERR_NO_THIS_LOOP;//�޴���ת��־
	 }
	 else
	 {
		 return ERR_NO_THIS_SUB;//�޴��ӳ���
	 }
}
/******************************************
 * �������ܣ����n ���� ָ�� n0~n49  nx++;nx--;nx = nx;nx = xx;
 * 						�Ӽ��˳�����
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
	 while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
	 {
		 i++;
	 }
	 while(*(strings+i) == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(*(strings+i) == '+')//++
	 {
		 i++;
		 if(*(strings+i) != '+') return ERR_CMD;//nx++;
		 i++;
		 while(*(strings+i) == ' ')//�����ո�
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
		 while(*(strings+i) == ' ')//�����ո�
		 {
			 i++;
		 }
		 if(*(strings+i) == '\0') return code_n_x;
		 else return ERR_CMD;//nx--;
	 }
	 else if(*(strings+i) == '=')// =
	 {
		 i++;
		 while(*(strings+i) == ' ')//�����ո�
		 {
			 i++;
		 }
		 if(*(strings+i)>='0' && *(strings+i)<='9')//nx = x;
		 {
			 while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
			 {
				 i++;
			 }
			 while(*(strings+i) == ' ')//�����ո�
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
			 while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
			 {
				 i++;
			 }
			 while(*(strings+i) == ' ')//�����ո�
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
				 sign = strings[i];	//��¼���ţ������õ�
				 i++;
				 while(*(strings+i) == ' ')//�����ո�
				 {
					 i++;
				 }
				 if(*(strings+i)>='0' && *(strings+i)<='9' )
				 {
					 if(sign == '/')
					 {
						 n_value = atol((char*)(strings+i));
						 if(n_value == 0) return ERR_CANT_ZERO;//��������Ϊ��
					 }
					 while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
					 {
						 i++;
					 }
					 while(*(strings+i) == ' ')//�����ո�
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
						 if(get_n_value(n_num) == 0) return ERR_CANT_ZERO;//��������Ϊ��
					 }
					 while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
					 {
						 i++;
					 }
					 while(*(strings+i) == ' ')//�����ո�
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
 * �������ܣ����f ���� ָ�� f0~n49
 * 			ֱ�Ӹ�ֵ��fx = fy;fx = z;
 * 			���ĳ��Ƕȣ�f0 = j1;
 * 			�Ӽ��˳���
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
	 while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
	 {
		 i++;
	 }
	 while(*(strings+i) == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(*(strings+i) == '=')// =
	 {
		 i++;	//���� =
		 while(*(strings+i) == ' ')//�����ո�
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
			 while(strings[i] >= '0' && strings[i] <= '9')//��������
			 {
				 i++;
			 }
			 while(strings[i] == ' ')//�����ո�
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
				 i++;	//����з��ţ����������ţ�Ȼ�����жϺ���ӵ��ǲ�������
				 //������治�ǽ����־ͱ���
				 if(strings[i]<'0' || strings[i]>'9') return ERR_CMD;
			 }
			 while((*(strings+i)>='0' && *(strings+i)<='9'))//��������
			 {
				 i++;
			 }
			 while(strings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 if(strings[i] == '.')
			 {
				 i++;
				 if(strings[i]<'0' || strings[i]>'9') return ERR_CMD;//fx = 12.?
				 while(strings[i] >= '0' && strings[i] <= '9')//��������
				 {
					 i++;
				 }
				 while(strings[i] == ' ')//�����ո�
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
			 i++;	//����f
			 if(*(strings+i)<'0' || *(strings+i)>'9') return ERR_CMD;
			 n_num = StringToUint(strings+i);
			 if(n_num >= MAX_F_NUM) return ERR_F_OVERFLOW;//f0~f49
			 while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
			 {
				 i++;
			 }
			 while(*(strings+i) == ' ')//�����ո�
			 {
				 i++;
			 }

			 if(*(strings+i) == '\0') return code_f_x;
			 else if(*(strings+i) == '+' || *(strings+i) == '-' ||
					 *(strings+i) == '*' || *(strings+i) == '/')
			 {
				 sign = strings[i];	//��¼���ţ�������
				 i++;//����+-*/
				 while(*(strings+i) == ' ')//�����ո�
				 {
					 i++;
				 }
				 if((*(strings+i)>='0' && *(strings+i)<='9') ||
						  *(strings+i)=='+' || *(strings+i)=='-')//fx = x;
				 {
					 if(*(strings+i)=='+' || *(strings+i)=='-')
					 {
						 i++;	//����з��ţ����������ţ�Ȼ�����жϺ���ӵ��ǲ�������
					 }
					 //������治�ǽ����־ͱ���
					 if(strings[i]<'0' || strings[i]>'9') return ERR_CMD;
					 if(sign == '/')
					 {
						 f_value = atof((char*)(strings+i));
						 if(fabs(f_value) < 1.0e-8) return ERR_CANT_ZERO;
					 }
					 while((*(strings+i)>='0' && *(strings+i)<='9'))//��������
					 {
						 i++;
					 }
					 while(*(strings+i) == ' ')//�����ո�
					 {
						 i++;
					 }
					 if(strings[i] == '.')
					 {
						 i++;
						 if(strings[i]<'0' || strings[i]>'9') return ERR_CMD;//fx = 12.?
						 while(strings[i] >= '0' && strings[i] <= '9')//��������
						 {
							 i++;
						 }
						 while(strings[i] == ' ')//�����ո�
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
					 while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
					 {
						 i++;
					 }
					 while(*(strings+i) == ' ')//�����ո�
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
			   return  ERR_CMD;//�����ȷ
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_CMD;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i] < '0' || codeStrings->codeStrings[i] > '9')
		 return ERR_BAUD;//���������ò���ȷ��ֻ��Ϊ����
	 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
			   return  ERR_CMD;//�����ȷ
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
			   return  ERR_CMD;//�����ȷ
		  }
	 }
	 if(codeStrings->codeStrings[i] < '1' || codeStrings->codeStrings[i] > '5') return ERR_DISP;
	 if(codeStrings->codeStrings[++i] != ' ') return ERR_DISP;
	 return code_disp;
}

/**************************************
 * ��� rs232 ָ��
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
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
			   return  ERR_CMD;//�����ȷ
		  }
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(codeStrings->codeStrings[i]=='\0'||codeStrings->codeStrings[i]=='\r'||codeStrings->codeStrings[i]=='\n')
		 return code_finish;
	 else return ERR_CMD;
}

/**************************************
 * ��� acc ָ�� acc 1~100
 */
unsigned char CheckToolLen(PrgCode* codeStrings)
{
	 unsigned char i;
	 for(i=0;i<7;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_toollen[i])
		  {
			   return  ERR_CMD;//�����ȷ
		  }
	 }
	 if(codeStrings->codeStrings[i] != ' ') return ERR_CMD;
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if((codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') &&
			 codeStrings->codeStrings[i]!='+' && codeStrings->codeStrings[i]!='-')
		 return ERR_CMD;

	 while((codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9') ||
			 codeStrings->codeStrings[i] == '+' || codeStrings->codeStrings[i] == '-')//��������
	 {
		 i++;
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 if(codeStrings->codeStrings[i] == '.')
	 {
		 i++;
		 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;//mov p1.x + 12.?
		 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
 * ��� tool ָ�� tool(Px,Py,Pz,Rx,Ry,Rz)/tool.px = n
 */
unsigned char CheckTool(PrgCode* codeStrings)
{
	 unsigned char i,j;
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_tool[i])
		  {
			   return  ERR_CMD;//�����ȷ
		  }
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 //tool(Px,Py,Pz,Rx,Ry,Rz) ��ʽ
	 if(codeStrings->codeStrings[i]=='(') //|| codeStrings->codeStrings[i]>'9') return ERR_CMD;
	 {
		 i++;	//���� '('
		 for(j=0;j<6;j++)
		 {
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 if((codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') &&
				 codeStrings->codeStrings[i]!='+' && codeStrings->codeStrings[i]!='-')
				 return ERR_CMD;
			 while((codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')||
					codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')//��������
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i++] == '.')	//�������С���㣬����(����,����Ҳ����)
			 {
				 //С�����û�����֣�����
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;
				 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
				 {
					 i++;
				 }
				 while(codeStrings->codeStrings[i] == ' ')//�����ո�
				 {
					 i++;
				 }
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')	//���������ֽ���
				 {
					 i++;
				 }
			 }
		 }
		 i--;
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
		 i++;	//���� '.'
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

		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] != '=') return ERR_CMD;
		 i++;
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if((codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') &&
			 codeStrings->codeStrings[i]!='+' && codeStrings->codeStrings[i]!='-')
			 return ERR_CMD;
		 while((codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')||
				codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] == '.')
		 {
			 i++;
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
		 return  ERR_CMD;//�����ȷ
	 }
	 return code_tool;
}

/**************************************
 * ���part ָ�� part(Px,Py,Pz,Rx,Ry,Rz)/part.px = n
 */
unsigned char CheckPart(PrgCode* codeStrings)
{
	 unsigned char i,j;
	 for(i=0;i<4;i++)
	 {
		  if(codeStrings->codeStrings[i] != s_code_part[i])
		  {
			   return  ERR_CMD;//�����ȷ
		  }
	 }
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	 //part(Px,Py,Pz,Rx,Ry,Rz) ��ʽ
	 if(codeStrings->codeStrings[i]=='(')
	 {
		 i++;	//���� '('
		 for(j=0;j<6;j++)
		 {
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 if((codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') &&
				 codeStrings->codeStrings[i]!='+' && codeStrings->codeStrings[i]!='-')
				 return ERR_CMD;
			 while((codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')||
					codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')//��������
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 if(codeStrings->codeStrings[i++] == '.')	//�������С���㣬����(����,����Ҳ����)
			 {
				 //С�����û�����֣�����
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;
				 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
				 {
					 i++;
				 }
				 while(codeStrings->codeStrings[i] == ' ')//�����ո�
				 {
					 i++;
				 }
				 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9')	//���������ֽ���
				 {
					 i++;
				 }
			 }
		 }
		 i--;
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
		 i++;	//���� '.'
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
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] != '=') return ERR_CMD;
		 i++;
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if((codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') &&
			 codeStrings->codeStrings[i]!='+' && codeStrings->codeStrings[i]!='-')
			 return ERR_CMD;
		 while((codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')||
				codeStrings->codeStrings[i]=='+' || codeStrings->codeStrings[i]=='-')//��������
		 {
			 i++;
		 }
		 while(codeStrings->codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }

		 if(codeStrings->codeStrings[i] == '.')
		 {
			 i++;
			 if(codeStrings->codeStrings[i]<'0' || codeStrings->codeStrings[i]>'9') return ERR_CMD;
			 while(codeStrings->codeStrings[i] >= '0' && codeStrings->codeStrings[i] <= '9')//��������
			 {
				 i++;
			 }
			 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
		 return  ERR_CMD;//�����ȷ
	 }
	 return code_part;
}
/**************************************
 * ���network ָ�� network n
 * ������//2018/06/01
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
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
 * ���teamwork ָ�� teamwork n
 * ������//2018/06/01
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
	 while(codeStrings->codeStrings[i] == ' ')//�����ո�
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
