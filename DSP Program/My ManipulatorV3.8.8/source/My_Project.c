/*
 * My_Project.c
 *
 *  Created on: 2015-3-18
 *      Author: rd49
 */

#include "My_Project.h"
#include "Man_DriverScreen.h"
#include "kinematic_explain.h"
union ButtonStru G_CTRL_BUTTON={0x45};//00000101B

double PulseOfCircle_INVERSE = 0.00005;	//������ȵĵ���

#pragma  DATA_SECTION (ROBOT_PARAMETER, ".MY_MEM0")
RobotParaStru ROBOT_PARAMETER;
struct EX_Position_stru EX_POSITION={0,{0,0,0}};
//У�鹫ʽ
u16 crc_cal_by_bit(u8* ptr, u32 len)
{
    unsigned long crc = 0xffff;
    unsigned char i;
    while((len--)!=0)
	{
        for(i=0x80;i!=0;i = i>>1)
        {
        	crc = crc << 1;//crc = crc * 2;
            if((crc&0x10000)!= 0) //��һλCRC�� 2������λ��1������� 0x11021
                crc = crc^0x11021;
            if(((*ptr)&i)!=0)    //�����λ��1����ôCRC = ��һλ��CRC + ��λ/CRC_CCITT
                crc = crc^0x1021;//CRC-CCITT����ʽ
        }
        ptr++;
	}
	return crc;
}
//===========================================================================
// No more.
//===========================================================================


