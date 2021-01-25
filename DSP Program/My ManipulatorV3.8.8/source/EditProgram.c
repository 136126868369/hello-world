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
#define CRC_PAGE_ADDR			63	//����У��λ��ҳ��ַ
#define CRC_BYTE_ADDR			0	//����У��λ��ҳ��ַ
long CURRENT_BAUD = 115200;		    //��ǰ������
//����ĩ����������ڻ�е��ĩ�˵������ϵ�Ĳ���
ToolParameter TOOL_END={
		{0,0,0,0,0,0},
		0
};

//��������������ڻ�е�ֻ������λ�ù�ϵ�Ĳ���
PartParameter PART_ROBOT={
		{0,0,0,0,0,0}
};

struct program_name_num s_program_name_num = {0,0};

static unsigned int s_row=0;			//��
static unsigned char s_column=0;		//��
static unsigned char s_page=0;		//ҳ
static unsigned int s_ACC = 0;		//���ٶ�ֵ
//static unsigned char s_run_flag=0;//0 û��ȫ������  1����������ȫ������
//static unsigned char s_program_open_flg=0;//����򿪱�־λ��0����ǰû�г����	1���Ѿ����˳���
struct SpeedStru
{
	 unsigned char ovrd:8;		//�������е�ȫ���ٶ�
	 unsigned char speed:8;	//�������еľֲ��ٶ�
	 unsigned int	 all_speed;//������¼�������е����ٶȣ�all_speed=ovrd*speed
};
static struct SpeedStru s_speed={20,50,10};

struct Position_Pulse  //sizeofΪ14
{
	unsigned char usedFlag:1; //P��λ���Ƿ�ʹ�õı�־λ�� 0:δʹ��  1���ѱ�ʹ��
	unsigned char reserved:7; //����
	unsigned char position; //Pxx
	long pulse[6];
};

struct ProgramInfo{
	unsigned char blockAddr;//���򱣴����ڵĿ��ַ
	unsigned char prgName[PROGRAM_NAME_LENGTH];//������
};


struct TrayStru s_tray[MAX_TRAY_NUM];//���̲��� (�Ӹ�����ֻ�ܽ���һ������ʼ��Ϊ0����)
#pragma  DATA_SECTION (s_Nx_variable, ".MY_MEM0")
static long s_Nx_variable[MAX_N_NUM];//Nx����
#pragma  DATA_SECTION (s_Sx_variable, ".MY_MEM0")
static long s_Sx_variable[MAX_N_NUM];//Sx����
#pragma  DATA_SECTION (s_Fx_variable, ".MY_MEM0")
static double s_Fx_variable[MAX_F_NUM];	//Fx����
#pragma  DATA_SECTION (s_program_code, ".MY_MEM0")
static PrgCode	   s_program_code[ROWS_IN_PROGRAM]={0};//�����д洢 750��
#pragma  DATA_SECTION (s_position_pulse, ".MY_MEM0")
static struct Position_Pulse s_position_pulse[MAX_POSITION_NUM]={0};

static unsigned int s_return_stack[MAX_RETURN]={0};//return�Ķ�ջ�������Ӻ���ǰ������return�󽫻᷵�ص���ֵ
static unsigned int *s_return_stack_pointer=NULL;//ָ��return��ջ�ĵײ�
int RX_POSITION_STA=0;//�ⲿλ�����ݽ��ձ�־λ��1����λ������  0����λ������
extern int USART_RX_STA_B;
//void MoveCursorPosition(void);
unsigned char GetRowPositionValue(unsigned int row);

void AngleToStrings(double* angle,unsigned char* angle_strings);

void ClearTrayData(void);
void SingleAxisAngleChange(unsigned char axis,double angle,long* pulse);
void SetSingleAxisAngle(unsigned char axis,double angle,long* pulse);

//#pragma CODE_SECTION(ExecuteGosub, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char ExecuteGosub(PrgCode codeStrings,unsigned int* row);
//#pragma CODE_SECTION(ExecuteReturn, "ramfuncs");//������//2018/06/01 for Extend RAM
void ExecuteReturn(PrgCode codeStrings,unsigned int* row);
//#pragma CODE_SECTION(ExecuteGoto, "ramfuncs");//������//2018/06/01 for Extend RAM
void ExecuteGoto(PrgCode codeStrings,unsigned int* row);
unsigned char ExecuteDly(PrgCode codeStrings);
void ExecuteEnd(PrgCode codeStrings);
void ExecuteFinish(PrgCode codeStrings);
//#pragma CODE_SECTION(ExecuteMov, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char ExecuteMov(PrgCode codeStrings,char teach);
//#pragma CODE_SECTION(ExecuteMvs, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char ExecuteMvs(PrgCode codeStrings,char teach);
//#pragma CODE_SECTION(ExecuteMvr, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char ExecuteMvr(PrgCode codeStrings,char teach);
//#pragma CODE_SECTION(ExecuteMvc, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char ExecuteMvc(PrgCode codeStrings,char teach);
//#pragma CODE_SECTION(ExecuteMvh, "ramfuncs");//������//2018/06/01 for Extend RAM
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
//#pragma CODE_SECTION(ExecuteIf, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char ExecuteIf(PrgCode codeStrings,unsigned int* row);
//#pragma CODE_SECTION(ExecuteThen, "ramfuncs");//������//2018/06/01 for Extend RAM
void ExecuteThen(PrgCode codeStrings,unsigned int* row);
//#pragma CODE_SECTION(ExecuteElse, "ramfuncs");//������//2018/06/01 for Extend RAM
void ExecuteElse(PrgCode codeStrings,unsigned int* row);
void ExecuteAcc(PrgCode codeStrings);
u8 ExecuteRstd(PrgCode codeStrings);
unsigned char ExecuteDisp(PrgCode codeStrings);
unsigned char ExecuteRS232(PrgCode codeStrings);
void ExecuteToolLength(PrgCode codeStrings);
void ExecuteTool(const PrgCode* codeStrings);
void ExecutePart(const PrgCode* codeStrings);
void ExecuteNetwork(PrgCode codeStrings,unsigned int* row);//������//2018/06/01 for network
void ExecuteTeamwork(PrgCode codeStrings,unsigned int* row);//������//2018/06/01 for teamwork


PositionEuler StringToPositionEuler(char* str);
/***************************************
 * �������ܣ���ó����е���ָ��
 * ���������void
 * ���������s_program_code��ָ��
 */
PrgCode* GetProgramCode(void)
{
	return s_program_code;
}

/***************************************
 * �������ܣ�������������
 * ���������NONE
 * ���������NONE
 */
void ClearProgramPulseData(void)
{
	   memset(s_program_code,0,sizeof(s_program_code));		//����
	   memset(s_position_pulse,0,sizeof(s_position_pulse)); 	//����
	   //memset(DISPLAY_BUF,0,sizeof(DISPLAY_BUF));
	   memset(s_Nx_variable,0,sizeof(s_Nx_variable));
	   memset(s_Fx_variable,0,sizeof(s_Fx_variable));
	   memset(s_tray,0,sizeof(s_tray));
}

/***************************************
 * �������ܣ����n������ֵ
 * ���������n_num���
 * ���������n������ֵ
 */
long get_n_value(int n_num)
{
	return s_Nx_variable[n_num];
}
/***************************************
 * �������ܣ����s������ֵ
 * ���������s_num���
 * ���������s������ֵ
 */
long get_s_value(int s_num)
{
	return s_Sx_variable[s_num];
}

/***************************************
 * �������ܣ����n������ֵ
 * ���������n_num���
 * ���������n������ֵ
 */
double get_f_value(int n_num)
{
	return s_Fx_variable[n_num];
}

/***************************************
 * �������ܣ��õ�row�л���ı�
 * ���������row	��
 * 			 strings	�ַ�����ָ��
 * 			 ˵�����ַ�����\0��Ϊ��β
 * ����ֵ��	 ���������Ϣ
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
 * �������ܣ�	��ù��λ��
 * ���������	none
 * ����ֵ��		���λ��	bit0~7���У�bit8~15����
 * �÷���		cursor = GetCursorPosition();
 * 		 		row = cursor&0xff��//��
 * 		 		column = (cursor>>8)0xff;//��
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
 * �������ܣ�	��õ�ǰҳ
 * ���������	none
 * ����ֵ��		��ǰҳ��	�ӵ�0ҳ��ʼ
 */
unsigned char GetPage(void)
{
	 return s_page;
}
/*********************************************
 * �������ܣ�	��õ�ǰ����Ĵ���������
 * ���������	none
 * ����ֵ��		��������0~750,0����һ�ж�û��
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
 * �������ܣ�	��õ�row�д���
 * ���������	row	�� ��0��ʼ
 * 				code	�����ı���ָ��
 * 				ע�⣺�����ı����������COLUMNS_IN_LINE(18)���ַ��Ĵ���,Ҫ���ȫ�����ݣ�
 * 				��ʹ��һ������Ϊ18��unsigned char���͵����飬��code[18]��
 * 				Ȼ������ָ���׵�ַ��Ϊ�������룺GetCodeTxt(row,code);
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
 * �������ܣ�	����ѱ����������
 * ���������	none
 * ����ֵ��		�ѱ���������������Χ0~MAX_PROGRAM_NUM��
 */
unsigned char GetProgramNum(void)
{
	 unsigned char num;
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//��ó������
	 if(num == 0xff) return 0;//0xff ��ʾ��һ��ʹ�ã�Flashȫ��ֵΪ0xff���ʷ���0
	 return num;
}
/*********************************************
 * �������ܣ�	��ö�Ӧ��ŵĳ��������
 * ���������	num��	��������ţ����ܴ���Ŀǰ�ѱ���ĳ���������
 * 				name��	��������ָ�루��
 * 				˵����	�����ǰ�˳���������еģ���0��ʼ��
 * ����ֵ��		0���ɹ�		1��ʧ��(�޴˳�����)
 */
unsigned char GetProgramName(unsigned char prg_num,unsigned char *name)
{
	 unsigned char i,num;
	 struct ProgramInfo prg_info;
	 memset(&prg_info,0,sizeof(prg_info));
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//��ó������
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
 * �������ܣ���ȡ��ǰ�е�λ�ã��Ѿ������﷨��飨������д�����P��λ��Pxx�Ļ���ʾ��ʹ�ã�
 * ���������row->��
 * 			 num->ָ��p_num��ָ��
 * ����ֵ��0->�ɹ�	0x91->��λ���ѱ�ʹ�ã�ѯ���Ƿ񸲸�  ������0������(���Ĵ������)
 */
unsigned int GetPositionNumInRow(unsigned int row,unsigned int* num)
{
	 unsigned int i,sign,p_num;
	 sign = CheckGrammar(&s_program_code[row]);
	 if((sign==code_mov) || (sign == code_mvh) || (sign==code_mvs) ||
			 (sign==code_p_x) || (sign==ERR_P_NO_VALUE))
	 {
		  p_num = StringToUint((unsigned char*)&s_program_code[row]);//��row���л�ȡPֵ��P�����ֵ
		  *num = p_num;
		  i=0;
		  while(s_position_pulse[i].usedFlag)
		  {
			   if(s_position_pulse[i].position == p_num)//���ظ���Pλ�ý��Ḳ�ǵ�
			   {
					return WARN_DUPLI_P;//  WARN_DUPLI_P
			   }
			   i++;
			   if(i>=MAX_POSITION_NUM)
			   {
					return ERR_NO_P;//һ������²��ᵽ���д���ֻ��Ԥ��������ѭ��
			   }
		  }
		  sign =0;
	 }
	 return sign;
}


/**************************************
 * �������ܣ�����ǰλ���������ݵ���row�е�Pλ���б��棬�Ѿ������﷨��飨������д�����P��λ��Pxx�Ļ���ʾ��ʹ�ã�
 * 			(Pλ�����������ǰ�˳��洢�ģ�����ʱ����԰�˳���ȡ���ȶ�Pλ�ø���n����ѭ��n�ηֱ����)
 * ���������row����
 * ����ֵ��0���ɹ�	��0������(���Ĵ������)
 */
unsigned char CapturePositionDataInRow(unsigned int row)
{
	 unsigned int i,sign,p_num;
	 sign = CheckGrammar(&s_program_code[row]);
	 if((sign==code_mov) || (sign == code_mvh) || (sign==code_mvs) ||
			 (sign==code_p_x) || (sign==ERR_P_NO_VALUE))
	 {
		  p_num = StringToUint((unsigned char*)&s_program_code[row]);//��row���л�ȡPֵ��P�����ֵ
		  i=0;
		  while(s_position_pulse[i].usedFlag)
		  {
			   if(s_position_pulse[i].position == p_num)//���ظ���Pλ�ý��Ḳ�ǵ�
			   {
					//ReadCurrentPulseEp(s_position_pulse[i].pulse);
					ReadCurrentPulseLp(s_position_pulse[i].pulse);//������//2018/06/01
					return 0;
					//return WARN_DUPLI_P;//  WARN_DUPLI_P
			   }
			   i++;
			   if(i>=MAX_POSITION_NUM)
			   {
					return ERR_NO_P;//һ������²��ᵽ���д���ֻ��Ԥ��������ѭ��
			   }
		  }
		  s_position_pulse[i].usedFlag = 1;//��Ǵ�Pλ���Ѿ���ʹ��
		  s_position_pulse[i].position = p_num;//����Pλ��ֵ
		  //ReadCurrentPulseEp(s_position_pulse[i].pulse);
		  ReadCurrentPulseLp(s_position_pulse[i].pulse);//������//2018/06/01
		  sign =0;
	 }
	 return sign;
}

/******************************************
 * �������ܣ�������ֵ���浽p_num
 */
void SavePositionData(unsigned char p_num,long* pulse)
{
	unsigned char i=0,j=0;
	while(s_position_pulse[i].usedFlag)
	{
	   if(s_position_pulse[i].position == p_num)//���ظ���Pλ�ý��Ḳ�ǵ�
	   {
		   break;
	   }
	   i++;
	}

	for(j=0;j<6;j++)
	{
		s_position_pulse[i].pulse[j] = *(pulse+j);
	}
	s_position_pulse[i].usedFlag = 1;//��Ǵ�Pλ���Ѿ���ʹ��
	s_position_pulse[i].position = p_num;//����Pλ��ֵ
}

/******************************************
 * �������ܣ���Pλ�ñ��Ϊ��ʹ��
 */
void SavePositionState(unsigned char p_num)
{
	unsigned char i=0;
	while(s_position_pulse[i].usedFlag)
	{
	   if(s_position_pulse[i].position == p_num)//���ظ���Pλ�ý��Ḳ�ǵ�
	   {
		   break;
	   }
	   i++;
	}
	s_position_pulse[i].usedFlag = 1;//��Ǵ�Pλ���Ѿ���ʹ��
	s_position_pulse[i].position = p_num;//����Pλ��ֵ
}

/**************************************
 * �������ܣ������û��Pλ�ã��Ѿ������﷨���
 * ���������row  ��
 * ����ֵ��	 0��û�з��ִ���
 * 			 0x91(WARN_DUPLI_P) �������ظ�Pλ�ã������ж��Ƿ񸲸�
 * 			 ����������(���Ĵ������)
 */
unsigned char CheckPnum(unsigned int row)
{
	 unsigned int sign,i,p_num;
	 sign = CheckGrammar(&s_program_code[row]);
	 if((sign==code_mov) || (sign == code_mvh) || (sign==code_mvs))
	 {
		  p_num = StringToUint((unsigned char*)&s_program_code[row]);//��row���л�ȡPֵ��P�����ֵ
		  i=0;
		  while(s_position_pulse[i].usedFlag)
		  {
			   if(s_position_pulse[i].position == p_num) return WARN_DUPLI_P;//���ظ���Pλ��
			   i++;
		  }
	 }
	 return sign;
}
/**************************************
 * �������ܣ�ɾ��Pxx��λ�ã����Pxx���ڵĻ���xxx
 * 			(Pλ�����������ǰ�˳��洢�ģ�����ʱ����԰�˳���ȡ���ȶ�Pλ�ø���n����ѭ��n�ηֱ����)
 * ���������row����
 * ����ֵ��0���ɹ�	��0������
 */
unsigned char DeletePosition(unsigned char p_num)
{
	 //ɾ���󣬺��油�ϣ������������а�˳�򱣴�
	 return 0;
}
/**************************************
 * �������ܣ���õ�ǰÿ��Ƕ�ת���õĵ��ַ���
 * ���������angle_strings	�ַ�����ָ��
 * 			 �ַ�����ʽ��angle_strings[6][8] 6��8�У�ÿһ�д���һ��������ݣ���λ����
 * 			 ����,20.123�ȣ��᷵��"\0\020.123"
 * 			 	  -1.230�����᷵��"\0\0-1.230"
 * 			 	  ʹ��ʱ���������������ʾ������\0�������ܹ����Ч��
 * ����ֵ��  none
 */
void GetCurrentAngleStrings(unsigned char* angle_strings)
{
	 double angle[6];
	 GetCurrentAngle(angle);
	 AngleToStrings(angle,angle_strings);
}

/**************************************
 * �������ܣ����Pxx��λ��ÿ��Ƕȣ�ת���õ��ַ�����
 * ���������p_num	pλ�õ�ֵ����p11��������11��������p11)
 * 			 p_to_angle	ָ���ַ�����ָ��
 * 			 �ַ�����ʽ��angle_strings[6][8] 6��8�У�ÿһ�д���һ��������ݣ���λ����
 * 			 ����,20.123�ȣ��᷵��"\0\020.123"
 * 			 	   -1.230�����᷵��"\0\0-1.230"
 * 			 	   ʹ��ʱ���������������ʾ������\0�������ܹ����Ч��
 * ����ֵ��	 0���ɹ�	1��P�㲻����
 */
unsigned char GetPnumAngleStrings(unsigned char p_num,unsigned char* angle_strings)
{
	 unsigned char i;
	 double angle[6];
	 for(i=0;i<MAX_POSITION_NUM;i++)
	 {
		  //����(s_position_pulse[i].usedFlag == 1)�ж�����Ϊ��s_position_pulse[i].position�ǿ϶�����
		  //��ֵ�ģ����ԣ����жϵĻ�������ֶ���Pλ�ÿ�ֵ�����������s_position_pulse[0].position
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
 * �������ܣ����pλ������ĵ�num����Ա��ÿ��ĽǶȣ�ת���õ��ַ�����Pλ�ù���ҳ���ã�
 * ���������num	������pλ������ĵ�num�����ݶ���
 * 			 p_name p�����֣���p01,p11,�ַ����ͣ�����Ϊ3
 * 			 p_to_angle	ָ���ַ�����ָ��
 * 			 �ַ�����ʽ��angle_strings[6][8] 6��8�У�ÿһ�д���һ��������ݣ���λ����
 * 			 ����,20.123�ȣ��᷵��"\0\020.123"
 * 			 	   -1.230�����᷵��"\0\0-1.230"
 * 			 	   ʹ��ʱ���������������ʾ������\0�������ܹ����Ч��
 * ����ֵ��	 0���ɹ�	1��P�㲻����
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
 * �������ܣ����Pxx��λ��ÿ�������ֵ
 * ���������p_num	pλ�õ�ֵ����p11��������11��������p11)
 * 			 pulse	ָ������ֵ��ָ�룬long����
 * ����ֵ��	 0���ɹ�	ERR_P_NO_VALUE��P�㲻����
 */
unsigned char GetPxxPulseValue(unsigned char p_num,long* pulse)
{
	 unsigned char i,j;

	 for(i=0;i<MAX_POSITION_NUM;i++)
	 {
		  //����(s_position_pulse[i].usedFlag == 1)�ж�����Ϊ��s_position_pulse[i].position�ǿ϶�����
		  //��ֵ�ģ����ԣ����жϵĻ�������ֶ���Pλ�ÿ�ֵ�����������s_position_pulse[0].position==0ʱ
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
			    	return ERR_P_NO_VALUE;//P��λ�ò�����
			    }
		  }
	 }
	 return ERR_P_NO_VALUE;//P��λ�ò�����
}
/*************************************
 * �������ܣ����Ŀǰ�ѱ�ʹ�õ�Pλ�ø���
 * ���������none
 * ����ֵ��	 �ѱ�ʹ�õ�Pλ�ø���
 */
