/*
 * BuildProgram.h
 *
 *  Created on: 2015-1-21
 *      Author: rd49
 */

#ifndef BUILDPROGRAM_H_
#define BUILDPROGRAM_H_
#include "EditProgram.h"
//�༭��������л���ֵ���ȷ��ָ��
#define code_start		0x01
#define code_dly		0x02
#define code_end		0x03
#define code_for		0x04
#define code_next		0x05
#define code_gosub		0x06
#define code_goto		0x07
#define code_stop		0x08
#define code_if			0x09
#define code_then		0x0a
#define code_else		0x0b
#define code_mov		0x0c
#define code_mvs		0x0d
#define code_mvh		0x0e
#define code_mvr		0x0f
#define code_mvc		0x10
#define code_ovrd		0x11
#define code_pin		0x12
#define code_pout		0x13
#define code_speed		0x14
#define code_wait		0x15
#define code_loop		0x16
#define code_sub		0x17
#define code_return		0x18
#define code_tray		0x19
#define code_t_x		0x1a
#define code_n_x		0x1b
#define code_p_x		0x1c
#define code_acc		0x1d
#define code_open		0x1e
#define code_baud		0x1f
#define code_rstd		0x20
#define code_disp		0x21
#define code_ft_x		0x22
#define code_finish		0x23
#define code_p_g		0x24	//��ʾλ��+ŷ���ǵ�����
#define code_rs232		0x25
#define code_toollen	0x26
#define code_tool		0x27
#define code_part		0x28
#define code_f_x		0x29	//fx�����ͱ���
#define code_network    0x2a    //������//2018/06/01 for network
#define code_teamwork   0x2b    //������//2018/06/01 for network
#define CORRECT			0x00	//������ȷ
//�༭��������л���ֵĴ�����Ϣ
#define ERR_CMD				0x31	//�������
#define ERR_START			0x32	//*start �ظ�
#define ERR_DLY				0x33	//��ʱ��Χ 0.1~999.9��
#define ERR_NO_END			0x34	//û��END����
#define ERR_TOO_MUCH_END	0x35	//end�ظ�
#define ERR_NO_P			0x36	//ȱ��Ŀ��λ��
#define ERR_P_OVERFLOW		0x37	//P��������ΧP0~P99
#define ERR_ORIENTATION		0x38	//Բ������ֻ���ǡ�-��������+��
#define ERR_OVRD			0x39	//ȫ���ٶȷ�Χ0~100
#define ERR_SPEED			0x3a	//�ֲ��ٶȷ�Χ0~100
#define ERR_POUT_OVERFLOW	0x3b	//�������IO����Ŀ����Χ0~11
#define ERR_PIN_OVERFLOW	0x3c	//��������IO����Ŀ����Χ0~11
#define ERR_WAIT			0x3d	//waitָ����pinָ��
#define ERR_STAR			0x3e	//ȱ��*��ͷ���ַ���
#define ERR_STR_NAME		0x3f	//*����������һ���ַ�������0~9��a~z��Χ�ڣ��м䲻���пո�
#define ERR_NO_THIS_LOOP	0x40	//û�д���ת��־
#define ERR_POUND			0x41	//ȱ��#��ͷ���ַ���
#define ERR_NO_THIS_SUB		0x42	//û�д��ӳ���
#define ERR_SUB_NAME		0x43	//#����������һ���ַ�������0~9��a~z��Χ�ڣ��м䲻���пո�
#define ERR_GUSTURE			0x44	//��̬��һ��
#define ERR_TRAY			0x45	//����ָ��ʹ��ʾ����t1 p12p13p14 10 11
#define ERR_TRAY_OVERFLOW	0x46	//���̱�ŷ�Χ0~9
#define ERR_TRAY_GRID_OVERFLOW	0x47//�������̵ĸ�������
#define ERR_TRAY_GRID_ZERO	0x48	//���̵ĸ��ӱ�Ŵ�1��ʼ
#define ERR_TRAY_UNDEFINE	0x49	//δ���������
#define ERR_N_OVERFLOW		0x4a	//����n�ı�ŷ�Χ0~49
#define ERR_IF_COMPARE		0x4b	//�ȽϷ���ֻ���� <,=,>,<=,>=
#define ERR_IF_OBJECT		0x4c	//�Ƚ϶���ֻ���Ǳ�����������ֵ
#define ERR_STAR_POUND		0x4d	//ȱ����ת��־���ӳ���
#define ERR_P_NO_VALUE		0x4e	//��Pλ��ֵΪ��
#define ERR_ALARM			0x4f	//��е�ֶ��������з�������
#define ERR_SAVE_BTN		0x50	//��е�ֶ����������ɿ���ȫ���أ�ʾ�̵�ʱ��
#define ERR_STOP			0x51	//��е�ֶ��������а�����stop��ť
#define ERR_CIRCLE1			0x52	//12���غϣ����㲻�ܳ�Բ
#define ERR_CIRCLE2			0x53	//13���غϣ����㲻�ܳ�Բ
#define ERR_CIRCLE3			0x54	//23���غϣ����㲻�ܳ�Բ
#define ERR_CIRCLE4			0x55	//�����ֱ�ߣ����㲻�ܳ�Բ
#define ERR_ACC				0x56	//�Ӽ��ٷ�Χ1~100,100��Ӧ���Ӽ���
#define ERR_PIN				0x57	//����IO:pin+������pin1
#define ERR_REPEATED_LOOP	0x58	//�ظ�����ת���
#define ERR_REPEATED_SUB	0x59	//�ظ����ӳ���
#define ERR_REPEATED_RETURN	0x5a	//�ظ���return
#define ERR_LACK_OF_RETURN	0x5b	//ȱ��return
#define ERR_STRING_NAME		0x5c	//ֻ�������ֺ���ĸ���
#define ERR_NO_PROGRAM		0x5d	//�����ڴ˳���
#define ERR_FLASH			0x5e	//FLASH����
#define ERR_BAUD			0x5f	//���������ò���ȷ
#define ERR_RSRD			0x60	//px = rsrd? or px = p(y) + rsrd?
#define ERR_RSRD_1			0x61	//rsrd?
#define ERR_OUT_OF_RANGE	0x62	//������е�ֶ�����Χ
#define ERR_RETURN_OVERFLOW 0x63	//return��ջ���
#define ERR_DISP			0x64	//��ʾָ�����
#define ERR_FT_X			0x65	//��������ָ��
#define ERR_OUTPUT			0x66	//����IO����ͨ���쳣
#define ERR_STM32COMM		0x67	//��stm32ͨ�ų���
#define ERR_RS_TIMEOUT		0x68	//�ȴ�rs232���ݳ�ʱ
#define ERR_SINGULARITY  	0x69	//���������
#define ERR_F_OVERFLOW		0x6a	//�����ͱ����ı�ŷ�Χ0~49
#define ERR_J1_J4			0x6b	//���4��
#define ERR_J1_J5			0x6c	//���5��
#define ERR_J1_J6			0x6d	//���6��
#define ERR_CANT_ZERO		0x6e	//��������Ϊ��
#define ERR_NO_RS232		0x6f	//�ð汾û��RS232
#define ERR_S_OVERFLOW		0x70	//�����ı��s��Χ0~49
#define ERR_NONE_PROGRAM	0xff	//������Ϊ��
//���򵥲����Թ����л���ֵĴ�����Ϣ

//������Ϣ
#define WARN_DUPLI_P		0x91	//ʾ��ʱ�������ظ���Pλ��,�����ж��Ƿ񸲸�
/***************************************
 * �������ܣ��﷨���
 * ������codeStrings  �������ַ���
 */
unsigned char CheckGrammar(PrgCode* codeStrings);

unsigned char GetCommand(PrgCode* codeStrings);

/***************************************
 * �������ܣ��������
 * ���������num ���մ������
 * 			 err ָ�������ŵ�ָ��
 * 			 line_of_err ָ�������е�ָ��
 * ����ֵ��  �˳���������
 * ˵����	 Build֮ǰ����ȴ�һ���ļ�
 * 			 ������Ϣһ�α�������ܹ��ռ�num��������Ϣ����������ǰ����0
 * 			 ���û�д�����Ϣ��err����ȫ����0����ȷ��Build��֮��errָ��
 * 			 ��ֵȫ��Ϊ0�������г���
 * 			 ����ʾ����������sizeof��
 */
unsigned char BuildProgram(unsigned char num,unsigned char* err,unsigned int* line_of_err);

#endif /* BUILDPROGRAM_H_ */
