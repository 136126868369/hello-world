/*
 * Sanxie_Servo.c
 *
 *  Created on: 2016-11-27
 *      Author: rd49
 */
#include "Sanxie_Servo.h"
#include "My_Delay.h"
#include "My_USART.h"
extern unsigned int USART_RX_STA_B;		// 串口b的接收状态标记
extern char USART_B_RX_BUF[32];
///************************************
// * 读取伺服脉冲数据
// * 输入参数：	len：数据长度
// *
// * 返回值：NONE
// */
//void SendCmdToServo(u8 len,u8 addr,u8 cmd,u16 par)
//{
//	u8 data_buf[8];
//	u16 crc;
//	data_buf[0] = len | 0x20;	//数据长度
//	data_buf[1] = addr;			//地址
//	data_buf[2] = 0x00;			//控制代码
//	data_buf[3] = cmd;			//命令代码
//	data_buf[4] = par>>8;		//数据
//	data_buf[5] = par&0x00ff;	//数据
//	crc = crc_cal_by_bit(data_buf,6);
//	data_buf[6] = crc>>8;		//CRC16
//	data_buf[7] = crc&0x00ff;	//CRC16
//	SendDataSciB(data_buf,8);
//}


/************************************
 * 设置伺服参数
 * 输入参数：	addr:地址
 *				cmd:命令代码（0~0x66）
 *				以下命令代码所带的data的长度(字节):
 *				NOP					0
 *				SET_PARAM_2			2
 *				SET_PARAM_4			4
 *				UNLOCK_PARAM_ALL	0
 *				SAVE_PARAM_ALL		2
 *				CLEAR_EA05_DATA		2
 *				SET_STATE_VALUE_WITHMASK_4	8（这种情况请使用GetServoPARAM函数来设置）
 *
 *				para:参数编号，有些命令所带的参数为空的，此时para设为任意值即可，如下
 *					NOP,UNLOCK_PARAM_ALL
 *
 *				data:设置para参数的值，有些para是没有data的，此时data为任意值即可，如下：
 *					NOP,UNLOCK_PARAM_ALL,CLEAR_EA05_DATA,SAVE_PARAM_ALL
 * 返回值：通信结果，0 ->OK  其他：
 * 					1 ->异常结果
 * 					2 ->接收到未定义的命令代码
 * 					3 ->信息格式不正确
 * 					4 ->操作模式无效
 * 					5 ->内部状态无效
 * 					6 ->参数值超出范围
 * 					7 ->拒绝存取
 * 					8 ->锁定解除失败
 * 					16->数据接收错误(校验出错)
 * 					17->应答超时
 * 					18->命令代码不属于设置参数的功能
 */
u8 SetServoPARAM(u8 addr,u8 cmd,u16 para,long data)
{
	u8 data_buf[35],i=0,j,data_len;
	u16 crc,times;

	//data_buf[0] = (len+2) | 0x20;
	switch(cmd)
	{
	case SX_NOP:
		data_len=0;	break;

	case SX_SET_PARAM_2:
		data_len=2;	break;

	case SX_SET_PARAM_4:
		data_len=4;	break;

	case SX_UNLOCK_PARAM_ALL:
		data_len=0;	break;

	case SX_SAVE_PARAM_ALL:
		data_len=0;	break;

	case SX_CLEAR_EA05_DATA:
		data_len=0;	break;

	default: return 18;
	}

	data_buf[1] = addr;			//地址
	data_buf[2] = 0x00;			//控制代码
	data_buf[3] = cmd;			//命令代码
	if(cmd == SX_NOP || cmd==SX_UNLOCK_PARAM_ALL)
	{
		data_buf[0] = (data_len+2) | 0x20;//数据代码长度(控制代码(1)+命令代码(1)+para(0)+data_len）
		i=4;
	}
	else
	{
		data_buf[0] = (data_len+4) | 0x20;//数据代码长度(控制代码(1)+命令代码(1)+para(2)+data_len）
		data_buf[4] = (para>>8) & 0x00ff;
		data_buf[5] = para & 0x00ff;
		i=6;
	}

	for(j=0;j<data_len;j++)
	{
		data_buf[i++] = (data>>((data_len-1-j)*8)) & 0x00ff;
	}

	crc = crc_cal_by_bit(data_buf,i);	//校验计算
	data_buf[i++] = crc>>8;		//CRC16
	data_buf[i++] = crc&0x00ff;	//CRC16
	SendDataSciB(data_buf,i);
	USART_RX_STA_B = 0;
	times = 0;
	while(1)
	{
		if(USART_RX_STA_B&0x8000)//校验在串口中断中已经处理
		{
			if(USART_RX_STA_B&0x4000)//接收数据出错
			{
				GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485发送模式
				delay_1ms(50);
				USART_RX_STA_B = 0;
				return 16;
			}
			if(USART_B_RX_BUF[2]&0x0f)
				return (USART_B_RX_BUF[2]&0x0f);//bit0~bit4:应答信息：异常结果
//
//			len = USART_B_RX_BUF[0]&0x001f;//bit0~bit5:获得此次接收到的数据的长度
//
//			for(i=0;i<len;i++)
//			{
//				*data = USART_B_RX_BUF[i+4];
//			}
			USART_RX_STA_B = 0;
			break;
		}
		times++;
		if(cmd == SX_SAVE_PARAM_ALL)
		{
			delay_1ms(1);
		}
		else
		{
			delay_1us(60);
		}
		if(times>3000)
		{
			GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485发送模式
			delay_1ms(2);
			InitSci();
			delay_1ms(10);
			return 17;	//超时 300ms
		}
	}
	GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485发送模式
	return 0;
}

