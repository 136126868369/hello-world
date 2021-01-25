#ifndef MAN_NANDFLASH_H_
#define MAN_NANDFLASH_H_

#define INFO_BLOCK_ADDR			0x00	//信息块地址
#define INFO_BLOCK_BACK_ADDR	0x01	//备用信息块地址
#define INFO_BLOCK_ADDR1		0x02	//信息块地址1 记录了机械手参数
#define INFO_BLOCK_ADDR2		0x03	//备用缓存块地址 记录了机械手参数

#define ROMDATA      GpioDataRegs.GPADAT.all
#define ROMCLE       GpioDataRegs.GPADAT.bit.GPIOA8  //命令锁存
#define ROMALE       GpioDataRegs.GPADAT.bit.GPIOA9  //地址锁存
#define ROMRE        GpioDataRegs.GPADAT.bit.GPIOA10 //读数据
#define ROMWE        GpioDataRegs.GPADAT.bit.GPIOA11 //写数据
#define ROMCE        GpioDataRegs.GPADAT.bit.GPIOA12 //使能端
#define ROMBUY       GpioDataRegs.GPADAT.bit.GPIOA13 //判忙端

#define ROMDATAIN	 EALLOW;GpioMuxRegs.GPADIR.all  = 0xDF00;EDIS;
#define ROMDATAOU	 EALLOW;GpioMuxRegs.GPADIR.all  = 0xDFFF;EDIS;

#define DELAY        NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;

#define COMM_ReadID      	0x90
#define COMM_PageWrite1    	0x80
#define COMM_PageWrite2    	0x10
#define COMM_PageRead1   	0x00
#define COMM_PageRead2 	    0x30
#define COMM_Reset       	0xFF
#define COMM_ReadBack1   	0x00
#define COMM_ReadBack2   	0x35
#define COMM_CachePGM1  	0x80
#define COMM_CachePGM2  	0x15
#define COMM_CopyBackPGM1   0x85
#define COMM_CopyBackPGM2   0x10
#define COMM_BlockErase1  	0x60
#define COMM_BlockErase2  	0xD0
#define COMM_ReadStatus  	0x70

void NAND_INIT(void);
void NAND_READ_PAG(unsigned long PageADDR, unsigned int ByteADDR,unsigned int Longth,unsigned char Mode,unsigned char *nanddata);
unsigned char NAND_WRITE_PAG(unsigned long PageADDR,unsigned int ByteADDR,unsigned int Length,unsigned char Mode,unsigned char *nanddata);
unsigned char FormatFlash(void);
extern unsigned char CheckInvalidBlock(unsigned int block);
extern unsigned int  ReadNandFlashId	(void);
extern unsigned char NandFlashCopyBack(unsigned long PageADDR1, unsigned int ByteADDR1,unsigned long PageADDR2, unsigned int ByteADDR2);
extern unsigned char EraseNandFlashBlock(unsigned int Block);
extern void ReadNandFlashData (unsigned int block,unsigned int page,unsigned int byte,unsigned int length,
									    unsigned char mode,unsigned char *data);
extern unsigned char WriteNandFlashData(unsigned int block,unsigned int page,unsigned int byte,unsigned int length,
									    unsigned char mode,unsigned char *data);
void ReadAndSendProgramName(void);
#endif /* MAN_NANDFLASH_H_ */
