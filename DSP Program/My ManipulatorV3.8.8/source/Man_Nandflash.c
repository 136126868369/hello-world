#include "My_Project.h"
#include "Man_Nandflash.h"
#include "EditProgram.h"
#include "DSP28335_Spi.h"
/*0块为信息记录块*/
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


//**********************************NAND初始化***********************************/
void NAND_INIT(void)
{
	 NAND_WRITE_CMD(COMM_Reset);
}
//**********************************NAND写命令***********************************/
void NAND_WRITE_CMD(unsigned char CMD)
{
	 delay_200ns();delay_200ns();
	 WRITE_CMD = CMD;
	 delay_200ns();delay_200ns();
}
//**********************************NAND写地址***********************************/
void NAND_WRITE_ADD(unsigned char ADD)
{
	 delay_200ns();delay_200ns();
	 WRITE_ADDR = ADD;
	 delay_200ns();delay_200ns();
}
//**********************************NAND读ID***********************************/
unsigned int ReadNandFlashId(void)
{
	 unsigned int ID; //ID为ECF1
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
 * NAND块擦除
 * 输入参数 block：需要擦除的块
 * 返回值	state：0：擦除成功	1：擦除失败
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
 * 功能：NAND页写数据
 * 参数：PageADDR 页地址
 * 		 ByteADDR 列地址
 * 		 Length   需要写入的长度
 * 		 Mode	  0：一次写1byte   1:1次写2byte
 * 		 		  (如果想保存的数据类型为char，则Mode=0；
 * 		 		   如果想保存的其他的数据类型，则Mode=1；
 * 		 		   需要注意的是sizeof(char)和sizeof(int)在这里（估计编译器原因)的值都为1，
 * 		 		   也即系char和int都是占16bit的，务必注意！！）
 * 		 nanddata 要写入的数据的指针
 * 返回值：state  0:写成功	1：写失败
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
 * 功能：NAND页读数据
 * 参数：PageADDR 页地址
 * 		 ByteADDR 列地址
 * 		 Length   需要读出的长度
 * 		 Mode	  0：一次读1byte   1:1次读2byte
 * 		 		  (如果想读取的数据类型为char，则Mode=0；
 * 		 		   如果想读取的其他的数据类型，则Mode=1；
 * 		 		   需要注意的是sizeof(char)和sizeof(int)在这里（估计编译器原因)的值都为1，
 * 		 		   也即系char和int都是占16bit的，务必注意！！）
 * 		 nanddata 要存放的数据的指针
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
 * 函数功能：格式化Flash
 * 输入参数：NONE
 * 输出参数：坏块数量
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
 * 检测坏块
 * 参数：block 需要检测的块地址
 * 返回值 1：好块
 * 		  0：坏块
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
 * 功能：NandFlash	写数据,能够实现在同一块中，在任意页、列写入任意长度数据，并且连续存储
 * 参数：block		块地址
 * 		 page		页地址
 * 		 byte		列地址
 * 		 length		需要写入的长度
 * 		 Mode	  	0：一次写1byte   1:1次写2byte
 * 		 		  	(如果想保存的数据类型为char，则Mode=0；如果想保存的其他的数据类型，则Mode=1；
 * 		 		   	需要注意的是sizeof(char)和sizeof(int)在这里（估计编译器原因)的值都为1，
 * 		 		   	也即char和int都是占16bit的，务必注意！！）
 * 		 data		要写入的数据的指针
 * 返回值 err		0：写成功	1：写失败
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
		  times = (length-(value-byte))/value;//一页最多只能存1024*(2-mode)byte,超过1024*(2-mode)分多页储存
		  //left = (length-byte)%value;
		  left  = (length-(value-byte))%value;//修正于2018/06/01
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
 * 功能：NandFlash	读数据
 * 参数：block		块地址
 * 		 page		页地址
 * 		 byte		列地址
 * 		 length		需要读取的长度
 * 		 Mode	  	0：一次取1byte   1:一次取2byte
 * 		 		  	(如果想保存的数据类型为char，则Mode=0；如果想保存的其他的数据类型，则Mode=1；
 * 		 		   	需要注意的是sizeof(char)和sizeof(int)在这里（估计编译器原因)的值都为1，
 * 		 		   	也即char和int都是占16bit的，务必注意！！）
 * 		 data		要保存数据的指针
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
		  times = (length-(value-byte))/value;//一页最多只能存1024*(2-mode)byte,超过1024*(2-mode)分多页储存
		  //left = (length-byte)%value;
		  left =  (length-(value-byte))%value;//修正于2018/06/01
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

