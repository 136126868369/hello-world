/*
 * My_USART.c
 *
 *  Created on: 2015-3-18
 *      Author: rd49
 */
#include "My_USART.h"

/******************************************
* 名    称： InitScibBuad()
* 功    能： 设置串口B波特率，默认9600
* 入口参数： 波特率
* 出口参数： 无
 ******************************************/
void SetScibBaud(long buad_rate)
{
	 int brr;
	 brr=75000000/(buad_rate*8)-1;
	 ScibRegs.SCIHBAUD = brr>>8;    //200000  45
	 ScibRegs.SCILBAUD = brr&0x00ff;
}

/******************************************
* 名    称： SendChar2()
* 功    能： 发送1个字节
* 入口参数： t  发送的字节
* 出口参数： 无
 ******************************************/
void SendCharB(char t)
{
     ScibRegs.SCITXBUF=t&0xFF;
	 while(!ScibRegs.SCICTL2.bit.TXRDY);
}

/******************************************
* 名    称： SendStringsB()
* 功    能： 发送字符串
* 入口参数： str  发送的字符串
* 出口参数： 无
 ******************************************/
void SendStringB(char *str)
{
	   while(*str)
	   {
	       SendCharB(*(str++));
	   }
}
/******************************************
* 名    称： SendStringsB()
* 功    能： 发送字符串
* 入口参数： ptr  数据的指针
* 			len	数据的长度
* 出口参数： 无
* */
void SendDataSciB(u8 *ptr,u16 len)
{
	u16 i=0;
	GpioDataRegs.GPADAT.bit.GPIO20=1;	//RS485发送模式
	delay_1ms(2);
	for(i=0;i<len;i++)
	{
		SendCharB(*(ptr++));
	}
	delay_1ms(2);
	GpioDataRegs.GPADAT.bit.GPIO20=0;	//RS485接收模式
}
/*****************************************************************
* 名    称： SendChar()
* 功    能： 发送1个字节
* 入口参数： t  发送的字节
* 出口参数： 无
 *****************************************************************/
//修正于//2018/06/01(为了加入CRC校验，故将该函数移动至Man_DriverScreen.c中)
/*
void  SendChar(unsigned char t)
{
      ScicRegs.SCITXBUF=t&0xFF;
	  while(!ScicRegs.SCICTL2.bit.TXRDY);
}
*/
/*****************************************************************
* 名    称： SendStrings()
* 功    能： 发送字符串
* 入口参数： str  发送的字符串
* 出口参数： 无
 *****************************************************************/
void SendStrings(unsigned char *str)
{
	   while(*str)
	   {
	       SendChar(*(str++));
	   }
}




