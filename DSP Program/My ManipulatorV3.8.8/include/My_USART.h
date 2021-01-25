/*
 * My_USART.h
 *
 *  Created on: 2015-3-18
 *      Author: rd49
 */

#ifndef MY_USART_H_
#define MY_USART_H_
#include "My_Project.h"
//void SendChar2(unsigned char t);
//void SendStrings2(unsigned char *str,unsigned char length);
void sendData(unsigned char temp);//修正于//2018/06/01
void SendChar(unsigned char t);
void SendStrings(unsigned char *str);
void SetScibBaud(long buad_rate);//设置串口B波特率
void SendCharB(char t);
void SendStringB(char *str);
void SendDataSciB(u8 *ptr,u16 len);
#endif /* MY_USART_H_ */
