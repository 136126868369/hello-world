/*
 * Sanxie_Servo.c
 *
 *  Created on: 2016-11-27
 *      Author: rd49
 */
#include "Sanxie_Servo.h"
#include "My_Delay.h"
#include "My_USART.h"
extern unsigned int USART_RX_STA_B;		// ����b�Ľ���״̬���
extern char USART_B_RX_BUF[32];
///************************************
// * ��ȡ�ŷ���������
// * ���������	len�����ݳ���
// *
// * ����ֵ��NONE
// */
//void SendCmdToServo(u8 len,u8 addr,u8 cmd,u16 par)
//{
//	u8 data_buf[8];
//	u16 crc;
//	data_buf[0] = len | 0x20;	//���ݳ���
//	data_buf[1] = addr;			//��ַ
//	data_buf[2] = 0x00;			//���ƴ���
//	data_buf[3] = cmd;			//�������
//	data_buf[4] = par>>8;		//����
//	data_buf[5] = par&0x00ff;	//����
//	crc = crc_cal_by_bit(data_buf,6);
//	data_buf[6] = crc>>8;		//CRC16
//	data_buf[7] = crc&0x00ff;	//CRC16
//	SendDataSciB(data_buf,8);
//}


/************************************
 * �����ŷ�����
 * ���������	addr:��ַ
 *				cmd:������루0~0x66��
 *				�����������������data�ĳ���(�ֽ�):
 *				NOP					0
 *				SET_PARAM_2			2
 *				SET_PARAM_4			4
 *				UNLOCK_PARAM_ALL	0
 *				SAVE_PARAM_ALL		2
 *				CLEAR_EA05_DATA		2
 *				SET_STATE_VALUE_WITHMASK_4	8�����������ʹ��GetServoPARAM���������ã�
 *
 *				para:������ţ���Щ���������Ĳ���Ϊ�յģ���ʱpara��Ϊ����ֵ���ɣ�����
 *					NOP,UNLOCK_PARAM_ALL
 *
 *				data:����para������ֵ����Щpara��û��data�ģ���ʱdataΪ����ֵ���ɣ����£�
 *					NOP,UNLOCK_PARAM_ALL,CLEAR_EA05_DATA,SAVE_PARAM_ALL
 * ����ֵ��ͨ�Ž����0 ->OK  ������
 * 					1 ->�쳣���
 * 					2 ->���յ�δ������������
 * 					3 ->��Ϣ��ʽ����ȷ
 * 					4 ->����ģʽ��Ч
 * 					5 ->�ڲ�״̬��Ч
 * 					6 ->����ֵ������Χ
 * 					7 ->�ܾ���ȡ
 * 					8 ->�������ʧ��
 * 					16->���ݽ��մ���(У�����)
 * 					17->Ӧ��ʱ
 * 					18->������벻�������ò����Ĺ���
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

	data_buf[1] = addr;			//��ַ
	data_buf[2] = 0x00;			//���ƴ���
	data_buf[3] = cmd;			//�������
	if(cmd == SX_NOP || cmd==SX_UNLOCK_PARAM_ALL)
	{
		data_buf[0] = (data_len+2) | 0x20;//���ݴ��볤��(���ƴ���(1)+�������(1)+para(0)+data_len��
		i=4;
	}
	else
	{
		data_buf[0] = (data_len+4) | 0x20;//���ݴ��볤��(���ƴ���(1)+�������(1)+para(2)+data_len��
		data_buf[4] = (para>>8) & 0x00ff;
		data_buf[5] = para & 0x00ff;
		i=6;
	}

	for(j=0;j<data_len;j++)
	{
		data_buf[i++] = (data>>((data_len-1-j)*8)) & 0x00ff;
	}

	crc = crc_cal_by_bit(data_buf,i);	//У�����
	data_buf[i++] = crc>>8;		//CRC16
	data_buf[i++] = crc&0x00ff;	//CRC16
	SendDataSciB(data_buf,i);
	USART_RX_STA_B = 0;
	times = 0;
	while(1)
	{
		if(USART_RX_STA_B&0x8000)//У���ڴ����ж����Ѿ�����
		{
			if(USART_RX_STA_B&0x4000)//�������ݳ���
			{
				GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485����ģʽ
				delay_1ms(50);
				USART_RX_STA_B = 0;
				return 16;
			}
			if(USART_B_RX_BUF[2]&0x0f)
				return (USART_B_RX_BUF[2]&0x0f);//bit0~bit4:Ӧ����Ϣ���쳣���
//
//			len = USART_B_RX_BUF[0]&0x001f;//bit0~bit5:��ô˴ν��յ������ݵĳ���
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
			GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485����ģʽ
			delay_1ms(2);
			InitSci();
			delay_1ms(10);
			return 17;	//��ʱ 300ms
		}
	}
	GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485����ģʽ
	return 0;
}

/************************************
 * ��ȡ�ŷ�����
 * ���������	addr:��ַ
 *				cmd:������루0~0x66��
 *				para:������ţ���Щ���������Ĳ���Ϊ�յģ���ʱpara��Ϊ����ֵ���ɣ�����
 *					NOP,UNLOCK_PARAM_ALL
 *				data:�����ò���ʱ��data���������ò�����ֵ
 *					  �ڻ�����ݺ����ָ��ָ����ڴ潫�ᱣ����յ�������
 *				len�����ݳ���(ָdata�ĳ���,0~29)
 * ����ֵ��ͨ�Ž����0 ->OK  ������
 * 					1 ->�쳣���
 * 					2 ->���յ�δ������������
 * 					3 ->��Ϣ��ʽ����ȷ
 * 					4 ->����ģʽ��Ч
 * 					5 ->�ڲ�״̬��Ч
 * 					6 ->����ֵ������Χ
 * 					7 ->�ܾ���ȡ
 * 					8 ->�������ʧ��
 * 					16->���ݽ��մ���(У�����)
 * 					17->Ӧ��ʱ
 */
