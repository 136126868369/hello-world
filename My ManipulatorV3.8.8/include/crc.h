#ifndef _CRC_H_
#define _CRC_H_
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "My_Project.h"
#include "stdlib.h"

u16 GetCRC16(u8 *data,  u16 size);
u16 Crc16(u8 * pData, u16 nLength);
void AddCRC16(u8 *buffer,u16 n,u16 *pcrc);
u16 CheckCRC16(u8 *buffer,u16 n);
#endif  //_CRC_H_
