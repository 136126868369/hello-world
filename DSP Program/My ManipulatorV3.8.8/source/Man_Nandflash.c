#include "My_Project.h"
#include "Man_Nandflash.h"
#include "EditProgram.h"
#include "DSP28335_Spi.h"
/*0��Ϊ��Ϣ��¼��*/
unsigned  int  *Ex0Start = (unsigned  int *)0x004000;
#define	READ	0
#define 	WRITE	0
#define	ALE		1
#define 	CLE		2
#define WRITE_ADDR	*(Ex0Start+ALE)
#define WRITE_CMD		*(Ex0Start+CLE)
#define READ_DATA		*(Ex0Start+READ)
#define WRITE_DATA	*(Ex0Start+WRITE)
static void NAND_WRITE_CMD          (unsigned char CMD);
static void NAND_WRITE_ADD          (unsigned char ADD);


//**********************************NAND��ʼ��***********************************/
void NAND_INIT(void)
{
	 NAND_WRITE_CMD(COMM_Reset);
}
//**********************************NANDд����***********************************/
void NAND_WRITE_CMD(unsigned char CMD)
{
	 delay_200ns();delay_200ns();
	 WRITE_CMD = CMD;
	 delay_200ns();delay_200ns();
}
//**********************************NANDд��ַ***********************************/
void NAND_WRITE_ADD(unsigned char ADD)
{
	 delay_200ns();delay_200ns();
	 WRITE_ADDR = ADD;
	 delay_200ns();delay_200ns();
}
//**********************************NAND��ID***********************************/
unsigned int ReadNandFlashId(void)
{
	 unsigned int ID; //IDΪECF1
	 NAND_WRITE_CMD(COMM_ReadID);
	 delay_1us(2);
	 delay_200ns();
	 WRITE_ADDR = 0x00;
	 delay_200ns();
	 delay_1us(1);
	 ID = READ_DATA&0xFF;
	 delay_1us(1);
	 ID <<= 8;
	 delay_1us(1);
	 ID = ID|(READ_DATA&0xFF);
	 delay_1ms(5);
	 return ID;
}
/*****************************************
 * NAND�����
 * ������� block����Ҫ�����Ŀ�
 * ����ֵ	state��0�������ɹ�	1������ʧ��
 */
unsigned char EraseNandFlashBlock(unsigned int Block)
{
	 unsigned char state=1;
	 Block <<= 6;
	 NAND_WRITE_CMD(COMM_BlockErase1);
	 NAND_WRITE_ADD(Block&0xFF);
	 NAND_WRITE_ADD(Block>>8);
	 NAND_WRITE_CMD(COMM_BlockErase2);
	 NAND_WRITE_CMD(COMM_ReadStatus);
	 delay_1ms(3);
	 state = (READ_DATA&0x01);
	 delay_1ms(3);
	 return state;
}
/*****************************************
 * ���ܣ�NANDҳд����
 * ������PageADDR ҳ��ַ
 * 		 ByteADDR �е�ַ
 * 		 Length   ��Ҫд��ĳ���
 * 		 Mode	  0��һ��д1byte   1:1��д2byte
 * 		 		  (����뱣�����������Ϊchar����Mode=0��
 * 		 		   ����뱣����������������ͣ���Mode=1��
 * 		 		   ��Ҫע�����sizeof(char)��sizeof(int)��������Ʊ�����ԭ��)��ֵ��Ϊ1��
 * 		 		   Ҳ��ϵchar��int����ռ16bit�ģ����ע�⣡����
 * 		 nanddata Ҫд������ݵ�ָ��
 * ����ֵ��state  0:д�ɹ�	1��дʧ��
 */
