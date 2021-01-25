/*
 * Sanxie_Servo.h
 *
 *  Created on: 2016-11-27
 *      Author: rd49
 */

#ifndef SANXIE_SERVO_H_
#define SANXIE_SERVO_H_
#include "My_Project.h"

// |---------------У�鲿��-------------|
// |-��ʼ����-| |--------���ݴ���--------|  |-У��-|
//  24    01     00    04   ** ** ... **    ** **
//|-A-| |-B-|  |-C-| |-D-|  |-----E-----|  |--F--|
//A:Э���ͷ��bit7~bit5:�̶�0x1	bit4~bit0:���ݴ���ĳ��ȣ�2~31
//B:Ŀ�ĵ�ַ��1~31
//C:���ƴ��룬bit7:0->����(�������ݵ�ʱ��,����λ��Ϊ1)
//				  1->����(���յ����ݵ�ʱ��,��λ�յ�Ϊ1)
//			 bit4~0:�������ݵ�ʱ���⼸��λ��ʾ���յ������ݽ��״̬��0->�������  ����->�쳣���
//					�������ݵ�ʱ����Ϊ0����
//D:������룬0~0x66
//E:���ݣ�����Ϊ0~29
//F:У�飬CRC-16-CCITT,�����ֽ�,����ʽ��0x1021����ʼֵ��0xFFFF�����ͷ�����
//



//void SendCmdToServo(u8 len,u8 addr,u8 cmd,u16 par);
u8 SetServoPARAM(u8 addr,u8 cmd,u16 para,long data);
u8 GetServoPARAM(u8 addr,u8 cmd,u16 para,u8* data,u8 len);

#define ABS_DATA		74	//ABSλ��ָ��ֵ����
#define ABS_FEEDBACK	76	//ABSλ�÷���ֵ����

#define ZHUANJU1		147	//ת������ֵ1
#define ZHUANJU2		148	//ת������ֵ2



//�������(SX:��Э��ƴ����д)
#define SX_NOP			0x0
#define SX_GET_PARAM_2	0x04	//���趨�������� RAM �Ĳ���ֵ���� 2 �ֽ�Ϊһ��λ����
#define SX_GET_PARAM_4	0x05	//���趨�������� RAM �Ĳ���ֵ���� 4 �ֽ�Ϊһ��λ����
#define SX_SET_PARAM_2	0x07	//������ֵ�� PC д�������� RAM���� 2 �ֽ�Ϊһ��λд��
#define SX_SET_PARAM_4	0x08	//�� 4 �ֽ�Ϊһ��λ��������ֵ�� PC д��������RAM
#define SX_UNLOCK_PARAM_ALL	0x0a	//��������������ʱ������д���������
#define SX_SAVE_PARAM_ALL	0x0b	//���趨�������� RAM �е�ȫ��������������������
#define SX_GET_STATE_VALUE_2	0x10	//�� 2 �ֽ�Ϊһ��λ���������������趨��״ֵ̬
#define SX_GET_STATE_VALUE_4	0x11	//�� 4 �ֽ�Ϊһ��λ���������������趨��״ֵ̬
#define SX_READ_EA05_DATA		0x1e	//��������������
#define SX_CLEAR_EA05_DATA		0x1f	//�������������    para=1->�������  para=2->��������Ͷ�Ȧ����
#define SX_READ_EA05_DATA_EX	0x62	//������������Ȧ�������Ȧ����
#define SX_SET_STATE_VALUE_WITHMASK_4	0x66	//���������߼� I/O ��״ֵ̬�� 4 �ֽڵ�λ�趨


#endif /* SANXIE_SERVO_H_ */
