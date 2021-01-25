/*
 *  版权声明： 广州新豪五金精密制品有限公司研发部BU28拥有所有最终解析权，未经批准，不得转载，违者必究
 *  文件名称：Touch_screen.h
 *  摘	 要： 主函数需要引用的函数为all_scan();
 *          串口接收中断数据的处理函数manage_usart_return_value();
 *  当前版本：
 *  作	 者：王文东
 *  创建日期：2015-1-10
 *  完成日期：2015-4-27
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

void teach_speed(void);//调节示教速度模式

void keyboard_speed_meter(u8 sign);//4.6
void display_speed_meter(void);//4.61
void key_change_speed_value(u8 speed);//4.62

void clear_alarm(void);//7.14
void servo_motor(u8 flag);//7.15
void IO_Screen_Switch(void);

void display_sixangle(u8 screen_id);//8.582 全速运行时显示六个轴的角度

void SetCurrentScreen(void);//发送当前页面ID

void Fresh_program_name_num(unsigned int num,unsigned char* name);//修改s_program_name_num
void net_control_function(u8 id_value,u8 status);//修正于//2018/06/01
void return_home_network(void);//修正于//2018/06/01
#endif /* TOUCH_SCREEN_H_ */


//一级目录
/*********************************************************1
 *
 *
 *
**********************************************************/

//二级目录
/*****2
 */

//三级目录
/*****3
 */
/*----------------------------------------
-----------------------------------------*/

//四级目录
/*****4
 */
/*----------------------------------------------
 */

//五级目录
/*****5
 */
//
//
//