unsigned char GetPositionUsedNum(void)
{
	 unsigned char i=0;
	 while(s_position_pulse[i].usedFlag)//��ΪPλ����˳��洢��
	 {
		 i++;
	 }
	 return i;
}

/**************************************
 * ��������: ���Pxxλ��ʹ��״̬
 * �������: p_num	Pλ��ֵ��Pxx�е�xx����P11��11��
 * ����ֵ��	 1��Pλ���ѱ�ʹ��	0��Pλ��δ��ʹ��
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
 * �������ܣ��ӵ�row�л��Pλ��ֵ��������д�����P��λ��Pxx���ͽ�xx��ֵ��ȡ������
 * 			 ��P11�ͻ��11���Ƚ��ʺ�ʾ��ʹ�ã�
 * ���������row����
 * ����ֵ��	 Pλ��ֵ
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
 * �������ܣ��޸ĵ�row�е�column�еĴ���
 * ���������row	��	����Χ0~255��
 * 			 column	��	 (��Χ0~14)
 * 			 Character	�ַ�
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
 * �������ܣ��ڵ�row������һ�пհ��У�Ҳ���ǻس���ʱ��,����һ�У�
 * ���������row	��	����Χ0~749��
 * ����ֵ��	 0���ɹ�	1��ʧ�ܣ�>=749�У�
 */
unsigned char IncreaseEmptyRow(unsigned int row)
{
	 unsigned int i;
	 if(row > (ROWS_IN_PROGRAM-1))return 1;			//�ж���û�г��������

	 //if(s_program_code[ROWS_IN_PROGRAM-1].codeStrings[0]!=0)//�ж����һ����û�����ݣ���0��
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
 * �������ܣ�ɾ�������row�е�column�е��ַ�
 * ���������row		��row�У�row��Χ��0~255��
 * 			 column		��column�У���Χ��0~13��
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
	 s_column--;		//�����һ��
}
/***************************************
 * �������ܣ�ɾ��ĳһ�еĴ����ı�
 * ���������row	����ɾ���ĵ�row�д��루row��Χ��0~255��
 */
void ClearRowChar(unsigned int row)
{
	 unsigned int i;
	 for(i=0;i<COLUMNS_IN_LINE;i++)
	 {
		  s_program_code[row].codeStrings[i] = 0;
	 }
	 s_column = 0;		//���ص���һ��
}
/***************************************
 * �������ܣ�ɾ������ĵ�row��
 * ���������row	����ɾ���ĵ�row�У�row��Χ��0~255��
 * 			 ��DeleteRowCodeTxt��������������DeleteRowCodeTxt
 * 			 ֻ��ɾ���ı������Ǳ����ģ�ֻ�����ǿ��У���DeleteRowCode������ɾ��
 * 			 ��row�У����Һ�����л���ǰƽ��1����λ
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
		  s_program_code[ROWS_IN_PROGRAM-1].codeStrings[i] = 0;//���һ�У���255�У�����
	 }
	 //memset(&s_program_code[ROWS_IN_PROGRAM-1],0,sizeof(&s_program_code[ROWS_IN_PROGRAM-1]));
	 //s_column = COLUMNS_IN_LINE-1;
	 s_row--;			//���λ��
	// ModifyCursorPosition(s_row,s_column);//�ƶ����
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/***************************************
 * �������ܣ��޸Ĺ��λ�ã��������༭����ʱ�ã�
 * ���������row	��	����Χ0~14��
 * 			 column	��	����Χ0~13��
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
 * �������ܣ��޸ĵ�ǰҳ�루�����ҳ��ťʱ�ã�
 * ���������page	ҳ��
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
 * �������ܣ��жϴ˳����Ƿ����
 * ���������prg_name	��������ָ��
 * ����ֵ��	 0���������
 * 			 1���޴˳���
 * 			 2��flash����
 */
unsigned char IsProgramValid(unsigned char* prg_name)
{
	 unsigned char num,err;
	 int i;
	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));

	 //�������
	 err = CheckPrgName((char*)prg_name);
	 if(err) return 1;

	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//��ó������
	 if(num > MAX_PROGRAM_NUM) return 2;
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//��������Ϣ����
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
 * �½�����
 * ����		name	��������ָ�룬�����19���ַ�
 * 			mdoe:	0->�½�ʱ��մ洢����	1->�½�ʱ����մ洢����
 * ����ֵ	0x00���ɹ�
 * 			0x01��ʧ��(������Ϣ��ʧ��)
 * 			0x03��ʧ�ܣ������ظ���
 * 			0x04: ���ֹ�����19���ַ����ڣ�
 * 			0xff��ʧ�ܣ����������������
 */
unsigned char NewProgram(unsigned char* name,unsigned char mode)
{
	 unsigned char j;
	 int i;
	 unsigned char err=0,num=0,flag=0,byte=0;
	 long crc[2];
	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));

	 //�������
	 err = CheckPrgName((char*)name);
	 if(err) return 4;

	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//��ó������
	 if((num==0xff) || (num==0))//0xff��ʾ���ǵ�һ��ʹ�ã���û�б����,���Գ������Ϊ0
	 {
		  num =0;
		  //memset(prg_info,0,sizeof(prg_info));
	 }
	 if(num >= MAX_PROGRAM_NUM) return 0xff;				//����������������������½�
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info[0])*num,0,(unsigned char*)prg_info);//��������Ϣ����
	 for(i=0;i<num;i++)
	 {
		  if(!StringCmp(PROGRAM_NAME_LENGTH,prg_info[i].prgName,name))
		  {
			  return 3;//�������ظ�
		  }
	 }

	 for(i=4;i<(MAX_PROGRAM_NUM*2+40);i++)	//�ӵ�5�鿪ʼ������100*2=200�������Խ�MAX_PROGRAM_NUM������
	 {
		  ReadNandFlashData(i,0,0,1,0,&byte);//��ÿ��page0���׸�byte
		  if(byte == 0xff)					  	//0xff��δʹ�ù�  ��0xff����ʹ��
		  {
			   if(!EraseNandFlashBlock(i))	//��������ɹ�
			   {
					flag++;
			   }
			   else
			   {
					flag = 0;				 	//���¿�ʼ
					WriteNandFlashData(i,0,0,1,0,0x00);	//��һҳ��һbyteд���0xff������������(��Ϊ���鲻�����ã�
			   }
		  }
		  else
		  {
			   flag = 0;					 	//���¿�ʼ
		  }
		  if(flag >= 2)//�������������Ŀտ飨һ�鱣����룬һ�鱣���������ݣ�
		  {
				/*if(!NandFlashCopyBack(INFO_BLOCK_ADDR*64,0,CACHE_BLOCK_ADDR*64,0))//���ݵ�0��page0��Ϣ
				{
					 if(!NandFlashCopyBack(INFO_BLOCK_ADDR,0,CACHE_BLOCK_BACK_ADDR,0))
					 {
						  return 1;//�ڴ����
					 }
				}*/
			   prg_info[num].blockAddr = i-1;							//���򱣴�����ڿ��ַ
			   for(j=0;j<PROGRAM_NAME_LENGTH;j++)
			   {
					prg_info[num].prgName[j] = *(name+j);				//���������
					if(!*(name+j)) break;
			   }
			   if(mode == 0)
			   {
				   memset(s_program_code,0,sizeof(s_program_code));		//����
				   memset(s_position_pulse,0,sizeof(s_position_pulse)); 	//����
			   }
			   crc[0] = 0;
			   crc[1] = 0;

			   err = 0;
			   //����У��λ
			   err |= WriteNandFlashData(prg_info[num].blockAddr,CRC_PAGE_ADDR,CRC_BYTE_ADDR,sizeof(crc),1,(unsigned char*)&crc);
			   //ȷ���½����򱣴����ڿ��У�ȫ��ֵΪ0�����򣬴򿪳����δ��������е�ֵȫ��Ϊ0xff
			   err  = WriteNandFlashData(prg_info[num].blockAddr,0,0,sizeof(s_program_code),0,(unsigned char*)s_program_code);
			   err |= WriteNandFlashData(prg_info[num].blockAddr+1,0,0,sizeof(s_position_pulse),1,(unsigned char*)s_position_pulse);
			   err |= EraseNandFlashBlock(INFO_BLOCK_ADDR);			//������Ϣ��
			   num++;													//���������1
			   err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);	//��һ��byte������ǳ������
			   err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//����
			   ClearReturnStack();//���return��ջ
			   break;													//�˳�forѭ��
		  }
	 }
	 return err;
}

/******************
 * �������ܣ���ȡ�����λ��
 * ����ֵ��0���ɹ�
 * 		   1����ȡ����У��ʧ��
 */
unsigned char ReadPrgPos(unsigned int block)
{
	long crc_old[2],crc_new[2];
	int j,k,tmp;

	memset(s_program_code,0,sizeof(s_program_code));		//����
	memset(s_position_pulse,0,sizeof(s_position_pulse)); 	//����
	ReadNandFlashData(block,CRC_PAGE_ADDR,CRC_BYTE_ADDR,sizeof(crc_old),1,(unsigned char*)crc_old);
	ReadNandFlashData(block,0,0,sizeof(s_program_code),0,(unsigned char*)s_program_code);
	ReadNandFlashData(block+1,0,0,sizeof(s_position_pulse),1,(unsigned char*)s_position_pulse);

	//У����㣬8bit����ȫ�����
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
				memset(s_program_code,0,sizeof(s_program_code));		//����
				memset(s_position_pulse,0,sizeof(s_position_pulse)); 	//����
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
			 memset(s_program_code,0,sizeof(s_program_code));		//����
			 memset(s_position_pulse,0,sizeof(s_position_pulse)); 	//����
			 return 1;
		 }
	}
	return 0;
}
/**************************************
 * �������ܣ��򿪳����ı���λ������
 * ���������name	��������ָ��
 * 		   ��num		ָ��ڼ��������ָ��
 * ����ֵ��	 0���򿪳ɹ�
 * 			 1: ��ʧ�ܣ���ȡ����У��ʧ�ܣ�
 * 			 2����ʧ�ܣ��޴˳���
 */
unsigned char OpenProgram(unsigned char* name,unsigned int* num)
{
	 unsigned char n,err;
	 int i;

	 err = CheckPrgName((char*)name);
	 if(err) return 2;

	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&n);//��ó������
	 if(n > MAX_PROGRAM_NUM) return 1;
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//��������Ϣ����
	 for(i=0;i<n;i++)
	 {
		  if(!StringCmp(PROGRAM_NAME_LENGTH,prg_info[i].prgName,name))
		  {
			   *num = i+1;
			   ClearTrayData();//������������
			   ClearReturnStack();//���return��ջ

			   if(ReadPrgPos(prg_info[i].blockAddr))
			   {
				   if(ReadPrgPos(prg_info[i].blockAddr))
				   {
					   if(ReadPrgPos(prg_info[i].blockAddr))
						   return 1;//��ȡ����У��ʧ��
				   }
			   }
			   return 0;
		  }
	 }
	 return 2;//�޴˳���
}


/**************************************
 * ��������ı���λ������
 * �������ܣ���������λ�õ���Ӧ�ļ���
 * ���������name �ļ���������ָ��
 * ����ֵ��	 0������ɹ�
 * 			 1������ʧ��(�����������Flash��)
 * 			 2: ����ʧ��(�޴��ļ���)
 */
unsigned char SaveProgram(unsigned char* name)
{
	 unsigned char i,err=0,num,block,flag=0,byte=0;;
	 int j,k,tmp,prg_num;
	 long crc[2];
	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//��ó������
	 if(num > MAX_PROGRAM_NUM) return 1;
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//��������Ϣ����
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

	 //У����㣬8bit����ȫ�����
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

	 err |= WriteNandFlashData(block,CRC_PAGE_ADDR,CRC_BYTE_ADDR,sizeof(crc),1,(unsigned char*)&crc);//����У��λ

	 err |= WriteNandFlashData(block,0,0,sizeof(s_program_code),0,(unsigned char*)s_program_code);
	 err |= WriteNandFlashData(block+1,0,0,sizeof(s_position_pulse),1,(unsigned char*)s_position_pulse);

	 if(err)//��������������һ��
	 {
		 err  = EraseNandFlashBlock(block);
		 err |= EraseNandFlashBlock(block+1);

		 err |= WriteNandFlashData(block,CRC_PAGE_ADDR,CRC_BYTE_ADDR,sizeof(crc),1,(unsigned char*)&crc);//����У��λ

		 err |= WriteNandFlashData(block,0,0,sizeof(s_program_code),0,(unsigned char*)s_program_code);
		 err |= WriteNandFlashData(block+1,0,0,sizeof(s_position_pulse),1,(unsigned char*)s_position_pulse);
	 }

	 if(err)//���������֤�����Ѿ�����
	 {
		 for(i=4;i<(MAX_PROGRAM_NUM*2+40);i++)	//�ӵ�5�鿪ʼ������100*2=200�������Խ�MAX_PROGRAM_NUM������
		 {
			  ReadNandFlashData(i,0,0,1,0,&byte);//��ÿ��page0���׸�byte
			  if(byte == 0xff)					  	//0xff��δʹ�ù�  ��0xff����ʹ��
			  {
				   if(!EraseNandFlashBlock(i))	//��������ɹ�
				   {
						flag++;
				   }
				   else
				   {
						flag = 0;				 	//���¿�ʼ
						WriteNandFlashData(i,0,0,1,0,0x00);	//��һҳ��һbyteд���0xff������������(��Ϊ���鲻�����ã�
				   }
			  }
			  else
			  {
				   flag = 0;					 	//���¿�ʼ
			  }
			  if(flag >= 2)//�������������Ŀտ飨һ�鱣����룬һ�鱣���������ݣ�
			  {
				   err = 0;
				   prg_info[prg_num].blockAddr = i-1;							//���򱣴�����ڿ��ַ
				   err |= EraseNandFlashBlock(INFO_BLOCK_ADDR);			//������Ϣ��												//���������1
				   err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);	//��һ��byte������ǳ������
				   err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//����

				   err |= WriteNandFlashData(prg_info[prg_num].blockAddr,CRC_PAGE_ADDR,CRC_BYTE_ADDR,sizeof(crc),1,(unsigned char*)&crc);//����У��λ
				   err |= WriteNandFlashData(prg_info[prg_num].blockAddr,0,0,sizeof(s_program_code),0,(unsigned char*)s_program_code);
				   err |= WriteNandFlashData(prg_info[prg_num].blockAddr+1,0,0,sizeof(s_position_pulse),1,(unsigned char*)s_position_pulse);
				   break;//�˳�forѭ��
			  }
		 }
	 }
	 return err;
}
/***************************************
 * �������ܣ�	ɾ������
 * ���������	name	����ɾ���ĳ�������
 * ���������	0��ɾ���ɹ�
 * 				1��ɾ��ʧ�ܣ�������Ϣ����󣬿���flash�𻵣�
 * 				2: ɾ��ʧ�� (�޴˳���)
 */
