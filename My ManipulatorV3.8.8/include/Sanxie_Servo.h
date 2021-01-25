/*
 * Sanxie_Servo.h
 *
 *  Created on: 2016-11-27
 *      Author: rd49
 */

#ifndef SANXIE_SERVO_H_
#define SANXIE_SERVO_H_
#include "My_Project.h"

// |---------------校验部分-------------|
// |-起始代码-| |--------数据代码--------|  |-校验-|
//  24    01     00    04   ** ** ... **    ** **
//|-A-| |-B-|  |-C-| |-D-|  |-----E-----|  |--F--|
//A:协议标头，bit7~bit5:固定0x1	bit4~bit0:数据代码的长度，2~31
//B:目的地址，1~31
//C:控制代码，bit7:0->发送(发送数据的时候,将此位设为1)
//				  1->接收(接收到数据的时候,此位收到为1)
//			 bit4~0:接收数据的时候，这几个位表示接收到的数据结果状态，0->正常结果  其他->异常结果
//					发送数据的时候设为0即可
//D:命令代码，0~0x66
//E:数据，长度为0~29
//F:校验，CRC-16-CCITT,两个字节,多项式：0x1021，初始值：0xFFFF，传送方向：左传
//



//void SendCmdToServo(u8 len,u8 addr,u8 cmd,u16 par);
u8 SetServoPARAM(u8 addr,u8 cmd,u16 para,long data);
u8 GetServoPARAM(u8 addr,u8 cmd,u16 para,u8* data,u8 len);

#define ABS_DATA		74	//ABS位置指令值数据
#define ABS_FEEDBACK	76	//ABS位置反馈值数据

#define ZHUANJU1		147	//转矩限制值1
#define ZHUANJU2		148	//转矩限制值2



//命令代码(SX:三协的拼音缩写)
#define SX_NOP			0x0
#define SX_GET_PARAM_2	0x04	//将设定于驱动器 RAM 的参数值，以 2 字节为一单位读出
#define SX_GET_PARAM_4	0x05	//将设定于驱动器 RAM 的参数值，以 4 字节为一单位读出
#define SX_SET_PARAM_2	0x07	//将参数值由 PC 写入驱动器 RAM，以 2 字节为一单位写入
#define SX_SET_PARAM_4	0x08	//以 4 字节为一单位，将参数值由 PC 写入驱动器RAM
#define SX_UNLOCK_PARAM_ALL	0x0a	//保存驱动器参数时，解锁写入操作锁定
#define SX_SAVE_PARAM_ALL	0x0b	//将设定于驱动器 RAM 中的全部参数，保存于驱动器
#define SX_GET_STATE_VALUE_2	0x10	//以 2 字节为一单位，从驱动器读出设定的状态值
#define SX_GET_STATE_VALUE_4	0x11	//以 4 字节为一单位，从驱动器读出设定的状态值
#define SX_READ_EA05_DATA		0x1e	//读出编码器数据
#define SX_CLEAR_EA05_DATA		0x1f	//清除编码器数据    para=1->清除报警  para=2->清除报警和多圈数据
#define SX_READ_EA05_DATA_EX	0x62	//读出编码器单圈数据与多圈数据
#define SX_SET_STATE_VALUE_WITHMASK_4	0x66	//驱动器的逻辑 I/O 的状态值以 4 字节单位设定


#endif /* SANXIE_SERVO_H_ */
