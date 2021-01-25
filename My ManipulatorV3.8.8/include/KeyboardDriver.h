/*
 * KeyboardDriver.h
 *
 *  Created on: 2015-2-8
 *      Author: xiaojian
 */

#ifndef KEYBOARDDRIVER_H_
#define KEYBOARDDRIVER_H_
unsigned long ReadKeyboardValue(void);
unsigned long JudgeKeyboardValue(void);//������//2018/06/01
void KeyboardDataProcessing(unsigned long value);
unsigned char KeyBoard_PressOK(void);
/*********************************
 * ����ֵ��1->��ʾ������stop��ť
 */
unsigned char KeyBoard_StopSpeed(void);
/*********************************
 * ֹͣ�������У����������е��ô˺���������ͣ��������
 */
void StopProgramRunning(void);
void ClearStopProgramFlag(void);
unsigned char KeyBoard_Shift(void);

/********************************
 * �������ܣ���ȡ��ǰʾ��ģʽ
 * ���������NONE
 * ���������0������ģʽ	1��ֱ�߲岹ģʽ
 */
unsigned char GetModeState(void);

/********************************
 * �������ܣ�����ʾ��ģʽ
 * ���������0������Ϊ����ģʽ	1������Ϊֱ�߲岹ģʽ
 * ���������NONE
 */
void SetModeState(unsigned char mode);
#endif /* KEYBOARDDRIVER_H_ */
