/*
 * My_USART.c
 *
 *  Created on: 2015-3-18
 *      Author: rd49
 */
#include "My_USART.h"

/******************************************
* ��    �ƣ� InitScibBuad()
* ��    �ܣ� ���ô���B�����ʣ�Ĭ��9600
* ��ڲ����� ������
* ���ڲ����� ��
 ******************************************/
void SetScibBaud(long buad_rate)
{
	 int brr;
	 brr=75000000/(buad_rate*8)-1;
	 ScibRegs.SCIHBAUD = brr>>8;    //200000  45
	 ScibRegs.SCILBAUD = brr&0x00ff;
}

/******************************************
* ��    �ƣ� SendChar2()
* ��    �ܣ� ����1���ֽ�
* ��ڲ����� t  ���͵��ֽ�
* ���ڲ����� ��
 ******************************************/
void SendCharB(char t)
{
     ScibRegs.SCITXBUF=t&0xFF;
	 while(!ScibRegs.SCICTL2.bit.TXRDY);
}

/******************************************
* ��    �ƣ� SendStringsB()
* ��    �ܣ� �����ַ���
* ��ڲ����� str  ���͵��ַ���
* ���ڲ����� ��
 ******************************************/
void SendStringB(char *str)
{
	   while(*str)
	   {
	       SendCharB(*(str++));
	   }
}
/******************************************
* ��    �ƣ� SendStringsB()
* ��    �ܣ� �����ַ���
* ��ڲ����� ptr  ���ݵ�ָ��
* 			len	���ݵĳ���
* ���ڲ����� ��
* */
void SendDataSciB(u8 *ptr,u16 len)
{
	u16 i=0;
	GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485����ģʽ
	delay_1ms(2);
	for(i=0;i<len;i++)
	{
		SendCharB(*(ptr++));
	}
	delay_1ms(2);
	GpioDataRegs.GPADAT.bit.GPIO20=0;	//RS485����ģʽ
}
/*****************************************************************
* ��    �ƣ� SendChar()
* ��    �ܣ� ����1���ֽ�
* ��ڲ����� t  ���͵��ֽ�
* ���ڲ����� ��
 *****************************************************************/
//������//2018/06/01(Ϊ�˼���CRCУ�飬�ʽ��ú����ƶ���Man_DriverScreen.c��)
/*
void  SendChar(unsigned char t)
{
      ScicRegs.SCITXBUF=t&0xFF;
	  while(!ScicRegs.SCICTL2.bit.TXRDY);
}
*/
/*****************************************************************
* ��    �ƣ� SendStrings()
* ��    �ܣ� �����ַ���
* ��ڲ����� str  ���͵��ַ���
* ���ڲ����� ��
 *****************************************************************/
void SendStrings(unsigned char *str)
{
	   while(*str)
	   {
	       SendChar(*(str++));
	   }
}




