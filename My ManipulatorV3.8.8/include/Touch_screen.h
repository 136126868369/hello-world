/*
 *  ��Ȩ������ �����º��������Ʒ���޹�˾�з���BU28ӵ���������ս���Ȩ��δ����׼������ת�أ�Υ�߱ؾ�
 *  �ļ����ƣ�Touch_screen.h
 *  ժ	 Ҫ�� ��������Ҫ���õĺ���Ϊall_scan();
 *          ���ڽ����ж����ݵĴ�����manage_usart_return_value();
 *  ��ǰ�汾��
 *  ��	 �ߣ����Ķ�
 *  �������ڣ�2015-1-10
 *  ������ڣ�2015-4-27
 */

#ifndef TOUCH_SCREEN_H_
#define TOUCH_SCREEN_H_
void setting_robot_para(void);
void scan_touch_screen(void);//4.1
void manage_usart_return_value(u8 usart_receive_value[USART_REC_LEN]);//4.2
u16 GetScreenId(void);//4.3
void display_servo_alarm(u8 axis, u8 error, u8 screen_id);//4.4
u8 DisplayErrInfo(unsigned char enable);
unsigned int RecoverSpeed(void);//4.5

void teach_speed(void);//����ʾ���ٶ�ģʽ

void keyboard_speed_meter(u8 sign);//4.6
void display_speed_meter(void);//4.61
void key_change_speed_value(u8 speed);//4.62

void clear_alarm(void);//7.14
void servo_motor(u8 flag);//7.15
void IO_Screen_Switch(void);

void display_sixangle(u8 screen_id);//8.582 ȫ������ʱ��ʾ������ĽǶ�

void SetCurrentScreen(void);//���͵�ǰҳ��ID

void Fresh_program_name_num(unsigned int num,unsigned char* name);//�޸�s_program_name_num
void net_control_function(u8 id_value,u8 status);//������//2018/06/01
void return_home_network(void);//������//2018/06/01
#endif /* TOUCH_SCREEN_H_ */


//һ��Ŀ¼
/*********************************************************1
 *
 *
 *
**********************************************************/

//����Ŀ¼
/*****2
 */

//����Ŀ¼
/*****3
 */
/*----------------------------------------
-----------------------------------------*/

//�ļ�Ŀ¼
/*****4
 */
/*----------------------------------------------
 */

//�弶Ŀ¼
/*****5
 */
//
//
//