unsigned char DeleteProgram(unsigned char* prg_name)
{
	 unsigned char i,j,block;
	 unsigned char err=0,num=0;
	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//��ó������
	 if(num > MAX_PROGRAM_NUM) return 2;
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//��������Ϣ���ݶ���

	 for(j=0;j<num;j++)
	 {
		  if(!StringCmp(PROGRAM_NAME_LENGTH,prg_info[j].prgName,prg_name)) break;
	 }
	 if(j == num)	return 2;

	 block = prg_info[j].blockAddr;	//����ǰ�ݴ���ַ
	 for(i=j;i<num-1;i++)			//��ɾ����λ�ý��������滻(�����������ǰ��һ����λ)
	 {
		  prg_info[i] = prg_info[i+1];
	 }
	 memset(&prg_info[num-1],0,sizeof(struct ProgramInfo));	//���һ�����ã�����
	 num--;											 	 		//���������һ
	 err = EraseNandFlashBlock(INFO_BLOCK_ADDR);	 		//������Ϣ��
	 err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);		//��һ��byte������ǳ������
	 err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//���������Ϣ
	 err |= EraseNandFlashBlock(block);	 	//������ɾ���Ŀ�
	 err |= EraseNandFlashBlock(block+1);	//������ɾ���Ŀ�
	 return err;
}

/***************************************
 * ��������	 copy_program
 * ���ܣ�   ���Ƴ���
 * �������	prg_name	�������Ƶĳ�������
 * 			new_name	���ƺ�ĳ�������
 * �������	0�����Ƴɹ�
 * 			1������ʧ�ܣ�������Ϣ����󣬿���flash�𻵣�
 * 			2: �޴˳���
 * 			3: �����ظ�
 * 			4�����ֹ�����19���ַ�����
 * 			0xff��ʧ�ܣ�����������������������½���
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
		 return err;//�޴˳���
	 }
	 err = SaveProgram(new_name);
	 if(err != 0)
	 {
		 return 1;
	 }
	 return 0;
}

/***************************************
 * ��������	 CopyProgram
 * ���ܣ�   ���Ƴ���Ȼ���֮ǰ�Ѿ����򿪵ĳ���
 * �������	prg_name	�������Ƶĳ�������
 * 			new_name	���ƺ�ĳ�������
 * �������	0�����Ƴɹ�
 * 			1������ʧ�ܣ�������Ϣ����󣬿���flash�𻵣�
 * 			2: �޴˳���
 * 			3: �����ظ�
 * 			4�����ֹ�����19���ַ�����
 * 			0xff��ʧ�ܣ�����������������������½���
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
			//��ʧ�������
			s_program_name_num.program_name[0] = 0;
			s_program_name_num.num = 0;
			SetTextValue(7, 82, "");
			SetTextValue(7, 83, "");
		}
	}
	return err;
}

/***************************************
 * ��������	 RenameProgram
 * �������	old_name	���������ĳ�������
 * 			new_name	�µ�����
 * �������	0���������ɹ�
 * 			1��������ʧ�ܣ�������Ϣ����󣬿���flash�𻵣�
 * 			2: ������ʧ�� (�޴˳���)
 * 			3: �����ظ�
 * 			4�����ֹ���
 */
unsigned char RenameProgram(unsigned char* old_name,unsigned char* new_name)
{
	 unsigned char j=0,num,err=0;
	 int i;
	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));

	 err = CheckPrgName((char*)new_name);
	 if(err) return 4;

	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);//��ó������
	 if(num > MAX_PROGRAM_NUM) return 1;//��ȡ����
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//��������Ϣ����

	 for(i=0;i<num;i++)
	 {
		 if(!StringCmp(PROGRAM_NAME_LENGTH,prg_info[i].prgName,new_name))
		 {
			 return 3;//�½��������ظ�
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
			   err |= EraseNandFlashBlock(INFO_BLOCK_ADDR);				//������Ϣ��
			   err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);	//��һ��byte������ǳ������
			   err |= WriteNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info),0,(unsigned char*)prg_info);//����
			   return err;
		  }
	 }
	 return 2;
}

/*****************************************
 * �������ܣ�SaveAxisNumber �����е�������
 * ���������axis_num	������
 * ���������0������ɹ�	1������ʧ��
 */
unsigned char SaveAxisNumber(unsigned char axis_num)
{
	unsigned char err=0;
	//err |= EraseNandFlashBlock(INFO_BLOCK_ADDR1);				//������Ϣ��
	//err |= WriteNandFlashData(INFO_BLOCK_ADDR1,0,0,1,0,&axis_num);	//��һ��byte������������
	return err;
}

/*****************************************
 * �������ܣ�ReadAxisNumber ����е�������
 * ���������NONE
 * �����������е�������
 */
unsigned char ReadAxisNumber(void)
{
	unsigned char axis_num=0;
	ReadNandFlashData(INFO_BLOCK_ADDR1,0,0,1,0,&axis_num);//��������
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
 * �������ܣ����򵥲�����
 * ���������cmd:ָ�����
 * 			codeStrings:�����ı�
 * ����ֵ��	 0���ɹ�	������0�����󣨲��ģ�
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
			 display_sixangle(6);//ȫ������ʱ��ʾ������ĽǶ�
			 break;
		 case code_mvs:
			 err = ExecuteMvs(*codeStrings,0);
			 display_sixangle(6);//ȫ������ʱ��ʾ������ĽǶ�
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
	//		 		 //*row = 0;//����򿪺�ӵ�0�п�ʼִ��
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
				ProgramRuningFlag(2);	//��ͣ
				SetButtonValue(6,2,1);	//���������С�
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
		while(ProgramRuningFlag(0) == 1)	//��ͣ
		{
			if(ErrCheck(6))
			{
			  G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//ʹ�ð�ȫ����
			  ClearStopProgramFlag();//�����ͣ��־λ
			  //SPIA_RX_STA = 0;
			  return ERR_ALARM;
			}
			KeyBoard_StopSpeed();
		}
		if(ProgramRuningFlag(0)==2)	//�˳�
		{
			CloseFan();
			ClearStopProgramFlag();
			G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//ʹ�ð�ȫ����
			//SPIA_RX_STA = 0;
			return 0;
		}
		else	//����
		{
			ClearStopProgramFlag();
			continue;
		}
	 }
	 return 0;
}


/*****************************************
 * �������ܣ����򵥲�����
 * ���������row�������е��У���0��ʼ
 * 			 ע�⣺���ִ�е�������ת���Ӻ�������ô���ֵ�����޸�Ϊ������Ӧ����
 * 			 ������������Լ�1����ʾ������һ��
 * 			 check: 0:�����﷨��飨ȫ�٣�	1�����﷨���(ʾ��)
 * ����ֵ��	 0���ɹ�	������0�����󣨲��ģ�
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
		 //��ֹ�������е�ʱ��ExecutePx ExecuteNxִ�����Σ��﷨����ʱ���Ѿ�ִ�й�һ���ˣ�
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
		 display_sixangle(6);//ȫ������ʱ��ʾ������ĽǶ�
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
		 display_sixangle(6);//ȫ������ʱ��ʾ������ĽǶ�
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
		 		 *row = 0;//����򿪺�ӵ�0�п�ʼִ��
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
	 	 display_sixangle(6);//ȫ������ʱ��ʾ������ĽǶ�
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
	 case code_network: ExecuteNetwork(codeStrings,row);break;//������//2018/06/01
	 case code_teamwork: ExecuteTeamwork(codeStrings,row);break;//������//2018/06/01
	 default:return cmd;
	 }
	 (*row)++;
	 return 0;
}
/*****************************************
* �������ܣ��жϸı��־λ��ִ�г���ʱȡ��־λ����ֵ
* ���������sign=0��ִ��״̬��sign=1��ʼִ�У�sign=2����ͣ���У�sign=3���˳�����
* ���������0����ʼ����	1����ͣ���� 2���˳�����
*/
u8 ProgramRuningFlag(u8 sign)
{
	static u8 sign_pause=1;
	if(sign>0)
	{
		if(1 == sign)
		{
			StartCpuTimer0();
			sign_pause=0;//��ʼ����
		}
		if(2 == sign)
		{
			StopCpuTimer0();
			sign_pause=1;//��ͣ����
		}
		if(3 == sign)
		{
			StopCpuTimer0();
			sign_pause=2;//�˳�����
		}
	}
	return sign_pause;
}

/*****************************************
 * �������ܣ�����ȫ������(������ָʾ���е���һ��)
 * ���������f ��ʾָ�� ����ָ��
 * 			 row �������е���
 * 			 next_row  �������е���һ�е�ָ��
 * ����ֵ��  �������  0����ȷ		����ֵ������
 */
extern struct EX_Position_stru EX_POSITION;
u8 ProgramRunWithPointer(void (*f)(unsigned char),unsigned int* row)
{
	 unsigned int num,err,i;
	 *row = 0;
	 SetButtonValue(6,2,0);
	 InitSci();	//��ʼ������
	 memset(&EX_POSITION,0,sizeof(EX_POSITION));
	 USART_RX_STA_B  = 0;
	 RX_POSITION_STA = 0;

	 num = GetLinesCurrentProgram();//��ó�������
	 ProgramRuningFlag(1);
	 ClearStopProgramFlag();
	 ClearReturnStack();
	 StartCpuTimer0();	//������ʱ��0����ʼ��ʱ

	 //���disp��ʾ����
	 for(i=0;i<6;i++)
	 {
		 SetTextValue(6, 28+i, " ");
	 }
	 G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 0;//��ʹ�ð�ȫ����
	 if(num>0)
	 {
		 OpenFan();
		 for(;*row<num;)//��ѭ��
		 {
			  if(ProgramRuningFlag(0) == 0)
			  {
				   f(*row);					//�ı��i�б���ɫ
				   err = ProgramStepRun(row,0);//ִ�е�i�г���
				   if((err>=ERR_CMD)&&(err!=ERR_STOP))
				   {
					   G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//ʹ�ð�ȫ����
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
				   if(*row>=num) *row=0;		//����ͷ�����¿�ʼ
			  }
			  else if(ProgramRuningFlag(0) == 2)//�˳�����
			  {
				  CloseFan();
				  ClearStopProgramFlag();
				  StopCpuTimer0();
				 // SetButtonValue(6,2,0);//�������һ�£�������ĸ�����
				  G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//ʹ�ð�ȫ����
				  return 0;
			  }
			  else//��ͣ״̬
			  {
				  if(ErrCheck(6))
				  {
					  G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//ʹ�ð�ȫ����
					  ClearStopProgramFlag();//�����ͣ��־λ
					  StopCpuTimer0();
					  return ERR_ALARM;
				  }
				  delay_1ms(100);//�޶���//2018/06/01��ǰֵΪ1��
				  KeyBoard_StopSpeed();
				  delay_1ms(100);//�޶���//2018/06/01��ǰֵΪ2��
				  Pause_network_function();//������//2018/06/01
			  }
		 }
	 }
	 G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//ʹ�ð�ȫ����
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
		SpiSendNetdata(5,Continue_OK,0);//��STM32���ͻ�е�ּ�����������
	}
	else if(Frame_Value==5)
	{
		net_control_function(18,0);
		SpiSendNetdata(5,Stop_Ok,0);//��STM32���ͻ�е��ֹͣ��������
	}
}
/*****************************************
 * �������ܣ�����ȫ������
 * ���������none
 * ����ֵ��	 none
 */
void ProgramRun(void)
{
	//
}

/*****************************************
 * �������ܣ���ʼ����
 * ���������row  �ӵ�row�п�ʼ
 * ����ֵ��
 */
/*void StartRun(void)
{
	 s_run_flag = 0;
}*/
/*****************************************
 * �������ܣ�ֹͣ����
 * ���������none
 * ����ֵ:	 none
 */
/*void StopRun(void)
{
	 s_run_flag = 1;
}*/
/*****************************************
 * �������ܣ���ͣ����
 * ���������
 * ����ֵ
 */
/*void PuaseRun(void)
{
	 s_run_flag = 2;
}*/
/*****************************************
 * �������ܣ�ִ�� gotoָ��
 * ���������PrgCode���͵� ����
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
	 current_lines = GetLinesCurrentProgram();//��ǰ����������
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
 * �������ܣ�ִ�� gosubָ��
 * ���������PrgCode���͵� ����
 * 			 u16* row �У�������ȥ����
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
	 current_lines = GetLinesCurrentProgram();//��ǰ����������
	 for(i=0;i<current_lines;i++)
	 {
		 GetRowCode(i,code_strings);
		 if(code_strings[0]=='#')
		 {
			 if(!StringCmp(COLUMNS_IN_LINE,code_strings,str_tmp))
			 {

				 if(s_return_stack_pointer==NULL)//����ǿ�ָ��
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
				 *s_return_stack_pointer = *row + 1;//ѹջ
				 *row = i;
				 break;
			 }
		 }
	 }
	 return 0;
}
/*****************************************
 * �������ܣ�ִ�� return ָ��
 * ���������PrgCode���͵� ����
 */
void ExecuteReturn(PrgCode codeStrings,unsigned int* row)
{
	if(s_return_stack_pointer==NULL)//����ǿ�ָ��
	{
		*row = 0;
	}
	else
	{
		*row = *s_return_stack_pointer;
		if(s_return_stack_pointer == s_return_stack)//��������˶�ջ�ĵײ�
		{
			s_return_stack_pointer = NULL;//��Ϊ��ָ��
		}
		else
		{
			s_return_stack_pointer--;
		}
	}
}
/*****************************************
 * �������ܣ����return�Ķ�ջ
 */
void ClearReturnStack(void)
{
	s_return_stack_pointer = NULL;//��Ϊ��ָ��
}
/*****************************************
 * �������ܣ�ִ����ʱ���� dly x.x (��)
 * ���������PrgCode���͵� ����
 * ����ֵ��
 * 			0����������
 * 			����ֵ�����ĳ������
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
 * �������ܣ�ִ�н����������� end
 * ���������PrgCode���͵� ����
 */
void ExecuteEnd(PrgCode codeStrings)
{
	ProgramRuningFlag(3);//ֹͣȫ������
}

/*****************************************
 * �������ܣ�ִ�н����������� finish
 * ���������PrgCode���͵� ����
 */
void ExecuteFinish(PrgCode codeStrings)
{
/*
	unsigned char i;
	unsigned short t;


	 for(t=0;t<3577;t++)
	 {

		 while(ReadDriState())//�ȴ���������
		 {
			  //delay_200ns();
			  NOP;NOP;
		 }

		 for(i=0;i<6;i++)
		 {
			 number[i]= pulse_num[t][i];
		 }
		 TranslationProccess(number,1);//ƽ�����ݴ��������������//7000
	 }
*/
	 //ProgramRuningFlag(3);//ֹͣȫ������
}

/*****************************************
 * �������ܣ�ִ����ͣ�������� stop
 * ���������PrgCode���͵� ����
 */
