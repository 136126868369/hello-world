#ifndef MAN_MCX314AS_H_
#define MAN_MCX314AS_H_

#define MCX_DIR  GpioDataRegs.GPADAT.bit.GPIO25
#define MCX_RET  GpioDataRegs.GPADAT.bit.GPIO24
/******************************MCX312 ����/��� �ܽŶ���**********************************/

#define   X00_4      0x0001  //X���ź�����ܽ�out0
#define   X01_4      0x0002  //X���ź�����ܽ�out1
#define   X02_4      0x0004  //X���ź�����ܽ�out2
#define   X03_4      0x0008  //X���ź�����ܽ�out3
#define   X04_4      0x0101  //X���ź�����ܽ�out4
#define   X05_4      0x0201  //X���ź�����ܽ�out5
#define   X06_4      0x0401  //X���ź�����ܽ�out6
#define   X07_4      0x0801  //X���ź�����ܽ�out7

#define   Y00_4      0x0010  //Y���ź�����ܽ�out0
#define   Y01_4      0x0020  //Y���ź�����ܽ�out0
#define   Y02_4      0x0040  //Y���ź�����ܽ�out0
#define   Y03_4      0x0080  //Y���ź�����ܽ�out0
#define   Y04_4      0x0102  //Y���ź�����ܽ�out0
#define   Y05_4      0x0202  //Y���ź�����ܽ�out0
#define   Y06_4      0x0402  //Y���ź�����ܽ�out0
#define   Y07_4      0x0802  //Y���ź�����ܽ�out0

#define   XALM_4     0x0080  //X���ŷ����������淴���ź�����ܽ�
#define   YALM_4     0x8000  //Y���ŷ����������淴���ź�����ܽ�

//******************************mcx312 �Ĵ�����ַ����***********************************/
#define   WR0          0x00             //����Ĵ���
#define   WR1          0x01             //�Ĵ�����ʽ1
#define   WR2          0x02             //�Ĵ�����ʽ2
#define   WR3          0x03             //�Ĵ�����ʽ3
#define   WR4          0x04             //����Ĵ���
#define   WR5          0x05             //�岹��ʽ�Ĵ���
#define   WR6          0x06             //д�Ĵ�����С����λ
#define   WR7          0x07             //д�Ĵ����������λ

#define   RR0          0x00             //��״̬�Ĵ���
#define   RR1          0x01             //�Ĵ�1״̬
#define   RR2          0x02             //�Ĵ�2״̬
#define   RR3          0x03             //�Ĵ�3״̬
#define   RR4          0x04             //�Ĵ�1����
#define   RR5          0x05             //�Ĵ�2����
#define   RR6          0x06             //���Ĵ�����С����λ
#define   RR7          0x07             //���Ĵ����������λ

extern  void WriteWR1   (unsigned int axis,unsigned int wdata);
extern  void WriteWR2   (unsigned int axis,unsigned int wdata);
extern  void WriteWR3   (unsigned int axis,unsigned int wdata);
extern  void WriteWR4   (unsigned int wdata);
extern  void WriteWR5   (unsigned int axis,unsigned int wdata);
extern  void WriteMCX312_WR4(unsigned int wdata);
void SetRange(unsigned int axis,long wdata);
void SetAcac(unsigned int axis,unsigned int wdata);
void SetAcc(unsigned int axis,unsigned int wdata);
void SetDec(unsigned int axis,unsigned int wdata);
void SetStartV(unsigned int axis,unsigned int wdata);
void SetPulseFreq(unsigned int axis,unsigned int wdata);
void SetPulse(unsigned int axis,long wdata);
void SetLp(unsigned int axis,long wdata);
void SetEp(unsigned int axis,long wdata);
void SetCompp(unsigned int axis,long wdata);
void SetCompm(unsigned int axis,long wdata);
void SetAo(unsigned int axis,long wdata);
void SetDriCmd(unsigned int axis,unsigned int cmd);
unsigned int ReadRR1(unsigned int axis);
unsigned int ReadRR2(unsigned int axis);
unsigned int ReadMCX312_RR4(void);
unsigned int ReadMCX312_RR5(void);
unsigned int ReadMCX314_RR4(void);
long ReadPulseLp(unsigned int axis);
long ReadPulseEp(unsigned int axis);
unsigned int ReadCv(unsigned int axis);
unsigned int ReadDriState(void);
unsigned int ReadErrState(void);
void InitMcx31x(void);
#endif
