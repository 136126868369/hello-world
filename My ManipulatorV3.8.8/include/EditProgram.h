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

//DSP��STM32ͨ�ŵ���Ƶ�������
#define TR_CMD_OK				1	//���ͻ���� ����/������ȷ
#define TR_CMD_REPEAT			2	//�������������ط�
#define TR_CMD_PRG_NAME		    3	//�ظ���������ָ��
#define TR_CMD_CODE_T			4	//���ʹ��������������
#define TR_CMD_CODE_TF		    5	//���ʹ��������������
#define TR_CMD_CODE_R			6	//���ͽ��ճ�����������
#define TR_CMD_OVER			    7	//��������������ظ����ʹ���ʱ��

#define T_CMD_U_STATE			8	//���Ͳ�ѯU��״̬����
#define T_CMD_READ			    9	//���Ͷ�ȡ����ָ��(��stm32ȥU�̻�ȡ����)
#define T_CMD_SAVE_STATE		10	//���Ͳ�ѯ��һ���Ƿ񱣴�ɹ�
#define T_CMD_READ_STATE		11	//���Ͳ�ѯ��һ���Ƿ��ȡ�ɹ�

#define R_CMD_CONNECT			12	//U�������� �����ĸ��ֽڱ�ʾ�Ѵ��ڵĳ������
#define R_CMD_DISCONNECT		13	//U��δ����
#define R_CMD_PRG_FLOW		    14	//����Ŀǰ����Χ
#define R_CMD_SAVE_FAILED	    15	//����ʧ��
#define R_CMD_READ_FAILED	    16	//��ȡʧ��
#define R_CMD_EXIST			    17	//�ļ��Ѵ���
#define R_CMD_INEXIST			18	//�ļ�������
#define R_CMD_IDLE			    19	//����״̬
#define R_CMD_BUSY			    20	//æ״̬

#define T_CMD_SET_IO		    21	//����IO״̬
#define T_CMD_VERSION		    22	//��ѯ��ǰ�汾��

#define T_CMD_USART			    23	//dsp�������ݵ�stm32�������ô��ڷ��ͳ�ȥ
#define R_CMD_USART			    24	//stm32���ڽ��յ������ݷ��͸�DSP
#define R_CMD_USART_RX_STA	    25	//��ѯ�����Ƿ������ݵ���
#define R_CMD_USART_RX_OK	    26	//���������ݵ���
#define R_CMD_USART_RX_NO	    27	//���������ݵ���
#define TR_CMD_NG			    28	//���յ�������У�����

#define CMD_RS232_OK			29	//�������˿ڴ������������ظ���ָ��
#define CMD_RS232_BEGIN			30	//rs232 �ӵ�һ����ʼ
#define CMD_RS232_OVER			31	//�������з���ĳЩԭ��(�ŷ���������λ����ͣ��)��ֹͣ


#define CMD_ETH_OK				33	//�������˿ڴ������������ظ���ָ��
#define CMD_ERROR				34	//spi���յ���������ݣ������ط�
#define CMD_ETH_BEGIN			35	//eth �ӵ�һ����ʼ
#define CMD_ETH_OVER			36	//�������з���ĳЩԭ��(�ŷ���������λ����ͣ��)��ֹͣ

#define MXA_REPEAT			5	//���ݴ�����̳���ʱ������ظ����͵Ĵ���

#define COLUMNS_IN_LINE		19//һ�����19���ַ������һ����������'\0'��
#define ROWS_IN_PAGE  		15//һҳ������
#define PROGRAM_NAME_LENGTH	20//�������ֳ���
#define ROWS_IN_PROGRAM	    750//�����������,(��Ӧ��������50ҳ)750
#define MAX_POSITION_NUM	100//���Pλ�ø���100
#define MAX_PROGRAM_NUM	    50//����ܱ���ĳ�������50
#define MAX_POUT_NUM		24//���IO����Ŀ
#define MAX_PIN_NUM			18//����IO����Ŀ
#define MAX_TRAY_NUM		20//������̱��20
#define MAX_N_NUM			50//���ı������
#define MAX_F_NUM			50//�����ͱ����������
#define MAX_RETURN			20//return��ջ���ֵ
#define ASCII_0 			0x30//0��ASCII��
#define ASCII_9 			0x39//9��ASCII��
#define ASCII_ENTER		    0x0d//�س�
#define ASCII_BACKSPACE  	0x08//backspace
#define ASCII_DEL			0x0c//ɾ��һ��
#define ASCII_ESC			0x1b//ESC�˳�
#define ASCII_SAVE		    0x11//����
#define MAX_NUM_COMPONENT   65000//���ӹ������
#define MAX_STATUS_VALUE    60000//��е�����λ��״̬��

struct ProgramCode{
	 //unsigned char num;
	 unsigned char codeStrings[COLUMNS_IN_LINE];
};
typedef struct ProgramCode PrgCode;

struct program_name_num{
	unsigned int num;//��¼��ǰ���򿪵ڼ�������,0��ʾû�д�
	char program_name[PROGRAM_NAME_LENGTH];//��¼��ǰ�򿪵ĳ�����
};

struct ThreeDimCoordStru{//��ά����
	double x;
	double y;
	double z;
};

struct TrayStru{
	unsigned char def_flag;//����Ƿ񱻶������0��δ����	1���Ѷ���
	unsigned int x_num;//���̵Ŀ�����x_num��
	unsigned int y_num;//���̵ĳ�������y_num��
	double width;//��ÿ�ݵĳ���
	double length;//��ÿ�ݵĳ���
	double fix_x;	//����x
	double fix_y;	//����y
	double fix_z;	//����z
	//double x,y;//����ÿ�ݵĳ���
	//long pulse[6];//��ʼ������λ��
	PositionGesture pos_ges;//������ʼ��λ��
	struct ThreeDimCoordStru unit_vector_x;//x��λ����
	struct ThreeDimCoordStru unit_vector_y;//y��λ����
	double angle[6];	//�����һ����ĽǶȣ��������������Ž�Ĳο���
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
extern long CURRENT_BAUD;	//��ǰ������
/***************************************
 * �������ܣ���ó����е���ָ��
 * ���������void
 * ���������s_program_code��ָ��
 */
PrgCode* GetProgramCode(void);

/***************************************
 * �������ܣ�������������
 * ���������NONE
 * ���������NONE
 */
void ClearProgramPulseData(void);


/***************************************
 * �������ܣ��õ�row�л���ı�
 * ���������row	��
 * 			 strings	�ַ�����ָ��
 * 			 ˵������õ��ַ�����\0��Ϊ��β
 * ����ֵ��	 ���������Ϣ
 */
unsigned char GetCodeStrings(unsigned int row,unsigned char* strings);


/*********************************************
 * �������ܣ�	��õ�ǰ����Ĵ���������
 * ���������	none
 * ����ֵ��		��������
 */
unsigned int GetLinesCurrentProgram(void);


/*********************************************
 * �������ܣ�	��õ�row�д���
 * ���������	row	��
 * 				code	�����ı���ָ��
 * 				ע�⣺�����ı����������14���ַ��Ĵ���,Ҫ���ȫ�����ݣ�
 * 				��ʹ��һ������Ϊ19��unsigned char���͵����飬��code[19]��
 * 				Ȼ������ָ���׵�ַ��Ϊ�������룺GetCodeTxt(row,code);
 */
void GetRowCode(unsigned int row,unsigned char* codeStrings);


/*********************************************
 * �������ܣ�	����ѱ����������
 * ���������	none
 * ����ֵ��		�ѱ���������������Χ0~99��
 */
unsigned char GetProgramNum(void);


/*********************************************
 * �������ܣ�	��ö�Ӧ��ŵĳ��������
 * ���������	num��	��������ţ����ܴ���Ŀǰ�ѱ���ĳ���������
 * 				name��	��������ָ�루��
 * 				˵����	�����ǰ�˳���������еģ���0��ʼ��
 * ����ֵ��		0���ɹ�		1��ʧ��
 */
unsigned char GetProgramName(unsigned char prg_num,unsigned char *name);


/***************************************************
 * ���ܣ���õ�ǰ��Чλ�ø���
 * ������NONE
 * ����ֵ����Чλ�ø���
 */
Uint32 GetPositionNum(void);

/**************************************
 * �������ܣ���ȡ��ǰ�е�λ�ã��Ѿ������﷨��飨������д�����P��λ��Pxx�Ļ���ʾ��ʹ�ã�
 * ���������row����
 * ����ֵ��0->�ɹ�	0x91->��λ���ѱ�ʹ�ã�ѯ���Ƿ񸲸�  ������0������(���Ĵ������)
 */
unsigned int GetPositionNumInRow(unsigned int row,unsigned int* num);

/**************************************
 * ��������	 CapturePositionDataInRow
 * �������ܣ�����ǰλ���������ݵ���row�е�Pλ���б��棨������д�����P��λ��Pxx�Ļ���ʾ��ʹ�ã�
 * ���������row����
 * ����ֵ��0���ɹ�	��0������
 */
unsigned char CapturePositionDataInRow(unsigned int row);


/******************************************
 * �������ܣ�������ֵ���浽p_num
 */
void SavePositionData(unsigned char p_num,long* pulse);


/******************************************
 * �������ܣ���Pλ�ñ��Ϊ��ʹ��
 */
void SavePositionState(unsigned char p_num);


/**************************************
 * �������ܣ������û��Pλ�ã��Ѿ������﷨���
 * ���������row  ��
 * ����ֵ��	 0��û�з��ִ���
 * 			 0x91(WARN_DUPLI_P) �������ظ�Pλ�ã������ж��Ƿ񸲸�
 * 			 ����������(���Ĵ������)
 */
unsigned char CheckPnum(unsigned int row);


/**************************************
 * ��������	 GetCurrentAngleStrings
 * �������ܣ���õ�ǰÿ��Ƕ�ת���õĵ��ַ���
 * ���������angle_strings	�ַ�����ָ��
 * 			 �ַ�����ʽ��angle_strings[6][8] 6��8�У�ÿһ�д���һ��������ݣ���λ����
 * 			 ����,20.123�ȣ��᷵��"\0\020.123"
 * 			 	  -1.230�����᷵��"\0\0-1.230"
 * 			 	  ʹ��ʱ���������������ʾ������\0�������ܹ����Ч��
 * ����ֵ��  none
 */
void GetCurrentAngleStrings(unsigned char* angle_strings);


/**************************************
 * �������ܣ����Pxx��λ��ÿ�������ֵ
 * ���������p_num	pλ�õ�ֵ����p11��������11��������p11)
 * 			 pulse	ָ������ֵ��ָ�룬long����
 * ����ֵ��	 0���ɹ�	1��P�㲻����
 */
unsigned char GetPxxPulseValue(unsigned char p_num,long* pulse);


/*************************************
 * �������ܣ����Ŀǰ�ѱ�ʹ�õ�Pλ�ø���
 * ���������none
 * ����ֵ��	 �ѱ�ʹ�õ�Pλ�ø���
 */
unsigned char GetPositionUsedNum(void);


/**************************************
 * ��������	 CheckPositionState
 * ��������: ���Pλ��ʹ��״̬
 * �������: p_num	Pλ��ֵ��Pxx�е�xx����P11��11��
 * ����ֵ��	 0��Pλ���ѱ�ʹ��	1��Pλ��δ��ʹ��
 */
unsigned char CheckPositionState(unsigned char p_num);


void IncreaseColumnChar(unsigned int row,unsigned char column,unsigned char character);


/***************************************
 * ��������	 IncreaseEmptyRow
 * �������ܣ��ڵ�row������һ�пհ��У�Ҳ���ǻس���ʱ��,����һ�У�
 * ���������row	��	����Χ0~255��
 * ����ֵ��	 0���ɹ�	1��ʧ�ܣ�����255�У�
 */
unsigned char IncreaseEmptyRow(unsigned int row);


void DeleteColumnChar(unsigned int row,unsigned char column);


void ClearRowChar(unsigned int row);
/***************************************
 * ��������	 DeleteRow
 * �������ܣ�ɾ������ĵ�row��
 * ���������row	����ɾ���ĵ�row�У�row��Χ��0~255��
 * 			 ��DeleteRowCodeTxt��������������DeleteRowCodeTxt
 * 			 ֻ��ɾ���ı������Ǳ����ģ�ֻ�����ǿ��У���DeleteRowCode������ɾ��
 * 			 ��row�У����Һ�����л���ǰƽ��1����λ
 */
void DeleteRow(unsigned int row);//


void ModifyCursorPosition(unsigned int row,unsigned char column);

/**************************************
 * �������ܣ��жϴ˳����Ƿ����
 * ���������prg_name	��������ָ��
 * ����ֵ��	 0���������
 * 			 1���޴˳���
 * 			 2��flash����
 */
unsigned char IsProgramValid(unsigned char* prg_name);

/**************************************
 * ��������  NewProgram
 * �������ܣ��½�����
 * ����		name	��������ָ�룬�����19���ַ�
 * 			mdoe:	0->�½�ʱ��մ洢����	1->�½�ʱ����մ洢����
 * ����ֵ	0x00���ɹ�
 * 			0x01��ʧ��(������Ϣ��ʧ��)
 * 			0x03��ʧ�ܣ������ظ���
 * 			0x04: ���ֹ�����19���ַ����ڣ�
 * 			0xff��ʧ�ܣ����������������
 */
unsigned char NewProgram(unsigned char* name,unsigned char mode);


/**************************************
 * ��������	 OpenProgram
 * �������ܣ��򿪳����ı���λ������
 * ���������name	��������ָ��
 * 		   ��num		ָ��ڼ��������ָ��
 * ����ֵ��	 0���򿪳ɹ�
 * 			 1����ʧ�ܣ��޴˳���
 */
unsigned char OpenProgram(unsigned char* prg_name,unsigned int* num);


/**************************************
 * ��������	 SaveProgram
 * ��������ı���λ������
 * �������ܣ���������λ�õ���Ӧ�ļ���
 * ���������name �ļ���������ָ��
 * ����ֵ��	 0������ɹ�
 * 			 1������ʧ��(�����������Flash��)
 * 			 2: ����ʧ��(�޴��ļ���)
 */
unsigned char SaveProgram(unsigned char* prg_name);


/***************************************
 * ��������	 DeleteProgram
 * ɾ������
 * �������	name	����ɾ���ĳ�������
 * �������	0��ɾ���ɹ�
 * 			1��ɾ��ʧ�ܣ�������Ϣ����󣬿���flash�𻵣�
 * 			2: ɾ��ʧ�� (�޴˳���)
 */
unsigned char DeleteProgram(unsigned char* prg_name);


/***************************************
 * ��������	 CopyProgram
 * ɾ������
 * �������	prg_name	�������Ƶĳ�������
 * 			new_name	���ƺ�ĳ�������
 * �������	0�����Ƴɹ�
 * 			1������ʧ�ܣ�������Ϣ����󣬿���flash�𻵣�
 * 			2: ����ʧ�� (�޴˳���)
 * 			3: �����ظ�
 */
unsigned char CopyProgram(unsigned char* prg_name,unsigned char* new_name);


/***************************************
 * ��������	 RenameProgram
 * ɾ������
 * �������	old_name	���������ĳ�������
 * 			new_name	�µ�����
 * �������	0���������ɹ�
 * 			1��������ʧ�ܣ�������Ϣ����󣬿���flash�𻵣�
 * 			2: ������ʧ�� (�޴˳���)
 * 			3: �����ظ�
 */
unsigned char RenameProgram(unsigned char* old_name,unsigned char* new_name);


/*****************************************
 * �������ܣ�SaveAxisNumber �����е�������
 * ���������axis_num	������
 * ���������0������ɹ�	1������ʧ��
 */
unsigned char SaveAxisNumber(unsigned char axis_num);


/*****************************************
 * �������ܣ�ReadAxisNumber ����е�������
 * ���������NONE
 * �����������е�������
 */
unsigned char ReadAxisNumber(void);

/*****************************************
 * �������ܣ����򵥲�����
 * ���������cmd:ָ�����
 * 			codeStrings:�����ı�
 * ����ֵ��	 0���ɹ�	������0�����󣨲��ģ�
 */
unsigned int ProgramStepRun_Code(unsigned char cmd,const PrgCode* codeStrings);

/*****************************************
 * �������ܣ����򵥲�����
 * ���������row���������е��У�
 * 			 ע�⣺���ִ�е�������ת���Ӻ�������ô���ֵ�����޸�Ϊ������Ӧ����
 * 			 ������������Լ�1����ʾ������һ��
 * ����ֵ��	 0���ɹ�	������0�����󣨲��ģ�
 */
unsigned int ProgramStepRun(unsigned int* row,unsigned char check);


/*****************************************
 * �������ܣ�����ȫ������
 * ���������none
 * ����ֵ��	 none
 */
void ProgramRun(void);


/*****************************************
* �������ܣ��жϸı��־λ��ִ�г���ʱȡ��־λ����ֵ
* ���������
*
*/
unsigned char ProgramRuningFlag(unsigned char sign);


/*****************************************
 * �������ܣ�����ȫ������(������ָʾ���е���һ��)
 * ���������f,���������б���ɫ������ָ��
 * 			 row,�ӵڼ��п�ʼ����
 * ����ֵ��  �������е��ڼ���
 */
unsigned char ProgramRunWithPointer(void (*f)(unsigned char),unsigned int* row);


/*****************************************
 * �������ܣ���ʼ����
 * ���������row  �ӵ�row�п�ʼ
 * ����ֵ��
 */
void StartRun(void);


/*****************************************
 * �������ܣ�ֹͣ����
 * ���������none
 * ����ֵ:	 none
 */
void StopRun(void);


/*****************************************
 * �������ܣ���ͣ����
 * ���������
 * ����ֵ
 */
void PuaseRun(void);


/*****************************************
 * �������ܣ����return�Ķ�ջ
 */
void ClearReturnStack(void);


unsigned char GetProgramRunSpeed(void);
void ModifyAllSpeed(unsigned char speed);

/*****************************
 * �������ܣ����ȫ���ٶ�ovrd
 */
unsigned char GetOvrd(void);

/*****************************
 * �������ܣ���õ�ǰ���ٶ�
 */
unsigned int GetAcc(void);

/***************************************
 * �������ܣ����n������ֵ
 * ���������n_num���
 * ���������n������ֵ
 */
long get_n_value(int n_num);

/***************************************
 * �������ܣ����n������ֵ
 * ���������n_num���
 * ���������n������ֵ
 */
double get_f_value(int n_num);

/******************
 * �޸�ȫ���ٶ�
 */
unsigned int ModifyOvrd(unsigned char speed);
/******************
 * �޸ľֲ��ٶ�speed
 * ����ֵ�����ٶ�
 */
unsigned int ModifySpeed(unsigned char speed);

unsigned char StringCmp(unsigned char length,unsigned char* s1,unsigned char* s2);
long GetTrayGridNum(unsigned char t_num);
unsigned char GetTrayDefState(unsigned char t_num);


/**************************************
 * �������ܣ����ַ���ת��Ϊint��������
 * 			 ��"123"ת��Ϊ123��
 * ���������ָ���ַ�����ָ��
 * 			 ע�⣺�������ֿ�ʼת����Ȼ������������ת������
 * ����ֵ��  unsigned long ���͵�ֵ
 */
unsigned long StringToUint(unsigned char* string);


/**************************************
 * �������ܣ����ַ���ת��Ϊfloat��������
 * 			 ��"123.1"ת��Ϊ123.1��
 * ���������ָ���ַ�����ָ��
 * 			 ע�⣺�������ֿ�ʼת����Ȼ������������ת������
 * ����ֵ��  float ���͵�ֵ
 */
float StringToFloat(unsigned char* string);

/**************************************
 * �������ܣ�������������ת��Ϊ�ַ���
 * ���������value	Ҫת������ֵ
 * 			 str	ת����ָ���ַ�����ָ��
 * 			 str	ת����ָ���ַ�����ָ�루��Ԥ���㹻�ĳ��ȣ�����+-��,С����ͽ�����\0��
 * 			 len	str�ĳ���(����������)
 * 			 zero	0->ǰ�沿����0��䣬������
 * 			 1->ǰ�沿�ֲ����,������
 * ����ֵ��	ָ��string��ָ��
 */
char *My_DoubleToString(double value,char* str,int len,int f_len,char zero);

char *longToString(long value,char* str,int len);

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
unsigned char CheckPrgName(char* name);

unsigned char GetExStorageState(long* prg_num);
unsigned char GetExPrgName(Uint32 num,char* prg_name);
unsigned char ExportProgram(char* prg_name,char* new_name,unsigned char mode);
unsigned char ImportProgram(Uint32 read_num,char* save_name,unsigned char mode);


/**************************************
 * �������ܣ�������ͣ״̬�³��������ͨ�Ź���
 * �����������
 * ����ֵ��   ��
 */
void Pause_network_function(void);//������//2018/06/01
#endif /* PICKUPTEXTCODE_H_ */