unsigned char NAND_WRITE_PAG(unsigned long PageADDR,unsigned int ByteADDR,unsigned int Length,unsigned char Mode,unsigned char *nanddata)
{
	 unsigned int   i;
	 unsigned char  state=1;
	 delay_1us(4);
	 NAND_WRITE_CMD(COMM_PageWrite1);
	 delay_1us(4);
	 NAND_WRITE_ADD(ByteADDR&0xFF);
	 delay_1us(4);
	 NAND_WRITE_ADD(ByteADDR>>8);
	 delay_1us(4);
	 NAND_WRITE_ADD(PageADDR&0xFF);
	 delay_1us(4);
	 NAND_WRITE_ADD(PageADDR>>8);
	 delay_1us(4);

	 for(i=0; i<Length; i++)
	 {
			 WRITE_DATA = *nanddata&0xFF;
			 delay_1us(4);
			 if(Mode)
			 {
				    WRITE_DATA = *nanddata>>8;
				    delay_1us(4);
			 }
			 nanddata++;
	 }

	 NAND_WRITE_CMD(COMM_PageWrite2);
	 delay_1us(50);
	 NAND_WRITE_CMD(COMM_ReadStatus);
	 delay_1us(100);
	 state = READ_DATA&0x01;
	 delay_1ms(4);
	 return(state);
}
/************************************
 * ���ܣ�NANDҳ������
 * ������PageADDR ҳ��ַ
 * 		 ByteADDR �е�ַ
 * 		 Length   ��Ҫ�����ĳ���
 * 		 Mode	  0��һ�ζ�1byte   1:1�ζ�2byte
 * 		 		  (������ȡ����������Ϊchar����Mode=0��
 * 		 		   ������ȡ���������������ͣ���Mode=1��
 * 		 		   ��Ҫע�����sizeof(char)��sizeof(int)��������Ʊ�����ԭ��)��ֵ��Ϊ1��
 * 		 		   Ҳ��ϵchar��int����ռ16bit�ģ����ע�⣡����
 * 		 nanddata Ҫ��ŵ����ݵ�ָ��
 */
void NAND_READ_PAG(unsigned long PageADDR, unsigned int ByteADDR,unsigned int Length,unsigned char Mode,unsigned char *nanddata)
{
	 unsigned int   i;
	 delay_1us(3);
	 NAND_WRITE_CMD(COMM_PageRead1);delay_1us(3);
	 NAND_WRITE_ADD(ByteADDR&0xFF);delay_1us(3);
	 NAND_WRITE_ADD(ByteADDR>>8);delay_1us(3);
	 NAND_WRITE_ADD(PageADDR&0xFF);delay_1us(3);
	 NAND_WRITE_ADD(PageADDR>>8);delay_1us(3);
	 NAND_WRITE_CMD(COMM_PageRead2);delay_1us(3);
	 for(i=0; i<Length; i++)
	 {
		  delay_1us(3);
		   *nanddata = READ_DATA&0xFF;
		   delay_1us(3);
		   if(Mode)
		   {
				delay_1us(3);
				*nanddata |= (READ_DATA<<8);
				delay_1us(3);
		   }
		   nanddata++;
	 }
	 delay_1ms(5);
}
/*****************************************
 * �������ܣ���ʽ��Flash
 * ���������NONE
 * �����������������
 */
unsigned char FormatFlash(void)
{
	 unsigned int i,j=0;
	 //unsigned int e[10]={0};
     for(i=0;i<1024;i++)
     {
    	  if(EraseNandFlashBlock(i))
    	  {
    		   //e[j]=i;
    		   j++;
    	  }
    	  delay_1us(1);
     }
     return j;
}
/************************************
 * ��⻵��
 * ������block ��Ҫ���Ŀ��ַ
 * ����ֵ 1���ÿ�
 * 		  0������
 */
unsigned char CheckInvalidBlock(unsigned int block)
{
	 unsigned char err1,err2;

	 NAND_READ_PAG(block*64,2048,1,0,&err1);
	 NAND_READ_PAG(block*64+1,2048,1,0,&err2);

	 if((err1 != 0xff) || (err2 != 0xff))//
	 {
		  return 0;
	 }
	 return 1;
}
/************************************
 * ���ܣ�NandFlash	д����,�ܹ�ʵ����ͬһ���У�������ҳ����д�����ⳤ�����ݣ����������洢
 * ������block		���ַ
 * 		 page		ҳ��ַ
 * 		 byte		�е�ַ
 * 		 length		��Ҫд��ĳ���
 * 		 Mode	  	0��һ��д1byte   1:1��д2byte
 * 		 		  	(����뱣�����������Ϊchar����Mode=0������뱣����������������ͣ���Mode=1��
 * 		 		   	��Ҫע�����sizeof(char)��sizeof(int)��������Ʊ�����ԭ��)��ֵ��Ϊ1��
 * 		 		   	Ҳ��char��int����ռ16bit�ģ����ע�⣡����
 * 		 data		Ҫд������ݵ�ָ��
 * ����ֵ err		0��д�ɹ�	1��дʧ��
 */