void ExecuteStop(PrgCode codeStrings)
{
	ProgramRuningFlag(2);//ֹͣȫ������
	SetButtonValue(6,2,1);//���������С�
}
/*****************************************
 * �������ܣ�ִ�йؽڲ岹���� mov
 * ���������PrgCode���͵� ����
 * 			teach��0->ȫ��	1->ʾ��
 * ����ֵ��	 0���ɹ�	 ����������
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
				 || (codeStrings.codeStrings[i] == 'n') )//pxx�����
		 {
			 if(codeStrings.codeStrings[i] == 'n')//pnx�����
			 {
				 i++;
				 n_num = StringToUint((unsigned char*)&codeStrings);
				 p_num = s_Nx_variable[n_num];
				 if(p_num >= MAX_POSITION_NUM) return ERR_P_OVERFLOW;
				 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//Pλ��ֵ����Ϊ��
			 }
			 else
			 {
				 p_num = StringToUint((unsigned char*)&codeStrings);
			 }

			 err = GetPxxPulseValue(p_num,next_pulse);
			 PulseToAngle(next_pulse,angle);
			 while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//��������
			 {
				 i++;
			 }
			 while(codeStrings.codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
		 }
		 else
		 {
			 //ReadCurrentPulseEp(next_pulse);
			 ReadCurrentPulseLp(next_pulse);//������//2018/06/01
		 }

		 if(codeStrings.codeStrings[i] == '.')
		 {
			 i++;	//����С����
			 if(codeStrings.codeStrings[i] == 'j')//.jx
			 {
				 i++;//����j
				 i_value = StringToUint(&codeStrings.codeStrings[i]);//��ȡ��ת��
				 i++;//����x
				 while(codeStrings.codeStrings[i] == ' ')//�����ո�
				 {
					 i++;
				 }
				 if(codeStrings.codeStrings[i] == '+')
				 {
					 f_value = StringToFloat(&codeStrings.codeStrings[i]);//��ȡ��ת�ĽǶ�
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
				 p_g = GetPositionGesture(angle);//�������
				 if(codeStrings.codeStrings[i] == 'x')
				 {
					 i++;	//����x
					 while(codeStrings.codeStrings[i] == ' ')//�����ո�
					 {
						 i++;
					 }
					 if(codeStrings.codeStrings[i++] == '+')	//˳������+/-
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
					 while(codeStrings.codeStrings[i] == ' ')//�����ո�
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
					 while(codeStrings.codeStrings[i] == ' ')//�����ո�
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
				 ModifyPositionGesture(p_g);//��������
				 if(GetBestSolution(angle))//������
					 return ERR_OUT_OF_RANGE;//����������Χ
				 AngleToPulse(angle,next_pulse);
			 }

			 while(codeStrings.codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 while((codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9') ||
					 (codeStrings.codeStrings[i]=='.'))//�������ֻ�С����
			 {
			 	 i++;
			 }
			 while(codeStrings.codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//�����speed
			 {
				 speed = atoi(((char*)&codeStrings)+i);
				 if(speed>0 && speed<=100)
				 {
					 if(teach == 0)//ȫ������ʱ��ִ�У�����ʾ�̲�ִ��
					 {
						 ModifySpeed(speed);
					 }
				 }
			 }
		 }
		 else
		 {
			 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//�����speed
			 {
				 speed = atoi(((char*)&codeStrings)+i);
				 if(speed>0 && speed<=100)
				 {
					 if(teach == 0)//ȫ������ʱ��ִ�У�����ʾ�̲�ִ��
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
		  THE_FOUR_AIXS_XUANZHUAN = 0;//��������ת��־Ϊ0
		  if(err1 == 0)//�ؽڲ岹�˶���Ŀ��λ��
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
 * �������ܣ�ִ��ֱ�߲岹���� Mvs p0.x + 10
 * ���������PrgCode���͵� ����
 * ����ֵ��0���ɹ�	����ֵ��ʧ��
 */
#pragma CODE_SECTION(ExecuteMvs, "ramfuncs");
unsigned char ExecuteMvs(PrgCode codeStrings,char teach)
{
	 unsigned char i=0,p_num,err=0,err1,n_num;
	 unsigned char flg=0;//��¼�Ƿ���Ҫ����
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
			 if(codeStrings.codeStrings[i] == 'n')//pnx�����
			 {
				 i++;
				 n_num = StringToUint((unsigned char*)&codeStrings);
				 p_num = s_Nx_variable[n_num];
				 if(p_num >= MAX_POSITION_NUM) return ERR_P_OVERFLOW;
				 if(!CheckPositionState(p_num)) return ERR_P_NO_VALUE;//Pλ��ֵ����Ϊ��
			 }
			 else//pxx�����
			 {
				 p_num = StringToUint((unsigned char*)&codeStrings);

			 }

			 err = GetPxxPulseValue(p_num,next_pulse);

			 while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//��������
			 {
				 i++;
			 }
			 while(codeStrings.codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
		 }
		 else//������p����� mvs p.x+n
		 {
			 //ReadCurrentPulseEp(next_pulse);
			 ReadCurrentPulseLp(next_pulse);//������//2018/06/01
		 }

		 PulseToAngle(next_pulse,angle);
		 p_g = GetPositionGesture(angle);//�������

		 if(codeStrings.codeStrings[i] == '.')
		 {
			 i++;	//����С����
			 f_value = StringToFloat(&codeStrings.codeStrings[i]);

			 if(codeStrings.codeStrings[i] == 'x')
			 {
				 i++;
				 while(codeStrings.codeStrings[i] == ' ')//�����ո�
				 {
					 i++;
				 }
				 if(codeStrings.codeStrings[i++] == '+')//����+/-
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
				 while(codeStrings.codeStrings[i] == ' ')//�����ո�
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
				 while(codeStrings.codeStrings[i] == ' ')//�����ո�
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
			 ModifyPositionGesture(p_g);	//��������
			 if(GetBestSolution(angle))		//������
				 return ERR_OUT_OF_RANGE;//����������Χ
			 AngleToPulse(angle,next_pulse);

			 while(codeStrings.codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 while((codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9') ||
					 (codeStrings.codeStrings[i]=='.'))//�������ֻ�С����
			 {
			 	 i++;
			 }
			 while(codeStrings.codeStrings[i] == ' ')//�����ո�
			 {
				 i++;
			 }
			 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//�����speed
			 {
				 speed = atoi(((char*)&codeStrings)+i);
				 if(speed>0 && speed<=100)
				 {
					 if(teach == 0)//ȫ������ʱ��ִ�У�����ʾ�̲�ִ��
					 {
						 ModifySpeed(speed);
					 }
				 }
			 }
		 }
		 else
		 {
			 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//�����speed
			 {
				 speed = atoi(((char*)&codeStrings)+i);
				 if(speed>0 && speed<=100)
				 {
					 if(teach == 0)//ȫ������ʱ��ִ�У�����ʾ�̲�ִ��
					 {
						 ModifySpeed(speed);
					 }
				 }
			 }
		 }
		 flg = 1;	//��¼ֱ�߲岹����Ҫ�ؽڲ岹����
	 }
	 else//'t'
	 {
		 err = ExecuteTx(((unsigned char*)&codeStrings)+i,next_pulse);
		 flg = 1;	//��¼ֱ�߲岹����Ҫ�ؽڲ岹����
	 }

	 if(!err)	//��һ������λ��
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
			  return ERR_OUT_OF_RANGE;	//����������Χ
		  }
		  else if(err1 == 6)
		  {
			  return ERR_SINGULARITY;
		  }
//		  else if(err1 == 5)
//		  {
//			  return ERR_GUSTURE;	//��̬��һ��
//		  }

		  if(err1==0)	//ֱ�߲岹�ɹ�
		  {
			  if(flg == 1)//�õ�ǰλ����Ϊ�ο������¼������Ž�
			  {
				  //ReadCurrentPulseEp(current_pulse);	//����ǰ����λ��
				  ReadCurrentPulseLp(current_pulse);       //������//2018/06/01
				  PulseToAngle(current_pulse,angle);	//����ת������
				  update_sin_cos_data(angle);			//���¿�
				  ModifyPositionGesture(p_g);			//��������
				  if(GetBestSolution(angle))				//������
					  return ERR_OUT_OF_RANGE;//����������Χ
				  AngleToPulse(angle,next_pulse);		//�Ƕ�ת����
			  }
		  }

		  err1 = JointInterpolation(next_pulse);//��ؽڲ岹,����
		  if(err1 == 0)	//�ؽڲ岹�˶���Ŀ��λ��
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
		  return err;//P��λ�ò�����
	 }
}

/*****************************************
 * �������ܣ�ִ��Բ���岹���� mvr
 * ���������PrgCode���͵� ����
 * ���������  0x00����ȷ
 * 			 0x01����е�ֶ��������з�������
 * 			 0x02����е�ֶ����������ɿ��˰�ȫ���أ�ʾ��ʱ��
 * 			 0x03�����������а�����stop��ť
 * 			 0x04: ����������Χ
 * 			 0x05: ������;
 * 			 0x06: �����
 * 			 0x10������ִ��
 * 			 0x20����ʼ����м���غ�
 * 			 0x30����ʼ����յ��غ�
 * 			 0x40���м����յ��غ�
 * 			 0x50��������ͬһֱ��
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
		i++;//����p
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

	while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//��������
	{
	 i++;
	}
	while(codeStrings.codeStrings[i] == ' ')//�����ո�
	{
	 i++;
	}

	 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//�����speed
	 {
		 speed = atoi(((char*)&codeStrings)+i);
		 if(speed>0 && speed<=100)
		 {
			 if(teach == 0)//ȫ������ʱ��ִ�У�����ʾ�̲�ִ��
			 {
				 ModifySpeed(speed);
			 }
		 }
	 }

	err = drive_arc(p_g,0x02)<<4;//Բ������ͷ��ʼ
	//err |= BaseCoordSlineMinPrecisionRun();

	if(err) return err;
	while(1)
	{
		err = drive_arc(p_g,0x0)<<4;//Բ��������ִ��
		err |= BaseCoordSlineMinPrecisionRun();//����
		if(err)break;
	}
	  if(err&0x10)//�ؽڲ岹�˶���Ŀ��λ��
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
		  return ERR_SINGULARITY;//�����singularity
	  }
}

/*****************************************
 * �������ܣ�ִ����Բ�岹���� mvc
 * ���������PrgCode���͵� ����
 * ���������0x00����ȷ
 * 			 0x01����е�ֶ��������з�������
 * 			 0x02����е�ֶ����������ɿ��˰�ȫ���أ�ʾ��ʱ��
 * 			 0x03�����������а�����stop��ť
 * 			 0x04: ����������Χ
 * 			 0x05: ������;
 * 			 0x06: �����
 * 			 0x10������ִ��
 * 			 0x20����ʼ����м���غ�
 * 			 0x30����ʼ����յ��غ�
 * 			 0x40���м����յ��غ�
 * 			 0x50��������ͬһֱ��
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
		i++;//����p
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

	while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//��������
	{
	 i++;
	}
	while(codeStrings.codeStrings[i] == ' ')//�����ո�
	{
	 i++;
	}

	 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//�����speed
	 {
		 speed = atoi(((char*)&codeStrings)+i);
		 if(speed>0 && speed<=100)
		 {
			 if(teach == 0)//ȫ������ʱ��ִ�У�����ʾ�̲�ִ��
			 {
				 ModifySpeed(speed);
			 }
		 }
	 }

	err = drive_arc(p_g,0x03)<<4;//��Բ����ͷ��ʼ
	if(err) return err;
	while(1)
	{
		err = drive_arc(p_g,0x01)<<4;//��Բ������ִ��
		err |= BaseCoordSlineMinPrecisionRun();//����
		if(err)break;
	}
	  if(err&0x10)//�ؽڲ岹�˶���Ŀ��λ��
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
		  return ERR_SINGULARITY;//�����singularity
	  }
}
/*****************************************
 * �������ܣ�ִ������Բ�岹���� mvh
 * ���������PrgCode���͵� ����
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
	 i++;//����p
	 p_num = StringToUint(((unsigned char*)&codeStrings) + i);
	 while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//��������
	 {
		 i++;
	 }
	 while(codeStrings.codeStrings[i] == ' ')//�����ո�
	 {
		 i++;
	 }

	if((codeStrings.codeStrings[i] == '+')||(codeStrings.codeStrings[i]=='-'))
	{
		if(codeStrings.codeStrings[i]=='-')
		{
			orientation = 1;
		}else{
			orientation = 0;//Ĭ����0
		}
		i++;
		while(codeStrings.codeStrings[i] == ' ')//�����ո�
		{
			i++;
		}
	}

	 if(codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9')//�����speed
	 {
		 speed = atoi(((char*)&codeStrings)+i);
		 if(speed>0 && speed<=100)
		 {
			 if(teach == 0)//ȫ������ʱ��ִ�У�����ʾ�̲�ִ��
			 {
				 ModifySpeed(speed);
			 }
		 }
	 }

	 if(!GetPxxPulseValue(p_num,next_pulse))	//��һ������λ��
	 {

		  err1 = CircleInterpolation(next_pulse,orientation);
		  if(err1 == 3) return ERR_STOP;
		  else if(err1 == 4) return ERR_OUT_OF_RANGE;
		  else if(err1 == 6) return ERR_SINGULARITY;
		  else if(err1 == 7) return ERR_GUSTURE;
		  err1 = JointInterpolation(next_pulse);		//��ؽڲ岹,����
		  if(err1 == 0)//�ؽڲ岹�˶���Ŀ��λ��
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
		  return ERR_P_NO_VALUE;//P��λ�ò�����
	 }
}
/*********************************************
 * �������ܣ�ִ��Pxָ�� 	p0=p,p0=p1, p0=p1.x-20.1;
 * 						p0=t1 n;
 * 						p0 = (p1+)rsrd
 * 					  	P0.j2 = (-/+)y/fy +/- (-)z/fz;
 * ����ֵ��0���ɹ�	����ֵ��ʧ��
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
	 i++;//����p
	 p_num1 = StringToUint(&codeStrings.codeStrings[i]);//��ȡp����

	 while(codeStrings.codeStrings[i] != 'p' && codeStrings.codeStrings[i] != 't'&&
			codeStrings.codeStrings[i] != 'r'&& codeStrings.codeStrings[i] != 'j')
	 {
		 i++;
	 }

	 //P0.j2 = y/fy (- z/fz);
	 if(codeStrings.codeStrings[i] == 'j')
	 {
		 i++;	//����j
		 j_num = StringToUint(&codeStrings.codeStrings[i]);//��ȡj����
		 GetPxxPulseValue(p_num1,pulse);
		 while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//��������
		 {
			 i++;
		 }
		 while(codeStrings.codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 i++;	//���� =
		 while(codeStrings.codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if((codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9') ||
				 codeStrings.codeStrings[i]=='+' || codeStrings.codeStrings[i]=='-')// +/-y
		 {
			 f_value1 = atof((char*)(&codeStrings.codeStrings[i]));
			 while((codeStrings.codeStrings[i]>='0' && codeStrings.codeStrings[i]<='9') ||
				   codeStrings.codeStrings[i]=='+' || codeStrings.codeStrings[i]=='-' ||
				   codeStrings.codeStrings[i] == '.')//��������,+,-,.
			 {
				 i++;
			 }
		 }
		 else	//fy
		 {
			 i++;	//����f
			 f_num1 = StringToUint(&codeStrings.codeStrings[i]);//��ȡf����
			 f_value1 = s_Fx_variable[f_num1];
			 while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//��������
			 {
				 i++;
			 }

		 }
		 while(codeStrings.codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 if(codeStrings.codeStrings[i] == '\0')	//p0.j1 = x/fy
		 {
			 SetSingleAxisAngle(j_num,f_value1,pulse);
			 SavePositionData(p_num1,pulse);//����
			 return 0;
		 }
		 j = codeStrings.codeStrings[i];	//��¼�� +/-����

		 i++;	//��������
		 while(codeStrings.codeStrings[i] == ' ')//�����ո�
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
			 i++;	//����f
			 f_num2 = StringToUint(&codeStrings.codeStrings[i]);//��ȡf����
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
		 SavePositionData(p_num1,pulse);//����
		 return 0;
	 }else
	 if(codeStrings.codeStrings[i] == 'p')
	 {
		 i++;
		 while(codeStrings.codeStrings[i] == ' ')//�����ո�
		 {
			 i++;
		 }
		 //px = p or px = p +/- rsrd�����
		 if(codeStrings.codeStrings[i]=='\0' || codeStrings.codeStrings[i]=='+' ||
				 codeStrings.codeStrings[i]=='-')
		 {
			 if(codeStrings.codeStrings[i] == '+' || codeStrings.codeStrings[i]=='-')//px = p + rsrd
			 {
				 if(ROBOT_PARAMETER.HARDWARE_REV >= 8)	//��Э REV08�汾
				 {
					 if((RX_POSITION_STA == 0))
					 {
						 delay_1ms(20);
						 //ReadCurrentPulseEp(pulse);
						 ReadCurrentPulseLp(pulse);//������//2018/06/01
						 SavePositionData(p_num1,pulse);//����
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
							 ReadCurrentPulseLp(pulse);//������//2018/06/01
							 SavePositionData(p_num1,pulse);//����
							 return 0;
						 }
					 }
				 }
				 type = GetPosition_SCI(position);
				 //ReadCurrentPulseEp(pulse);	//��õ�ǰλ��
				 ReadCurrentPulseLp(pulse);//������//2018/06/01
				 PulseToAngle(pulse,angle);
				 p_g = GetPositionGesture(angle);//�������
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

				 ModifyPositionGesture(p_g);//��������
				 if(GetBestSolution(angle))	//������
					 return ERR_OUT_OF_RANGE;//����������Χ
				 AngleToPulse(angle,pulse);
				 SavePositionData(p_num1,pulse);//����
				 RX_POSITION_STA = 0;
				 return 0;
			 }
			 else	// if(codeStrings.codeStrings[i] == '\0')//px = p
			 {
				  //ReadCurrentPulseEp(pulse);	//��õ�ǰλ��
				  ReadCurrentPulseLp(pulse);//������//2018/06/01
				  SavePositionData(p_num1,pulse);//px = p
				  return 0;
			 }
		 }

		 if(codeStrings.codeStrings[i] == '.')
		 {
			 //ReadCurrentPulseEp(pulse);	//��õ�ǰλ��
			 ReadCurrentPulseLp(pulse);     //������//2018/06/01
			 //i++;	//���� .
		 }
		 else
		 {
			 //px = p(y)(.z + n)  or  px = py + rsrd
			 p_num2 = StringToUint(&codeStrings.codeStrings[i]);//��ȡp����
			 GetPxxPulseValue(p_num2,pulse);
			 while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//��������
			 {
			 	 i++;
			 }
		 }


		 while(codeStrings.codeStrings[i] == ' ')//�����ո�
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
			 if(ROBOT_PARAMETER.HARDWARE_REV == 8)	//��Э REV08�汾
			 {
				 if((RX_POSITION_STA == 0))
				 {
					 delay_1ms(20);
					 //ReadCurrentPulseEp(pulse);
					 ReadCurrentPulseLp(pulse);//������//2018/06/01
					 SavePositionData(p_num1,pulse);//����
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
						 ReadCurrentPulseLp(pulse);//������//2018/06/01
						 SavePositionData(p_num1,pulse);//����
						 return 0;
					 }
				 }
			 }
			 type = GetPosition_SCI(position);
			 PulseToAngle(pulse,angle);
			 p_g = GetPositionGesture(angle);//�������
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

			 ModifyPositionGesture(p_g);//��������
			 if(GetBestSolution(angle))	//������
			 	 return ERR_OUT_OF_RANGE;//����������Χ
			 AngleToPulse(angle,pulse);
			 SavePositionData(p_num1,pulse);//����
			 RX_POSITION_STA = 0;
			 return 0;
		 }
		 else if(codeStrings.codeStrings[i] == '.')//p0 = p(1).
		 {
			 i++;
			 if(codeStrings.codeStrings[i] == 'j')
			 {
				 i++;//����j
				 i_value = StringToUint(&codeStrings.codeStrings[i]);
				 i++;//����x
				 while(codeStrings.codeStrings[i] == ' ')//�����ո�
				 {
					 i++;
				 }
				 sign = codeStrings.codeStrings[i];
				 i++;	//��������
				 while(codeStrings.codeStrings[i] == ' ')//�����ո�
				 {
					 i++;
				 }
				 if(codeStrings.codeStrings[i] == 'f')
				 {
					 i++;	//����f
					 f_num1 = StringToUint(&codeStrings.codeStrings[i]);//��ȡf����
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
				 c =  codeStrings.codeStrings[i];	//����x,y,z
				 i++;
				 while(codeStrings.codeStrings[i] == ' ')//�����ո�
				 {
					 i++;
				 }
				 sign = codeStrings.codeStrings[i];	//����+��-��
				 i++;	//��������
				 while(codeStrings.codeStrings[i] == ' ')//�����ո�
				 {
					 i++;
				 }

				 if(codeStrings.codeStrings[i] == 'f')
				 {
					 i++;	//����f
					 f_num1 = StringToUint(&codeStrings.codeStrings[i]);//��ȡf����
					 f_value = s_Fx_variable[f_num1];
				 }
				 else
				 {
					 f_value = atof((char*)(&codeStrings.codeStrings[i]));
				 }

				 PulseToAngle(pulse,angle);
				 p_g = GetPositionGesture(angle);//�������

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
				 ModifyPositionGesture(p_g);//��������
				 if(GetBestSolution(angle))	//������
					 return ERR_OUT_OF_RANGE;//����������Χ
				 AngleToPulse(angle,pulse);
			 }
			 SavePositionData(p_num1,pulse);//����
			 return 0;
		 }
	 }
	 else if(codeStrings.codeStrings[i] == 't')//'t'
	 {
		 err = ExecuteTx(&codeStrings.codeStrings[i],pulse);
		 if(err == 0)
		 {
			 SavePositionData(p_num1,pulse);//����
		 }
		 return err;
	 }
	 else
	 {
		 i=0;

		 if(ROBOT_PARAMETER.MOTOR_TYPE == 2)	//��Э REV08�汾
		 {
			 if((RX_POSITION_STA == 0))
			 {
				 delay_1ms(20);
				 //ReadCurrentPulseEp(pulse);
				 ReadCurrentPulseLp(pulse);//������//2018/06/01
				 SavePositionData(p_num1,pulse);//����
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
					 ReadCurrentPulseLp(pulse);//������//2018/06/01
					 SavePositionData(p_num1,pulse);//����
					 return 0;
				 }
			 }
		 }

		 type = GetPosition_SCI(position);

		 if(type == 2)
		 {
			 GetPxxPulseValue(p_num1,pulse);
			 PulseToAngle(pulse,angle);
			 p_g = GetPositionGesture(angle);//�������
			 p_g.px = position[0];
			 p_g.py = position[1];
		 }
		 else if(type == 3)
		 {
			 GetPxxPulseValue(p_num1,pulse);
			 PulseToAngle(pulse,angle);
			 p_g = GetPositionGesture(angle);//�������
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
			 //ReadCurrentPulseEp(pulse);	//��õ�ǰλ��
			 ReadCurrentPulseLp(pulse);//������//2018/06/01
			 PulseToAngle(pulse,angle);
			 //p_g = GetPositionGesture(angle);//�������
			 update_sin_cos_data(angle);	//���¿�(�漰���Ž�)
	//xxx
	//		 ToolPointToEndPoint(*((PositionEuler*)position),&p_g,ToolPara.ToolLength);
		 }
		 else{
			 //ReadCurrentPulseEp(pulse);	//��õ�ǰλ��
			 ReadCurrentPulseLp(pulse);//������//2018/06/01
			 PulseToAngle(pulse,angle);
			 p_g = GetPositionGesture(angle);//�������
		 }

		 ModifyPositionGesture(p_g);//��������
		 if(GetBestSolution(angle))	//������
		 	 return ERR_OUT_OF_RANGE;//����������Χ
		 AngleToPulse(angle,pulse);
		 SavePositionData(p_num1,pulse);//����
		 RX_POSITION_STA = 0;
	 }
	 return 0;
}
/*****************************************
 * �������ܣ�ִ��ȫ���ٶ��趨���� ovrd
 * ���������PrgCode���͵� ����
 */
