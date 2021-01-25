/*
 * ReadEncoder.h
 *
 *  Created on: 2015-3-19
 *      Author: rd49
 */

#ifndef READENCODER_H_
#define READENCODER_H_
#include "My_Project.h"
/**********************伺服控制口定义***************************/
#define SetAbsr         GpioDataRegs.GPADAT.bit.GPIO27=1
#define ResetAbsr       GpioDataRegs.GPADAT.bit.GPIO27=0

#define SetAbsm1        GpioDataRegs.GPBDAT.bit.GPIO48=1
#define ResetAbsm1      GpioDataRegs.GPBDAT.bit.GPIO48=0

#define SetAbsm2        GpioDataRegs.GPBDAT.bit.GPIO49=1
#define ResetAbsm2      GpioDataRegs.GPBDAT.bit.GPIO49=0

#define SetAbsm3        GpioDataRegs.GPBDAT.bit.GPIO50=1
#define ResetAbsm3      GpioDataRegs.GPBDAT.bit.GPIO50=0

#define SetAbsm4        GpioDataRegs.GPBDAT.bit.GPIO51=1
#define ResetAbsm4      GpioDataRegs.GPBDAT.bit.GPIO51=0

#define SetAbsm5        GpioDataRegs.GPBDAT.bit.GPIO52=1
#define ResetAbsm5      GpioDataRegs.GPBDAT.bit.GPIO52=0

#define SetAbsm6        GpioDataRegs.GPBDAT.bit.GPIO53=1
#define ResetAbsm6      GpioDataRegs.GPBDAT.bit.GPIO53=0

#define J1_RD           GpioDataRegs.GPADAT.bit.GPIO0	//X1	GPIO0
#define J2_RD           GpioDataRegs.GPADAT.bit.GPIO3//x4		GPIO3
#define J3_RD           GpioDataRegs.GPADAT.bit.GPIO6//x7		GPIO6
#define J4_RD           GpioDataRegs.GPADAT.bit.GPIO9//x10		GPIO9
#define J5_RD           GpioDataRegs.GPADAT.bit.GPIO12//x13		GPIO12
#define J6_RD           GpioDataRegs.GPADAT.bit.GPIO17//x16		GPIO17
#define J1_ABSB0        GpioDataRegs.GPADAT.bit.GPIO1//x2		GPIO1
#define J1_ABSB1        GpioDataRegs.GPADAT.bit.GPIO2//x3	GPIO2
#define J2_ABSB0        GpioDataRegs.GPADAT.bit.GPIO4//x5	GPIO4
#define J2_ABSB1        GpioDataRegs.GPADAT.bit.GPIO5//x6	GPIO5
#define J3_ABSB0        GpioDataRegs.GPADAT.bit.GPIO7//x8	GPIO7
#define J3_ABSB1        GpioDataRegs.GPADAT.bit.GPIO8//x9		GPIO8
#define J4_ABSB0        GpioDataRegs.GPADAT.bit.GPIO10//x11	GPIO10
#define J4_ABSB1        GpioDataRegs.GPADAT.bit.GPIO11//x12	GPIO11
#define J5_ABSB0        GpioDataRegs.GPADAT.bit.GPIO13//x14	GPIO13
#define J5_ABSB1        GpioDataRegs.GPADAT.bit.GPIO16//x15	GPIO16
#define J6_ABSB0        GpioDataRegs.GPADAT.bit.GPIO18//x17	GPIO18
#define J6_ABSB1        GpioDataRegs.GPADAT.bit.GPIO19//x18	GPIO19
#define ABST            GpioDataRegs.GPADAT.bit.GPIO26//gpio26

unsigned char ReadEncoderPulseAll(long *pulseData);
unsigned char ReadEncoderPulse(unsigned char Axis,long *pulseData,unsigned char mode);

#endif /* READENCODER_H_ */