unsigned char WriteNandFlashData(unsigned int block,unsigned int page,unsigned int byte,unsigned int length,
								 unsigned char mode,unsigned char *data)
{
	 unsigned char i,state=0,times;
	 unsigned int value,left;
	 value = 1024*(2-mode);
	 if(length<= (value-byte))
	 {
		  state = NAND_WRITE_PAG(block*64+page,byte,length,mode,data);
	 }
	 else
	 {
		  times = (length-(value-byte))/value;//һҳ���ֻ�ܴ�1024*(2-mode)byte,����1024*(2-mode)�ֶ�ҳ����
		  //left = (length-byte)%value;
		  left  = (length-(value-byte))%value;//������2018/06/01
		  state = NAND_WRITE_PAG(block*64+page,byte,(value-byte),mode,data);
		  for(i=0;i<times;i++)
		  {
			 state |= NAND_WRITE_PAG(block*64+page+i+1,0,value,mode,data+value*i+(value-byte));
		  }
		  state |= NAND_WRITE_PAG(block*64+page+i+1,byte,left,mode,data+value*i+(value-byte));
	 }
	 return state;
}
/************************************
 * ���ܣ�NandFlash	������
 * ������block		���ַ
 * 		 page		ҳ��ַ
 * 		 byte		�е�ַ
 * 		 length		��Ҫ��ȡ�ĳ���
 * 		 Mode	  	0��һ��ȡ1byte   1:һ��ȡ2byte
 * 		 		  	(����뱣�����������Ϊchar����Mode=0������뱣����������������ͣ���Mode=1��
 * 		 		   	��Ҫע�����sizeof(char)��sizeof(int)��������Ʊ�����ԭ��)��ֵ��Ϊ1��
 * 		 		   	Ҳ��char��int����ռ16bit�ģ����ע�⣡����
 * 		 data		Ҫ�������ݵ�ָ��
 */
void ReadNandFlashData(unsigned int block,unsigned int page,unsigned int byte,unsigned int length,
								 unsigned char mode,unsigned char *data)
{
	 unsigned char i,times;
	 unsigned int value,left;
	 value = 1024*(2-mode);
	 if(length<= (value-byte))
	 {
		  NAND_READ_PAG(block*64+page,byte,length,mode,data);
	 }
	 else
	 {
		  times = (length-(value-byte))/value;//һҳ���ֻ�ܴ�1024*(2-mode)byte,����1024*(2-mode)�ֶ�ҳ����
		  //left = (length-byte)%value;
		  left =  (length-(value-byte))%value;//������2018/06/01
		  NAND_READ_PAG(block*64+page,byte,(value-byte),mode,data);
		  for(i=0;i<times;i++)
		  {
			   NAND_READ_PAG(block*64+page+i+1,0,value,mode,data+value*i+(value-byte));
		  }
		  NAND_READ_PAG(block*64+page+i+1,byte,left,mode,data+value*i+(value-byte));
	 }
}
void ReadAndSendProgramName(void)
{
	u8 program_num=0,i=0,j=0;
	u8 program_name[20];
	program_num = GetProgramNum();
	SendProgramNum(program_num);
	for(i=0;i<20;i++)
	{
		program_name[i]=0x00;
	}
	for(i=0; i<(program_num+1); i++)
	{
		j = GetProgramName(i, program_name);
		if(j==0)
		{
			SendProgramName(program_name);
			 //delay_1ms(10);
		}
	}
}
//******************************The end***********************************/