void ExecuteOvrd(PrgCode codeStrings)
{
	 char str[4];
	 unsigned int value;

	 value = StringToUint((unsigned char*)&codeStrings);
	 s_speed.ovrd = value;
	 ltoa(value,str);
	 SetTextValue(6, 19, str);//������ʾovrd

	 s_speed.all_speed = (float)s_speed.ovrd*s_speed.speed/100;
	 if(s_speed.all_speed <= 0)
	 {
		 s_speed.all_speed  = 1;
	 }
}

/*****************************************
 * �������ܣ�ִ�д򿪳���ָ��
 * ���������PrgCode���͵� ����
 * ����ֵ��0���򿪳ɹ�
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
	 while(codeStrings.codeStrings[i] == ' ')//�����ո�
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
 * �������ܣ�ִ�оֲ��ٶ����� speed
 * ���������PrgCode���͵� ����
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
 * ������ٶ�
 */
unsigned char GetProgramRunSpeed(void)
{
	 return s_speed.all_speed;
}
/******************
 * �޸����ٶ�
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
	 s_speed.all_speed = speed;//���ٶ�
}

/*****************************
 * �������ܣ����ȫ���ٶ�ovrd
 */
unsigned char GetOvrd(void)
{
	return s_speed.ovrd;
}

/******************
 * �޸�ȫ���ٶ�ovrd
 * ����ֵ�����ٶ�
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
 * �޸ľֲ��ٶ�speed
 * ����ֵ�����ٶ�
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
 * �������ܣ�ִ������IO����(Ӧ�����wait ָ��һ��ʹ��)
 * ���������PrgCode���͵� ����
 * ����ֵ:	 IO��״̬�������ж�����
 */
/*unsigned char ExecutePin(PrgCode codeStrings)
{
	 unsigned char pin_value=0,io_state;
	 unsigned char i=0;
	 pin_value = StringToUint((unsigned char*)&codeStrings);//������ĸ�IO��
	 while(codeStrings.codeStrings[i] != '=')//ֱ������'='������ѭ��
	 {
		  i++;
	 }

	 while((codeStrings.codeStrings[i]<'0') || (codeStrings.codeStrings[i]>'9'))//ֱ���������־�����ѭ��
	 {
		  i++;
	 }
	 io_state = codeStrings.codeStrings[i]-'0';
	 return io_state;
}*/
/*****************************************
 * �������ܣ�ִ�п���IO����
 * ���������PrgCode���͵� ����
 * ����ֵ��0->OK ����->����
 */
