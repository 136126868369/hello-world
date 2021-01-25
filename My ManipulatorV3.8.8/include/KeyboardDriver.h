/*
 * KeyboardDriver.h
 *
 *  Created on: 2015-2-8
 *      Author: xiaojian
 */

#ifndef KEYBOARDDRIVER_H_
#define KEYBOARDDRIVER_H_
unsigned long ReadKeyboardValue(void);
unsigned long JudgeKeyboardValue(void);//修正于//2018/06/01
void KeyboardDataProcessing(unsigned long value);
unsigned char KeyBoard_PressOK(void);
/*********************************
 * 返回值：1->表示按下了stop按钮
 */
unsigned char KeyBoard_StopSpeed(void);
/*********************************
 * 停止程序运行，程序运行中调用此函数可以暂停程序运行
 */
void StopProgramRunning(void);
void ClearStopProgramFlag(void);
unsigned char KeyBoard_Shift(void);

/********************************
 * 函数功能：读取当前示教模式
 * 输入参数：NONE
 * 输出参数：0：单轴模式	1：直线插补模式
 */
unsigned char GetModeState(void);

/********************************
 * 函数功能：设置示教模式
 * 输入参数：0：设置为单轴模式	1：设置为直线插补模式
 * 输出参数：NONE
 */
void SetModeState(unsigned char mode);
#endif /* KEYBOARDDRIVER_H_ */