/************************************
 * 读取伺服参数
 * 输入参数：	addr:地址
 *				cmd:命令代码（0~0x66）
 *				para:参数编号，有些命令所带的参数为空的，此时para设为任意值即可，如下
 *					NOP,UNLOCK_PARAM_ALL
 *				data:在设置参数时，data保存了设置参数的值
 *					  在获得数据后，这个指针指向的内存将会保存接收到的数据
 *				len：数据长度(指data的长度,0~29)
 * 返回值：通信结果，0 ->OK  其他：
 * 					1 ->异常结果
 * 					2 ->接收到未定义的命令代码
 * 					3 ->信息格式不正确
 * 					4 ->操作模式无效
 * 					5 ->内部状态无效
 * 					6 ->参数值超出范围
 * 					7 ->拒绝存取
 * 					8 ->锁定解除失败
 * 					16->数据接收错误(校验出错)
 * 					17->应答超时
 */
u8 GetServoPARAM(u8 addr,u8 cmd,u16 para,u8* data,u8 len)
{
	u8 data_buf[35],i,j,l;
	u16 crc,times;

	//data_buf[0] = (len+2) | 0x20;	//数据代码长度(这个数据代码长度是控制代码(1)+命令代码(1)+数据(len)的总和）
	data_buf[1] = addr;			//地址
	data_buf[2] = 0x00;			//控制代码
	data_buf[3] = cmd;			//命令代码

	if((cmd==SX_NOP) || (cmd==SX_UNLOCK_PARAM_ALL))//这两个命令代码的数据部是没数据的
	{
		data_buf[0] = (0+2) | 0x20;//数据代码长度(控制代码(1)+命令代码(1)+para(0)+data_len)
		i=4;
	}
	else
	{
		data_buf[0] = (len+4) | 0x20;//数据代码长度(控制代码(1)+命令代码(1)+para(2)+data_len)
		data_buf[4] = (para>>8)&0x00ff;
		data_buf[5] = para & 0x00ff;
		i=6;
	}

	for(j=0;j<len;j++)
	{
		data_buf[i++] = data[j];
	}

	crc = crc_cal_by_bit(data_buf,i);	//校验计算

	data_buf[i++] = crc>>8;		//CRC16
	data_buf[i++] = crc&0x00ff;	//CRC16

	SendDataSciB(data_buf,i);

	USART_RX_STA_B = 0;
	times = 0;
	while(1)
	{
		if(USART_RX_STA_B&0x8000)//校验在串口中断中已经处理
		{
			if(USART_RX_STA_B&0x4000)//接收数据出错
			{
				GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485发送模式
				delay_1ms(50);
				USART_RX_STA_B = 0;
				return 16;
			}
			if(USART_B_RX_BUF[2]&0x0f)
				return USART_B_RX_BUF[2]&0x0f;//bit0~bit4:应答信息：异常结果

			l = USART_B_RX_BUF[0]&0x001f;//bit0~bit5:获得此次接收到的数据代码的长度
			l=l-2;	//2是代码部的长度(控制代码+命令代码)，剩下的是数据的长度
			for(i=0;i<l;i++)
			{
				*(data+i) = USART_B_RX_BUF[i+4];
			}
			USART_RX_STA_B = 0;
			break;
		}
		times++;
		delay_1us(60);
		if(times>3000)
		{
			GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485发送模式
			delay_1ms(2);
			InitSci();
			delay_1ms(10);
			return 17;	//超时 300ms
		}
	}
	GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485发送模式
	return 0;
}