u8 ExecutePout(PrgCode codeStrings)
{
	 unsigned char pout_value=0,io_state;
	 unsigned char i=0,err;
	 pout_value = StringToUint((unsigned char*)&codeStrings);//������ĸ�IO��
	 while(codeStrings.codeStrings[i] != '=')//ֱ������'='������ѭ��
	 {
		  i++;
	 }

	 while((codeStrings.codeStrings[i]<'0') || (codeStrings.codeStrings[i]>'9'))//ֱ���������־�����ѭ��
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
 * �������ܣ�ִ��wait����(waitָ���Ӧ�ý�pinָ��һ��ʹ�ã�
 * 			 ��wait pin=1������Ϊ���ȴ�ֱ������IO1�ű�Ϊ��Ч�����е�ѹ����)
 * ���������PrgCode���͵� ����
 * ����ֵ��
 * 			0����������
 * 			����ֵ�����ĳ������
 */
unsigned char ExecuteWait(PrgCode codeStrings)
{
	unsigned char pin_value=0,sta,err=0;
	unsigned char i=0;
	u32 io_state;
	pin_value = StringToUint((unsigned char*)&codeStrings);//������ĸ�IO��,����IO��Χ0~11
	while(codeStrings.codeStrings[i] != '=')//ֱ������'='������ѭ��
	{
	  i++;
	}
	while((codeStrings.codeStrings[i]<'0') || (codeStrings.codeStrings[i]>'9'))//ֱ���������־�����ѭ��
	{
	  i++;
	}
	sta = codeStrings.codeStrings[i]-'0';//�����Ҫ�жϵ�IO״̬

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
 * �������ܣ���������   �÷���ft1 p0
 */
void ExecuteFtx(PrgCode codeStrings)
{
	unsigned int t_num,p_num;
	int i=0;
	long pulse[6];
	double angle[6];
	struct ThreeDimCoordStru p_g;
	PositionGesture p_g_fix;

	t_num = StringToUint((unsigned char*)&codeStrings);//��ȡ���̱��
	while(codeStrings.codeStrings[i++] != 'p');//ֱ������'p'������ѭ��
	p_num = StringToUint((unsigned char*)&codeStrings+i);//��ȡP
	GetPxxPulseValue(p_num,pulse);//�����������
	PulseToAngle(pulse,angle);	//������ת��Ϊ�Ƕ�
	p_g_fix = GetPositionGesture(angle);//���λ��

	//��s_tray[tray_num].y_num�����λ��
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
 * �������ܣ�ִ��trayָ�� ��ʼ������   �÷���t1 p0p1p2 10 11
 * 			 ע�⣺p0��p2����Ϊ�Խ�
 */
void ExecuteTray(PrgCode codeStrings)
{
	unsigned char i=0,j;
	unsigned char p_num[3],tray_num;
	unsigned int x_num,y_num;
	double width,length;//���̵ĳ���
	long pulse[3][6];
	double angle[3][6];

	PositionGesture p_g[3];//λ��
	tray_num = StringToUint((unsigned char*)&codeStrings);//��ȡ���̱��
	for(j=0;j<3;j++)
	{
		while(codeStrings.codeStrings[i++] != 'p');//ֱ������'p'������ѭ��

		p_num[j] = StringToUint(((unsigned char*)&codeStrings)+i);//��ȡ3��pλ��
		GetPxxPulseValue(p_num[j],pulse[j]);//�����������
		PulseToAngle(pulse[j],angle[j]);	//������ת��Ϊ�Ƕ�
		p_g[j] = GetPositionGesture(angle[j]);//���λ��
	}
	for(j=0;j<6;j++)
	{
		s_tray[tray_num].angle[j] = angle[0][j];	//�����һ����ĽǶ�
	}

	s_tray[tray_num].pos_ges = p_g[0];//������ʼ��
	while(codeStrings.codeStrings[i++] != ' ');//ֱ������' '������ѭ��
	x_num = StringToUint(((unsigned char*)&codeStrings)+i);//��ȡx(����Ŀ����x��)
	while(codeStrings.codeStrings[i++] != ' ');//ֱ������' '������ѭ��
	y_num = StringToUint(((unsigned char*)&codeStrings)+i);//��ȡy(����ĳ������y��)
	width = sqrt((p_g[0].px - p_g[1].px)*(p_g[0].px - p_g[1].px) +
					 (p_g[0].py - p_g[1].py)*(p_g[0].py - p_g[1].py) +
					 (p_g[0].pz - p_g[1].pz)*(p_g[0].pz - p_g[1].pz));
	length = sqrt((p_g[1].px - p_g[2].px)*(p_g[1].px - p_g[2].px) +
					 (p_g[1].py - p_g[2].py)*(p_g[1].py - p_g[2].py) +
					 (p_g[1].pz - p_g[2].pz)*(p_g[1].pz - p_g[2].pz));
	if(x_num==1)//��ֹ���ֳ���0�����
	{
		s_tray[tray_num].width = 0;
	}
	else{
		s_tray[tray_num].width = width/(x_num-1);//��ֳ�x_num-1��(10����ֻ��ֳ�9�ݾ���10���㣬���Լ�1)
	}
	if(y_num==1)//��ֹ���ֳ���0�����
	{
		s_tray[tray_num].length = 0;
	}
	else{
		s_tray[tray_num].length = length/(y_num-1);//���ֳ�y_num��
	}
	s_tray[tray_num].x_num = x_num;//���̵Ŀ�����x_num��
	s_tray[tray_num].y_num = y_num;//���̵ĳ�������y_num��

	s_tray[tray_num].unit_vector_x.x = (p_g[1].px - p_g[0].px)/width;//��ĵ�λ������x
	s_tray[tray_num].unit_vector_x.y = (p_g[1].py - p_g[0].py)/width;//��ĵ�λ������y
	s_tray[tray_num].unit_vector_x.z = (p_g[1].pz - p_g[0].pz)/width;//��ĵ�λ������z

	s_tray[tray_num].unit_vector_y.x = (p_g[2].px - p_g[1].px)/length;//���ĵ�λ������x
	s_tray[tray_num].unit_vector_y.y = (p_g[2].py - p_g[1].py)/length;
	s_tray[tray_num].unit_vector_y.z = (p_g[2].pz - p_g[1].pz)/length;

	s_tray[tray_num].fix_x = 0;
	s_tray[tray_num].fix_y = 0;
	s_tray[tray_num].fix_z = 0;

	s_tray[tray_num].def_flag = 1;//��Ǵ�����Ϊ�ѱ�����
}

/********************************************
 * �������ܣ�ִ��txָ�� ������������  ���磺mov t1 10�е�t1 10
 * 			 ע�⣺p0��p2����Ϊ�Խ�
 * ����ֵ��0���ɹ�	����ֵ������
 */
unsigned char ExecuteTx(unsigned char* strings,long* pulse)
{
	unsigned char i=0,k,tray_num,n_num;
	unsigned long num;//�����ܸ���
	unsigned int x_num,y_num;
	int speed;
	double angle[6];
	struct ThreeDimCoordStru fix;
	PositionGesture next_p_g;

	while(*(strings+i) != 't')//ֱ������'t'������ѭ��
	{
		i++;
	}
	i++;//����t
	tray_num = StringToUint(strings+i);//��ȡ���̱��
	if(tray_num>MAX_TRAY_NUM) return ERR_CMD;
	if(!GetTrayDefState(tray_num)) return ERR_TRAY_UNDEFINE;//δ���������

	 while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
	 {
		 i++;
	 }
	while(*(strings+i) == ' ')//�����ո�
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
		num = StringToUint(strings+i);//��ȡ���ӱ��(ֵ��1��ʼ��)
	}
	 while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
	 {
		 i++;
	 }

	if(num==0) return ERR_TRAY_GRID_ZERO;
	if(num>(s_tray[tray_num].x_num*s_tray[tray_num].y_num)) return ERR_TRAY_GRID_OVERFLOW;//������������

	 while(*(strings+i) == ' ')//�����ո�
	 {
		 i++;
	 }
	 if(*(strings+i)>='0' && *(strings+i)<='9')//�����speed
	 {
		 speed = atoi((char*)strings+i);
		 if(speed>0 && speed<=100)
		 {
			 ModifySpeed(speed);
		 }
	 }

	num = num -1;//�����ʱ����ӱ���Ǵ�0��ʼ��
	/*if(tray_num<=9)
	{
		x_num = num/s_tray[tray_num].y_num;	//�ڼ���
		if(x_num%2 == 0)
		{
			y_num = num%s_tray[tray_num].y_num;	//�ڼ���
		}
		else
		{
			y_num = s_tray[tray_num].y_num-1-num%s_tray[tray_num].y_num;	//�ڼ���
		}
	}
	else
	{*/
		y_num = num%s_tray[tray_num].y_num;	//�ڼ���
		x_num = num/s_tray[tray_num].y_num;	//�ڼ���
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
		ModifyPositionGesture(next_p_g);//����һ����̬
		if(GetBestSolution(angle))	//������
			return ERR_OUT_OF_RANGE;//����������Χ
	}
	fix.x = (s_tray[tray_num].fix_x/(s_tray[tray_num].y_num-1) *	//�������ȣ�ÿһ�ݵĳ���
			y_num /	//��Ҫ�޲����ٷ�
			(s_tray[tray_num].x_num-1) *	//ƽ̯
			(s_tray[tray_num].x_num-x_num-1) );//�ۻ�

	fix.y = (s_tray[tray_num].fix_y/(s_tray[tray_num].y_num-1))*//�������ȣ�ÿһ�ݵĳ���
			y_num /		//��Ҫ�޲����ٷ�
			(s_tray[tray_num].x_num-1) *		//ƽ̯
			(s_tray[tray_num].x_num-x_num-1);	//�ۻ�

	fix.z = (s_tray[tray_num].fix_z/(s_tray[tray_num].y_num-1))*//�������ȣ�ÿһ�ݵĳ���
			y_num /		//��Ҫ�޲����ٷ�
			(s_tray[tray_num].x_num-1) *		//ƽ̯
			(s_tray[tray_num].x_num-x_num-1);	//�ۻ�

	for(k=0;k<=y_num;k++)
	{
		update_sin_cos_data(angle);
		next_p_g.px = s_tray[tray_num].pos_ges.px + (x_num*s_tray[tray_num].width*s_tray[tray_num].unit_vector_x.x) +
				(k*s_tray[tray_num].length*s_tray[tray_num].unit_vector_y.x) + fix.x;

		next_p_g.py = s_tray[tray_num].pos_ges.py + (x_num*s_tray[tray_num].width*s_tray[tray_num].unit_vector_x.y) +
				(k*s_tray[tray_num].length*s_tray[tray_num].unit_vector_y.y) + fix.y;

		next_p_g.pz = s_tray[tray_num].pos_ges.pz + (x_num*s_tray[tray_num].width*s_tray[tray_num].unit_vector_x.z) +
				(k*s_tray[tray_num].length*s_tray[tray_num].unit_vector_y.z) + fix.z;

		ModifyPositionGesture(next_p_g);//����һ����̬
		if(GetBestSolution(angle))	//������
			return ERR_OUT_OF_RANGE;//����������Χ
	}
	if(GetBestSolution(angle))	//������
		return ERR_OUT_OF_RANGE;//����������Χ
	AngleToPulse(angle,pulse);
	return 0;
}

/********************************************
 * �������ܣ�ִ��Nxָ�� ��������  ���磺nx++;nx--;//nx = ny;nx = z;nx = ny - z;
 * 				(mov t1 n0;Px = tx nx;if n0>100 )
 * 			 ����ֵ��n_value ������ֵ
 * ����ֵ��0���ɹ�	����ֵ������
 */
unsigned char ExecuteNx(unsigned char* strings,long* nx)
{
	unsigned char i=0,n_num,n_num1,n_num2;
	long value;

	while(*(strings+i) != 'n')//ֱ������'n'������ѭ��
	{
		i++;
	}
	i++;//����n
	n_num = StringToUint(strings+i);//��ȡn����

	while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
	{
		i++;
	}
	while(*(strings+i) == ' ')//�����ո�
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
		while(*(strings+i) == ' ')//�����ո�
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
			while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
			{
				i++;
			}
			while(*(strings+i) == ' ')//�����ո�
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
				while(*(strings+i) == ' ')//�����ո�
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
				while(*(strings+i) == ' ')//�����ո�
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
				while(*(strings+i) == ' ')//�����ո�
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
				while(*(strings+i) == ' ')//�����ո�
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
				while(*(strings+i) == ' ')//�����ո�
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
 * �������ܣ�ִ��Fxָ�� �����ͱ���  ���磺
 * 			fx = fy;fx = +/-z;fx = fy - +/-z;fx = fy - fz;f0 = j1;
 * 			(Px.j2 = y/fy (- z/fz); )
 * ���������strings:ָ���ַ���ָ��
 * 			fx: double����ָ�룬����fx��ֵ
 * ����ֵ��0���ɹ�	����ֵ������
 */
unsigned char ExecuteFx(unsigned char* strings,double* fx)
{
	unsigned char i=0,f_num,f_num1,f_num2;
	double f_value1;

	while(*(strings+i) != 'f')//ֱ������'n'������ѭ��
	{
		i++;
	}
	i++;//����f
	f_num = StringToUint(strings+i);//��ȡf����

	while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
	{
		i++;
	}
	while(*(strings+i) == ' ')//�����ո�
	{
	  i++;
	}

	//fx = +/-z;
	//fx = j1;
	//fx = fy;fx = fy - +/-z;fx = fy - fz;
	if(*(strings+i) == '=')
	{
		i++;
		while(*(strings+i) == ' ')//�����ո�
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
			i++;	//����j
			f_num1 = atol((char*)(strings+i));	//����ĸ���

			double angle[6];
			GetCurrentAngle(angle);//���ÿ�ᵱǰ�Ƕ�
			 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)	//ת������
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
			i++;	//����f
			f_num1 = StringToUint(strings+i);
			while(*(strings+i) >= '0' && *(strings+i) <= '9')//��������
			{
				i++;
			}
			while(*(strings+i) == ' ')//�����ո�
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
				while(*(strings+i) == ' ')//�����ո�
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
				while(*(strings+i) == ' ')//�����ո�
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
				while(*(strings+i) == ' ')//�����ո�
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
				while(*(strings+i) == ' ')//�����ո�
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
 * �������ܣ�ִ��Ifָ�� Nx/x ��>��<��= ��Nx/x��Nx x xx��
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
	while(codeStrings.codeStrings[i] != 'f')//ֱ������'f'������ѭ��
	{
		i++;
	}
	i++;
	while(codeStrings.codeStrings[i] == ' ')//�����ո�
	{
	  i++;
	}

	if(codeStrings.codeStrings[i] == 'n')//�����N����
	{
		i++;
		n_num1 = StringToUint(&codeStrings.codeStrings[i]);//��ȡn����
		value1 = s_Nx_variable[n_num1];
	}
	else if(codeStrings.codeStrings[i] == 'p')//���������IO
	{
		pin_value = StringToUint(&codeStrings.codeStrings[i]);//��ȡ����IO
		i = i+3;//����pin
		value1 = ReadPinState()&((long)0x01<<pin_value);
		value1 = value1>>pin_value;//���IO״̬
	}
	else if(codeStrings.codeStrings[i] == 'r')//�����rsrd�ⲿ���ݽ��ձ�־λ
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
				return ERR_STM32COMM;	//ͨ�ų���
			}
		}
		else if(ROBOT_PARAMETER.HARDWARE_REV == 7)
		{
			return ERR_NO_RS232;	//07��û��RS232
		}
		value1 = RX_POSITION_STA;
		i=i+4;
	}
	else//��ֵ
	{
		value1 = StringToUint(&codeStrings.codeStrings[i]);
	}

	while(codeStrings.codeStrings[i] >= '0' && codeStrings.codeStrings[i] <= '9')//��������
	{
		i++;
	}
	while(codeStrings.codeStrings[i] == ' ')//�����ո�
	{
	  i++;
	}
	flg = codeStrings.codeStrings[i];//��¼�� <��> �� =
	i++;
	if(codeStrings.codeStrings[i] == '=')
	{
		flg += '=';//'<'==60;'='==61;'>'==62;
		i++;
	}
	while(codeStrings.codeStrings[i] == ' ')//�����ո�
	{
	  i++;
	}
	if(codeStrings.codeStrings[i] == 'n')
	{
		n_num2 = StringToUint(&codeStrings.codeStrings[i]);//��ȡn����
		value2 = s_Nx_variable[n_num2];
	}
	else if(codeStrings.codeStrings[i] == 'p')//���������IO
	{
		pin_value = StringToUint(&codeStrings.codeStrings[i]);//��ȡ����IO
		//i = i+3;//����pin
		value2 = ReadPinState()&((long)0x01<<pin_value);
		value2 = value2>>pin_value;//���IO״̬
	}
	else if(codeStrings.codeStrings[i] == 'r')//�����rsrd�ⲿ���ݽ��ձ�־λ
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
				return ERR_STM32COMM;	//ͨ�ų���
			}
		}
		else if(ROBOT_PARAMETER.HARDWARE_REV == 7)
		{
			return ERR_NO_RS232;	//07��û��RS232
		}
		value2 = RX_POSITION_STA;
	}
	else
	{
		value2 = StringToUint(&codeStrings.codeStrings[i]);//��ȡֵ
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
 * �������ܣ�ִ��thenָ��
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
	 current_lines = GetLinesCurrentProgram();//��ǰ����������

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

					 if(s_return_stack_pointer==NULL)//����ǿ�ָ��
					 {
						 s_return_stack_pointer = s_return_stack;
					 }
					 else
					 {
						 s_return_stack_pointer++;
					 }
					 *s_return_stack_pointer = *row + 2;//�����У�����then��else
					 *row = i;
				}
			}
		}
	 }
}

/*******************************************
 * �������ܣ�ִ��elseָ��
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
	 current_lines = GetLinesCurrentProgram();//��ǰ����������

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

					 if(s_return_stack_pointer==NULL)//����ǿ�ָ��
					 {
						 s_return_stack_pointer = s_return_stack;
					 }
					 else
					 {
						 s_return_stack_pointer++;
					 }
					 *s_return_stack_pointer = *row + 1;//�����У�����else
					 *row = i;
				}
			}
		}
	 }
}

/*******************************************
 * �������ܣ�ִ��accָ�� acc 1~100
 */
void ExecuteAcc(PrgCode codeStrings)
{
	 s_ACC = StringToUint((unsigned char*)&codeStrings);
	 if(s_ACC >=100)
	 {
		 s_ACC = 100;
		 SetAcc(0x3f,8000);      //���ٶ�  A*125	 1~8000
	 }
	 else
	 {
		 //SetStartV(0x3F,(unsigned int)speed*6);      //��ʼ�ٶ�   ��Χ��1~8000
		 //SetAcac(0x3f,80*value);      //���ٶȱ仯�� 1~65535  (62.5*10^6)/K*(8000000/R)
		 SetAcc(0x3f,10*s_ACC);      //���ٶ�  A*125	 1~8000
	 }
}

/*****************************
 * �������ܣ���õ�ǰ���ٶ�
 */
unsigned int GetAcc(void)
{
	return s_ACC;
}

/*****************************
 * �������ܣ��������ô��ڲ�����
 * ����ֵ:0->�ɹ�	1->ʧ��
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
	while(codeStrings.codeStrings[i] == ' ')//�����ո�
	{
		i++;
	}
	if(ROBOT_PARAMETER.MOTOR_TYPE == 2)
	{
		str[0] = 0;
		strcpy(str,(char*)&codeStrings.codeStrings[i]);
		strcat(str,"\r\n");	//�Զ���ӻس�
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
		SendStringB((char*)&codeStrings.codeStrings[i]);//��������
		SendStringB("\r\n");//��������
		USART_RX_STA_B = 0;//����������֮�󣬰ѱ�־λ���㣬Ȼ�����½���
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
	line_num = atoi((char*)(codeStrings.codeStrings+i));	//����ڵڼ�����ʾ
	if(line_num<=0 || line_num>5) return ERR_DISP;
	i++;	//��������
	while(codeStrings.codeStrings[i] == ' ')//�����ո�
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
				sign = codeStrings.codeStrings[i+2];	//��¼n����f
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
				//��������
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
				//��n_num������ַ���������
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
	//��ʾ

	//ҳ��7��ʾ����successful����һ�п�ʼ��ʾ�������dispָ����ˢ�£��κΰ����ᱻˢ��?��
	SetTextValue(7, 87, str);

	//ȫ������ʱҳ��6��ʾdisp������
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
		if(i<5)//�����β���Ҫ��
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
	p_e.Rx = p_e.Rx*PI_DIVIDE_180;	//�Ƕ�ת��Ϊ����
	p_e.Ry = p_e.Ry*PI_DIVIDE_180;
	p_e.Rz = p_e.Rz*PI_DIVIDE_180;
	return p_e;
}


/********************************************
 * �������ܣ�ִ��rs232ָ�ִ�к�ͽ�����ѭ�����Ӵ��ڽ����������
 * 			 ֱ��ִ�н��������������ʱ�˳�
 * ����ֵ��0���ɹ�	����ֵ������
 */
