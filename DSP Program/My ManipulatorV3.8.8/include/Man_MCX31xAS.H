#ifndef MAN_MCX314AS_H_
#define MAN_MCX314AS_H_

#define MCX_DIR  GpioDataRegs.GPADAT.bit.GPIO25
#define MCX_RET  GpioDataRegs.GPADAT.bit.GPIO24
/******************************MCX312 输入/输出 管脚定义**********************************/

#define   X00_4      0x0001  //X轴信号输出管脚out0
#define   X01_4      0x0002  //X轴信号输出管脚out1
#define   X02_4      0x0004  //X轴信号输出管脚out2
#define   X03_4      0x0008  //X轴信号输出管脚out3
#define   X04_4      0x0101  //X轴信号输出管脚out4
#define   X05_4      0x0201  //X轴信号输出管脚out5
#define   X06_4      0x0401  //X轴信号输出管脚out6
#define   X07_4      0x0801  //X轴信号输出管脚out7

#define   Y00_4      0x0010  //Y轴信号输出管脚out0
#define   Y01_4      0x0020  //Y轴信号输出管脚out0
#define   Y02_4      0x0040  //Y轴信号输出管脚out0
#define   Y03_4      0x0080  //Y轴信号输出管脚out0
#define   Y04_4      0x0102  //Y轴信号输出管脚out0
#define   Y05_4      0x0202  //Y轴信号输出管脚out0
#define   Y06_4      0x0402  //Y轴信号输出管脚out0
#define   Y07_4      0x0802  //Y轴信号输出管脚out0

#define   XALM_4     0x0080  //X轴伺服驱动器警告反馈信号输入管脚
#define   YALM_4     0x8000  //Y轴伺服驱动器警告反馈信号输入管脚

//******************************mcx312 寄存器地址定义***********************************/
#define   WR0          0x00             //命令寄存器
#define   WR1          0x01             //寄存器方式1
#define   WR2          0x02             //寄存器方式2
#define   WR3          0x03             //寄存器方式3
#define   WR4          0x04             //输出寄存器
#define   WR5          0x05             //插补方式寄存器
#define   WR6          0x06             //写寄存器最小数据位
#define   WR7          0x07             //写寄存器最大数据位

#define   RR0          0x00             //主状态寄存器
#define   RR1          0x01             //寄存1状态
#define   RR2          0x02             //寄存2状态
#define   RR3          0x03             //寄存3状态
#define   RR4          0x04             //寄存1输入
#define   RR5          0x05             //寄存2输入
#define   RR6          0x06             //读寄存器最小数据位
#define   RR7          0x07             //读寄存器最大数据位

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