u8 GetServoPARAM(u8 addr,u8 cmd,u16 para,u8* data,u8 len)
{
	u8 data_buf[35],i,j,l;
	u16 crc,times;

	//data_buf[0] = (len+2) | 0x20;	//���ݴ��볤��(������ݴ��볤���ǿ��ƴ���(1)+�������(1)+����(len)���ܺͣ�
	data_buf[1] = addr;			//��ַ
	data_buf[2] = 0x00;			//���ƴ���
	data_buf[3] = cmd;			//�������

	if((cmd==SX_NOP) || (cmd==SX_UNLOCK_PARAM_ALL))//�����������������ݲ���û���ݵ�
	{
		data_buf[0] = (0+2) | 0x20;//���ݴ��볤��(���ƴ���(1)+�������(1)+para(0)+data_len)
		i=4;
	}
	else
	{
		data_buf[0] = (len+4) | 0x20;//���ݴ��볤��(���ƴ���(1)+�������(1)+para(2)+data_len)
		data_buf[4] = (para>>8)&0x00ff;
		data_buf[5] = para & 0x00ff;
		i=6;
	}

	for(j=0;j<len;j++)
	{
		data_buf[i++] = data[j];
	}

	crc = crc_cal_by_bit(data_buf,i);	//У�����

	data_buf[i++] = crc>>8;		//CRC16
	data_buf[i++] = crc&0x00ff;	//CRC16

	SendDataSciB(data_buf,i);

	USART_RX_STA_B = 0;
	times = 0;
	while(1)
	{
		if(USART_RX_STA_B&0x8000)//У���ڴ����ж����Ѿ�����
		{
			if(USART_RX_STA_B&0x4000)//�������ݳ���
			{
				GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485����ģʽ
				delay_1ms(50);
				USART_RX_STA_B = 0;
				return 16;
			}
			if(USART_B_RX_BUF[2]&0x0f)
				return USART_B_RX_BUF[2]&0x0f;//bit0~bit4:Ӧ����Ϣ���쳣���

			l = USART_B_RX_BUF[0]&0x001f;//bit0~bit5:��ô˴ν��յ������ݴ���ĳ���
			l=l-2;	//2�Ǵ��벿�ĳ���(���ƴ���+�������)��ʣ�µ������ݵĳ���
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
			GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485����ģʽ
			delay_1ms(2);
			InitSci();
			delay_1ms(10);
			return 17;	//��ʱ 300ms
		}
	}
	GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485����ģʽ
	return 0;
}