unsigned char ExecuteRS232(PrgCode codeStrings)
{
	u8 i,t,err_t;
	//double angle[6];
	//long pulse[6];
	unsigned char tx_cmd[CMD_LEN-1],rx_cmd[CMD_LEN-1],code_err,err;
	//PositionEuler p_e;//2018/05/22ע�͵�
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


	UpdateCurrentPositionGesture();			//���µ�ǰλ����̬

//	unsigned char rx_cmd[5],tx_cmd[5],i,t=0;
	//tmp = GetSTM32USART_STA(data);
	memset(tx_cmd,0,sizeof(tx_cmd));
	tx_cmd[0] = CMD_RS232_BEGIN;	//��stm32ͨ�����ڷ����ַ���begin������
	SpiSendCmd(tx_cmd);	//��������
	//delay_1us(200);
	while(ProgramRuningFlag(0) == 0)
	{
		t=0;
		err_t = 0;
		//��ѯ�����Ƿ���
		while(1)
		{
			if(t>250)	//�ȵ���ʱ
			{
				return ERR_RS_TIMEOUT;
			}
			delay_1ms(2);
			tx_cmd[0] = R_CMD_USART_RX_STA;
			SpiSendCmd(tx_cmd);	//���Ͳ�ѯ����
			delay_1us(20);
			if(SpiReceiveCmd(rx_cmd) == 0)
			{
				if(rx_cmd[0] == R_CMD_USART_RX_OK)
				{
					data_len = 0;
					for(i=0;i<4;i++)
					{
						data_len |= (Uint32)rx_cmd[i+1]<<(i*8);//���ݳ���
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
			delay_1ms(100);//�ȴ�stm32�˳�����ģʽ
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
			code_err = CheckGrammar((PrgCode*)data);//����﷨
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
					err = BaseCoordSlineMinPrecisionRun();	//����Ҫ�� �ƶ�����̫��������£���̬���ܻᷢ���ı� xxx
					 while(ReadDriState())//�ȴ���������
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
									   err = 1;	//����
									   break;
								  }
							  }
						  }
						  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:����,ʹ�ð�ȫ���ص�ʱ���ɿ�(�ߵ�ƽ)���˳�
						  {
							  delay_1us(1);
							  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)
							  {
								  SetDriCmd(0x3F,0x27);
								  err = 2;	//��ȫ����
								  break;
							  }
						  }
						  if(KeyBoard_StopSpeed())
						  {
							   SetDriCmd(0x3F,0x26);	//����ֹͣ
							   err = 3;
							   break;
						  }
						  NOP;NOP;
					 }
					//if(err) return err;

					//AngleToPulse(angle,pulse);
					//err = JointInterpolation(pulse);
					if(err == 0)//�ؽڲ岹�˶���Ŀ��λ��
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
					    return ERR_SINGULARITY;//�����singularity
				    }
					else if(err == 3)
					{
						ProgramRuningFlag(2);	//��ͣ
						SetButtonValue(6,2,1);	//���������С�
						while(ProgramRuningFlag(0) == 1)	//��ͣ
						{
							if(ErrCheck(6))
							{
							  G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//ʹ�ð�ȫ����
							  ClearStopProgramFlag();//�����ͣ��־λ
							  //SPIA_RX_STA = 0;
							  return ERR_ALARM;
							}
							KeyBoard_StopSpeed();
						}
						if(ProgramRuningFlag(0)==2)	//�˳�
						{
							CloseFan();
							ClearStopProgramFlag();
							G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//ʹ�ð�ȫ����
							//SPIA_RX_STA = 0;
							return 0;
						}
						else	//����
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
				ProgramRuningFlag(2);	//��ͣ
				SetButtonValue(6,2,1);	//���������С�
				while(ProgramRuningFlag(0) == 1)	//��ͣ
				{
					if(ErrCheck(6))
					{
					  G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//ʹ�ð�ȫ����
					  ClearStopProgramFlag();//�����ͣ��־λ
					  //SPIA_RX_STA = 0;
					  return ERR_ALARM;
					}
					KeyBoard_StopSpeed();
				}
				if(ProgramRuningFlag(0)==2)	//�˳�
				{
					CloseFan();
					ClearStopProgramFlag();
					G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//ʹ�ð�ȫ����
					//SPIA_RX_STA = 0;
					return 0;
				}
				else	//����
				{
					ClearStopProgramFlag();
					err = 0;
				}
			}
			else if(code_err == code_end)
			{
				ProgramRuningFlag(3);	//�˳�ȫ������
				CloseFan();
				ClearStopProgramFlag();
				G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//ʹ�ð�ȫ����
				return 0;
			}
			else
			{
				err = ProgramStepRun_Code(code_err,((PrgCode*)data));
			}

			if(err >= ERR_CMD)
			{
				//memset(tx_cmd,0,sizeof(tx_cmd));
				//tx_cmd[0] = CMD_RS232_OVER;	//����ֹͣ
				//SpiSendCmd(tx_cmd);
				//SPIA_RX_STA = 0;
				break;
			}
			else
			{
				if(ProgramRuningFlag(0) == 0)
				{
					memset(tx_cmd,0,sizeof(tx_cmd));
					tx_cmd[0] = CMD_RS232_OK;	//ָ�����
					SpiSendCmd(tx_cmd);
				}
			}
			//SPIA_RX_STA = 0;
		}
	}
	//SPIA_RX_STA = 0;
	while(ReadDriState())//�ȴ���������
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
		  if(G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN && SAVE_BUTTON)//SAVE_BUTTON:����,ʹ�ð�ȫ���ص�ʱ���ɿ�(�ߵ�ƽ)���˳�
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
 * �������ܣ�ִ�� ExecuteNetworkָ��ϴ��ӹ������������
 * 		    ��λ���Ŀ���ָ��
 * ����ֵ��0���ɹ�	����ֵ������
 * //������//2018/06/01 for network
 */
void ExecuteNetwork(PrgCode codeStrings,unsigned int* row)
{
	unsigned int i=0,n_num1;
	unsigned int value1;
	long component_value=0;
	u8 Re_Data[8]={0,0,0,0};
	u8 Frame_value=0;
	while(codeStrings.codeStrings[i] != 'k')//ֱ������'k'������ѭ��
	{
		i++;
	}
	i++;
	while(codeStrings.codeStrings[i] == ' ')//�����ո�
	{
		i++;
	}

	if(codeStrings.codeStrings[i] == 'n')//�����N����
	{
		i++;
		n_num1 = StringToUint(&codeStrings.codeStrings[i]);//��ȡn����
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
	//���������
	SendComponentNum(value1);
	delay_1ms(3);
	SpiReceiveNetData(8,Re_Data,0);

	Frame_value=JudgeReceiveFrame(Re_Data);
	switch(Frame_value)
	{
	case 1:
		break;//��λ��û�з�������֡��STM32
	case 2:
		//SetButtonValue(6,2,1);//��������������ͣ����
		net_control_function(2,1);
		SpiSendNetdata(5,Pause_Ok,0);//��STM32���ͻ�е�ּ�����������
		break;
	case 3:
		//SetButtonValue(6,2,1);//�����������ͼ�����������
		//SpiSendNetdata(5,Continue_OK,0);//��STM32���ͻ�е�ּ�����������
		net_control_function(2,0);
		SpiSendNetdata(5,Continue_OK,0);//��STM32���ͻ�е�ּ�����������
		break;
	case 4:
		//SetScreen(2);//�л�ҳ��
		//SetButtonValue(2,9,0);//�����������ͷ���ԭ������
		//SetScreen(17);//�л�ҳ��
		//SetButtonValue(17,4,0);//�����������ͷ���ԭ������
		//SpiSendNetdata(5,origin_OK,0);//��STM32���ͻ�е�ַ���ԭ������
		//SetScreen(7);//�л�ҳ��
		return_home_network();
		break;
	case 5:
		//SetButtonValue(6,18,0);//������������ֹͣ��������
		//SpiSendNetdata(5,Stop_Ok,0);//��STM32���ͻ�е�ַ���ֹͣ����
		net_control_function(18,0);
		SpiSendNetdata(5,Stop_Ok,0);//��STM32���ͻ�е�ַ���ֹͣ����
		break;
	default:break;
	}
	//����һ֡����

/*
	���������
	����һ֡����,�������ݣ����ж�STM32�Ƿ��յ�DSP����ָ��
	�����յ��������ظ�һ֡����
	���յ�����
	����STM32�����������
	����ִ��
	��ִ������ظ���STM32
*/

}

/********************************************
 * �������ܣ�ִ��rs232ָ�ִ�к�ͽ�����ѭ�����Ӵ��ڽ����������
 * 			 ֱ��ִ�н��������������ʱ�˳�
 * ����ֵ��0���ɹ�	����ֵ������
 * //������//2018/06/01 for network
 */
void ExecuteTeamwork(PrgCode codeStrings,unsigned int* row)
{
	unsigned int i=0,n_num1;
	unsigned int value1;
	long component_value=0;
	while(codeStrings.codeStrings[i] != 'k')//ֱ������'k'������ѭ��
	{
		i++;
	}
	i++;
	while(codeStrings.codeStrings[i] == ' ')//�����ո�
	{
		i++;
	}

	if(codeStrings.codeStrings[i] == 'n')//�����n����
	{
		i++;
		n_num1 = StringToUint(&codeStrings.codeStrings[i]);//��ȡn����
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
	//����Ŀǰ״ֵ̬
	SendManipulatorPositionValue(value1);
	delay_1ms(3);
}
/********************************************
 * �������ܣ�ִ��tlָ�
 * ����ֵ��0���ɹ�	����ֵ������
 */
void ExecuteToolLength(PrgCode codeStrings)
{
	unsigned char j=0;
	while( ((codeStrings.codeStrings[j]<'0') || (codeStrings.codeStrings[j]>'9')) &&
	        (codeStrings.codeStrings[j]!='+') && (codeStrings.codeStrings[j]!='-') )//ֱ���������ֻ�'+' '-'
	{
	  j++;
	}
	TOOL_END.ToolLength = atof((char*)&codeStrings.codeStrings[j]);
}

/********************************************
 * �������ܣ�ִ��toolָ�����ĩ�����е�ֵ�����ĩ�˵�λ�ù�ϵ
 * ����ֵ��0���ɹ�	����ֵ������
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
		p_e = StringToPositionEuler((char*)(&codeStrings->codeStrings[i]));	//�ѽ��Ƕ�ת��Ϊ����
		TOOL_END.PosEuler = p_e;//���ݵĴ��� xxx
	}
	else //tool.px = -12.34
	{
		i++;	//���� '.'
		while(codeStrings->codeStrings[i] == ' ')//�����ո�
		{
			i++;
		}

		j = i;
		while( ((codeStrings->codeStrings[j]<'0') || (codeStrings->codeStrings[j]>'9')) &&
		        (codeStrings->codeStrings[j]!='+') && (codeStrings->codeStrings[j]!='-') )//ֱ���������ֻ�'+' '-'
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
			case 'x':	TOOL_END.PosEuler.Rx = data*PI_DIVIDE_180;	//�Ƕ�ת��Ϊ����
				break;
			case 'y':	TOOL_END.PosEuler.Ry = data*PI_DIVIDE_180;	//�Ƕ�ת��Ϊ����
				break;
			case 'z':	TOOL_END.PosEuler.Rz = data*PI_DIVIDE_180;	//�Ƕ�ת��Ϊ����
				break;
			}
		}
	}
}

/********************************************
 * �������ܣ�ִ��Partָ�������������е�ֻ������λ�ù�ϵ
 * ����ֵ��0���ɹ�	����ֵ������
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
		p_e = StringToPositionEuler((char*)(&codeStrings->codeStrings[i]));	//�ѽ��Ƕ�ת��Ϊ����
		PART_ROBOT.PosEuler = p_e;//���ݵĴ��� xxx
	}
	else
	{
		i++;	//���� '.'
		while(codeStrings->codeStrings[i] == ' ')//�����ո�
		{
			i++;
		}

		j = i;
		while( ((codeStrings->codeStrings[j]<'0') || (codeStrings->codeStrings[j]>'9')) &&
		        (codeStrings->codeStrings[j]!='+') && (codeStrings->codeStrings[j]!='-') )//ֱ���������ֻ�'+' '-'
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
			case 'x':	PART_ROBOT.PosEuler.Rx = data*PI_DIVIDE_180;	//�Ƕ�ת��Ϊ����
				break;
			case 'y':	PART_ROBOT.PosEuler.Ry = data*PI_DIVIDE_180;	//�Ƕ�ת��Ϊ����
				break;
			case 'z':	PART_ROBOT.PosEuler.Rz = data*PI_DIVIDE_180;	//�Ƕ�ת��Ϊ����
				break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/**************************************
 * �������ܣ��ַ��Ƚ�,����\0����,������ĸ��Сд
 * ���������length		��Ҫ�Ƚϵĳ���
 * 			 s1:�ַ���1
 * 			 s2���ַ���2
 * ����ֵ��	 0�����	1�������
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
 * �������ܣ���double���͵ĽǶ�ת��Ϊ�ַ�����������0.001��һ��ת��6��
 * ���������angle	ָ������Ƕȵ�ָ��
 * 			 angle_strings ָ���ַ�����ָ��
 * �ַ�����ʽ��angle_strings[6][8] 6��8�У�ÿһ�д���һ��������ݣ���λ����
 * 			   ����,20.123�ȣ��᷵��"\0\020.123"
 * 			 	    -1.230�����᷵��"\0\0-1.230"
 * 			   ʹ��ʱ���������������ʾ������\0�������ܹ����Ч��
 */
void AngleToStrings(double* angle,unsigned char* angle_strings)
{
	 long	 angle_tmp[6];
	 unsigned char i,j,k,sign_flag=0;
	 for(i=0;i<6;i++)
	 {
		  angle_tmp[i] = (long)(((*angle+i)*1000)*PI_MULTI_180_INVERSE);//����ת���ɽǶ���*1000
		  if(angle_tmp[i]<0)
		  {
			   angle_tmp[i] = -angle_tmp[i];
			   sign_flag |= (0x01<<i);//��¼����
		  }
	 }
	 for(i=0;i<6;i++)
	 {
		  //*(angle_strings+i*7+0)//��� '-'��
		  *(angle_strings+i*8+1) = angle_tmp[i]/100000;//888.888
		  *(angle_strings+i*8+2) = angle_tmp[i]/10000%10;
		  *(angle_strings+i*8+3) = angle_tmp[i]/1000%10+ '0';
		  *(angle_strings+i*8+4) = '.';			//С����
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
		  if(j==3) *(angle_strings+i*8+j-1) = '-'; //û������ʱ��-x.xxx��û�С�-��
		  for(k=j;k<3;k++)
		  {
			   *(angle_strings+i*8+k) = *(angle_strings+i*8+k)+ '0';
		  }
	 }

}

/**************************************
 * �������ܣ�������������ת��Ϊ�ַ���
 * ���������value	Ҫת������ֵ
 * 			 str	ת����ָ���ַ�����ָ�루��Ԥ���㹻�ĳ��ȣ�����+-��,С����ͽ�����\0��
 * 			 len	str�ĳ���(����������)
 * 			 f_len	С���㲿�ֵľ���
 * 			 zero	0->ǰ�沿����0��䣬������
 * 			 		1->ǰ�沿�ֲ����,������
 * ����ֵ��	ָ��string��ָ��
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
	for(i=0;i<f_len;i++)	//С�����������ַֿ���
	{
		value_tmp = value_tmp*10;
		tmp_zhengshu *= 10;
	}
	l_value = (long)(value_tmp+0.5) + tmp_zhengshu;//ǿ��ת��Ϊ����(+0.5����������)

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
		j=0;	//��¼�ж��ٸ�'0'
		for(i=1; ;i++)
		{
			if(str[i] == '0')
			{
				if((str[i+1] == '.') || (str[i+1] == '\0'))	//�����һ����.��\0  0.1 1.2
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
				str[i] = str[i+j+1];//+000123.10	//���Ų���Ҫ��
			}
			str[i] = 0;
		}
		else
		{
			for(i=0;i<(len-j-1);i++)
			{
				str[i+1] = str[i+j+1];//-000123.10	//����Ҫ��
			}
			str[i+1] = 0;
		}
	}

	str[len-1] = 0;
	return str;
}

/**************************************
 * �������ܣ�������������ת��Ϊ�ַ���
 * ���������value	Ҫת������ֵ
 * 			 str	ת����ָ���ַ�����ָ�루��Ԥ���㹻�ĳ��ȣ�����+-��,С����ͽ�����\0��
 * 			 len	str�ĳ���(����������)
 * ����ֵ��	ָ��string��ָ��
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

	j=0;	//��¼�ж��ٸ�'0'
	for(i=1; ;i++)
	{
		if(str[i] == '0')
		{
			if((str[i+1] == '.') || (str[i+1] == '\0'))	//�����һ����.��\0  0.1 1.2
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
			str[i] = str[i+j+1];//+000123.10	//���Ų���Ҫ��
		}
		str[i] = 0;
	}
	else
	{
		for(i=0;i<(len-j-1);i++)
		{
			str[i+1] = str[i+j+1];//-000123.10	//����Ҫ��
		}
		str[i+1] = 0;
	}

	str[len-1] = 0;
	return str;
}

/**************************************
 * �������ܣ����ַ���ת��Ϊint��������
 * 			 ��"123"ת��Ϊ123��
 * ���������ָ���ַ�����ָ��
 * 			 ע�⣺�������ֿ�ʼת����Ȼ������������ת������
 * ����ֵ��  unsigned long ���͵�ֵ
 */
unsigned long StringToUint(unsigned char* string)
{
	 unsigned long value=0;
	 unsigned char i=0;
	 while((*(string+i)<'0') || (*(string+i)>'9'))//ֱ����������
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
 * �������ܣ����ַ���ת��Ϊfloat��������(ע�⣬��������(+,-)���д���,
 * 			��Ϊ��Щָ����ź������Ƿֿ�(�м��пո�)��������޷�����,���˹�������������)
 * 			 ��"123.1"ת��Ϊ123.1��
 * ���������ָ���ַ�����ָ��
 * 			 ע�⣺�������ֿ�ʼת����Ȼ������������ת������
 * ����ֵ��  float ���͵�ֵ
 */
float StringToFloat(unsigned char* string)
{
	 float f_value=0;
	 float f_tmp=1;
	 int i_value=0;
	 unsigned char i=0;

	 while((*(string+i)<'0') || (*(string+i)>'9'))//ֱ���������ֻ�'+' '-'
	 {
		  i++;
	 }

//	 while((*(string+i)<'0') || (*(string+i)>'9') ||
//		   (*(string+i)=='+')|| (*(string+i)=='-'))//ֱ���������ֻ�'+' '-'
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
 * ���ܣ�������֣�����ǰ�������涼���ܳ��ֿո�һ�����ֶ���ȥ���ո�
 *
 * ������prg_name:������ĳ�����
 * 		 save_name:��ʲô���ֱ���
 * 		 prg_num:Ҫ��U���еڼ��������Ƶ�DSP���(��0��ʼ)
 *		 mode:����ģʽ��0->�����Ǳ���(����Ѵ����ظ��ĳ����򲻸��ǣ��ҷ����ļ��Ѵ��ڣ�
 *		 				1->���Ǳ���(����Ѵ����ظ��ĳ����򸲸Ǳ��棩
 * ����ֵ��0->���Ϲ淶������
 * 		   6->���ֲ���Ϊ��
 * 		   7->���ֹ���
 */
u8 CheckPrgName(char* name)
{
	int i,j;

	 i=0;
	 if(*(name+0) == 0)
	 {
		 return 6;//���ֲ���Ϊ��
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
	 while(*(name+i) == ' ')//���ֽ�β���ܳ��ֿո�
	 {
		 *(name+i) = 0;
		 i--;
		 if(i<0) return 6;//���ֲ���Ϊ��
	 }

	 i++;
	 if(i>=PROGRAM_NAME_LENGTH) return 7;//���ֹ���

	 return 0;
}

/***************************************
 * �������ܣ�����tray(����)���� ���򿪳����ʱ�������
 */
void ClearTrayData(void)
{
	memset(s_tray,0,sizeof(s_tray));
}
/***************************************
 * �������ܣ�������̵��ܸ�������
 * ���������t_num:���̱��
 * ����ֵ����������
 */
long GetTrayGridNum(unsigned char t_num)
{
	return s_tray[t_num].x_num*s_tray[t_num].y_num;
}
/***************************************
 * �������ܣ���������Ƿ��ж�����ı�־
 * ���������t_num:���̱��
 * ����ֵ��0��δ����	1���Ѷ���
 */
unsigned char GetTrayDefState(unsigned char t_num)
{
	return s_tray[t_num].def_flag;
}
/***************************************
 * �޸�λ�ã�������
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
 * �������ܣ�����ĳλ�õ�ĳ��ĽǶ�
 * ���������axis:�ڼ���,1~6
 * 			 angle������Ϊ���ٶȣ���λ�Ƕȣ�
 * 			 pulse�������������λ�õ�ָ��
 * ���������none
 */
//ִ����px = py.j4+a ָ�����ת�˵�����ʱ����־Ϊ1��������������ź�9��Ч�ǣ�ֹͣ��ת��(��������ϵͳ)
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
			//��ת�����ᣬΪ�˱���λ�ò��䣬��������Ҫ�ƶ�
			if(ROBOT_PARAMETER.AXIS_NUM == 4)
			{
				THE_FOUR_AIXS_XUANZHUAN = 1;
			}
			if(ROBOT_PARAMETER.SYS==FOUR_AXIS_B || ROBOT_PARAMETER.SYS==FOUR_AXIS_C)//(angle/(2*PI)*�ݾ�)/�ݸ˳��� *ROBOT_PARAMETER.RATIO[3]*ROBOT_PARAMETER.MOTOR_PRECISION
			{
				//�ȼ��㵱ǰ������ĽǶ�
				ag_current = *(pulse+axis-1)/ROBOT_PARAMETER.MOTOR_PRECISION/ROBOT_PARAMETER.RATIO[3]/360.0;
				//�õ�������仯��,������仯�����ݵ�����仯���仯���仯���Ա��ֻ�е��ĩ�˸߶Ȳ���
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
 * �������ܣ�����ĳλ�õ�ĳ��ĽǶ�
 * ���������axis:�ڼ���,1~6
 * 			 angle�����Ķ��ٶȣ���λ�Ƕȣ�
 * 			 pulse�������������λ�õ�ָ��
 * ���������none
 */
//ִ����px = py.j4+a ָ�����ת�˵�����ʱ����־Ϊ1��������������ź�9��Ч�ǣ�ֹͣ��ת��(��������ϵͳ)
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
			//��ת�����ᣬΪ�˱���λ�ò��䣬��������Ҫ�ƶ�
			if(ROBOT_PARAMETER.AXIS_NUM == 4)
			{
				THE_FOUR_AIXS_XUANZHUAN = 1;
			}
			if(ROBOT_PARAMETER.SYS==FOUR_AXIS_B || ROBOT_PARAMETER.SYS==FOUR_AXIS_C)//(angle/(2*PI)*�ݾ�)/�ݸ˳��� *ROBOT_PARAMETER.RATIO[3]*ROBOT_PARAMETER.MOTOR_PRECISION
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
 * ���ܣ���ѯU������״̬
 * ������prg_num:���������ָ��
 * ����ֵ��0->U��������	1->U��δ���� 5->DSP��STM32ͨ�ų���
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
 * ���ܣ���ó������֣�ÿ��һ��
 * ������num��Ҫ��õڼ������������,��0��ʼ
 * 		 prg_name:�������ֵ�ָ��
 * ����ֵ��0->�ɹ�	1->ʧ��,num������Χ��û�д˳��� 5->DSP��STM32ͨ�ų���
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
					rx_len |= (Uint32)rx_cmd[i+1]<<(i*8);//����������ֵĳ���
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
 * ���ܣ���õ�ǰ��Чλ�ø���
 * ������NONE
 * ����ֵ����Чλ�ø���
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
 * ���ܣ���������ڲ��洢������U��
 * ������prg_name:�������ĳ���
 * 		 new_name:��ʲô���ֱ���
 *		 mode:����ģʽ��0->�����Ǳ���(����Ѵ����ظ��ĳ����򲻸��ǣ��ҷ����ļ��Ѵ��ڣ�
 *		 				1->���Ǳ���(����Ѵ����ظ��ĳ����򸲸Ǳ��棩
 * ����ֵ��0->����ɹ�
 * 		   1: �����ȡʧ�ܣ���ȡ����У��ʧ�ܣ�
 * 		   2���޴˳���
 * 		   3�������ظ�
 * 		   4������ʧ�ܣ�������
 * 		   5: DSP��STM32ͨ�ų���
 * 		   6������������Ϊ��
 * 		   7������������
 */
unsigned char export_program(char* prg_name,char* new_name,u8 mode)
{
	u8 rx_cmd[5],tx_cmd[5],err,t=0,t1=0,t2=0;
	int i;
	unsigned int num=0,lines_num=0;
	long data_len=0,name_len=0,p_data_len;
	memset(tx_cmd,0,sizeof(tx_cmd));

	//���ּ��
	err = CheckPrgName(prg_name);
	if(err) return err;

	err = CheckPrgName(new_name);
	if(err) return err;

	//������򳤶�
	err = OpenProgram((u8*)prg_name,&num);
	if(err) return err;

	lines_num = GetLinesCurrentProgram();
	data_len = lines_num * COLUMNS_IN_LINE;//ÿ��COLUMNS_IN_LINE���ַ����������һλ��0

	p_data_len =GetPositionNum()*sizeof(s_position_pulse[0]);//sizeof(s_position_pulse[0])=14

	name_len = strlen(new_name)+1;

	for(i=0;i<4;i++)
	{
		tx_cmd[i+1] = name_len>>(i*8)&0x000000FF;
	}

	if(mode) tx_cmd[0] = TR_CMD_CODE_TF;//���Ǳ���
	else tx_cmd[0] = TR_CMD_CODE_T;//�����Ǳ���

	while(1)
	{
		err = 0;
		SpiSendCmd(tx_cmd);//�ȷ��͸��Ƶĳ������ֵĳ��ȣ����Ƶ�U�̺󱣴�����֣�
		delay_1us(100);
		SpiSendData(name_len,(u8*)new_name,0);//���͸��Ƶĳ�������
		delay_1ms(20);
		if(SpiReceiveCmd(rx_cmd) == 0)//�Ȼظ�
		{
			if(rx_cmd[0] == TR_CMD_OK)
			{

			}
			else if(rx_cmd[0] == R_CMD_EXIST)//����ļ��Ѵ���
			{
				return 3;//�����ظ�
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
			SpiSendCmd(tx_cmd);//�������ݳ���
			delay_1us(100);
			SpiSendData(data_len,(u8*)s_program_code,0);//���ͳ����ı�
			delay_1us(100);
			for(i=0;i<4;i++)
			{
				//*2����ΪDSP�����16bits���ͣ�STM32�Ǳ���8bits����
				tx_cmd[i+1] = (p_data_len*2)>>(i*8)&0x000000FF;
			}
			SpiSendCmd(tx_cmd);//�������ݳ���
			delay_1us(100);
			SpiSendData(p_data_len,(u8*)s_position_pulse,1);//����P��λ��
			delay_1us(100);

			t1 = 0;
			t2 = 0;
			while(1)//��ѯ�����Ƿ�ɹ�
			{
				err = 0;
				delay_1ms(500);
				tx_cmd[0] = T_CMD_SAVE_STATE;
				SpiSendCmd(tx_cmd);//���Ͳ�ѯ����
				delay_1us(100);
				err = SpiReceiveCmd(rx_cmd);//����
				if(err)//����ͨ�Ŵ���5�ξ��˳�
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
 * ���ܣ���������ڲ��洢������U��,֮���֮ǰ�Ѿ����򿪵ĳ���
 * ������prg_name:�������ĳ���
 * 		 new_name:��ʲô���ֱ���
 *		 mode:����ģʽ��0->�����Ǳ���(����Ѵ����ظ��ĳ����򲻸��ǣ��ҷ����ļ��Ѵ��ڣ�
 *		 				1->���Ǳ���(����Ѵ����ظ��ĳ����򸲸Ǳ��棩
 * ����ֵ��0->����ɹ�
 * 		   1: �����ȡʧ�ܣ���ȡ����У��ʧ�ܣ�
 * 		   2���޴˳���
 * 		   3�������ظ�
 * 		   4������ʧ�ܣ�������
 * 		   5: DSP��STM32ͨ�ų���
 * 		   6������������Ϊ��
 * 		   7������������
 */
unsigned char ExportProgram(char* prg_name,char* new_name,u8 mode)
{
	unsigned char err,err1;
	unsigned int num;
	err = export_program(prg_name,new_name,mode);//�������ڴ�ᱻˢ�£�����Ҫ���´�֮ǰ���򿪵ĳ���
	if(StringCmp(PROGRAM_NAME_LENGTH,(unsigned char*)prg_name,(unsigned char*)s_program_name_num.program_name))
	{
		if(err != 6 || err != 7)
		{
			err1 = OpenProgram((unsigned char*)s_program_name_num.program_name,&num);
			if(err1)
			{
				//��ʧ�������
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
 * ���ܣ��������U�̵����ڲ��洢
 * ������prg_name:������ĳ�����
 * 		 save_name:��ʲô���ֱ���
 * 		 prg_num:Ҫ��U���еڼ��������Ƶ�DSP���(��0��ʼ)
 *		 mode:����ģʽ��0->�����Ǳ���(����Ѵ����ظ��ĳ����򲻸��ǣ��ҷ����ļ��Ѵ��ڣ�
 *		 				1->���Ǳ���(����Ѵ����ظ��ĳ����򸲸Ǳ��棩
 * ����ֵ��0->����ɹ�
 * 		   1->���浽flashʧ��
 * 		   2->����ʧ��
 * 		   3->�����ظ�
 * 		   4->��ȡU�̳���
 * 		   5->DSP��STM32ͨ�ų���
 * 		   6->���ֲ���Ϊ��
 * 		   7->���ֹ���
 * 		   0xff->������������������������½�
 */
unsigned char import_program(u32 read_num,char* save_name,u8 mode)
{
	 u8 tx_cmd[5],rx_cmd[5],t1=0,err=0,num;
	 int i;
	 u8 dupli_flg=0;//��ǳ����Ƿ��ظ� 0->û���ظ�	1->�ظ�
	 u32 prg_len=0,pos_len=0;
	 struct ProgramInfo prg_info[MAX_PROGRAM_NUM];
	 memset(prg_info,0,sizeof(prg_info));

	 //�������
	 err = CheckPrgName(save_name);
	 if(err) return err;

	//�ж�prg_name�Ƿ��ظ�
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,0,1,0,&num);		//����ڲ��洢�г������
	 if((num==0xff) || (num==0))	//0xff��ʾ���ǵ�һ��ʹ�ã���û�б����,���Գ������Ϊ0
	 {
		  num =0;
	 }
	 ReadNandFlashData(INFO_BLOCK_ADDR,0,1,sizeof(prg_info[0])*num,0,(unsigned char*)prg_info);//��������Ϣ����
	 for(i=0;i<num;i++)
	 {
		  if(!StringCmp(PROGRAM_NAME_LENGTH,prg_info[i].prgName,(u8*)save_name))
		  {
			  dupli_flg = 1;//��ǳ����ظ�
			  if(mode)
			  {
				  //
			  }
			  else return 3;//�������ظ�
			  break;
		  }
	 }
	 if(dupli_flg == 0)//������Ǹ��ǣ������½����򣬴�ʱӦ�ü���Ƿ񳬹�����������
	 {
		 if(num >= MAX_PROGRAM_NUM) return 0xff;				//����������������������½�
	 }

	//������stm32��U�̶�ȡ����
	tx_cmd[0] = T_CMD_READ;
	for(i=0;i<4;i++)
	{
		tx_cmd[i+1] = (read_num>>(i*8)) & 0x000000ff;
	}
	SpiSendCmd(tx_cmd);

	memset(tx_cmd,0,sizeof(tx_cmd));
	while(1)
	{
		//�ȴ�stm32��U�̶�ȡ�������
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
		if(rx_cmd[0] == TR_CMD_OK)//stm32��ȡU�̳ɹ�
		{
			while(1)
			{
				//������stm32�������ݵ�����
				tx_cmd[0] = TR_CMD_CODE_R;
				SpiSendCmd(tx_cmd);
				delay_1us(100);
				if(SpiReceiveCmd(rx_cmd))
				{
					delay_1ms(300);//�ȴ�stm32�˳�����ģʽ
					return 5;
				}

				//��ʼ��������
				prg_len=0;
				for(i=0;i<4;i++)
				{
					prg_len |= (Uint32)rx_cmd[i+1]<<(i*8);
				}
				memset(s_program_code,0,sizeof(s_program_code));		//����
				if(SpiReceiveData(prg_len,(u8*)s_program_code,0))
				{
					delay_1ms(200);//�ȴ�stm32�˳�����ģʽ
					return 5;
				}
				delay_1us(100);
				if(SpiReceiveCmd(rx_cmd))
				{
					delay_1ms(100);//�ȴ�stm32�˳�����ģʽ
					return 5;
				}
				pos_len=0;
				for(i=0;i<4;i++)
				{
					pos_len |= (Uint32)rx_cmd[i+1]<<(i*8);
				}
				memset(s_position_pulse,0,sizeof(s_position_pulse)); 	//����
				if(SpiReceiveData(pos_len/2,(u8*)s_position_pulse,1))
				{
					return 5;
				}

				//���浽flash
				err = 0;
				if(dupli_flg==1)//��������ظ��򸲸�
				{
					err = SaveProgram((u8*)save_name);
					if(err)	return err;
					else	return 0;
				}
				else//�����½��ٱ���
				{
					err = NewProgram((u8*)save_name,1);//������½�
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
	//return 0;//2018/05/22ע�͵���
}

/***************************************************
 * ���ܣ��������U�̵����ڲ��洢,����֮ǰ�Ѿ����򿪵ĳ���
 * ������prg_name:������ĳ�����
 * 		 save_name:��ʲô���ֱ���
 * 		 prg_num:Ҫ��U���еڼ��������Ƶ�DSP���(��0��ʼ)
 *		 mode:����ģʽ��0->�����Ǳ���(����Ѵ����ظ��ĳ����򲻸��ǣ��ҷ����ļ��Ѵ��ڣ�
 *		 				1->���Ǳ���(����Ѵ����ظ��ĳ����򸲸Ǳ��棩
 * ����ֵ��0->����ɹ�
 * 		   1->���浽flashʧ��
 * 		   2->����ʧ��
 * 		   3->�����ظ�
 * 		   4->��ȡU�̳���
 * 		   5->DSP��STM32ͨ�ų���
 * 		   6->���ֲ���Ϊ��
 * 		   7->���ֹ���
 * 		   0xff->������������������������½�
 */
unsigned char ImportProgram(u32 read_num,char* save_name,u8 mode)
{
	unsigned char err,err1;
	unsigned int num;
	err = import_program(read_num,save_name,mode);//�������ڴ�ᱻˢ�£�����Ҫ���´�֮ǰ���򿪵ĳ���
	if(StringCmp(PROGRAM_NAME_LENGTH,(unsigned char*)save_name,(unsigned char*)s_program_name_num.program_name))
	{
		if(err != 6 || err != 7 )
		{
			err1 = OpenProgram((unsigned char*)s_program_name_num.program_name,&num);
			if(err1)
			{
				//��ʧ�������
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
