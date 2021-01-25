/*
 *  ��Ȩ�����������º��������Ʒ���޹�˾�з���BU28ӵ���������ս���Ȩ��δ����׼������ת�أ�Υ�߱ؾ�
 *  �ļ����ƣ�Touch_screen.c
 *  ժ	 Ҫ�� ��1�£�ͷ�ļ�������
 *  		��2�£�ȫ�ֱ���������
 *  		��3�£��ڲ�������������
 *  		��4�£��ⲿ����ʵ����
 *  		��5�£�������������
 *  		��6�£���е��ʾ�̺�IO�ڹ�������
 *  		��7�£�������������
 *  		��8�£�����༭���Թ�����
 *  		��9�£���������������
 *  		��10�£�ʹ��˵������͹ػ�����
 *  ��ǰ�汾��
 *  ��	 �ߣ����Ķ�
 *  �������ڣ�2015-1-10
 *  ������ڣ�2015-4-27
 *  �޸����ݣ���ʽ��
 *
 *  	         �����ࣺɾ������manage_usart_return_value()�����а����л�ҳ��ʱ��ȡ����ֵ�е�ǰҳ���ŵ���Ϣ
 *  �׹��޸ģ�step by step
 *
 */


//��1�£�ͷ�ļ�������
/********************************************************************************
* ��    �ƣ� ͷ�ļ�
* ��    �ܣ� �ⲿ�ӿ�����
* ˵	   ����ͷ�ļ�����ʽ2������ĸ�������򣨷�ʽ1�����ȶ����ȶ�����
 ********************************************************************************/
#include "BuildProgram.h"
#include "DSP2833x_DefaultIsr.h"
#include "EditProgram.h"
#include "KeyboardDriver.h"
#include "kinematic_explain.h"
#include "Man_DriverScreen.h"
#include "Man_Nandflash.h"//������2018/06/01
#include "Man_MCX31xAS.h"
#include "My_Project.h"
#include "ScreenApi.h"
#include "stdlib.h"
#include "stdio.h"
#include "Touch_screen.h"
#include"DSP28335_Spi.h"

//��2�£�ȫ�ֱ���������
/********************************************************************************
* ��    �ƣ� ������
* ��    �ܣ� ���������һЩ�궨���
 ********************************************************************************/
#define error_information_length  50
#define PAGE_NUMBER (unsigned int)(ROWS_IN_PROGRAM/15)
static u8 SCREEN_SIGN = 1;//ҳ���ű�־λ
static u8 USART_RECEIVE_STRING[USART_REC_LEN] = {0};//���ڱ����������ַ���
static u8 SPEED_VALUE = 5;//ʾ���ٶ�ֵ
static u8 OPEN_PROGRAM_SIGN = 0;//�Ƿ�򿪳����־λ��1ȷ�ϴ򿪣�0��ʾ����
static u8 MEMORY_TO_FLASH_SIGN = 1;//�ڴ�������ͬ���ı�־,1->û�г�����Ҫ����  2->�г�����Ҫ����
static u8 TEACH_MODEL=0;
static u16 ERROR_LINE[6]={0};//0~4�洢��������������ڵ�������5�洢������Ϣ��0��ʾ����ɹ���1��ʾ����ʧ��,
static u8 DISPLAY_ERROR_LINE=0; //�洢���´������������Ϣ���ڵ�����
static u8 DEVELOPER_MODE=0;//0��ʾ�ͻ�ģʽ��1��ʾ������ģʽ
extern unsigned char Teach_mode;//������//2018/06/01
extern long TIMER0_CNT;
extern struct program_name_num s_program_name_num;
static u8 net_control=0;//������//2018/06/01
static u16 Software_name_check=0;
static u8 build_program_sign = 0;//������//2018/06/01
struct button//������ť�ṹ�壬���ڱ�����ֿؼ���ֵ
{
	u8 BUTTON_STATUS;
	u8 KEY_VALUE;
	u8 SLIDER_VALUE;
	u8 METER_VALUE;
	u8 CONTROL_ID;
	u8 CONTROL_TYPE;
	u16 SCREEN_ID;
	u16 screencoordinate_x;
	u16 screencoordinate_y;
}	;
static struct button G_BUTTON={0,0,0,0,0,0,0,0,0};

const char *SERVO_ALARM="Servo alarm,please check the servos!";
const char SERVO_OFF[]={"Servos are off!"};
const char LOST_ORIGIN[]={"The origin is lost!Please check the servos!"};
const char BRAKES_CLOSING[]={"Brakes are closing!"};
const char AXIS_LIMITED[]={"Limited,move to unlimited area!"};
const char EMERGENCY_STOP[]={"Servoes are emergency stop!"};
const char KEEP_SAVE_BTN[]={"Save-button unpressing!"};
const char NO_PROGRAM[]={"No Program!"};
const char *RUN_SUCCEED="Running successfully!";
const char *PROG_IS_OVER="Program is over!";

const char EXPORT_SUCCESSFUL[]="Exported successful!";
const char IMPORT_SUCCESSFUL[]="Imported successful!";
const char FLASH_ERROR[]="Flash error!";
const char NAME_REPETITION[]="Name repeat!";
const char SAVE_FAILED[]="Failed to save��";
const char READ_EX_ERR[]="Failed to read storage!";
const char INTERNAL_COMM_ERR[]="Internal communication error";
const char NAME_EMPTY[]="Please input a name.";
const char NAME_TOO_LONG[]="Name too long!";
const char _ERROR[] = "ERROR";
const char STORAGE_FULL[]="The store was full!";
const char error_information[][error_information_length-4] ={
/*31*/	{"#1:Wrong command!"},
/*32*/	{"#2:*start was repeat!"},
/*33*/	{"#3:Delay range is 0 to 999.9"},
/*34*/	{"#4:Have no end at the last line"},
/*35*/	{"#5:End repeated!"},
/*36*/	{"#6:miss position"},
/*37*/	{"#7:Position number:0~99"},
/*38*/	{"#8:circle direction:'+' or '-'"},
/*39*/	{"#9:Global-speed range is 1 to 100"},
/*3a*/	{"#10:Local-speed range is 1 to 100"},
/*3b*/	{"#11:Output IO range is 0 to 23!"},
/*3c*/	{"#12:Input IO range is 0 to 17!"},
/*3d*/	{"#13:For example,wait pin0 = 0/1"},
/*3e*/	{"#14:Must start with *,such as goto *loop"},
/*3f*/  {"#15:Goto flag contain letters and num only"},
/*40*/  {"#16:Have no this goto flag!"},
/*41*/	{"#17:Must start with #,such as gosub #sub"},
/*42*/	{"#18:Have no this subprogram!"},
/*43*/	{"#19:Sub name contain letters and num only!"},
/*44*/	{"#20:Different gesture!"},
/*45*/	{"#21:Command of tray:t1 p12p13p14 10 11"},
/*46*/	{"#22:Tray range is 0 to 19!"},
/*47*/	{"#23:Tray cell number overflow!"},
/*48*/	{"#24:Tray cell number start at 1!"},
/*49*/	{"#25:Undefine this Tray!"},
/*4a*/	{"#26:'n' variable range is 0 to 49"},
/*4b*/	{"#27:Comparision only can be:>,<,=,>=,<="},
/*4c*/	{"#28:Only n variable or num can be compare"},
/*4d*/	{"#29:Missing goto flag(*x) or subprogram(#y)"},
/*4e*/  {"#30:This position's value can not be empty"},
/*4f*/  {"#31:Servo is alarm when the robot running!"},
/*50*/  {"#32:Save button can't release when teaching"},
/*51*/  {"#33:STOP!"},
/*52*/  {"#34:The 1st and 2nd point are coincident!"},
/*53*/  {"#35:The 1st and 3rd point are coincident!"},
/*54*/  {"#36:The 2nd and 3rd point are coincident!"},
/*55*/  {"#37:Three points are in a straight line!"},
/*56*/	{"#38:Acc Range:1~100!"},
/*57*/	{"#39:Input IO:pin+number,for example,pin1"},
/*58*/	{"#40:Goto Flag repeated!"},
/*59*/	{"#41:Subprogram repeated"},
/*5a*/	{"#42:Return repeated!"},
/*5b*/	{"#43:This subprogram have no return!"},
/*5c*/	{"#44:Must contain letters and numbers only!"},
/*5d*/	{"#45:Have no this program!"},
/*5e*/	{"#46:Open program failed!"},
/*5f*/	{"#47:Baud rate must be a number!"},
/*60*/	{"#48:px=rsrd? px=p(y)+rsrd? or px=p.y+n?"},
/*61*/	{"#49:rsrd?"},
/*62*/	{"#50:Out of range,position is unreachable!"},
/*63*/	{"#51:return stack overflow!!"},
/*64*/	{"#52:disp1~5"},
/*65*/	{"#53:ft px?"},
/*66*/	{"#54:Set output IO failure!"},
/*67*/	{"#55:Internal communication error!"},
/*68*/	{"#56:RS232 time out"},
/*69*/	{"#57:Singularity!"},
/*6a*/	{"#58:'f' variable range is 0 to 49"},
/*6b*/	{"#59:J1~J4"},
/*6c*/	{"#60:J1~J5"},
/*6d*/	{"#61:J1~J6"},
/*6e*/	{"#62:Cannot divide by zero!"},
/*6f*/	{"#63:This hardware have no RS232!"}
};



//��3�£��ڲ�������������
/********************************************************************************
* ��    �ƣ��ڲ���������
* ��    �ܣ����ú���
* ˵	   ����
 ********************************************************************************/
//������������5-3
u8 DisplayErrInfo(unsigned char enable);//5.1
static void manage_button_slider_text(u8 usart_receive_value[32]);//5.2
static void manage_touch_screen_coordinate(u8 usart_receive_value[32]);//5.3

//��е��ʾ�̺�IO�ڹ�������6-5
static void screen1(void);//6
static void screen12(char screen_id);//6.1
static u8 switch_teach_model(u32 key_value, u8 screen_sign);
static void screen10(void);//6.2
static void screen22(void);//6.3
static void screen23(void);//6.4
//������������7-20
static void screen2(void);//7
static void origin_sound_alarm(void);//7.1
static void locked_origin(void);//7.11
static void sure_locked_origin(void);//7.111
static void return_home(void);//7.12
static void noparameter(u8 screen_id);//7.121
static void sure_return_home(u8 speed);//7.122
static void sound_settings(void);//7.13��������
static void display_open_servo_fail_inform(u8 servo_inform);//7.151
static void formatting_sd(void);//7.16
static void developer_mode(void);//7.17
static void switch_alarm_signal_mode(void);
static void switch_limit_mode(void);
static void switch_teach_speed_mode(void);
static void switch_mode(void);//������2018/06/01
static void screen8(void);//7.2
static void screen11(void);//7.3
static void screen13(void);//7.4
static u16 array_to_date(void);//7.41
static void reset_text(u8 clocks[2], u8 minutes[2]);//7.42
static void start_text(u8 clocks[2], u8 minutes[2]);//7.43
static u8 check_real_time(u8 clocks[2], u8 minutes[2]);//7.431
static void display_runing_test_screen(u8 clocks[2], u8 minutes[2]);//7.432
static void run_intime(u8 clocks[2], u8 minutes[2], u8 program_name[18]);//7.433

//����༭���Թ�����8-22
static void screen4(void);//
static void screen7(void);//8
static void display_ten_page(unsigned long sign);//��10ҳ
static void jump_to_error_line(u8 *text_num);
static u8 switch_screen(u8 text_num,u8 *sign_edit);//8.1
static void update_program(u8 *text_num0, u8 *page_num, u8 *text_num, u8 *sign_edit);//8.2
static void return_keyboard_value(u8 *text_num, u8 *text_num0, u8 *page_num, u8 *sign_edit);//8.3
static void press_enter(u8 *text_num, u8 * page_num, u8 *text_num0);//8.31
static void delete_line(u8 *line_num, u8 * page_num);//8.32
static u8 touch_edit(u8 *text_num0, u8 *text_num, u8 *sign_edit);//8.4
static void dispose_button(u8 *text_num0, u8 *text_num, u8 *sign_edit, u8 *page_num, u8 *open_program_sign);//8.5
static void display_first_page(void);
static void save_last_edit_program(u8 text_num, u8 page_num, u8 *sign_edit);//8.51
static void screen7_1(u8 *pages);//8.52
static void screen7_2(u8 *pages);//8.53
static void new_program(u8 *text_num0, u8 *text_num, u8 *page_num);//8.54
static u8 build_program(void);//8.55
static void error_number_to_informations(u8 error_number, char *build_informatiom);//8.551
static void save_program(void);//8.56
static void step_by_step(u8 *text_num0, u8 *text_num, u8 *page_num);//8.57
static void screen6(u8 *text_num,u8 *text_num0,u8 *page_num,const u8 *sign_edit, u8 build_program_sign);//8.58
static void display_current_runing_program_position(u8 text_num);//8.581
static void save_position(u8 text_num, u8 page_num);//8.59
static u8 prompt_dialog_box(u8 screen_id1, u8 screen_id2);//8.6
static void refresh(u8 page_num, u8 screen_id);//8.7

//��������������9-6
static void programs_manage(u8 last_screen);//�������//9
static void programs_net_manage(u8 last_screen,u8 text_num,u8* check_value);//ͨ��������г������
static u8 exit_programs_manage_screen(u8 text_num, u8 last_screen, u8 sign_program_num);//9.1
static void display_objective_program(u8 *text_num, u8 *sign_program_num);//9.2
static void copy_delete_rename_page(u8 *text_num, u8 *sign_program_num);//9.3
static void copy_or_rename_program(u8 *sign_program_num, u8 *text_num, u8 f(u8 *old_name, u8 *new_name));//9.31
static void display_nextpage_program_name();//9.33
static void display_previouspage_program_name();//9.34
static void delete_program(u8 *text_num, u8 *sign_program_num);//9.35
static void refresh_display_program_name(u8 pages);//9.4
static void open_program(u8 *page_num, u8 *text_num, u8 *text_num0, u8 *sign_edit, u8 *open_program_sign);//9.5
static void DispalyProgEditInfo(u8 err);//9.6
static void export_program_to_udisk(u8 text_num , u8 *sign_program_num);
static void export_judge(u8 sign_return);
static u8 save_program_to_flash();
static void display_udisk_program();
static void manage_udisk_program();
static void change_page(long pro_num,u8 *page_num);
static void import_program(u8 *sign_program_num, u8 text_num ,u8 page_num);

//ʹ��˵������͹ػ�����10-2
static void screen3(void);//10.1
static void screen5(void);//10.2


//��4�£��ⲿ����ʵ����
/********************************************************************************
*********************************************************************************/
/*
 * ��������ִ��openָ���һ�������ʱ�򣬱����޸�s_program_name_num������ʹ��
 * ��򿪵ĳ�����ͬ����Ȼ��ˢ�³�����ҳ�档
 * �������ܣ��޸�s_program_name_num
 */
void Fresh_program_name_num(unsigned int num,unsigned char* name)
{
	unsigned int i;
	char str[4];
	s_program_name_num.num = num;
	memset(s_program_name_num.program_name,0,sizeof(s_program_name_num.program_name));
	i=0;
	while(*(name+i))
	{
		s_program_name_num.program_name[i] = *(name+i);
		i++;
		if(i>=PROGRAM_NAME_LENGTH)
		{
			s_program_name_num.program_name[PROGRAM_NAME_LENGTH-1] = 0;
			break;
		}
	}
	if(G_BUTTON.SCREEN_ID == 6)
	{
		SetTextValue(6, 83, &s_program_name_num.program_name[0]);//ˢ����ʾ�������еĳ�����
		refresh(1,6);//ˢ�³���ҳ��
	}
	else
	{
		ltoa(num,str);
		SetTextValue(7, 82, str);
		SetTextValue(7, 83, &s_program_name_num.program_name[0]);//ˢ����ʾ�������еĳ�����
		refresh(1,7);
	}
}

/***************************************
* ��    �ƣ� scan_touch_screen(4.1)
* ��    �ܣ� ȫ��ɨ�貢�е�ҳ�棬ʵ�ֲ���ҳ��֮����л�
*/
void scan_touch_screen(void)
{
/*	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			USART_RX_STA = 0;
		}
	}*/
	unsigned long key_value;
	SCREEN_SIGN = 1; //ҳ���־λ�趨Ϊ1������ʱĬ�Ͻ������˵�һ�Ľ���
	G_BUTTON.SCREEN_ID = 1; //��Ļ�����Ϊ1

	SetTextValue(13, 12, "00"); //��ʱ����ʱ������
	SetTextValue(13, 13, "00");
	SetTextValue(13, 14, "00");
	SetTextValue(13, 15, "00");
	//delay_1ms(500); //�ȴ�������̬ͼ��ʾ��

    while(1)
    {
		USART_RX_STA = 0; //ÿ���л�ҳ��ʱ��տؼ���Ϣ�ʹ��ڽ����жϱ�־λ
		G_BUTTON.KEY_VALUE = 0;
		G_BUTTON.CONTROL_ID = 0;
		G_BUTTON.CONTROL_TYPE = 0;
		G_BUTTON.BUTTON_STATUS = 0;

		switch(SCREEN_SIGN)//�л�����Ӧ��־λ��ҳ��
		{
			case 1: screen1(); break;
			case 2: screen2(); break;
			case 3: screen3(); break;
			case 4: screen4(); break;
			case 5: screen5(); break;
			case 7: screen7(); break;
			case 10: screen10(); break;
			case 14: screen22(); break;
			case 15: screen23(); break;
			default:	         break;
	    }

		#if KEY_BOARD
			key_value = ReadKeyboardValue();
			if(key_value == ReadKeyboardValue())
			{
				KeyboardDataProcessing(key_value);
			}
		#endif
		ErrCheck(1);
    }
}

/**************************************************************
* ��       �ƣ� s_button_slider(4.2)
* ��       �ܣ� �����ڽ��ջ�������ֵ
* ��ڲ����� ���ݴ����жϻ���ֵ
*/
void manage_usart_return_value(u8 usart_receive_value[USART_REC_LEN])
{
	if( (usart_receive_value[1]==0xB1) && (usart_receive_value[2]==0x11) )//���µ�Ϊ��ť��������߷����ı�ֵ
	{
		manage_button_slider_text(usart_receive_value);
	}

	else
	{
		if( usart_receive_value[1] == 0xf7 )//����RTC�ؼ�����
		{
			USART_RECEIVE_STRING[0] = usart_receive_value[6];//0001 0101
			USART_RECEIVE_STRING[1] = usart_receive_value[7];//0010 0000
			USART_RECEIVE_STRING[0] = ( (USART_RECEIVE_STRING[0]>>4)*10 + (USART_RECEIVE_STRING[0]&0x0f) );//BCD����ת��
			USART_RECEIVE_STRING[1] = ( (USART_RECEIVE_STRING[1]>>4)*10 + (USART_RECEIVE_STRING[1]&0x0f) );
		}
		else
		{
			if( 0x07 == usart_receive_value[1])
			{
				SetScreen(G_BUTTON.SCREEN_ID);
				USART_RX_STA = 0;
			}
			else
			{
				keyboard_speed_meter(3);
				manage_touch_screen_coordinate(usart_receive_value);
			}
		}
	}
}

/******************************
* ��    �ƣ� GetScreenId(4.3)
* ��    �ܣ� ��õ�ǰҳ��ID
*/
u16 GetScreenId(void)
{
	 return G_BUTTON.SCREEN_ID;
}

/****************************************************
* ��       �ƣ� display_servo_alarm(4.4)
* ��       �ܣ� ��ʾ�ŷ�������Ϣ
* ��ڲ����� axis���������ţ���Χ1~6
* 		  error����������Ϣ
* 		  screen_id����ǰ���������ڵ���Ļ���
*/
void display_servo_alarm(u8 axis,u8 error,u8 screen_id)
{
	char str[] = {"Jx:"};
	unsigned int time_flg=0;
	unsigned char alarm_flg = 0;
	str[1] = axis + '0';
	OpenBuzzer();
	if( ((error>>4) & 0x01) == 1 )//�ŷ�����
    {
		SetScreen(20);
		G_BUTTON.SCREEN_ID = 20;
		SetTextValue(20, 4, str);
		SetTextValue(20, 3, "ALARM");
		alarm_flg = 1;
	}
	else if( ((error>>5) & 0x01) == 1 )//��ͣ
	{
		SetScreen(33);
		delay_1ms(200);
	}
	else if((error&0x01) == 1)//����λ
	{
	    SetScreen(20);
	    SetTextValue(20, 4, str);
	    if((axis == 1) || (axis == 4) || (axis == 6))
	    {
	    	SetTextValue(20, 3, "- Limited");
	    }
	    else
	    {
	    	SetTextValue(20, 3, "+ Limited");
	    }
	}
	else if( ((error>>1) & 0x01) == 1 )//����λ
	{
		SetScreen(20);
		SetTextValue(20, 4, str);
	    if((axis == 1) || (axis == 4) || (axis == 6))
	    {
	    	SetTextValue(20, 3, "+ Limited");
	    }
	    else
	    {
	    	SetTextValue(20, 3, "- Limited");
	    }
	}
	else
	{
		SetScreen(20);
		SetTextValue(20, 4, str);
		SetTextValue(20, 3, "ERROR #001");//xxx������ŷ�����û�н��������ô�͸�Ϊ��ͣ��
	}
	USART_RX_STA = 0;
	while(1)
	{
		if( KeyBoard_PressOK())
		{
			 break;
		}

	    if((USART_RX_STA&0x8000))
	    {
	    	if(G_BUTTON.CONTROL_ID == 2)
	    	{
	    		G_BUTTON.SCREEN_ID = screen_id;
	    		break;
	    	}
	    	USART_RX_STA = 0;
	    }

	    if(alarm_flg == 0)
	    {
			if(time_flg >= 18000)
			{
				CloseBuzzer();
				//if(alarm_flg == 0)
				//{
					break;
				//}
				//time_flg = 60000;
			}
			time_flg++;
	    }
	    CheckScreenConnectState();
	    delay_1us(11);
	}
	CloseBuzzer();
	SetScreen(screen_id);
	G_BUTTON.KEY_VALUE = 0;
	G_BUTTON.CONTROL_ID = 0;
	G_BUTTON.CONTROL_TYPE = 0;
	G_BUTTON.screencoordinate_y=0;
	G_BUTTON.screencoordinate_x=0;
	USART_RX_STA = 0;
}

/*********************************************
 * ���ƣ�RecoverSpeed-4.5
 * ���ܣ��ָ��ٶȣ�ÿ��ֱ�߲岹���߳���������֮���ٶȶ�Ҫ�ָ����ٶȻ�������ֵ
 */
unsigned int RecoverSpeed(void)
{
	unsigned int speed;
	unsigned int tmp_speed;
	//if(!GetSpeedMode())
	//	tmp_speed = SPEED_VALUE*SPEED_MODE_BASE;
	//else
		tmp_speed = SPEED_VALUE;
	if(ROBOT_PARAMETER.SYS==FOUR_AXIS_C)
	{
		switch(tmp_speed)
		{
		case 1:speed = 1;break;
		case 2:speed = 25;break;
		case 3:speed = 50;break;
		case 4:speed = 80;break;
		case 5:speed = 150;break;
		case 6:speed = 200;break;
		case 7:speed = 250;break;
		case 8:speed = 300;break;
		case 9:speed = 350;break;
		case 10:speed = 400;break;
		case 11:speed = 460;break;
		case 12:speed = 520;break;
		case 13:speed = 580;break;
		case 14:speed = 640;break;
		case 15:speed = 700;break;
		case 20:speed = 1200;break;
		default:speed = tmp_speed*80;break;
		}
	}
	else
	{
		switch(tmp_speed)
		{
		case 1:speed = 1;break;
		case 2:speed = 5;break;
		case 3:speed = 20;break;
		case 4:speed = 50;break;
		case 5:speed = 100;break;
		case 6:speed = 200;break;
		case 7:speed = 300;break;
		case 8:speed = 400;break;
		case 9:speed = 600;break;
		default:speed = tmp_speed*80;break;
		}
	}
	SetSpeed(0x3f, speed);
	//���ٱ仯��Χ1~100
	ModifyAllSpeed(tmp_speed);
	return speed;
}

/*********************************************
 * ���ƣ�RecoverSpeed-4.6
 * ���ܣ��Ǳ�����ʾ��ǰ�ٶ�ֵ
 * ������sign:20��ʾ�ٶ��Ǳ��̣�23�����ٶȣ�22��С�ٶȣ�3�ر���ʾ�ٶ��Ǳ���
 */
void keyboard_speed_meter(u8 sign)
{
	static u8 select_sign=0;
	static u8 screen_id=0;

	if( (select_sign == 1) && ((sign==22)||(sign==23)) )//������ö࣬���Է�����ǰ�����Ч��
	{
		key_change_speed_value(sign);
	}
	else if( (sign == 20) && (select_sign == 0) )
	{
		display_speed_meter();
		screen_id =G_BUTTON.SCREEN_ID;
		G_BUTTON.SCREEN_ID=40;
		select_sign = 1;
	}
	else if( (sign==3) && (select_sign==1) )
	{
		SetScreen(screen_id);
		G_BUTTON.SCREEN_ID = screen_id;
		select_sign = 0;
	}
	else if(sign == 20)
	{
		display_speed_meter();
	}
}

/*********************************************
 * ���ƣ�RecoverSpeed-4.61
 * ���ܣ��Ǳ�����ʾ��ǰ�ٶ�ֵ
 */
void display_speed_meter(void)
{
	char str[4]={0};

	SetMeterValue(36, 1, SPEED_VALUE);
	SetScreen(36);
	ltoa(SPEED_VALUE,str);
	SetTextValue(36, 2, str);
}

/*********************************************
 * ���ƣ����������ٶ�ֵ-4.62
 * ���ܣ��Ǳ�����ʾ��ǰ�ٶ�ֵ
 * ������speed��23�����ٶȣ�22��С�ٶ�
 */
void key_change_speed_value(u8 speed)
{
	static u8 sign = 0;
	char str[4]={0};

	if(!GetSpeedMode())//����ǵ���ģʽ
	{
		if( speed==23 )
		{
			if(SPEED_VALUE<5)
			{
				SPEED_VALUE += 1;
			}
			else if((SPEED_VALUE>=5) && (SPEED_VALUE<25))
			{
				SPEED_VALUE += 5;
			}
			if(SPEED_VALUE > 15 )
			{
				SPEED_VALUE = 15;
			}
		}
	}
	else
	{
		if( speed==23 )
		{
			if(SPEED_VALUE<5)
			{
				SPEED_VALUE += 1;
			}
			else if((SPEED_VALUE>=5) && (SPEED_VALUE<50))
			{
				SPEED_VALUE += 5;
			}
			else if((SPEED_VALUE>=50) && (SPEED_VALUE<100))
			{
				SPEED_VALUE += 10;
			}
			if(SPEED_VALUE > 100 )
			{
				SPEED_VALUE = 100;
			}
		}
	}
	if( (speed==22)  )
	{
		if((SPEED_VALUE>1) && (SPEED_VALUE<=5))
		{
			SPEED_VALUE -= 1;
		}
		else if((SPEED_VALUE>5) && (SPEED_VALUE<=50))
		{
			SPEED_VALUE -= 5;
		}
		else if((SPEED_VALUE>50) && (SPEED_VALUE<=100))
		{
			SPEED_VALUE -= 10;
		}
	}

	SetMeterValue(36, 1, SPEED_VALUE);
	SetMeterValue(40, 1, SPEED_VALUE);
	ltoa(SPEED_VALUE,str);

	SetSliderValue(10,5,SPEED_VALUE);//�����������ҳ���ٶ�������
	SetTextValue(10,18,str);//�ٶ�����ֵ����
	SetSliderValue(22,5,SPEED_VALUE);//ֱ�߲岹�������ҳ���ٶ�������
	SetTextValue(22,19,str);//�ٶ�����ֵ����
	SetSliderValue(23,5,SPEED_VALUE);//ĩ�������������ҳ���ٶ�������
	SetTextValue(23,18,str);//�ٶ�����ֵ����

	if(sign == 0)
	{
		SetTextValue(36, 2, str);
		SetScreen(36);
		sign = 1;
		return;
	}
	if(sign == 1)
	{
		SetTextValue(40, 2, str);
		SetScreen(40);
		sign = 0;
		return;
	}
}


//��5�£�������������
/********************************************************************************
 * 5.1
 * 5.2
 * 5.3
 * 5.4
 * 5.5
*********************************************************************************/

/******************************************
 * ��        �ƣ�DisplayErrInfo(5.1)
 * ��        �ܣ���ʾ������Ϣ
 * ������� ��enable����ӦbitΪ1��ʹ����Ӧ�ı���
 * 			bit0���ŷ�on
 * 			bit1������
 * 			bit2������ԭ��
 * 			bit3����ͣ
 * 			bit4, ����λ
 * 			bit5, ����λ
 * 			bit6, ��ȫ����δ����
 * 			bit7, 1��ɲ��δ��
 * 			bit8, 2��ɲ��δ��
 * 			bit9, 3��ɲ��δ��
 * ����ֵ    ��0���޾���/������Ϣ	1���о���/������Ϣ
*/
u8 DisplayErrInfo(u8 enable)
{
	 u16 err;
	 unsigned char i,err_axis;
	 char str[]={"Jx:"};
	 err = ReadErrInfo()&enable;
	 err_axis=ReadErrState();//bit0~bit5:J1~6,�������״̬
	 for(i=0;i<6;i++)
	 {
		 if(err_axis>>i)
		 {
			 str[1] = i + '0';
			 break;
		 }

	 }
	 if(err)//������ԭ��
	 {
		  SetDriCmd(0x3F,0x27);
		  delay_1ms(1);
		  SetScreen(20);
		  SetTextValue(20, 4, "");

		  if(err&0x02)//�ŷ�����
		  {
			   SetTextValue(20, 3, "Servo Alarm!");
		  }
		  else if(err&0x01)//�ŷ�off
		  {
			   SetTextValue(20, 3, "Servo OFF!");
		  }
		  else if( (err&0x10) == 1 )//����λ
		  {
			   SetTextValue(20, 4, str);
			   SetTextValue(20, 3, "+Limited!");
		  }
		  else if( (err&0x20) == 1 )//-��λ
		  {
			   SetTextValue(20, 4, str);
			   SetTextValue(20, 3, "-Limited!");
		  }
		  else if(err&0x380)
		  {
			  SetTextValue(20, 3, "Brake is closing!");
		  }
		  else if(err&0x08)//��ͣ
		  {
			   //SetScreen(33);
			   SetTextValue(20, 3, "Emergency Stop!");
		  }
		  else if(err&0x40)
		  {
			  SetTextValue(20, 3, "Save-button is unpressing!");
		  }

		   delay_1ms(1000);
		   SetScreen(G_BUTTON.SCREEN_ID);
		   G_BUTTON.KEY_VALUE = 0;
		   G_BUTTON.CONTROL_TYPE = 0;
		   USART_RX_STA = 0;
		   return 1;
	 }
	 return 0;
}

/*****************************************************
* ��       �ƣ� manage_button_slider_text(5.2)
* ��       �ܣ� �������������ڽ��ջ������ǰ�ť��������ı���Ϣ������
* ��ڲ����� ���ڽ���ֵ
*/
static void manage_button_slider_text(u8 usart_receive_value[USART_REC_LEN])
{
	u8 i = 8;//ͨ��i����ѭ����
	G_BUTTON.CONTROL_ID = (usart_receive_value[5]<<8) + usart_receive_value[6];

	if(usart_receive_value[7] == 0x10)//��ʾ���µ�Ϊ��ť�ؼ�
	{
		switch(usart_receive_value[8])
		{
			case 0x00://�л����湦��
				if(usart_receive_value[9] == 1)
				{
					G_BUTTON.CONTROL_TYPE = 10;
					break;
				}
				if(usart_receive_value[9] == 0)
				{
					USART_RX_STA = 0;
					break;
				}
			case 0x01://����ģʽ
				G_BUTTON.CONTROL_TYPE = 11;
				G_BUTTON.BUTTON_STATUS = usart_receive_value[9];
				if(G_BUTTON.SCREEN_ID == 6)
				{
					if(G_BUTTON.CONTROL_ID == 18)
					{
						ProgramRuningFlag(3); //�����˳�����
						StopProgramRunning();
					}
					if((G_BUTTON.CONTROL_ID==2) && (G_BUTTON.BUTTON_STATUS==1))//������ͣ��
					{
						StopProgramRunning();//�˴��������ŷ��������ֹͣ�ĺ���
						//SetButtonValue(6,2,1);
//						ProgramRuningFlag(2);
					}
					if((G_BUTTON.CONTROL_ID==2) && (G_BUTTON.BUTTON_STATUS==0))//���¼������м�
					{
						ProgramRuningFlag(1);
						OpenFan();//�򿪷���
					}
					USART_RX_STA = 0;
				}
				break;
			case 0x02://����ģʽ
				G_BUTTON.CONTROL_TYPE = 12;
				G_BUTTON.KEY_VALUE = usart_receive_value[9];
				break;
			default  : 	break;
		}
	}//end if
	if(usart_receive_value[7] == 0x13)//���ػ���ؼ�������
	{
		G_BUTTON.SLIDER_VALUE = usart_receive_value[11];
		G_BUTTON.CONTROL_TYPE = 13;
	}
	if(usart_receive_value[7] == 0x14)//�����Ǳ�ؼ�������
	{
		G_BUTTON.METER_VALUE = usart_receive_value[11];
		G_BUTTON.CONTROL_TYPE = 14;
	}
	if(usart_receive_value[7] == 0x11)//�����ı��ؼ����ݣ�����0����
	{
		G_BUTTON.CONTROL_TYPE = 15;
		do
		{
			USART_RECEIVE_STRING[i-8] = usart_receive_value[i];
		}
		while(usart_receive_value[i++] > 0);//��Ϊ���յ����ַ�����'0'��β,��ASCII��Ϊ��ʱ����ѭ��
	}
}

/****************************************************************
* ��       �ƣ� manage_touch_screen_coordinate(5.3)
* ��       �ܣ� �����������ص�����ֵ
* ��ڲ����� ���ڽ���ֵ
*/
static void manage_touch_screen_coordinate(u8 usart_receive_value[USART_REC_LEN])
{
	u16 i;
	if( (usart_receive_value[1]==0x01)//����ǰΪ����7����24����41���������ǰ���µ��������Ϣ
	    && ( (G_BUTTON.SCREEN_ID==7) || (G_BUTTON.SCREEN_ID==24) || (G_BUTTON.SCREEN_ID==41) ) )
	{
		G_BUTTON.screencoordinate_x = (usart_receive_value[2]<<8) + usart_receive_value[3];
		G_BUTTON.screencoordinate_y = (usart_receive_value[4]<<8) + usart_receive_value[5];

		if( (G_BUTTON.CONTROL_ID!=97)//�����ڽ���7���ı��ؼ���ʾ����������ǰ����ֵ�����ұ�־λ������
			 && (G_BUTTON.SCREEN_ID==7) && (G_BUTTON.screencoordinate_y>63)
			 && (G_BUTTON.screencoordinate_y<530) && (G_BUTTON.screencoordinate_x<238)
			 && (G_BUTTON.screencoordinate_x>16) )
		{	return;}

		if( (G_BUTTON.SCREEN_ID==7) //����enter��
			 &&(G_BUTTON.screencoordinate_x>705) && (G_BUTTON.screencoordinate_x<787)//������ڱ༭����
			 && (G_BUTTON.screencoordinate_y>484) && (G_BUTTON.screencoordinate_y<525) )
		{
			USART_RX_STA = 0;
			return;
		}
		if( (G_BUTTON.SCREEN_ID==7) //���¼�����
			 &&(G_BUTTON.screencoordinate_x>250) && (G_BUTTON.screencoordinate_x<790)//������ڱ༭����
			 && (G_BUTTON.screencoordinate_y>250) && (G_BUTTON.screencoordinate_y<525) )
		{
			return;
		}
		if( (G_BUTTON.SCREEN_ID==24) //������ڱ༭����
			&&(G_BUTTON.screencoordinate_x<720) && (G_BUTTON.screencoordinate_y>89)
			&&(G_BUTTON.screencoordinate_y<390) && (G_BUTTON.screencoordinate_x>100))
		{	return;}//������ڱ༭���򣬱��淵������ֵ���������λ��

		if( (G_BUTTON.SCREEN_ID==41) //������ڱ༭����
			&&(G_BUTTON.screencoordinate_x<570) && (G_BUTTON.screencoordinate_y>119)
			&&(G_BUTTON.screencoordinate_y<449) && (G_BUTTON.screencoordinate_x>270))
		{	return;}//������ڱ༭���򣬱��淵������ֵ���������λ��

		if( (1==ERROR_LINE[5]) && (G_BUTTON.screencoordinate_y>89) && (G_BUTTON.screencoordinate_y<214)
			 && (G_BUTTON.screencoordinate_x>244) && (G_BUTTON.screencoordinate_x<794) )
		 {
			 DISPLAY_ERROR_LINE=(G_BUTTON.screencoordinate_y-89)/25+1;//ȡֵΪ1~5������
			 for(i=0;i<5;i++)
			 {
				 if(ERROR_LINE[i]==0)break;//�����ж��ٸ����������Ϣ
			 }
			 if(DISPLAY_ERROR_LINE>i)
			 {
				 G_BUTTON.screencoordinate_x = 0;
				 G_BUTTON.screencoordinate_y = 0;
				 DISPLAY_ERROR_LINE = 0;
				 return;
			 }
			 G_BUTTON.screencoordinate_x = 0;
			 G_BUTTON.screencoordinate_y = 0;
			 return;
		 }

	 	 G_BUTTON.screencoordinate_x = 0;
		 G_BUTTON.screencoordinate_y = 0;
		 USART_RX_STA = 0;
	}

	else
	{
		USART_RX_STA = 0;
	}
}

/*******************************************************************
***********��������ʱ����ͨ�ſ��ƺ���//������//2018/06/01**********************
********************************************************************/
void net_control_function(u8 id_value,u8 status)
{
	G_BUTTON.CONTROL_TYPE = 11;
	G_BUTTON.SCREEN_ID =6;
	G_BUTTON.CONTROL_ID = id_value;
	G_BUTTON.BUTTON_STATUS = status;
	if(G_BUTTON.SCREEN_ID == 6)
	{
		if(G_BUTTON.CONTROL_ID == 18)
		{
			ProgramRuningFlag(3); //�����˳�����
			StopProgramRunning();
		}
		if((G_BUTTON.CONTROL_ID==2) && (G_BUTTON.BUTTON_STATUS==1))//������ͣ��
		{
			StopProgramRunning();//�˴��������ŷ��������ֹͣ�ĺ���
			SetButtonValue(6,2,1);
	        //ProgramRuningFlag(2);
		}
		if((G_BUTTON.CONTROL_ID==2) && (G_BUTTON.BUTTON_STATUS==0))//���¼������м�
		{
			ProgramRuningFlag(1);
			SetButtonValue(6,2,0);
			OpenFan();//�򿪷���
		}
		USART_RX_STA = 0;
	}
}

//�����£���е��ʾ�̺�IO�ڹ�������
/*******************************************************************************
 * 6
 * 6.1
 * 6.2
 * 6.3
 * 6.4
********************************************************************************/

/*****************************************
* ��    �ƣ� screen1(6)
* ��    �ܣ� ���˵�һ���ж�����һ����ť����,�л���Ӧ��ҳ��
*/
static void screen1()
{
	unsigned long key_value;
	SetScreen(1);
	G_BUTTON.SCREEN_ID = 1;

	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			switch(G_BUTTON.CONTROL_ID)//�ж�����һ����ť����,�л�����Ӧҳ��
			{
				case 1: SCREEN_SIGN = 2;  break;
				case 2: SCREEN_SIGN = 3;  break;
 				case 3: SCREEN_SIGN = 4;  break;
				case 4: SCREEN_SIGN = 5;  break;
				case 5: SCREEN_SIGN = 10; break;
			   default:		       break;
			}

			USART_RX_STA = 0;			//��־λ����

			if(G_BUTTON.CONTROL_ID == 6)//�����ҳ��12��ֱ���ڲ����ã�����ҳ��Ļ������˺���
			{
				screen12(1);
			}
			else break;
		}
		#if KEY_BOARD
		key_value = ReadKeyboardValue();
		if(key_value == ReadKeyboardValue())
		{
			if(1 == switch_teach_model(key_value, 10)){return;}
			KeyboardDataProcessing(key_value);
		}
		#endif
		ErrCheck(1);

	}
}

/****************************************
* ��    �ƣ� screen12(6.1)
* ��    �ܣ� �ж�����һ����ť����,�򿪻��߹رն�Ӧ��IO��
*/
static void screen12(char screen_id)
{
	unsigned long key_value;
	u32 input_state=0,t=0;
	u8 i;

	SetScreen(12);
	G_BUTTON.SCREEN_ID = 12;
	G_BUTTON.CONTROL_ID = 0;

	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_ID == 18 || G_BUTTON.CONTROL_ID==200)
			{
				G_BUTTON.CONTROL_ID = 0;
				USART_RX_STA = 0;
				SetScreen(screen_id);
				G_BUTTON.SCREEN_ID = screen_id;
				break;
			}

			//G_BUTTON.CONTROL_ID��ΧΪ2~9���ֱ��Ӧ��ť��0~7; ID15~17�ֱ��Ӧɲ��J1~J3��ɲ��
			if(G_BUTTON.BUTTON_STATUS == 1)///G_BUTTON.BUTTON_STATUSΪ1��ʱ���ʾ�򿪣�0��ʱ���ʾ�ر�
			{
				 if((G_BUTTON.CONTROL_ID>=2) && (G_BUTTON.CONTROL_ID <=13))
				 {
					  SetPoutState(G_BUTTON.CONTROL_ID-2);
				 }
				 else if((G_BUTTON.CONTROL_ID>=15) && (G_BUTTON.CONTROL_ID<=17))
				 {
					  OpenBrake(G_BUTTON.CONTROL_ID-15);
				 }
				 else if((G_BUTTON.CONTROL_ID>=20) && (G_BUTTON.CONTROL_ID<=31))
				 {
					  SetPoutState(G_BUTTON.CONTROL_ID-8);
				 }
			}
			else//�ر�
			{
				 if((G_BUTTON.CONTROL_ID>=2) && (G_BUTTON.CONTROL_ID <=13))
				 {
					  ResetPoutState(G_BUTTON.CONTROL_ID-2);
				 }
				 else if((G_BUTTON.CONTROL_ID>=15) && (G_BUTTON.CONTROL_ID<=17))
				 {
					  CloseBrake(G_BUTTON.CONTROL_ID-15);
				 }
				 else if((G_BUTTON.CONTROL_ID>=20) && (G_BUTTON.CONTROL_ID<=31))
				 {
					  ResetPoutState(G_BUTTON.CONTROL_ID-8);
				 }
			}

			G_BUTTON.CONTROL_ID = 0;
			USART_RX_STA = 0;
		}
#if KEY_BOARD
		key_value = ReadKeyboardValue();
		if(key_value == ReadKeyboardValue())
		{
			if(key_value == 0x00008000)
			{
				SetScreen(screen_id);
				G_BUTTON.SCREEN_ID = screen_id;
				delay_1ms(400);
				break;
			}
			KeyboardDataProcessing(key_value);
		}
#endif
		ErrCheck(12);

		t++;
		if(t>3000)
		{
			input_state = ReadPinState();	//��ȡ��ǰ����IO״̬
			for(i=0;i<MAX_PIN_NUM;i++)
			{
				DisGifFrame(48,i+50,(input_state>>i)&((u32)0x01));
			}
			t=0;
		}
	}
}

/*******************************
* ��    �ƣ�
* ��    �ܣ� �л�ʾ������
*/
static u8 switch_teach_model(u32 key_value, u8 screen_sign)
{
	if(0x00000001 == key_value)
	{
		SCREEN_SIGN = screen_sign;
		delay_1ms(250);
		return 1;
	}
	return 0;
}
/*******************************
* ��    �ƣ� screen10(6.2)
* ��    �ܣ� ����ʾ�̻���
*/
static void screen10()
{
	unsigned long key_value;
	char str[4]={0,0,0,0};
	SetScreen(10);
	G_BUTTON.SCREEN_ID = 10;
	SetSliderValue(10,5,SPEED_VALUE);
	ltoa(SPEED_VALUE,str);
	SetTextValue(10,18,str);
	USART_RX_STA = 0;
	TEACH_MODEL = 0;
	SetModeState(TEACH_MODEL);
	delay_1ms(200);
	SetModeState(TEACH_MODEL);
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_TYPE==11)
			{
				switch(G_BUTTON.CONTROL_ID)
				{
					case 1: SCREEN_SIGN=14; break;
					case 2: SCREEN_SIGN=15; break;
					case 3: SCREEN_SIGN=7; break;
					case 4: SCREEN_SIGN=1; break;
				   default: SCREEN_SIGN=0; break;
				}
				if(SCREEN_SIGN>0) break;
			}

			if(G_BUTTON.CONTROL_TYPE==11)
			{
				if(!DisplayErrInfo(0x4B))//0000 0100 1011
				{
					 RecoverSpeed();
					 USART_RX_STA = 0;
					 switch(G_BUTTON.CONTROL_ID)
					 {
						 case 6://J1-
							 if(DisplayErrInfo(0xCB)) break;
							 while(!USART_RX_STA)
							 {
								  if(AxisSingleRun(1,1,1)) break;
							 }
							 AxisSingleRun(0,1,1);//ֹͣ����
							 break;

						 case 7://J2-
							 if(DisplayErrInfo(0x14B)) break;
							 while(!USART_RX_STA)
							 {
								 if(AxisSingleRun(1,2,1)) break;
							 }
							 AxisSingleRun(0,2,1);
							 break;

						 case 8://J3-
							 if(DisplayErrInfo(0x24B)) break;
							 while(!USART_RX_STA)
							 {
								 if(AxisSingleRun(1,3,1)) break;
							 }
							 AxisSingleRun(0,3,1);
							 break;

						 case 9:
							 while(!USART_RX_STA)
							 {
								 if(ROBOT_PARAMETER.AXIS_NUM == 6)
								 {
									 if(AxisSingleRun(1,4,1)) break;
								 }
							 }
							 AxisSingleRun(0,4,1);
							 break;//J4-

						 case 10:
							 while(!USART_RX_STA)
							 {
								 if(AxisSingleRun(1,5,1)) break;
							 }
							 AxisSingleRun(0,5,1);
							 break;//J5-

						 case 11:
							 while(!USART_RX_STA)
							 {
								 if(AxisSingleRun(1,6,1)) break;
							 }
							 AxisSingleRun(0,6,1);
							 break;//J6-

						 case 12:
							 if(DisplayErrInfo(0xCB)) break;
							 while(!USART_RX_STA)
							 {
								 if(AxisSingleRun(1,1,0)) break;
							 }
							 AxisSingleRun(0,1,0);
							 break;//J1+

						 case 13:
							 if(DisplayErrInfo(0x14B)) break;
							 while(!USART_RX_STA)
							 {
								 if(AxisSingleRun(1,2,0)) break;
							 }
							 AxisSingleRun(0,2,0);
							 break;//J2+

						 case 14:
							 if(DisplayErrInfo(0x24B)) break;
							 while(!USART_RX_STA)
							 {
								  if(AxisSingleRun(1,3,0)) break;
							 }
							 AxisSingleRun(0,3,0);
							 break;//J3+

						 case 15:
							 while(!USART_RX_STA)
							 {
								 if(ROBOT_PARAMETER.AXIS_NUM == 6)
								 {
									 if(AxisSingleRun(1,4,0)) break;
								 }
							 }
							 AxisSingleRun(0,4,0);
							 break;//J4+

						 case 16:
							 while(!USART_RX_STA)
							 {
								  if(AxisSingleRun(1,5,0)) break;
							 }
							 AxisSingleRun(0,5,0);
							 break;//J5+

						 case 17:
							 while(!USART_RX_STA)
							 {
								  if(AxisSingleRun(1,6,0)) break;
							 }
							 AxisSingleRun(0,6,0);
							 break;//J6+
					 }
					 display_sixangle(7);//ȫ������ʱ��ʾ������ĽǶ�
				}
				//SetButtonValue(10,G_BUTTON.CONTROL_ID,0);//��ť������
				G_BUTTON.BUTTON_STATUS=0;
			}
			if(G_BUTTON.CONTROL_TYPE==13)
			{
				if(G_BUTTON.SLIDER_VALUE==0)
				{
					 G_BUTTON.SLIDER_VALUE=1;
				}
				if(!GetSpeedMode() && (G_BUTTON.SLIDER_VALUE > 15))
				{
						G_BUTTON.SLIDER_VALUE = 15;
				}
				SetSliderValue(10,5,G_BUTTON.SLIDER_VALUE);
				ltoa(G_BUTTON.SLIDER_VALUE,str);
				SetTextValue(10,18,str);
				SPEED_VALUE=G_BUTTON.SLIDER_VALUE;
			}
			USART_RX_STA = 0;
		}
#if KEY_BOARD
		key_value = ReadKeyboardValue();
		if(key_value == ReadKeyboardValue())
		{
			if(1 == switch_teach_model(key_value, 14)){return;}
			KeyboardDataProcessing(key_value);
		}
#endif
		ErrCheck(10);//ɨ���Ƿ��б���
	}
}

/************************************
* ��    �ƣ� screen22(6.3)
* ��    �ܣ� ������ϵֱ������ת�˶�
*/
static void screen22()
{
	unsigned long key_value;
	char str[4]={0,0,0,0};
	SetScreen(22);
	G_BUTTON.SCREEN_ID = 22;
	SetSliderValue(22,5,SPEED_VALUE);
	ltoa(SPEED_VALUE,str);
	SetTextValue(22,19,str);
	TEACH_MODEL = 1;
	SetModeState(TEACH_MODEL);
	delay_1ms(200);
	SetModeState(TEACH_MODEL);
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_TYPE==11)
			{
				switch(G_BUTTON.CONTROL_ID)
				{
					case 1:SCREEN_SIGN=10;break;
					case 2:SCREEN_SIGN=15;break;
					case 3:SCREEN_SIGN=7;break;
					case 4:SCREEN_SIGN=1;break;
				   default:SCREEN_SIGN=0;break;
				}
				if(SCREEN_SIGN>0) break;
			}
			if(G_BUTTON.CONTROL_TYPE==11)
			{
				if(!DisplayErrInfo(0x3FB))
				{
					 USART_RX_STA = 0;
					 RecoverSpeed();
					 switch(G_BUTTON.CONTROL_ID)//6(J1-)-11(J6-)��12(J1+)-17(J6+)
					 {
						 case 6:
							   while(!USART_RX_STA)
							   {
								    if(BaseCoordSlineRun(G_BUTTON.BUTTON_STATUS,1))//X-
								    {
									    break;
								    }
							   }
							   BaseCoordSlineRun(0,0);//��ť�ɿ�����
							   break;


						 case 7:
							  while(!USART_RX_STA)
							  {
									if(BaseCoordSlineRun(G_BUTTON.BUTTON_STATUS,3))//Y-
									{
										break;
									}
							  }
							  BaseCoordSlineRun(0,0);//��ť�ɿ�����
							  break;

						 case 8:
							  while(!USART_RX_STA)
							  {
								   if(BaseCoordSlineRun(G_BUTTON.BUTTON_STATUS,5))//Z-
								   {
									   break;
								   }
							  }
							  BaseCoordSlineRun(0,0);//��ť�ɿ�����
							  break;

						 case 9:
							 while(G_BUTTON.BUTTON_STATUS);//
							 break;

						 case 10:
							 while(G_BUTTON.BUTTON_STATUS);//
							 break;

						 case 11:
							 while(G_BUTTON.BUTTON_STATUS);//
							 break;

						 case 12:
							  while(!USART_RX_STA)
							  {
									if(BaseCoordSlineRun(G_BUTTON.BUTTON_STATUS,2))//X+
									{
										break;
									}
							  }
							  BaseCoordSlineRun(0,0);//��ť�ɿ�����
							 break;

						 case 13:
							  while(!USART_RX_STA)
							  {
									if(BaseCoordSlineRun(G_BUTTON.BUTTON_STATUS,4))//Y+
									{
										break;
									}
							  }
							  BaseCoordSlineRun(0,0);//��ť�ɿ�����
							  break;

						 case 14:
							  while(!USART_RX_STA)
							  {
									if(BaseCoordSlineRun(G_BUTTON.BUTTON_STATUS,6))//Z+
									{
										break;
									}
							  }
							  BaseCoordSlineRun(0,0);//��ť�ɿ�����
							  break;

						 case 15:
							 while(G_BUTTON.BUTTON_STATUS);//
							 break;

						 case 16:
							 while(G_BUTTON.BUTTON_STATUS);//
							 break;

						 case 17:
							 while(G_BUTTON.BUTTON_STATUS);//
							 break;
					 }
					 display_sixangle(7);//ȫ������ʱ��ʾ������ĽǶ�
				}
				 SetButtonValue(22,G_BUTTON.CONTROL_ID,0);//��ť������
				 G_BUTTON.CONTROL_TYPE = 0;
				 G_BUTTON.BUTTON_STATUS =0;
			}
			if(G_BUTTON.CONTROL_TYPE==13)
			{
				if(G_BUTTON.SLIDER_VALUE==0)
				{
					 G_BUTTON.SLIDER_VALUE=1;
				}
				if(!GetSpeedMode() && (G_BUTTON.SLIDER_VALUE > 25))
				{
						G_BUTTON.SLIDER_VALUE = 25;
				}
				SetSliderValue(22,5,G_BUTTON.SLIDER_VALUE);

				ltoa(G_BUTTON.SLIDER_VALUE,str);
				SetTextValue(22,19,str);
				SPEED_VALUE=G_BUTTON.SLIDER_VALUE;
			}
			USART_RX_STA = 0;
		}
		ErrCheck(22);
#if KEY_BOARD
		key_value = ReadKeyboardValue();
		if(key_value == ReadKeyboardValue())
		{
			if(1 == switch_teach_model(key_value, 10)){return;}
			KeyboardDataProcessing(key_value);
		}
#endif
	}
}

/*******************************
* ��    �ƣ� screen23(6.4)
* ��    �ܣ� ĩ������ϵ����ϵ
*/
static void screen23()
{
	unsigned long key_value;
	char str[4]={0,0,0,0};
	SetScreen(23);
	G_BUTTON.SCREEN_ID = 23;
	SetSliderValue(23,5,SPEED_VALUE);
	ltoa(SPEED_VALUE,str);
	//SetSpeed(0x3f,SPEED_VALUE*80);
	SetTextValue(23,18,str);

	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_TYPE==11)
			{
				switch(G_BUTTON.CONTROL_ID)
				{
					case 1:SCREEN_SIGN=10;break;
					case 2:SCREEN_SIGN=14;break;
					case 3:SCREEN_SIGN=7;break;
					case 4:SCREEN_SIGN=1;break;
				   default:SCREEN_SIGN=0;break;
				}
				if(SCREEN_SIGN>0) break;
			}
			if(G_BUTTON.CONTROL_TYPE==11)
			{
				USART_RX_STA = 0;
				switch(G_BUTTON.CONTROL_ID)//6(J1-)-11(J6-)��12(J1+)-17(J6+)
				{
					case 6://J1-
						 break;

					case 7://J2-
						 break;

					case 8://J3-
						 break;

					case 9://J4-
						 break;

					case 10://J5-
						 while(G_BUTTON.BUTTON_STATUS);
						 break;

					case 11://J6-
						 while(G_BUTTON.BUTTON_STATUS);
						 break;

					case 12://J1+
						 while(G_BUTTON.BUTTON_STATUS);
						 break;

					case 13://J2+
						 while(G_BUTTON.BUTTON_STATUS);
						 break;

					case 14://J3+
						 while(G_BUTTON.BUTTON_STATUS);
						 break;

					case 15://J4+
						 while(G_BUTTON.BUTTON_STATUS);
						 break;

					case 16://J5+
						 while(G_BUTTON.BUTTON_STATUS);
						 break;

					case 17://J6+
						 break;
				}
			}
			if(G_BUTTON.CONTROL_TYPE==13)
			{
				if(G_BUTTON.SLIDER_VALUE==0)
				{
					 G_BUTTON.SLIDER_VALUE=1;
				}
				if(!GetSpeedMode() && (G_BUTTON.SLIDER_VALUE > 25))
				{
						G_BUTTON.SLIDER_VALUE = 25;
				}
				SetSliderValue(23,5,G_BUTTON.SLIDER_VALUE);

				ltoa(G_BUTTON.SLIDER_VALUE,str);
				SetTextValue(23,18,str);
				SPEED_VALUE=G_BUTTON.SLIDER_VALUE;
			}
			USART_RX_STA = 0;
			G_BUTTON.BUTTON_STATUS=0;
		}
#if KEY_BOARD
		key_value = ReadKeyboardValue();
		if(key_value == ReadKeyboardValue())
		{

			if(1 == switch_teach_model(key_value, 7)){return;}
			KeyboardDataProcessing(key_value);
		}
#endif
		ErrCheck(23);
	}
	SPEED_VALUE = G_BUTTON.SLIDER_VALUE;
}

void IO_Screen_Switch(void)
{
	static u16 s_screen_id=12;	//
	if(G_BUTTON.SCREEN_ID !=12)
	{
		s_screen_id = G_BUTTON.SCREEN_ID;
		G_BUTTON.SCREEN_ID = 12;
		SetScreen(12);
	}
	else
	{
		G_BUTTON.SCREEN_ID = s_screen_id;
		SetScreen(s_screen_id);
	}
	delay_1ms(350);
}

//�����£�������������
/********************************************************************************
 * 7
 * 7.1
 * 7.2
 * 7.3
 * 7.4
*********************************************************************************/

/********************************************
* ��    �ƣ� screen2(7)
* ��    �ܣ� ���˵�2���ж�����һ����ť����,�л�����Ӧҳ�����
*/
static void screen2()
{
	unsigned long key_value;
	char str[3]={0,0,0};

	if(DEVELOPER_MODE)
	{
		SetScreen(44);
		G_BUTTON.SCREEN_ID = 44;
	}
	else
	{
		SetScreen(2);
		G_BUTTON.SCREEN_ID = 2;
	}

	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_TYPE == 11)//��ť���ͣ�ʵ��ҳ���л�
			{
				switch(G_BUTTON.CONTROL_ID)//�ж�����һ����ť����,�л�����Ӧҳ��
				{
					case 1: SCREEN_SIGN=1; break;
					case 2: SCREEN_SIGN=3; break;
					case 3: SCREEN_SIGN=4; break;
					case 4: SCREEN_SIGN=5; break;
					case 5: screen8(); SCREEN_SIGN=0; break;	//P.λ�ù���(����)
					case 6: screen11(); SCREEN_SIGN=0; break;	//�ɵ���λ����ҳ��(����)
					case 7: screen13(); SCREEN_SIGN=0; break;	//��ʱ����
				  default : origin_sound_alarm(); SCREEN_SIGN=0; break;
				}
				if(SCREEN_SIGN > 0) break;
			}

			if(G_BUTTON.CONTROL_TYPE == 13)//�������ͣ�ʵ�ֱ������
			{
				ltoa(G_BUTTON.SLIDER_VALUE, str);
				SetTextValue(2, 12, str);
				if(G_BUTTON.SLIDER_VALUE < 20)
				{
					G_BUTTON.SLIDER_VALUE = 20;
				}
				SetBackLight((100-G_BUTTON.SLIDER_VALUE) * 2.55);
			}

			G_BUTTON.CONTROL_TYPE = 0;
			G_BUTTON.CONTROL_ID = 0;
			USART_RX_STA = 0;
		}
		#if KEY_BOARD
		key_value = ReadKeyboardValue();
		if(key_value == ReadKeyboardValue())
		{
			if(1 == switch_teach_model(key_value, 10)){return;}
			KeyboardDataProcessing(key_value);
		}
		#endif
		ErrCheck(G_BUTTON.SCREEN_ID);
	}
}


/**************************************
����:ԭ����������ȵ��趨(7.1)
*/
static void origin_sound_alarm()
{
	USART_RX_STA = 0;

	switch(G_BUTTON.CONTROL_ID)
	{
		case 8: locked_origin(); break;//���½���Ϊһ2.11
		case 9: return_home(); break;//ͬ��2.12
		case 10: sound_settings(); break;//2.13
		case 11: clear_alarm(); break;//2.14
		case 13: servo_motor(G_BUTTON.BUTTON_STATUS); break;//2.14
		case 14: formatting_sd(); break;//2.14
		case 16: developer_mode();break;
		case 20: switch_alarm_signal_mode();break;
		case 21: switch_limit_mode();break;
		case 22: switch_teach_speed_mode();break;
		case 23: setting_robot_para();break;
		case 24: switch_mode();break;
	    default:				  break;
	}

	if(DEVELOPER_MODE)
	{
		SetScreen(44);
		G_BUTTON.SCREEN_ID = 44;
	}
	else
	{
		SetScreen(2);
		G_BUTTON.SCREEN_ID = 2;
	}
	G_BUTTON.CONTROL_ID = 0;
	G_BUTTON.CONTROL_TYPE = 0;
	G_BUTTON.BUTTON_STATUS = 0;
}

/*************************************
����:ԭ������ѡ�����(7.11)
*/
static void locked_origin()
{
	u8 tmp;
	tmp = ErrCheck(2);//����֮ǰ������û�б���
	if(DisplayErrInfo(0x02))//�б�����������ԭ��
		return;

	SetScreen(15);//ԭ����������
	G_BUTTON.SCREEN_ID = 15;
	G_BUTTON.CONTROL_ID = 0;

	while(1)
	{
		if(G_BUTTON.CONTROL_ID > 1)//�м�����
		{
			switch(G_BUTTON.CONTROL_ID)
			{
				case 3: sure_locked_origin(); break;
				case 2:					      break;
			   default:				          break;
			}
			break;
		}

		if(ErrCheck(2)&(~tmp))
		{
			break;
		}//֮ǰ�б����ľ����ε�
	}
	G_BUTTON.SCREEN_ID = 2;

}

/*********************************
����:ԭ������(7.111)
����:��
*/
static void sure_locked_origin()
{
	u8 tmp;
	u8 return_value = 0;
	u8 sign_password = 0;
	tmp = ErrCheck(2);//����֮ǰ������û�б���
	USART_RX_STA = 0;
	SetTextValue(30,1,"Enter the password:");
	SetScreen(30);
	G_BUTTON.SCREEN_ID = 30;

	while(1)//�ȴ��������벢�������룬ִ������ԭ������Ĺ���ģ��
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_ID == 4)		//ֱ��ȡ��
			{	break;}

			if(G_BUTTON.CONTROL_ID == 3)		//ȷ�������˶����룬��ʾ�Ƿ���ȷ
			{
				sign_password = 1;				//�趨Ϊ1����������������Ϊ0
				if((USART_RECEIVE_STRING[0]-'0') != 1) sign_password = 0;
				if((USART_RECEIVE_STRING[1]-'0') != 2) sign_password = 0;
				if((USART_RECEIVE_STRING[2]-'0') != 3) sign_password = 0;
				if((USART_RECEIVE_STRING[3]-'0') != 4) sign_password = 0;
				if((USART_RECEIVE_STRING[4]-'0') != 5) sign_password = 0;
				if((USART_RECEIVE_STRING[5]-'0') != 6) sign_password = 0;
				if(USART_RECEIVE_STRING[6] != 0) sign_password = 0;

				if(sign_password == 1)//������ȷ
				{
					return_value = ClearOrigin();//��������ԭ�㺯�����ش�����Ϣ��sign_return
					if(return_value == 0)//����ԭ��ɹ���ҳ����ʾԭ�������ɹ�
					{
						G_BUTTON.SCREEN_ID = 16;
						SetScreen(16);
					}
					else/*ԭ������ʧ��*/
					{
						USART_RX_STA=0xc000;
						 SetScreen(20);
						 SetTextValue(20, 4, "");
						 SetTextValue(20, 3, "Lock origin point error!");
						 delay_1ms(1300);

						if(DEVELOPER_MODE)
						{
							SetScreen(44);
							G_BUTTON.SCREEN_ID = 44;
						}
						else
						{
							SetScreen(2);
							G_BUTTON.SCREEN_ID = 2;
						}

					}

					USART_RX_STA = 0;
					while(1)
					{
						if(USART_RX_STA&0x8000)
						{break;}

						if(ErrCheck(2)&(~tmp))
						{break;}
					}
				}
				if(sign_password == 0)//�������
				{
					SetTextValue(30,2,"");
					DispalyProgEditInfo(7);//�������
				}
				break;//����������
			}
			USART_RX_STA = 0;
		}
	}

	USART_RECEIVE_STRING[0] = 0;
	USART_RX_STA = 0;
	G_BUTTON.SCREEN_ID = 2;
}
/********************************
����:����ԭ��(7.12)
//static return_home ����Ϊ//2018/06/1
*/
static void return_home()
{
	static u8 speed = 8;
	if(DisplayErrInfo(0x38F))
	{
		 return;
	}
	SetScreen(17);
	G_BUTTON.SCREEN_ID = 17;
	G_BUTTON.CONTROL_ID = 0;

	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_ID == 4) //���·���ԭ��������ٶȲ���Ϊ��Чֵ
			{
				sure_return_home(speed);
				break;
			}

			if(G_BUTTON.CONTROL_ID == 3)//����ȡ���������������������˵������
			{	break;	}

			if(G_BUTTON.CONTROL_TYPE == 15)//��ȡ��������ķ���ԭ��ʱ���ٶȲ���
			{
				if((USART_RECEIVE_STRING[0]>='0') && (USART_RECEIVE_STRING[0]<='9'))
				{
					speed = atoi((char*)USART_RECEIVE_STRING);
				}
				else
				{
					SetTextValue(17, 2, "6");
					speed = 6;
				}
			}

			G_BUTTON.CONTROL_ID = 0;
			G_BUTTON.CONTROL_TYPE = 0;
			USART_RX_STA = 0;
		}

		if(ErrCheck(2)&0x30)break;
	}
	G_BUTTON.SCREEN_ID = 2;
}
/********************************
����:ͨ��������ƻ�е�ַ���ԭ��(7.12)
������ return_home_fornetwork()
 ������//2018/06/1
*********************************/
void return_home_network()
{
	SetScreen(2);
	G_BUTTON.SCREEN_ID = 2;
	G_BUTTON.CONTROL_ID = 0;
	SetButtonValue(2, 8, 1);
	delay_1ms(1000);
	sure_return_home(10);
}
/********************************
*����:���ٶȾ������(7.121)
*/
static void noparameter(u8 screen_id)
{
	u8 tmp;
	tmp = ErrCheck(2);//����֮ǰ������û�б���
	SetScreen(26);
	G_BUTTON.SCREEN_ID = 26;
	G_BUTTON.CONTROL_ID = 0;
	USART_RX_STA = 0;

	while(1)
	{
		if(G_BUTTON.CONTROL_ID == 2)
		{
			USART_RX_STA = 0;
			break;
		}

		if(ErrCheck(2)&(~tmp))
		{
			break;
		}
	}

	SetScreen(screen_id);
	G_BUTTON.CONTROL_ID = 0;
	G_BUTTON.SCREEN_ID = 2;
}

/***************************
����:ȷ�Ϸ���ԭ��(7.122)
*/
static void sure_return_home(u8 speed)
{
	SetScreen(18);
	G_BUTTON.SCREEN_ID = 18;
	G_BUTTON.CONTROL_ID = 0;
	USART_RX_STA = 0;

	if(!DisplayErrInfo(0x38F))
	{
		 ReturnHome(speed);
	}

	SetScreen(37);
	while(ReadDriState())
	{
		 if(ErrCheck(2))break;
	}

	/*while(1)
	{
		 //xxx ��̬��ʾ��ǰ�Ƕ�
		if(ErrCheck(2))
		{
			break;//������λ
		}

		if(G_BUTTON.CONTROL_ID == 2)
		{
			break;
		}
	}*/
}

/******************************
����:��������(7.13)
*/
static void sound_settings()
{
	if(G_BUTTON.BUTTON_STATUS)
	{
		delay_1ms(10);
		SetTouchScreen(1);//�ر�����
		delay_1ms(10);
		SetButtonValue(2, 10, 1);
		SetButtonValue(44, 10, 1);
		ROBOT_PARAMETER.SOUND = 0;
		SaveRobotPara(ROBOT_PARAMETER);//�����е�ֲ���
	}

	if(!G_BUTTON.BUTTON_STATUS)
	{
		delay_1ms(10);
		SetTouchScreen(3);//������
		delay_1ms(10);
		SetButtonValue(2, 10, 0);
		SetButtonValue(44, 10, 0);
		ROBOT_PARAMETER.SOUND = 1;
		SaveRobotPara(ROBOT_PARAMETER);//�����е�ֲ���
	}
}

/******************************
����:�������(7.14)
*/
void clear_alarm(void)
{
	 ClearAlarm();
	 G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG=0;//����ŷ�������־
	 ErrCheck(G_BUTTON.SCREEN_ID);
	 if(G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG)
	 {
		 SetScreen(21);
		 SetTextValue(21, 2, "Fail to clear alarm!");
		 delay_1ms(1000);
	 }
	 else
	 {
		 SetScreen(21);
		 SetTextValue(21, 2, "Clear alarm successful!");
		 delay_1ms(1000);
	 }
}

/*****************************
����:�ŷ�����(7.15)
������flag  0:�ر��ŷ�	1�����ŷ�
*/
void servo_motor(u8 flag)
{
	u8 return_value = 0;
	USART_RX_STA = 0;

	if(DisplayErrInfo(0x02))
	{
		SetButtonValue(2, 13, 0);//��ť��Ϊ �ŷ��ر� ״̬
		SetButtonValue(44, 13, 0);
		return;
	}

	if(flag == 1)//���ŷ�
	{
		SetScreen(37);
		return_value = ServoON(0);
		SetScreen(G_BUTTON.SCREEN_ID);
		//SetButtonValue(2, 13, 1);//����Ѿ��ŵ���ServoON������
	    if(return_value > 0)
	    {
	    	display_open_servo_fail_inform(return_value);
	    	//������ģʽ�£���ʹ�����ȡ�쳣��Ҳ����ŷ�
	    	if(DEVELOPER_MODE)
	    	{
	    		SetScreen(44);
	    		delay_1ms(10);
	    		SetScreen(37);
	    		ServoON(1);	//ǿ�ƴ��ŷ�
	    		SetScreen(44);
	    	}
	    }
	}

	if(flag == 0)//�ر��ŷ�
	{
		ServoOFF();
		//SetButtonValue(2, 13, 0);//����Ѿ��ŵ���ServoOFF������
	}

	USART_RX_STA = 0;
}

/*****************************
����:��ʾ���ŷ�ʧ����Ϣ(7.151)
*/
static void display_open_servo_fail_inform(u8 servo_inform)
{
	char str[60],i;
	USART_RX_STA = 0;
	SetScreen(20);//xxx Ҫ��ʾ�ڼ������������쳣

	str[0] = 0;
	if( (servo_inform & 0x01) )
	{
		strcat(str,"1,");
	}
	if( (servo_inform & 0x02) )
	{
		strcat(str,"2,");
	}
	if( (servo_inform & 0x04))
	{
		strcat(str,"3,");
	}
	if( (servo_inform & 0x08))
	{
		strcat(str,"4,");
	}
	if( (servo_inform & 0x10))
	{
		strcat(str,"5,");
	}
	if( (servo_inform & 0x20) )
	{
		strcat(str,"6,");

	}
	SetTextValue(20, 4, "");

	for(i=0;i<36;i++)
	{
		if(str[i] == 0) break;
	}
	if(i!=0)
	{
		if(str[i-1] == ',')
			str[i-1] = 0;
	}
	strcat(str," axis read encoder failed!");
	SetTextValue(20, 3, str);
	while(1)
	{
		  if(USART_RX_STA&0x8000)
		  {break;}
	}
}

/***********************
����:��ʽ��sd��(7.16)
*/
static void formatting_sd(void)
{
	u8 tmp,err_num;
	u8 sign_password = 0;
	USART_RX_STA = 0;

	SetScreen(19);
	SetTextValue(19, 1, "Formatting will clear all data,are you sure to format?");
	G_BUTTON.SCREEN_ID = 19;
	tmp=1;
	while(tmp)
	{
		if(USART_RX_STA&0x8000)
		{
			switch(G_BUTTON.CONTROL_ID)
			{
				case 2:	//NO
					USART_RX_STA = 0;
					G_BUTTON.CONTROL_ID = 0;
					return;

				case 3:	//YES
					tmp = 0;
					USART_RX_STA = 0;
					break;

				case 4:	//CANCEL
				USART_RX_STA = 0;
				G_BUTTON.CONTROL_ID = 0;
				return;

				default:break;
			}
			USART_RX_STA = 0;
			G_BUTTON.CONTROL_ID = 0;
		}
	}

	tmp = ErrCheck(2);//����֮ǰ������û�б���
	SetTextValue(30,1,"Enter the password!");
	SetScreen(30);
	G_BUTTON.SCREEN_ID = 30;
	USART_RX_STA = 0;
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_ID == 4)		//ֱ��ȡ��
			{	break;}

			if(G_BUTTON.CONTROL_ID == 3)		//ȷ�������˶����룬��ʾ�Ƿ���ȷ
			{
				sign_password = 1;
				if((USART_RECEIVE_STRING[0]-'0') != 1) sign_password = 0;
				if((USART_RECEIVE_STRING[1]-'0') != 2) sign_password = 0;
				if((USART_RECEIVE_STRING[2]-'0') != 3) sign_password = 0;
				if((USART_RECEIVE_STRING[3]-'0') != 4) sign_password = 0;
				if((USART_RECEIVE_STRING[4]-'0') != 5) sign_password = 0;
				if((USART_RECEIVE_STRING[5]-'0') != 6) sign_password = 0;
				if(USART_RECEIVE_STRING[6] != 0) sign_password = 0;

				if(sign_password == 1)//������ȷ
				{
					SetScreen(37);
					err_num = FormatFlash();
					if(err_num < 100)
					{
						/*SetScreen(29);//��ʾ�Ƿ��ʽ���ɹ�
						USART_RX_STA = 0;
						while(1)
						{
							if(USART_RX_STA&0x8000)
							{	break;}
						}
						*/
						DispalyProgEditInfo(5);//��ʾ��ʽ���ɹ�
					}
					else
					{
						DispalyProgEditInfo(6);//��ʾ��ʽ��ʧ��
					}

				}
				if(sign_password == 0)//�������
				{
					SetTextValue(30,2,"");
					DispalyProgEditInfo(7);//�������
				}
				USART_RECEIVE_STRING[0] = 0;
				break;//����������
			}
			USART_RX_STA = 0;
		}

		if(ErrCheck(2)&(~tmp))
		{break;}
	}
	USART_RX_STA = 0;
}

/***********************
����:ѡ�񿪷�ģʽ�����û�ģʽ
*/
static void developer_mode(void)
{
	u8 sign_password=0;
	char str[5];
	if(!G_BUTTON.BUTTON_STATUS)/*0��ʾ����״̬��1��ʾ����״̬*/
	{
		SetScreen(2);
		SetButtonValue(2,16,0);
		G_BUTTON.SCREEN_ID = 2;
		DEVELOPER_MODE = 0;
		DeveloperMode_Off();
		SPEED_VALUE = 5;//�ص�������ģʽ��ʾ���ٶȻָ�Ĭ��ֵ
		RecoverSpeed();
		ltoa(SPEED_VALUE,str);

		SetSliderValue(10,5,SPEED_VALUE);//�����������ҳ���ٶ�������
		SetTextValue(10,18,str);//�ٶ�����ֵ����
		SetSliderValue(22,5,SPEED_VALUE);//ֱ�߲岹�������ҳ���ٶ�������
		SetTextValue(22,19,str);//�ٶ�����ֵ����
		SetSliderValue(23,5,SPEED_VALUE);//ĩ�������������ҳ���ٶ�������
		SetTextValue(23,18,str);//�ٶ�����ֵ����
	}

	if(G_BUTTON.BUTTON_STATUS)
	{
		SetTextValue(30,1,"Enter the password!");
		SetScreen(30);//��ʾ�����������
		G_BUTTON.SCREEN_ID = 30;

		while(1)//���������������
		{
			if(USART_RX_STA&0x8000)
			{
				if(G_BUTTON.CONTROL_ID == 4)		//ֱ��ȡ��
				{	break;}

				if(G_BUTTON.CONTROL_ID == 3)		//ȷ�������˶����룬��ʾ�Ƿ���ȷ
				{
					sign_password = 1;
					if((USART_RECEIVE_STRING[0]-'0') != 1) sign_password = 0;
					if((USART_RECEIVE_STRING[1]-'0') != 2) sign_password = 0;
					if((USART_RECEIVE_STRING[2]-'0') != 3) sign_password = 0;
					if((USART_RECEIVE_STRING[3]-'0') != 4) sign_password = 0;
					if((USART_RECEIVE_STRING[4]-'0') != 5) sign_password = 0;
					if((USART_RECEIVE_STRING[5]-'0') != 6) sign_password = 0;
					if(USART_RECEIVE_STRING[6] != 0) sign_password = 0;

					if(sign_password == 0)//�������
					{
						SetTextValue(30,2,"");
						DispalyProgEditInfo(7);//�������
					}
					USART_RECEIVE_STRING[0] = 0;
					break;
				}
				USART_RX_STA = 0;
			}
		}

		if(1 == sign_password)//������ȷ
		{
			SetButtonValue(44,20,0);
			SetButtonValue(44,21,0);
			SetButtonValue(44,22,0);
			SetButtonValue(44,16,1);
			SetButtonValue(44,24,0);//������//2018/06/01
			DEVELOPER_MODE = 1;
			G_BUTTON.SCREEN_ID = 44;
			DeveloperMode_On();
			SetScreen(44);
		}
	}
}

/***********************
����:ѡ�񱨾�ģʽ�������α���ģʽ
*/
static void switch_alarm_signal_mode(void)
{
	if(1 == DEVELOPER_MODE)
	{
		if(!G_BUTTON.BUTTON_STATUS)/*0��ʾ����״̬��1��ʾ����״̬*/
		{
			EnableServoAlarm();
		}

		if(G_BUTTON.BUTTON_STATUS)
		{
			DisableServoAlarm();
		}
	}
}

/***********************
����:ѡ����λģʽ���߲���λģʽ
*/
static void switch_limit_mode(void)
{
	if(1 == DEVELOPER_MODE)
	{
		if(!G_BUTTON.BUTTON_STATUS)/*0��ʾ����״̬��1��ʾ����״̬*/
		{
			EnableLimited();
		}

		if(G_BUTTON.BUTTON_STATUS)
		{
			DisableLimited();
		}
	}
}

/***********************
����:ѡ��ʾ�̵ĸ���ģʽ���ߵ���ģʽ
*/
static void switch_teach_speed_mode(void)
{
	char str[5];
	if(1 == DEVELOPER_MODE)
	{
		if(!G_BUTTON.BUTTON_STATUS)/*0��ʾ����״̬��1��ʾ����״̬*/
		{
			SetSpeedMode_Low();
			SPEED_VALUE = 5;//�ص�������ģʽ��ʾ���ٶȻָ�Ĭ��ֵ
			RecoverSpeed();
			ltoa(SPEED_VALUE,str);

			SetSliderValue(10,5,SPEED_VALUE);//�����������ҳ���ٶ�������
			SetTextValue(10,18,str);//�ٶ�����ֵ����
			SetSliderValue(22,5,SPEED_VALUE);//ֱ�߲岹�������ҳ���ٶ�������
			SetTextValue(22,19,str);//�ٶ�����ֵ����
			SetSliderValue(23,5,SPEED_VALUE);//ĩ�������������ҳ���ٶ�������
			SetTextValue(23,18,str);//�ٶ�����ֵ����
		}

		if(G_BUTTON.BUTTON_STATUS)
		{
			SetSpeedMode_High();
		}
	}
}

//����ֵ:0->���	1->�����
char robot_para_compare(RobotParaStru r1,RobotParaStru r2)
{
	u16 len,i;
	u16* p1;
	u16* p2;
	p1 = (u16*)(&r1);
	p2 = (u16*)(&r2);
	len = sizeof(RobotParaStru);
	for(i=0;i<len;i++)
	{
		if(*(p1+i) != *(p2+i))
		{
			return 1;
		}
	}
	return 0;
}


//GH4440
const RobotParaStru ROBOT_PARA_GH4440={
	"GH4440",
	4,	//����Ŀ
	1,		//��¼�Ƿ񻵿�	0->����	1->�ÿ�
	0,	//��¼�����Ƿ���Ҫ������������ƫ���� 1->������Ҫ���� 0->����Ҫ
	1,	//��¼������� 1->����	2->��Э
	0,	//1->��һ��ʹ�� 0->��ʹ�ù�
	1,	//����
	1,	//����
	FOUR_AXIS_C,		//ϵͳ����
	8,		//��·��汾��
	0x0F,		//���ѡ��(bit0~bit7 �ֱ��Ӧ1~8��)
	5000,		//���ÿ����ת��
	0x08,//001000B,		//�����ת���� (bit0~bit7 �ֱ��Ӧ1~8��)
	0x09,//001001	//����������תʱ�ķ���
	20000,	//���ÿȦ��Ӧ��������
	{0,0,0,0,0,0,0,0},	//���������ƫ����
	{0,0,0,0,0,0,0,0},	//������к�
	0.1,		//�岹����(��λ:mm)
	{175,230,165,16,0},	//��е�ֱ۳�֮��Ĳ��� һ��۳�������۳����ݸ˳��ȣ��ݾ�
	{ 100, 130,0.01, 360,0,0,0,0},//����λ(��λ:�Ƕ�)0.01?��0�Ļ�������ԭ����ֱ�Ӵ�����λ״̬
	{-100,-130,-360,-360,0,0,0,0},	//����λ(��λ�ǽǶ�)
	{0,0,0,0,0,0,0,0},				//ԭ��λ��
	{80,80,12.890625,3,0,0,0,0}	//���ٱ� (165mm)/(16mm/r)*2(ͬ���ֱ���)=20.625
};

//H4440
const RobotParaStru ROBOT_PARA_H4440={
	"H4440",
	4,	//����Ŀ
	1,		//��¼�Ƿ񻵿�	0->����	1->�ÿ�
	0,	//��¼�����Ƿ���Ҫ������������ƫ���� 1->������Ҫ���� 0->����Ҫ
	1,	//��¼������� 1->����	2->��Э
	0,	//1->��һ��ʹ�� 0->��ʹ�ù�
	1,	//����
	1,	//����
	FOUR_AXIS_B,		//ϵͳ����
	8,		//��·��汾��
	0x0F,		//���ѡ��(bit0~bit7 �ֱ��Ӧ1~8��)
	5000,		//���ÿ����ת��
	0x08,//001000B,		//�����ת���� (bit0~bit7 �ֱ��Ӧ1~8��)
	0x09,//001001	//����������תʱ�ķ���
	20000,	//���ÿȦ��Ӧ��������
	{0,0,0,0,0,0,0,0},	//���������ƫ����
	{0,0,0,0,0,0,0,0},	//������к�
	0.1,		//�岹����(��λ:mm)
	{175,230,165,16,0},	//��е�ֱ۳�֮��Ĳ��� һ��۳�������۳����ݸ˳��ȣ��ݾ�
	{ 100, 130,0.01, 360,0,0,0,0},//����λ(��λ:�Ƕ�)0.01?��0�Ļ�������ԭ����ֱ�Ӵ�����λ״̬
	{-100,-130,-360,-360,0,0,0,0},	//����λ(��λ�ǽǶ�)
	{0,0,0,0,0,0,0,0},				//ԭ��λ��
	{120,100,20.625,80,0,0,0,0}	//���ٱ� (165mm)/(16mm/r)*2(ͬ���ֱ���)=20.625
};

//H4450
const RobotParaStru ROBOT_PARA_H4450={
	"H4450",
	4,	//����Ŀ
	1,		//��¼�Ƿ񻵿�	0->����	1->�ÿ�
	0,	//��¼�����Ƿ���Ҫ������������ƫ���� 1->������Ҫ���� 0->����Ҫ
	1,	//��¼������� 1->����	2->��Э
	0,	//1->��һ��ʹ�� 0->��ʹ�ù�
	1,	//����
	1,	//����
	FOUR_AXIS_B,		//ϵͳ����
	8,		//��·��汾��
	0x0F,		//���ѡ��(bit0~bit7 �ֱ��Ӧ1~8��)
	5000,		//���ÿ����ת��
	0x08,//001000B,		//�����ת���� (bit0~bit7 �ֱ��Ӧ1~8��)
	0x09,//001001	//����������תʱ�ķ���
	20000,	//���ÿȦ��Ӧ��������
	{0,0,0,0,0,0,0,0},	//���������ƫ����
	{0,0,0,0,0,0,0,0},	//������к�
	0.1,		//�岹����(��λ:mm)
	{270,230,165,16,0},	//��е�ֱ۳�֮��Ĳ���
	{ 100, 130,0.01, 360,0,0,0,0},		//����λ(��λ�ǽǶ�)
	{-100,-130,-360,-360,0,0,0,0},	//����λ(��λ�ǽǶ�)
	{0,0,0,0,0,0,0,0},				//ԭ��λ��
	{120,100,20.625,80,0,0,0,0}	//���ٱ�
};
//H4560
const RobotParaStru ROBOT_PARA_H4560={
	"H4560",
	4,	//����Ŀ
	1,		//��¼�Ƿ񻵿�	0->����	1->�ÿ�
	0,	//��¼�����Ƿ���Ҫ������������ƫ���� 1->������Ҫ���� 0->����Ҫ
	1,	//��¼������� 1->����	2->��Э
	0,	//1->��һ��ʹ�� 0->��ʹ�ù�
	1,	//����
	1,	//����
	FOUR_AXIS_A,		//ϵͳ����
	8,		//��·��汾��
	0x0F,		//���ѡ��(bit0~bit7 �ֱ��Ӧ1~8��)
	5000,		//���ÿ����ת��
	0x08,//001000B,		//�����ת���� (bit0~bit7 �ֱ��Ӧ1~8��)
	0x09,//001001	//����������תʱ�ķ���
	20000,	//���ÿȦ��Ӧ��������
	{0,0,0,0,0,0,0,0},	//���������ƫ����
	{0,0,0,0,0,0,0,0},	//������к�
	0.1,		//�岹����(��λ:mm)
	{250,350,300,5,0},	//��е�ֱ۳�֮��Ĳ���
	{ 150, 140,0.01, 360,0,0,0,0},		//����λ(��λ�ǽǶ�)
	{-150,-140,-360,-360,0,0,0,0},	//����λ(��λ�ǽǶ�)
	{0,0,0,0,0,0,0,0},				//ԭ��λ��
	{160,120,75,80,0,0,0,0}	//���ٱ�
};
//V5350
const RobotParaStru ROBOT_PARA_V5350={
	"V5350",
	6,	//����Ŀ
	1,		//��¼�Ƿ񻵿�	0->����	1->�ÿ�
	0,	//��¼�����Ƿ���Ҫ������������ƫ���� 1->������Ҫ���� 0->����Ҫ
	1,	//��¼������� 1->����	2->��Э
	0,	//1->��һ��ʹ�� 0->��ʹ�ù�
	1,	//����
	1,	//����
	51,		//ϵͳ����
	8,		//��·��汾��
	0x37,		//���ѡ��(bit0~bit7 �ֱ��Ӧ1~8��)
	3500,		//���ÿ����ת��
	0x14,//010100B,		//�����ת���� (bit0~bit7 �ֱ��Ӧ1~8��)
	0x29,//101001	//����������תʱ�ķ���
	20000,	//���ÿȦ��Ӧ��������
	{0,0,0,0,0,0,0,0},	//���������ƫ����
	{0,0,0,0,0,0,0,0},	//������к�
	0.1,		//�岹����(��λ:mm)
	{253.0, 0.0, 0, 200.0, 0},	//��е�ֱ۳�֮��Ĳ���
	{165, 15,  5,   160, 90, 360, 0,0},		//����λ(��λ�ǽǶ�)
	{-165,-180,-190,-160,-90,-360,0,0},	//����λ(��λ�ǽǶ�)
	{0,-90.0,0,0,0,0,0,0},		//ԭ��λ��
	{160,240,137.5,80,80,80,0,0}	//���ٱ�
};
//V6350
const RobotParaStru ROBOT_PARA_V6350={
	"V6350",
	6,	//����Ŀ
	1,		//��¼�Ƿ񻵿�	0->����	1->�ÿ�
	0,	//��¼�����Ƿ���Ҫ������������ƫ���� 1->������Ҫ���� 0->����Ҫ
	1,	//��¼������� 1->����	2->��Э
	0,	//1->��һ��ʹ�� 0->��ʹ�ù�
	1,	//����
	1,	//����
	61,		//ϵͳ����
	8,		//��·��汾��
	0x3F,		//���ѡ��(bit0~bit7 �ֱ��Ӧ1~8��)
	3500,		//���ÿ����ת��
	0x14,//010100B,		//�����ת���� (bit0~bit7 �ֱ��Ӧ1~8��)
	0x29,//101001	//����������תʱ�ķ���
	20000,	//���ÿȦ��Ӧ��������
	{0,0,0,0,0,0,0,0},	//���������ƫ����
	{0,0,0,0,0,0,0,0},	//������к�
	0.1,		//�岹����(��λ:mm)
	{253.0,0.0,98,213.5,0},	//��е�ֱ۳�֮��Ĳ���
	{165,15,81,160,90,360,0,0},		//����λ(��λ�ǽǶ�)
	{-165,-180,-85,-160,-90,-360,0,0},	//����λ(��λ�ǽǶ�)
	{0,-90.0,0,0,0,0,0,0},				//ԭ��λ��
	{160,240,137.5,80,80,80,0,0}	//���ٱ�
};
//SV6350
const RobotParaStru ROBOT_PARA_SV6350={
	"SV6350",
	6,	//����Ŀ
	1,		//��¼�Ƿ񻵿�	0->����	1->�ÿ�
	0,	//��¼�����Ƿ���Ҫ������������ƫ���� 1->������Ҫ���� 0->����Ҫ
	2,	//��¼������� 1->����	2->��Э
	0,	//1->��һ��ʹ�� 0->��ʹ�ù�
	1,	//����
	1,	//����
	61,		//ϵͳ����
	8,		//��·��汾��
	0x3F,		//���ѡ��(bit0~bit7 �ֱ��Ӧ1~8��)
	4000,		//���ÿ����ת��
	0x14,//010100B,		//�����ת���� (bit0~bit7 �ֱ��Ӧ1~8��)
	0x29,//101001	//����������תʱ�ķ���
	20000,	//���ÿȦ��Ӧ��������
	{0,0,0,0,0,0,0,0},	//���������ƫ����
	{0,0,0,0,0,0,0,0},	//������к�
	0.1,		//�岹����(��λ:mm)
	{253.0,0.0,98,213.5,0},	//��е�ֱ۳�֮��Ĳ���
	{165,15,81,160,90,360,0,0},		//������λ(��λ�ǽǶ�)
	{-165,-180,-85,-160,-90,-360,0,0},	//����λ(��λ�ǽǶ�)
	{0,-90.0,0,0,0,0,0,0},				//ԭ��λ��
	{160,240,137.5,80,80,80,0,0}	//���ٱ�
};
//GV6250
const RobotParaStru ROBOT_PARA_GV6250={
	"GV6250",
	6,	//����Ŀ
	1,		//��¼�Ƿ񻵿�	0->����	1->�ÿ�
	0,	//��¼�����Ƿ���Ҫ������������ƫ���� 1->������Ҫ���� 0->����Ҫ
	2,	//��¼������� 1->����	2->��Э
	0,	//1->��һ��ʹ�� 0->��ʹ�ù�
	1,	//����
	1,	//����
	61,		//ϵͳ����
	8,		//��·��汾��
	0x3F,		//���ѡ��(bit0~bit7 �ֱ��Ӧ1~8��)
	6000,		//���ÿ����ת��
	0x14,//010100B,		//�����ת���� (bit0~bit7 �ֱ��Ӧ1~8��)
	0x29,//101001	//����������תʱ�ķ���
	20000,	//���ÿȦ��Ӧ��������
	{0,0,0,0,0,0,0,0},	//���������ƫ����
	{0,0,0,0,0,0,0,0},	//������к�
	0.1,		//�岹����(��λ:mm)
	{253.0,0.0,98,213.5,0},	//��е�ֱ۳�֮��Ĳ���
	{165,15,81,160,90,360,0,0},		//����λ(��λ�ǽǶ�)
	{-165,-180,-85,-160,-90,-360,0,0},	//����λ(��λ�ǽǶ�)
	{0,-90.0,0,0,0,0,0,0},				//ԭ��λ��
	{160,120,100,80,80,80,0,0}	//���ٱ�
};

void display_robot_para(RobotParaStru robot_para)
{
	char i,str[17];
	//��ʾ����
	ltoa(robot_para.HARDWARE_REV,str);
	SetTextValue(46,29,str);	//ϵͳ

	ltoa(robot_para.SYS,str);
	SetTextValue(46,1,str);		//ϵͳ

	ltoa(robot_para.MOTOR_TYPE,str);
	SetTextValue(46,2,str);		//�������

	ltoa(robot_para.MOTOR_PRECISION,str);
	SetTextValue(46,3,str);		//�������

	ltoa(robot_para.MOTOR_SPEED,str);
	SetTextValue(46,4,str);		//������ת��

	My_DoubleToString(robot_para.PRECISION,str,11,3,1);
	SetTextValue(46,5,str);		//�岹����

	ltoa(robot_para.AXIS_NUM,str);
	SetTextValue(47,80,str);	//����Ŀ

	SetTextValue(47,75,robot_para.NAME);

	SetTextValue(47,3,(char*)ROBOT_PARA_H4440.NAME);
	SetTextValue(47,4,(char*)ROBOT_PARA_H4450.NAME);
	SetTextValue(47,5,(char*)ROBOT_PARA_H4560.NAME);
	SetTextValue(47,6,(char*)ROBOT_PARA_V5350.NAME);
	SetTextValue(47,7,(char*)ROBOT_PARA_V6350.NAME);
	SetTextValue(47,8,(char*)ROBOT_PARA_SV6350.NAME);
	SetTextValue(47,9,(char*)ROBOT_PARA_GV6250.NAME);
	SetTextValue(47,10,(char*)ROBOT_PARA_GH4440.NAME);

	//��ѡ��
	for(i=0;i<8;i++)
	{
		str[8-i-1] = ((robot_para.MOTOR_SELECT>>i) & 0x0001) + '0';
	}
	str[i] = 0;
	SetTextValue(47,77,str);
	//��ת��
	for(i=0;i<8;i++)
	{
		str[8-i-1] = ((robot_para.AXIS_ROTATE.all>>i) & 0x0001) + '0';
	}
	str[i] = 0;
	SetTextValue(47,78,str);
	//���ת��
	for(i=0;i<8;i++)
	{
		str[8-i-1] = ((robot_para.MOTOR_DIR.all>>i) & 0x0001) + '0';
	}
	str[i] = 0;
	SetTextValue(47,79,str);

	//��е�ֲ���
	for(i=0;i<5;i++)
	{
		My_DoubleToString(robot_para.ROBOT_PARA[i],str,11,3,1);
		SetTextValue(46,6+i,str);
	}

	//���ٱ�
	for(i=0;i<6;i++)
	{
		sprintf(str,"%11.6f",robot_para.RATIO[i]);
		//My_DoubleToString(robot_para.RATIO[i],str,12,6,1);
		SetTextValue(46,11+i,str);
	}

	//����λ
	for(i=0;i<6;i++)
	{
		My_DoubleToString(robot_para.LIMITED_P[i],str,9,3,0);
		SetTextValue(46,21+i,str);
	}

	//����λ
	for(i=0;i<6;i++)
	{
		My_DoubleToString(robot_para.LIMITED_M[i],str,9,3,0);
		SetTextValue(46,31+i,str);
	}

	//ԭ��λ��
	for(i=0;i<6;i++)
	{
		My_DoubleToString(robot_para.ORIGIN_POSITION[i],str,9,3,0);
		SetTextValue(47,81+i,str);
	}
}
/***********************
����:��е�ֲ�������
����ֵ��NONE
*/
void setting_robot_para(void)
{
	char i,tmp,err,len,str[25];
	double f_value,f_tmp;
	RobotParaStru robot_para;
	RobotParaStru robot_para_tmp;

	G_BUTTON.SCREEN_ID = 46;
	SetScreen(46);	//��������ҳ��
	ReadRobotPara(&robot_para);	//����е�ֲ���
	if(robot_para.HARDWARE_REV == 0)
		robot_para.HARDWARE_REV = 8;	//�����֮ǰ�ϰ汾�ģ����ֵĬ����0�����ڸ�ΪĬ�϶���8

	if(robot_para.FIRST == 1)
	{
		robot_para_tmp = ROBOT_PARA_V6350;	//δ����ʱ������Ĭ�ϸ�ֵV6350�Ĳ���
	}
	else
	{
		robot_para_tmp = robot_para;
	}
	display_robot_para(robot_para_tmp);//��ʾ����
//д����
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			f_value = atof((char*)USART_RECEIVE_STRING);
			GetControlValue(G_BUTTON.SCREEN_ID,G_BUTTON.CONTROL_ID);
			USART_RX_STA = 0;
			i=0;
			//�������ζ�ȡ����������ͬΪֹ�����6��
			while(1)
			{
				if(USART_RX_STA&0x8000)
				{
					f_tmp = atof((char*)USART_RECEIVE_STRING);
					if(f_tmp != f_value)
					{
						f_value = f_tmp;
						USART_RX_STA = 0;
						GetControlValue(G_BUTTON.SCREEN_ID,G_BUTTON.CONTROL_ID);
						i++;
						if(i>6)
						{
							  SetScreen(20);
							  SetTextValue(20, 4, "");
							  SetTextValue(20, 3, "Failure,enter again!");
							  delay_1ms(1500);
							  SetScreen(G_BUTTON.SCREEN_ID);
							  break;
						}
						continue;
					}
					USART_RX_STA = 0;
					break;
				}
			}
			switch(G_BUTTON.CONTROL_ID)
			{
			case 29: robot_para_tmp.HARDWARE_REV = atol((char*)USART_RECEIVE_STRING);	break;
			case 1:	robot_para_tmp.SYS = atol((char*)USART_RECEIVE_STRING);	break;
			case 2: robot_para_tmp.MOTOR_TYPE = atol((char*)USART_RECEIVE_STRING);	break;
			case 3: robot_para_tmp.MOTOR_PRECISION = atol((char*)USART_RECEIVE_STRING);break;
			case 4: robot_para_tmp.MOTOR_SPEED = atol((char*)USART_RECEIVE_STRING);break;
			case 5: robot_para_tmp.PRECISION = f_value;break;
			case 80:robot_para_tmp.AXIS_NUM = atol((char*)USART_RECEIVE_STRING);break;
			//��е�ֲ���
			case 6: robot_para_tmp.ROBOT_PARA[0] = f_value;break;
			case 7: robot_para_tmp.ROBOT_PARA[1] = f_value;break;
			case 8: robot_para_tmp.ROBOT_PARA[2] = f_value;break;
			case 9: robot_para_tmp.ROBOT_PARA[3] = f_value;break;
			case 10: robot_para_tmp.ROBOT_PARA[4] = f_value;break;
			//���ٱ�
			case 11: robot_para_tmp.RATIO[0] = f_value;break;
			case 12: robot_para_tmp.RATIO[1] = f_value;break;
			case 13: robot_para_tmp.RATIO[2] = f_value;break;
			case 14: robot_para_tmp.RATIO[3] = f_value;break;
			case 15: robot_para_tmp.RATIO[4] = f_value;break;
			case 16: robot_para_tmp.RATIO[5] = f_value;break;
			case 17: robot_para_tmp.RATIO[6] = f_value;break;
			case 18: robot_para_tmp.RATIO[7] = f_value;break;
			//����λ
			case 21: robot_para_tmp.LIMITED_P[0] = f_value;break;
			case 22: robot_para_tmp.LIMITED_P[1] = f_value;break;
			case 23: robot_para_tmp.LIMITED_P[2] = f_value;break;
			case 24: robot_para_tmp.LIMITED_P[3] = f_value;break;
			case 25: robot_para_tmp.LIMITED_P[4] = f_value;break;
			case 26: robot_para_tmp.LIMITED_P[5] = f_value;break;
			case 27: robot_para_tmp.LIMITED_P[6] = f_value;break;
			case 28: robot_para_tmp.LIMITED_P[7] = f_value;break;
			//����λ
			case 31: robot_para_tmp.LIMITED_M[0] = f_value;break;
			case 32: robot_para_tmp.LIMITED_M[1] = f_value;break;
			case 33: robot_para_tmp.LIMITED_M[2] = f_value;break;
			case 34: robot_para_tmp.LIMITED_M[3] = f_value;break;
			case 35: robot_para_tmp.LIMITED_M[4] = f_value;break;
			case 36: robot_para_tmp.LIMITED_M[5] = f_value;break;
			case 37: robot_para_tmp.LIMITED_M[6] = f_value;break;
			case 38: robot_para_tmp.LIMITED_M[7] = f_value;break;

			case 75:
				for(i=0;i<10;i++)
				{
					robot_para_tmp.NAME[i] = USART_RECEIVE_STRING[i];
				}
				robot_para_tmp.NAME[10] = '\0';
			case 77://��ѡ��
				robot_para_tmp.MOTOR_SELECT = 0;
				len = strlen((char*)USART_RECEIVE_STRING);
				for(i=0;i<len;i++)
				{
					robot_para_tmp.MOTOR_SELECT |= (USART_RECEIVE_STRING[i]=='0'?0:1)<<(len-i-1);
				}
				for(i=0;i<8;i++)
				{
					str[8-i-1] = ((robot_para_tmp.MOTOR_SELECT>>i) & 0x0001) + '0';
				}
				str[i] = 0;
				SetTextValue(47,77,str);
				break;
			case 78://��ת��
				robot_para_tmp.AXIS_ROTATE.all = 0;
				len = strlen((char*)USART_RECEIVE_STRING);
				for(i=0;i<len;i++)
				{
					robot_para_tmp.AXIS_ROTATE.all |= (USART_RECEIVE_STRING[i]=='0'?0:1)<<(len-i-1);
				}
				for(i=0;i<8;i++)
				{
					str[8-i-1] = ((robot_para_tmp.AXIS_ROTATE.all>>i) & 0x0001) + '0';
				}
				str[i] = 0;
				SetTextValue(47,78,str);
				break;
			case 79://���ת��
				robot_para_tmp.MOTOR_DIR.all = 0;
				len = strlen((char*)USART_RECEIVE_STRING);
				for(i=0;i<len;i++)
				{
					robot_para_tmp.MOTOR_DIR.all |= (USART_RECEIVE_STRING[i]=='0'?0:1)<<(len-i-1);
				}
				for(i=0;i<8;i++)
				{
					str[8-i-1] = ((robot_para_tmp.MOTOR_DIR.all>>i) & 0x0001) + '0';
				}
				str[i] = 0;
				SetTextValue(47,79,str);
				break;
			//ԭ��λ��
			case 81: robot_para_tmp.ORIGIN_POSITION[0] = f_value;break;
			case 82: robot_para_tmp.ORIGIN_POSITION[1] = f_value;break;
			case 83: robot_para_tmp.ORIGIN_POSITION[2] = f_value;break;
			case 84: robot_para_tmp.ORIGIN_POSITION[3] = f_value;break;
			case 85: robot_para_tmp.ORIGIN_POSITION[4] = f_value;break;
			case 86: robot_para_tmp.ORIGIN_POSITION[5] = f_value;break;
			case 87: robot_para_tmp.ORIGIN_POSITION[6] = f_value;break;
			case 88: robot_para_tmp.ORIGIN_POSITION[7] = f_value;break;

			case 101:SetScreen(19);
					 strcpy(str,"Set to ");
					 strcat(str,ROBOT_PARA_H4440.NAME);
					 strcat(str,"?");
					 SetTextValue(19, 1, str);
					 USART_RX_STA = 0;
					 tmp=1;
					 while(tmp)
					 {
						 if(USART_RX_STA&0x8000)
						 {
							 switch(G_BUTTON.CONTROL_ID)
							 {
							 case 3:	//YES
								 robot_para_tmp = ROBOT_PARA_H4440;
								 display_robot_para(robot_para_tmp);
								 SetScreen(20);
								 SetTextValue(20, 4, "");
								 SetTextValue(20, 3, "Setting successful!");
								 delay_1ms(1300);
								 SetScreen(G_BUTTON.SCREEN_ID);
								 tmp=0;
								 break;
							 case 2:	//NO
							 case 4:	//CANCEL
								 tmp=0;
								 SetScreen(G_BUTTON.SCREEN_ID);
								 break;
							 }
						 }
					 }
					 break;
			case 102:SetScreen(19);
					 strcpy(str,"Set to ");
					 strcat(str,ROBOT_PARA_H4450.NAME);
					 strcat(str,"?");
					 SetTextValue(19, 1, str);
					 USART_RX_STA = 0;
					 tmp=1;
					 while(tmp)
					 {
						 if(USART_RX_STA&0x8000)
						 {
							 switch(G_BUTTON.CONTROL_ID)
							 {
							 case 3:	//YES
								 robot_para_tmp = ROBOT_PARA_H4450;
								 display_robot_para(robot_para_tmp);
								 SetScreen(20);
								 SetTextValue(20, 4, "");
								 SetTextValue(20, 3, "Setting successful!");
								 delay_1ms(1300);
								 SetScreen(G_BUTTON.SCREEN_ID);
								 tmp=0;
								 break;
							 case 2:	//NO
							 case 4:	//CANCEL
								 tmp=0;
								 SetScreen(G_BUTTON.SCREEN_ID);
								 break;
							 }
						 }
					 }
					 break;
			case 103:SetScreen(19);
					 strcpy(str,"Set to ");
					 strcat(str,ROBOT_PARA_H4560.NAME);
					 strcat(str,"?");
					 SetTextValue(19, 1, str);
					 USART_RX_STA = 0;
					 tmp=1;
					 while(tmp)
					 {
						 if(USART_RX_STA&0x8000)
						 {
							 switch(G_BUTTON.CONTROL_ID)
							 {
							 case 3:	//YES
								 robot_para_tmp = ROBOT_PARA_H4560;
								 display_robot_para(robot_para_tmp);
								 SetScreen(20);
								 SetTextValue(20, 4, "");
								 SetTextValue(20, 3, "Setting successful!");
								 delay_1ms(1300);
								 SetScreen(G_BUTTON.SCREEN_ID);
								 tmp=0;
								 break;
							 case 2:	//NO
							 case 4:	//CANCEL
								 tmp=0;
								 SetScreen(G_BUTTON.SCREEN_ID);
								 break;
							 }
						 }
					 }
					 break;
			case 104:SetScreen(19);
					 strcpy(str,"Set to ");
					 strcat(str,ROBOT_PARA_V5350.NAME);
					 strcat(str,"?");
					 SetTextValue(19, 1, str);
					 USART_RX_STA = 0;
					 tmp=1;
					 while(tmp)
					 {
						 if(USART_RX_STA&0x8000)
						 {
							 switch(G_BUTTON.CONTROL_ID)
							 {
							 case 3:	//YES
								 robot_para_tmp = ROBOT_PARA_V5350;
								 display_robot_para(robot_para_tmp);
								 SetScreen(20);
								 SetTextValue(20, 4, "");
								 SetTextValue(20, 3, "Setting successful!");
								 delay_1ms(1300);
								 SetScreen(G_BUTTON.SCREEN_ID);
								 tmp=0;
								 break;
							 case 2:	//NO
							 case 4:	//CANCEL
								 tmp=0;
								 SetScreen(G_BUTTON.SCREEN_ID);
								 break;
							 }
						 }
					 }
					 break;
			case 105:SetScreen(19);
					 strcpy(str,"Set to ");
					 strcat(str,ROBOT_PARA_V6350.NAME);
					 strcat(str,"?");
					 SetTextValue(19, 1, str);
					 USART_RX_STA = 0;
					 tmp=1;
					 while(tmp)
					 {
						 if(USART_RX_STA&0x8000)
						 {
							 switch(G_BUTTON.CONTROL_ID)
							 {
							 case 3:	//YES
								 robot_para_tmp = ROBOT_PARA_V6350;
								 display_robot_para(robot_para_tmp);
								 SetScreen(20);
								 SetTextValue(20, 4, "");
								 SetTextValue(20, 3, "Setting successful!");
								 delay_1ms(1300);
								 SetScreen(G_BUTTON.SCREEN_ID);
								 tmp=0;
								 break;
							 case 2:	//NO
							 case 4:	//CANCEL
								 tmp=0;
								 SetScreen(G_BUTTON.SCREEN_ID);
								 break;
							 }
						 }
					 }
					 break;
			case 106:SetScreen(19);
					 strcpy(str,"Set to ");
					 strcat(str,ROBOT_PARA_SV6350.NAME);
					 strcat(str,"?");
					 SetTextValue(19, 1, str);
					 USART_RX_STA = 0;
					 tmp=1;
					 while(tmp)
					 {
						 if(USART_RX_STA&0x8000)
						 {
							 switch(G_BUTTON.CONTROL_ID)
							 {
							 case 3:	//YES
								 robot_para_tmp = ROBOT_PARA_SV6350;
								 display_robot_para(robot_para_tmp);
								 SetScreen(20);
								 SetTextValue(20, 4, "");
								 SetTextValue(20, 3, "Setting successful!");
								 delay_1ms(1300);
								 SetScreen(G_BUTTON.SCREEN_ID);
								 tmp=0;
								 break;
							 case 2:	//NO
							 case 4:	//CANCEL
								 tmp=0;
								 SetScreen(G_BUTTON.SCREEN_ID);
								 break;
							 }
						 }
					 }
					 break;
			case 107:SetScreen(19);
					 strcpy(str,"Set to ");
					 strcat(str,ROBOT_PARA_GV6250.NAME);
					 strcat(str,"?");
					 SetTextValue(19, 1, str);
					 USART_RX_STA = 0;
					 tmp=1;
					 while(tmp)
					 {
						 if(USART_RX_STA&0x8000)
						 {
							 switch(G_BUTTON.CONTROL_ID)
							 {
							 case 3:	//YES
								 robot_para_tmp = ROBOT_PARA_GV6250;
								 display_robot_para(robot_para_tmp);
								 SetScreen(20);
								 SetTextValue(20, 4, "");
								 SetTextValue(20, 3, "Setting successful!");
								 delay_1ms(1300);
								 SetScreen(G_BUTTON.SCREEN_ID);
								 tmp=0;
								 break;
							 case 2:	//NO
							 case 4:	//CANCEL
								 tmp=0;
								 SetScreen(G_BUTTON.SCREEN_ID);
								 break;
							 }
						 }
					 }
					 break;
			case 108:SetScreen(19);
					 strcpy(str,"Set to ");
					 strcat(str,ROBOT_PARA_GH4440.NAME);
					 strcat(str,"?");
					 SetTextValue(19, 1, str);
					 USART_RX_STA = 0;
					 tmp=1;
					 while(tmp)
					 {
						 if(USART_RX_STA&0x8000)
						 {
							 switch(G_BUTTON.CONTROL_ID)
							 {
							 case 3:	//YES
								 robot_para_tmp = ROBOT_PARA_GH4440;
								 display_robot_para(robot_para_tmp);
								 SetScreen(20);
								 SetTextValue(20, 4, "");
								 SetTextValue(20, 3, "Setting successful!");
								 delay_1ms(1300);
								 SetScreen(G_BUTTON.SCREEN_ID);
								 tmp=0;
								 break;
							 case 2:	//NO
							 case 4:	//CANCEL
								 tmp=0;
								 SetScreen(G_BUTTON.SCREEN_ID);
								 break;
							 }
						 }
					 }
					 break;
			case 93: SetScreen(46);G_BUTTON.SCREEN_ID=46;break;	//��һҳ
			case 94: SetScreen(47);G_BUTTON.SCREEN_ID=47;break;	//��һҳ
			case 91:	//����
				err = SaveRobotPara(robot_para_tmp);
				SetScreen(20);
				SetTextValue(20, 4, "");
				if(err == 0)
				{
					robot_para = robot_para_tmp;
					SetTextValue(20, 3, "Saved successful,take effect after restart!");
				}
				else
				{
					SetTextValue(20, 3, "Failed to save!");
				}
				USART_RX_STA = 0;
				while(1)
				{
					if(USART_RX_STA&0x8000)
					{
						USART_RX_STA = 0;
						break;
					}
				}
				SetScreen(G_BUTTON.SCREEN_ID);
				break;
			case 92:	//�˳�
				//���������޸ģ�����δ����
				USART_RX_STA = 0;
				if(robot_para_compare(robot_para,robot_para_tmp))
				{
					SetScreen(19);
					SetTextValue(19, 1, "Parameters were changed,save now?");
					tmp=1;
					while(tmp)
					{
						if(USART_RX_STA&0x8000)
						{
							switch(G_BUTTON.CONTROL_ID)
							{
								case 2:	//NO
									USART_RX_STA = 0;
									G_BUTTON.CONTROL_ID = 0;
									SetScreen(44);	//������ģʽҳ��
									G_BUTTON.SCREEN_ID = 44;
									return;

								case 3:	//YES
									err = SaveRobotPara(robot_para_tmp);
									SetScreen(20);
									SetTextValue(20, 4, "");
									if(err == 0)
									{
										robot_para = robot_para_tmp;
										SetTextValue(20, 3, "Saved successful,take effect after restart!");
									}
									else
									{
										SetTextValue(20, 3, "Failed to save!");
									}
									USART_RX_STA = 0;
									while(1)
									{
										if(USART_RX_STA&0x8000)
										{
											USART_RX_STA = 0;
											break;
										}
									}
									USART_RX_STA = 0;
									G_BUTTON.CONTROL_ID = 0;
									SetScreen(44);	//������ģʽҳ��
									G_BUTTON.SCREEN_ID = 44;
									return;

								case 4:	//CANCEL
									tmp = 0;
									SetScreen(G_BUTTON.SCREEN_ID);
									break;
								default:break;
							}
							USART_RX_STA = 0;
							G_BUTTON.CONTROL_ID = 0;
						}
					}
				}
				else
				{
					USART_RX_STA = 0;
					G_BUTTON.CONTROL_ID = 0;
					SetScreen(44);	//������ģʽҳ��
					G_BUTTON.SCREEN_ID = 44;
					return;
				}

			default:	break;
			}
			USART_RX_STA = 0;
			G_BUTTON.CONTROL_ID = 0;
		}
	}

	//SetScreen(44);	//2018/05/22ע�͵�//������ģʽҳ��
	//G_BUTTON.SCREEN_ID = 44;//2018/05/22ע�͵�
}
/***********************
����:ѡ�񿪻�ʾ��ģʽ��ջ�����ģʽ
*/
void switch_mode(void)
{
    if(G_BUTTON.BUTTON_STATUS==1)
    {
    	Teach_mode=1;//������//2018/06/01;

    }
    else
    {
    	Teach_mode=0;//������//2018/06/01;

    }
}
/***********************
����:
*/
void teach_speed(void)
{
	if(!G_BUTTON.BUTTON_STATUS)//���𣬼�Ĭ��״̬
	{

	}

	if(G_BUTTON.BUTTON_STATUS)//����״̬
	{

	}
}
/************************************
* ��    �ƣ� screen8(7.2)	//P.λ��ҳ��
* ��    �ܣ� �ж�����һ����ť����,�л�����Ӧҳ�����
*/
static void screen8()
{
//	unsigned long key_value;
//	SetScreen(8);
//	G_BUTTON.SCREEN_ID = 8;

//	while(1)
//	{
//		if(USART_RX_STA&0x8000)
//		{
//			if(G_BUTTON.CONTROL_ID == 2)
//			{
//				if(DEVELOPER_MODE)
//				{
//					SetScreen(44);
//					G_BUTTON.SCREEN_ID = 44;
//				}
//				else
//				{
//					SetScreen(2);
//					G_BUTTON.SCREEN_ID = 2;
//				}
//
//				USART_RX_STA = 0;
//				break;
//			}
//			else
//			{
//				USART_RX_STA = 0;
//			}
//		}
//#if KEY_BOARD
//		key_value = ReadKeyboardValue();
//		if(key_value == ReadKeyboardValue())
//		{
//			KeyboardDataProcessing(key_value);
//		}
//#endif
//		ErrCheck(8);
//	}
	USART_RX_STA = 0;
}

/************************************
* ��    �ƣ� screen11(7.3)�ɵ���λ����ҳ��
* ��    �ܣ� �ж�����һ����ť����,�л�����Ӧҳ�����
*/
static void screen11()
{
//	unsigned long key_value;
//	SetScreen(11);
//	G_BUTTON.SCREEN_ID = 11;
//	while(1)
//	{
//		if(USART_RX_STA&0x8000)
//		{
//			if(G_BUTTON.CONTROL_ID == 15)
//			{
//				if(DEVELOPER_MODE)
//				{
//					SetScreen(44);
//					G_BUTTON.SCREEN_ID = 44;
//				}
//				else
//				{
//					SetScreen(2);
//					G_BUTTON.SCREEN_ID = 2;
//				}
//
//				USART_RX_STA = 0;
//				break;
//			}
////			if(G_BUTTON.CONTROL_ID == 41)
////			{
////				if(!G_BUTTON.BUTTON_STATUS)
////				{
////					SetTextValue(11,29,"�ѿ���λ");
////					EnableLimited();//����λ
////				}
////
////				if(G_BUTTON.BUTTON_STATUS)
////				{
////					SetTextValue(11,29,"�ѹ���λ");
////					DisableLimited();//�ر���λ
////				}
////			}
//
//			USART_RX_STA = 0;
//		}
//		#if KEY_BOARD
//			key_value = ReadKeyboardValue();
//			if(key_value == ReadKeyboardValue())
//			{
//				KeyboardDataProcessing(key_value);
//			}
//		#endif
//		ErrCheck(11);
//	}
	USART_RX_STA = 0;
}


/*************************************
* ��    �ƣ� screen13(7.4)��ʱ����
* ��    �ܣ� �ж�����һ����ť����,�л�����Ӧҳ�����
*/
static void screen13()
{
	unsigned long key_value;
	static u8 clocks[2] = {0, 0};
	static u8 minutes[2] = {0, 0};
	SetScreen(13);
	G_BUTTON.SCREEN_ID = 13;
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_ID == 1)//������һ��
			{
				if(DEVELOPER_MODE)
				{
					SetScreen(44);
					G_BUTTON.SCREEN_ID = 44;
				}
				else
				{
					SetScreen(2);
					G_BUTTON.SCREEN_ID = 2;
				}
				G_BUTTON.CONTROL_ID = 0;
				break;
			}

			switch(G_BUTTON.CONTROL_ID)
			{
				case 3: reset_text(clocks, minutes); break;
				case 4: start_text(clocks, minutes); break;
				case 12: clocks[0] = array_to_date(); break;
				case 13: minutes[0] = array_to_date(); break;
				case 14: clocks[1] = array_to_date(); break;
				case 15: minutes[1] = array_to_date(); break;
				default: break;
			}

			USART_RX_STA = 0;
		}
#if KEY_BOARD
		key_value = ReadKeyboardValue();
		if(key_value == ReadKeyboardValue())
		{
			KeyboardDataProcessing(key_value);
		}
#endif
		ErrCheck(13);
	}
}

/****************************************
 * ��������    array_to_date  (7.41)
 * ��������: ��ʱ��ת��������
 * �������: date,ʱ�ֺ�������(ASCII��)
 * �������: ת���õ���������
*/
static u16 array_to_date(void)
{
	u8 i = 0;
	u8 date_data = 0;
	char str[2] = {0, 0};

	for(i=0; i<3; i++)
	{
		if( (USART_RECEIVE_STRING[i]<'0') || (USART_RECEIVE_STRING[i]>'9') )
		{ break;}
	}

	if(i == 0)
	{	noparameter(13);}
	else
	{
		switch(i)
		{
			case 1: date_data = (USART_RECEIVE_STRING[0]-'0'); break;
			case 2: date_data = (USART_RECEIVE_STRING[0]-'0')*10 + (USART_RECEIVE_STRING[1]-'0'); break;
			default: date_data = 0;
		}

		if((date_data>23) && ((G_BUTTON.CONTROL_ID==12)||(G_BUTTON.CONTROL_ID==14)))
		{date_data = 23;}
		if((date_data>59) && ((G_BUTTON.CONTROL_ID==13)||(G_BUTTON.CONTROL_ID==15)))
		{date_data = 59;}

		i = date_data;
		ltoa(i, str);
		SetTextValue(13,G_BUTTON.CONTROL_ID, str);
		return i;
	}
	return 0;
}

/******************************************************
 * ��������    reset_text  (7.42)
 * ��������: ��տ��ػ�ʱ��
*/
static void reset_text(u8 clocks[2], u8 minutes[2])
{
	SetTextValue(13, 12, "00");
	SetTextValue(13, 13, "00");
	SetTextValue(13, 14, "00");
	SetTextValue(13, 15, "00");

	clocks[0] = 0;
	clocks[1] = 0;
	minutes[0] = 0;
	minutes[1] = 0;
}

/******************************************************
 * ��������    start_text  (7.43)
 * ��������: ��ʱִ��ѡ�еĳ���
 * �������: clocks��minutes����ʼ�ͽ�����ʱ��
*/
static void start_text(u8 clocks[2],u8 minutes[2])//��ʼִ������
{
	int i = 0;
	u8 return_value = 0;
	u8 program_name[USART_REC_LEN] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	USART_RX_STA = 0;

	SetTextValue(34, 2, "Setting ruturn point?");
	if(prompt_dialog_box(34,34) == 1)	{ programs_manage(13);}
	display_runing_test_screen(clocks, minutes);
	for(; i<20; i++)	{ program_name[i] = USART_RECEIVE_STRING[i];}

	while(1)
	{
		return_value = check_real_time(clocks, minutes);
		if(return_value>0)	{ break;}
		if(ErrCheck(13))
		{
			 OPEN_PROGRAM_SIGN = 0;
			 break;
		}
	}

	if(return_value == 1)//check_working_tine()��⵱ǰʱ�䣬�ж��Ƿ�ִ������
	{
		run_intime(clocks, minutes, program_name);//����ִ��  xxx
	}

    SetRTCMode(0x00, 0x01, 0x06,0xffff,559,101);
	SetScreen(13);
	delay_1ms(50);//��ʱ����
}

/******************************************************
 * ��������    start_text (7.431)
 * ��������: ��⵱ǰʱ��
 * �������: clocks��minutes����ʼ�ͽ�����ʱ��
     	   check_touch:�Ƿ�������ж�
 * �������: 0��ʾδ��ִ��ʱ�䣬1��ʾ����ִ��ʱ�䣬2��ʾ��Ϊ�˳��������������ʱ������
*/
static u8 check_real_time(u8 clocks[2], u8 minutes[2])
{
	u16 real_time = 0;//����ʱ��
	u16 time0 = clocks[0]*60 + minutes[0];//����ʱ��
	u16 time1 = clocks[1]*60 + minutes[1];//�ػ�ʱ��
	delay_1ms(10);

	if((time0<1440) && (time1<1440))//�������ƻ���ʱ����������Χ�ڵĻ�������ִ�У����򷵻��趨����ƻ�����
	{
		if(USART_RX_STA&0x8000)
		{
			if(prompt_dialog_box(27,28) == 1)//ѡ���Ƿ��˳���ǰ����
			{
				 OPEN_PROGRAM_SIGN = 0;
				 return 2;
			}
			USART_RX_STA = 0;
		}

		ReadTime();//���Ͷ�ȡʱ��ָ�� xxx
		while(1)
		{
			if(USART_RX_STA&0x8000)//�ȴ��������ϴ�ʱ��
			{
				real_time = USART_RECEIVE_STRING[0]*60 + USART_RECEIVE_STRING[1];//��ʱ��ת���ɷ��ӱ�ʾ
				USART_RX_STA = 0;
				break;
			}
		}
		//��ʼ�жϵ�ǰʱ���Ƿ����������ʱ��
		if(time1 > time0)//���ػ�ʱ��ȿ���ʱ����ʱ��
		{
			if( ((real_time==time0)||(real_time>time0)) && (real_time<time1) )
			{	return 1;}
		}
		if(time1 < time0)//���ػ�ʱ��ȿ���ʱ��С��ʱ��
		{
			if( ((real_time==time0) || (real_time>time0)) //���ڻ��ߵ��ڿ���ʱ�䲢��С�ڻ��ߵ���24����
			  && ((real_time==1440) || (real_time<1440)))
			{	return 1;}
			if( ((real_time==0)||(real_time>0)) && (real_time<time1) )//���ڻ��ߵ���0��������С�ڻ��ߵ��ڹػ�ʱ��
			{	return 1;}
		}
		if(time1 == time0)//���ػ�ʱ����ͬ����ִ������
		{
			 OPEN_PROGRAM_SIGN = 0;
			 return 2;
		}
		return 0;
	}

	OPEN_PROGRAM_SIGN = 0;
	return 2;//����ʱ������
}

/******************************************************
* ��������    display_runing_test_screen  (7.432)
* ��������: ��ʱִ��ѡ�еĳ���
* �������: clocks��minutes����ʼ�ͽ�����ʱ��
*/
static void display_runing_test_screen(u8 clocks[2],u8 minutes[2])//��ʼִ������
{
	u8 str[3] = {0, 0, 0};
	SetScreen(28);
	G_BUTTON.SCREEN_ID = 28;
    u32 t=0;
	if(OPEN_PROGRAM_SIGN)
	{
		GetControlValue(24, 2*OPEN_PROGRAM_SIGN);//���Ͷ�ȡ�ı��ؼ�����ָ��
		while(1)//�ȴ����մ������������ı��ؼ���Ϣ
		{
			if(USART_RX_STA&0x8000)//���յ��������ϴ���Ϣ���������
			{
				SetTextValue(28, 8, (char*)USART_RECEIVE_STRING);
				break;
			}
			delay_1us(1);
			if(t++ == 2000000) break;
		}
		USART_RX_STA=0;
	}
	else SetTextValue(28, 8, "");

	ltoa(clocks[0], (char*)str);
	SetTextValue(28, 4, (char*)str);
	ltoa(minutes[0], (char*)str);
	SetTextValue(28, 5, (char*)str);
	ltoa(clocks[1], (char*)str);
	SetTextValue(28, 6, (char*)str);
	ltoa(minutes[1], (char*)str);
	SetTextValue(28, 7, (char*)str);
	USART_RX_STA = 0;
	G_BUTTON.SCREEN_ID = 13;
}

/******************************************************
 * ��������    run_intime  (7.433)
 * ��������: ��ʱִ��ѡ�еĳ���
 * �������: clocks��minutes����ʼ�ͽ�����ʱ��
*/
static void run_intime(u8 clocks[2],u8 minutes[2],u8 program_name[USART_REC_LEN])
{
	 unsigned char num,return_value=0;
	 unsigned int i=0,n;
	 SetRTCMode(0x01, 0x01, 0x06,0xf800,559,101);

	 num = GetLinesCurrentProgram();	//��ó�������
	 G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 0;//��ʹ�ð�ȫ����
	 for(; i<num; )
	 {
	     if(ProgramStepRun(&i,0))	//ִ�е�i�г���
	     {
	    	 break;
	     }
	     if( i >= (num-1) )
	     { i=0;}		//����ͷ�����¿�ʼ

	     return_value = check_real_time(clocks, minutes);//�ж�����ƻ��Ƿ����
	     if( return_value != 1 )
	     {
	    	 if(OPEN_PROGRAM_SIGN)
			 {
	    		 return_value = OpenProgram(program_name,&n);//��Ҫ��ȡ�ĳ�������ַ��͸��ײ�����,�ѳ�������ڴ档
	    		 if(return_value == 0)
	    		 {
					 num = GetLinesCurrentProgram();	//��ó�������
					 for(i=0; i<num; )
					 {
						 if(ProgramStepRun(&i,0))//ִ�е�i�г���
						 {
							 break;
						 }
					 }
	    		 }
			 }
	    	 break;
	     }
	 }
	 G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//ʹ�ð�ȫ����
	 OPEN_PROGRAM_SIGN = 0;
}


//��8�£�����༭���Թ�����
/********************************************************************************
 * 8
 * 8.1
 * 8.2
 * 8.3
 * 8.4
 * 8.5
 * 8.6
 * 8.7
*********************************************************************************/

/********************************************
* ��    �ƣ� screen4
* ��    �ܣ� ���˵�4���ж�����һ����ť����,�л�����Ӧҳ�����
*/
static void screen4()
{
	unsigned long key_value;
	SetScreen(4);//�жϴ�����ҳ�������ҳ���Ƿ�һ��
	G_BUTTON.SCREEN_ID = 4;

	while(4)
	{
		if(USART_RX_STA&0x8000)
		{
			switch(G_BUTTON.CONTROL_ID)//�ж�����һ����ť����,�л�����Ӧҳ��
			{
				case 1: SCREEN_SIGN=1; break;
				case 2: SCREEN_SIGN=2; break;
				case 3: SCREEN_SIGN=3; break;
				case 4: SCREEN_SIGN=5; break;
				case 5: SCREEN_SIGN=7; break;
			   default:		    break;
			}
			USART_RX_STA = 0;
			break;
		}
#if KEY_BOARD
		key_value = ReadKeyboardValue();
		if(key_value == ReadKeyboardValue())
		{
			if(1 == switch_teach_model(key_value, 10)){return;}
			KeyboardDataProcessing(key_value);
		}
#endif
		ErrCheck(4);
	}
}

/**********************************************
* ��    �ƣ� screen7-(8)
* ��    �ܣ� �༭�������
*/
static u8  page_num=1;
static void screen7()
{
	unsigned long key_value;
	u8 sign_edit = 0;
	u8 open_program_sign = 0;
	char str[4] = {0};
	static unsigned long s_key_value=0;
	static u8 text_num=1, text_num0=1;//��ǰ������ڵ��ı�����뵱ǰ��ʾ��ҳ��
	static u16 net_comcounter=0,program_select=0;//������//2018/06/01(��ʱ�ϱ���λ��״̬�Լ�������λ����������)
    static u8 warn_data=0,program_check=0,*check_value=0;

	text_num0 = text_num;//�༭��text_num�������µĹ��λ�������ı��ؼ���ţ�text_num0�򱣴�ǰ�����
	SetScreen(7);//�жϴ�����ҳ�������ҳ���Ƿ�һ��
	G_BUTTON.SCREEN_ID = 7;
	open_program(&page_num, &text_num, &text_num0, &sign_edit, &open_program_sign);
	ltoa(page_num, str);
	SetTextValue(7, 72, str);
	SetCursor(7, text_num+2, 1);
	ltoa(sign_edit, str);
	SetTextValue(7, 71, str);
	TEACH_MODEL = GetModeState();
	SetModeState(TEACH_MODEL);
	delay_1ms(110);
	SetModeState(TEACH_MODEL);
	net_comcounter=0;//������//2018/06/01(��ʱ�ϱ���λ��״̬�Լ�������λ����������)
	SetScreen(7);
	SetTextValue(7,96,"     OK!");
	while(1)//ѭ��ɨ�裬����ҳ��ͬ��
	{
		if(USART_RX_STA&0x8000)//�ж��Ƿ��м����£�����ҳ��ͬ��
		{
			G_BUTTON.SCREEN_ID = 7;

			if(switch_screen(text_num, &sign_edit))	{ break;}//�л�ҳ�溯(7.1)

			update_program(&text_num0,&text_num, &page_num, &sign_edit);//������µĳ����ı��ؼ�����(7.2)

			return_keyboard_value(&text_num,&text_num0, &page_num, &sign_edit);//�����ϴ��ļ���ֵ(7.3)

			touch_edit(&text_num0,&text_num,&sign_edit);//������������Чʱ�л����(7.4)

			dispose_button(&text_num0,&text_num,&sign_edit,&page_num, &open_program_sign);//����ť�ϴ�ֵ(7.6)

			jump_to_error_line(&text_num);

			G_BUTTON.CONTROL_TYPE=0;
			G_BUTTON.CONTROL_ID=0;
			USART_RX_STA = 0;
		}
#if KEY_BOARD
		key_value = ReadKeyboardValue();
		if(key_value == ReadKeyboardValue())//KeyBoard_Shift() 0x00000001
		{
			if(1 == switch_teach_model(key_value, 10))
			{
				TEACH_MODEL++;
				if(TEACH_MODEL>=2){TEACH_MODEL=0;}
				SetModeState(TEACH_MODEL);
				delay_1ms(100);
				SetModeState(TEACH_MODEL);
				delay_1ms(100);
			}
			KeyboardDataProcessing(key_value);
			if( (key_value&0x00000F00)&&(G_BUTTON.SCREEN_ID==7))
			{
				save_last_edit_program(text_num, page_num, &sign_edit);
				display_ten_page(key_value);
				delay_1ms(300);
			}
			if(key_value == 0x00008000)
			{
				SetScreen(12);
				delay_1ms(400);
				screen12(7);
			}
			if(key_value == 0x00200000)
			{
				step_by_step(&text_num0, &text_num, &page_num);
				if(key_value != s_key_value)
				{
					delay_1ms(380);
				}
			}
			s_key_value = key_value;
		}
#endif
		ErrCheck(7);
		net_comcounter=net_comcounter+1;//������//2018/06/01

		if(net_comcounter==200)//������//2018/06/01
		{
			 SetScreen(7);
			 SetTextValue(7,96,"    NET!");
			 delay_1ms(100);//��Ӵ���ʱ�����ַ����ı仯������ֻ�ܿ���һ���ַ�
			warn_data=Cycle_Query_Uppermachine(&program_select,&program_check);//������//2018/06/01
			if(warn_data==2)
			{
				G_BUTTON.CONTROL_TYPE = 11;
				G_BUTTON.CONTROL_ID = 0x63;
				net_control=0x88;
				dispose_button(&text_num0,&text_num,&sign_edit,&page_num, &open_program_sign);//����ť�ϴ�ֵ(7.6)
				net_control=0x00;
			}
			else if(warn_data==3)
			{
				return_home_network();

			}
			else if(warn_data==4)
			{
				*check_value=program_check;
				programs_net_manage(7,program_select,check_value);
			}
			net_comcounter=0;//������//2018/06/01
			 SetScreen(7);
			 SetTextValue(7,96,"     OK!");

		}

	}
	save_last_edit_program(text_num, page_num, &sign_edit);
}

/*****************************************
 * �������ܣ���/�·�ʮҳ
 * ���������sign: 0:��ʾ�Ϸ�10ҳ��1:��ʾ�·�10ҳ
 * 			 2:��ʾ�Ϸ�1ҳ��3:��ʾ�·�1ҳ
 * ���������NONE
 */
static void display_ten_page(unsigned long sign)
{
	char str[4]={0};
	if(sign==0x00000400)//�Ҽ�
	{
		page_num += 10;
		if(page_num>50)
		{
			page_num = 50;
		}
	}

	else if(sign==0x00000200)//���
	{
		if(page_num>10)
			page_num -= 10;
		else
			page_num = 1;
	}

	else if(sign==0x00000100)//�¼�
	{
		page_num += 1;
		if(page_num>50)
		{
			page_num = 50;
		}
	}

	else if(sign==0x00000800)//�ϼ�
	{
		if(page_num>1)
			page_num -= 1;
		else
			page_num = 1;
	}

	refresh(page_num,7);
	ltoa(page_num, str);
	SetTextValue(7, 72, str);
}

/**************************************************************
����:��ת������������ڴ�-(8.1)
*/
static void jump_to_error_line(u8 *text_num)
{
	char str[3]={0};
	if(DISPLAY_ERROR_LINE)
	{
		page_num =(ERROR_LINE[DISPLAY_ERROR_LINE-1]-1)/15+1;
		*text_num = ERROR_LINE[DISPLAY_ERROR_LINE-1]-15*(page_num-1);
		if(*text_num > 0)
		{
			refresh(page_num,7);
			ltoa(*text_num,str);
			SetTextValue(7, 71, str);
			ltoa(page_num,str);
			SetTextValue(7, 72, str);
			DISPLAY_ERROR_LINE = 0;
			SetCursor(7,*text_num+2,1);
		}
	}
}


/**************************************************************
����:�л�����-(8.1)
*/
static u8 switch_screen(u8 text_num,u8 *sign_edit)
{
	if( G_BUTTON.CONTROL_TYPE == 11 )//���µ�Ϊ�л�ҳ��İ�ť
	{
		switch(G_BUTTON.CONTROL_ID)
		{
			case 1 : SCREEN_SIGN=1; break;//���½���Ϊһ
			case 2 : SCREEN_SIGN=10;
					  if(1 == TEACH_MODEL)		{SCREEN_SIGN = 14; }
					  break;//ͬ��
			case 75: save_last_edit_program(text_num, page_num, sign_edit);
					  programs_manage(7);
					  SCREEN_SIGN=7;
					  break;
			default: SCREEN_SIGN=0;	break;
		}
		if( SCREEN_SIGN > 0 )
		{
			G_BUTTON.CONTROL_ID = 0;
			return 1;
		}
	}
	return 0;
}

/**************************************************************
����:������µĳ����ı��ؼ�����-(8.2)
����:
***text_num0��������ڵ��ı��ؼ���ǰһ�����
***sign_edit���ı��ؼ��Ƿ��б༭�ı�־λ��1Ϊ�б༭����0Ϊû�б༭��
***page_num ����¼��ǰ��ҳ����
*/
static void update_program(u8 *text_num0, u8 *text_num, u8 *page_num, u8 *sign_edit)
{
	if(G_BUTTON.CONTROL_TYPE == 15)//���յ���Ϊ�ı��ؼ�����ʱ
	{
		if( (G_BUTTON.screencoordinate_x>705) && (G_BUTTON.screencoordinate_x<787)//������enter���󷵻��ı��ؼ�������
		 	 && (G_BUTTON.screencoordinate_y>484) && (G_BUTTON.screencoordinate_y<525) )
		{
			GetCodeStrings( ((*text_num)+((*page_num)-1)*15-1), USART_RECEIVE_STRING );//д���ڴ��б���
			G_BUTTON.screencoordinate_x = 0;
			G_BUTTON.screencoordinate_y = 0;
			press_enter(text_num, page_num, text_num0);
		}
		else
		{
			 GetCodeStrings( ((*text_num0)+((*page_num)-1)*15-1), USART_RECEIVE_STRING );
		}

		(*sign_edit) = 0;
		G_BUTTON.CONTROL_TYPE = 0;
		USART_RX_STA = 0;
	}
}

/**************************************************************
����:�����ϴ��ļ���ֵ-(8.3)
����:
***text_num ��������ڵ��ı��ؼ������±��
***sign_edit���ı��ؼ��Ƿ��б༭�ı�־λ��1Ϊ�б༭����0Ϊû�б༭��
***page_num ����¼��ǰ��ҳ����
*/
static void return_keyboard_value(u8 *text_num, u8 *text_num0, u8 *page_num, u8 *sign_edit)
{
	if( (G_BUTTON.screencoordinate_x>250) && (G_BUTTON.screencoordinate_x<790)//������ڱ༭����
		 && (G_BUTTON.screencoordinate_y>250) && (G_BUTTON.screencoordinate_y<525) )
	{

		if((G_BUTTON.screencoordinate_x>705) && (G_BUTTON.screencoordinate_x<787)
			&&(G_BUTTON.screencoordinate_y>484) && (G_BUTTON.screencoordinate_y<525))
		{

		}
		else
		{
			if((G_BUTTON.screencoordinate_x>248) && (G_BUTTON.screencoordinate_x<326)
				&&(G_BUTTON.screencoordinate_y>434) && (G_BUTTON.screencoordinate_y<531))
			{

			}
			else
			{
				*sign_edit=1;//��־λ��Ϊ1��˵�����򱻱༭
				if(MEMORY_TO_FLASH_SIGN == 1)
				{	MEMORY_TO_FLASH_SIGN = 2;}
			}
			G_BUTTON.screencoordinate_x = 0;
			G_BUTTON.screencoordinate_y = 0;
		}
	}
	if(G_BUTTON.CONTROL_TYPE == 12)//���µ�Ϊ������ʽ(�����̷�ʽ)
	{
		switch(G_BUTTON.KEY_VALUE)
		{
			case 0x0d: press_enter(text_num, page_num, text_num0);
					   *sign_edit=0;
					   break;//���°���Ϊ�س���()

			case 0x1b: delete_line(text_num, page_num);
					   *sign_edit=0;
					   break;

			default  : *sign_edit=1;
						if(MEMORY_TO_FLASH_SIGN == 1)
						{	MEMORY_TO_FLASH_SIGN = 2;}
					   break;
		}
		G_BUTTON.KEY_VALUE = 0;
		G_BUTTON.CONTROL_TYPE = 0;
	}
}

/**************************************************************
����:�༭����ʱ����enter���Ĵ���(����һ��)-(8.31)
����:
***text_num ��������ڵ��ı��ؼ������±��
***page_num ����¼��ǰ��ҳ����
*/
static void press_enter(u8 *text_num, u8 * page_num, u8 *text_num0)//����enter���Ĵ���
{
	char str[5] = {0};
	u8 sign_page_change = 0;
	unsigned int row;
	row = ( (*text_num) + 15*((*page_num) - 1 ));//1~750
	if(row < ROWS_IN_PROGRAM)
	{
		if((*text_num) == 15)//��Ҫ��ҳ
		{
			(*page_num) += 1;
			(*text_num) = 1;
			(*text_num0) = (*text_num);
			IncreaseEmptyRow( row );//row:0~749
			SetCursor(7, (*text_num)+2, 1);
			ltoa(*page_num, str);
			SetTextValue(7, 72, str);
			sign_page_change = 1;
		}

		if( ((*text_num)<15) && (sign_page_change==0) )//����Ҫ��ҳ
		{
			(*text_num0) = (*text_num);
			IncreaseEmptyRow( row);//���»س�������һ������һ�пհ���
			(*text_num) += 1;
			SetCursor(7, (*text_num)+2, 1);
		}

		sign_page_change = 0;
		refresh(*page_num, 7);	//����ˢ��
		ltoa(*text_num, str);
		SetTextValue(7, 71, str);
	}
}

/**************************************************************
����:�༭����ʱ����ɾ�����Ĵ���ɾ��һ�У�-(8.32)
����:
***text_num ��������ڵ��ı��ؼ������±��
***page_num ����¼��ǰ��ҳ����
*/
static void delete_line(u8 *text_num, u8 *page_num)//ɾ��һ�����ݣ�����Ĳ���ȥ
{
	if( (*text_num) > 0 )
	{
		DeleteRow( (*text_num) + 15*((*page_num)-1) - 1 );//ɾ��һ�е�����
		refresh(*page_num, 7);//����ˢ��
		(*text_num) += 2;
		SetCursor(7, (*text_num), 1);
		(*text_num) -= 2;
	}
}

/**************************************************************
����:�����ϴ��ļ���ֵ-(8.4)
����:
****text_num0��
****text_num:
***sign_edit��
*/
static u8 touch_edit(u8 *text_num0, u8 *text_num, u8 *sign_edit)
{
	u8 str[3] = {0};
	if( (G_BUTTON.screencoordinate_x>0) && (G_BUTTON.screencoordinate_x<240) )//���λ��Ϊ�ı��ؼ�༭��
	{
		*sign_edit = 0;
		*text_num0 = *text_num;
		*text_num = (G_BUTTON.screencoordinate_y-64)/31 + 1;

		ltoa((*text_num), (char*)str);
		SetTextValue(7, 71, (char*)str);

		G_BUTTON.screencoordinate_x = 0;
		USART_RX_STA = 0;
		return 1;
	}
	return 0;
}

/**************************************************************
����:�����ϴ��ļ���ֵ-(8.5)
����:
***text_num0��������ڵ��ı��ؼ���ǰһ�����
***text_num ��������ڵ��ı��ؼ������±��
***sign_edit���ı��ؼ��Ƿ��б༭�ı�־λ��1Ϊ�б༭����0Ϊû�б༭��
***page_num ����¼��ǰ��ҳ����
*/
static void dispose_button(u8 *text_num0, u8 *text_num, u8 *sign_edit, u8 *page_num, u8 *open_program_sign)
{
	u8 control_id = 0;

	if( ((*sign_edit)==1) || (*open_program_sign==1) )	//��������б��༭������ô�������ɹ��ı�־λ��Ϊ1��Ҳ����˵�´����г����ǰҪ�ȱ������
	{
		build_program_sign = 0;
		*open_program_sign = 0;
	}

	if(G_BUTTON.CONTROL_TYPE == 11)//����Ϊ����ģʽ
	{
		control_id = G_BUTTON.CONTROL_ID;
		if(57 == control_id)
		{
			delete_line(text_num, page_num);//ɾ��һ�У�����Ĳ���
			sign_edit = 0;
		}
		save_last_edit_program(*text_num, *page_num, sign_edit);

		switch(control_id)
		{
			case 65:
					 TEACH_MODEL=!TEACH_MODEL;
					 SetModeState(TEACH_MODEL);
					 delay_1ms(200);
					 SetModeState(TEACH_MODEL);
					 break;
			case 73: screen7_1(page_num); break;//�Ϸ�ҳ
			case 74: screen7_2(page_num); break;//�·�ҳ
//					 refresh(1,7);
			case 76: new_program(text_num0, text_num, page_num);
					 break;
			case 77: build_program_sign = build_program(); break;
			case 78: save_program(); break;
			case 79: display_first_page(); break;
			case 80: break;
			case 81: break;
			case 97: step_by_step(text_num0, text_num, page_num); break;
			case 99:
				SetButtonValue(6,2,0);//С����ӣ�������ԣ�
				screen6(text_num, text_num0, page_num,sign_edit, build_program_sign);
				delay_1ms(50);
				TEACH_MODEL = GetModeState();
				if(TEACH_MODEL >= 2)
				{
					TEACH_MODEL = 0;
					SetModeState(TEACH_MODEL);
				}
				delay_1ms(100);
				SetModeState(TEACH_MODEL);
				delay_1ms(100);
				SetModeState(TEACH_MODEL);
				break;
			case 101: save_position(*text_num, *page_num); break;//����λ����Ϣ7.65
			default : break;
		}

		SetCursor(7, (*text_num)+2, 1);
		G_BUTTON.CONTROL_ID = 0;
		G_BUTTON.CONTROL_TYPE = 0;
	}
}

/**************************************************************
����:���ص���һҳ
*/
static void display_first_page(void)
{
	 refresh(1, 7);
	 page_num = 1;
}
/**************************************************************
����:�Ƿ񱣴����ı༭����-(8.51)
*/
static void save_last_edit_program(u8 text_num,u8 page_num,u8 *sign_edit)
{
	 USART_RX_STA = 0;

	 if( (*sign_edit) == 1 )
	 {
		 while(prompt_dialog_box(32, 7))
		 {
			 GetCodeStrings( (text_num+(page_num-1)*15-1), USART_RECEIVE_STRING );//�����༭�ĳ�������д���ڴ���
			 break;
		 }

		 (*sign_edit) = 0;
		 USART_RECEIVE_STRING[0] = 0;
		 refresh(page_num, 7);

		 G_BUTTON.CONTROL_ID = 0;
		 delay_1ms(10);
		 USART_RX_STA = 0;
	 }
}

/**************************************************************
����:ʵ�ַ�ҳ��ʾ����,�Ϸ�ҳ-(8.52)
����:
***pages ����¼��ǰ��ҳ����
*/
static void screen7_1(u8 * pages)
{
	u8 str[4] = {0,0,0,0};
	if( *pages > 1 )
	{
		(*pages)--;
		ltoa((*pages), (char*)str);
		SetTextValue(7, 72, (char*)str);

		refresh(*pages, 7);//����ˢ��
	}
}

/**************************************************************
����:ʵ�ַ�ҳ��ʾ����,�·�ҳ-(8.53)
����:
***pages ����¼��ǰ��ҳ����
*/
static void screen7_2(u8 * pages)
{
	u8 str[4] = {0,0,0,0};
	if( *pages < (int)(ROWS_IN_PROGRAM/15) )//���ROWS_IN_PROGRAM����15�ı�����������
	{
		(*pages)++;
		ltoa((*pages), (char*)str);
		SetTextValue(7, 72, (char*)str);

		refresh(*pages, 7);//����ˢ��
	}
}

/**************************************************************
����:�½�һ��Ӧ�ó�-(8.54)
����:
***text_num0��������ڵ��ı��ؼ���ǰһ�����
***text_num ��������ڵ��ı��ؼ������±��
***sign_edit���ı��ؼ��Ƿ��б༭�ı�־λ��1Ϊ�б༭����0Ϊû�б༭��
***page_num ����¼��ǰ��ҳ����
*/
static void new_program(u8 *text_num0,u8 *text_num,u8 *page_num)//�½�����
{
	char str[5] = {0};
	unsigned char tmp,err,i;
	u8 program_number=0;//������
	u8 tempory_value_for_error=ERROR_LINE[5];

	tmp = ErrCheck(2);//����֮ǰ������û�б���
	G_BUTTON.CONTROL_TYPE = 0;//�ṹ������
	G_BUTTON.BUTTON_STATUS = 0;
	G_BUTTON.KEY_VALUE = 0;
	G_BUTTON.CONTROL_ID = 0;
	USART_RX_STA = 0;		//��־λ����
	SetScreen(31);
	G_BUTTON.SCREEN_ID = 31;
	ERROR_LINE[5]=0;//���������Ϣ����
	SetTextValue(31, 2, "");

	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_ID == 3)
			{
				if( (USART_RECEIVE_STRING[0]>0) )
				{
					err = NewProgram(USART_RECEIVE_STRING,0);
					if(err == 0)
					{
						SetScreen(7);
						G_BUTTON.SCREEN_ID = 7;
						program_number = GetProgramNum();
						ltoa(program_number, str);
						s_program_name_num.num = program_number;
						for(i=0;i<PROGRAM_NAME_LENGTH;i++)
						{
							s_program_name_num.program_name[i] = USART_RECEIVE_STRING[i];
						}
						SetTextValue(7, 82, str);
						SetTextValue(7, 83, (char*)USART_RECEIVE_STRING);//��ճ���������ʾ�ı�
						refresh(1, 7);
						SetCursor(7, 3, 1);
						*text_num0 = 1;
						*text_num = 1;
						*page_num = 1;
	//					DISPLAY_ERROR_LINE=0;
					}
					else
					{
						DispalyProgEditInfo(err);
					}
					break;
				}
			}
			if( G_BUTTON.CONTROL_ID == 4 )
			{
				ERROR_LINE[5]=tempory_value_for_error;//���������Ϣ��ԭ
				SetScreen(7);
				SetCursor(7, (*text_num)+2, 1);
				break;
			}
			USART_RX_STA = 0;
		}

		if( ErrCheck(7)&(~tmp) )	{ break;}
	}
	G_BUTTON.SCREEN_ID = 7;
}
/**************************************************************
����:�������-(8.55)
����:
*/
static u8 build_program(void)
{
	u8 i=0, j;
	u8 return_value,lines;
	u8 err[5] = {0};
	u16 error_line[5] = {0};
	char error_numbers[] = {'T','h','e','r','e',' ','a','r','e',' ','0','0',' ','e','r','r','o','r','s',0};
	char error_information[error_information_length];
	//memset(error_information,0,sizeof(error_information));

	lines = GetLinesCurrentProgram();//��õ�ǰ�����������
	SetScreen(37);//��ʼ��ʾ���붯��
	return_value = BuildProgram(5, err, error_line);
	delay_1ms((lines+40));//��ʾ������ʱ��������lines������
	delay_1ms(40);
	G_BUTTON.SCREEN_ID = 7;
	SetScreen(7);//������ʾ���붯��
	delay_1ms(10);
	SetScreen(7);
	delay_1ms(5);
	for(i=0; i<6; i++)
	{
		SetTextValue(7, 85+2*i, " ");//�����
		ERROR_LINE[i] = 0;//��ձ��������Ϣ
	}
	if(return_value == 0)//����ɹ�
	{
		TIMER0_CNT = 0;	//ÿ��Build������֮������ʱ�����¿�ʼ����
		SetTextValue(7, 85, "Build program successfully!");
		return 1;
	}
	else//����ʧ��
	{
		 if(return_value == 0xff)
		 {
			  SetTextValue(7, 85, "Please open or new!");
			  return 0;
		 }

		 ERROR_LINE[5] = 1;
		 error_numbers[10] = return_value/10 + '0';
		 error_numbers[11] = return_value%10 + '0';
		 SetTextValue(7, 85, error_numbers);//��ʾ�������

		 if(return_value > 5){	return_value = 5;}//�����ʾ��������Ϣ
		 for(i=0; i<return_value; i++)//��ʾ����ʧ����Ϣ
		 {
			j=0;
			ERROR_LINE[i] = error_line[i]+1;
			ltoa(error_line[i]+1, error_information);
			while(error_information[j] > 0)	{ j++;}
			error_information[j] = ':';
			error_number_to_informations(err[i], ( &error_information[j+1] ) );
			SetTextValue(7, 87+2*i, error_information) ;
		 }
	}
	return 0;
}

/**************************************************************
����:���ݱ��������ת������صı���ʧ�ܵ���Ϣ-(8.551)
����:
*/
static void error_number_to_informations(u8 error_number, char *build_informatiom)
{
	u8 i=0;

	//if((error_number>=0x31) && (error_number<=0x55))
	//{
		do
		{
			*build_informatiom = error_information[error_number-49][i];
			build_informatiom++;
		}
		while(error_information[error_number-49][i++] > 0);
	//}
}

/**************************************************************
����:����Ӧ�ó���-(8.56)
����:
***text_num0��������ڵ��ı��ؼ���ǰһ�����
***text_num ��������ڵ��ı��ؼ������±��
***sign_edit���ı��ؼ��Ƿ��б༭�ı�־λ��1Ϊ�б༭����0Ϊû�б༭��
***page_num ����¼��ǰ��ҳ����
*/
static void save_program()//�������
{
	 u8 return_value = 0;
	 USART_RX_STA = 0;

	 if( prompt_dialog_box(27,7) )
	 {
		   return_value = SaveProgram((unsigned char *)s_program_name_num.program_name);
		   if(return_value == 0)
		   {
				 SetTextValue(7, 85, "Saved program successfully!") ;
				 MEMORY_TO_FLASH_SIGN = 1;
		   }
		   else
		   {
				 SetTextValue(7, 85, "Failed to save program!") ;
		   }
			SetTextValue(7, 87, "");
			SetTextValue(7, 89, "");
			SetTextValue(7, 91, "");
			SetTextValue(7, 93, "");
			SetTextValue(7, 95, "");
		   USART_RX_STA = 0;
	 }
}

/**************************************************************
����:��������-(8.57)
����:
***text_num0��������ڵ��ı��ؼ���ǰһ�����
***text_num ��������ڵ��ı��ؼ������±��
***sign_edit���ı��ؼ��Ƿ��б༭�ı�־λ��1Ϊ�б༭����0Ϊû�б༭��
***page_num ����¼��ǰ��ҳ����
*/
static void step_by_step(u8 *text_num0,u8 *text_num,u8 *page_num)
{
	 u8 lines=0,i,page_tmp,prg_err=0;
	 u16 row;
	 Uint16 err;
	 char str[5] = {0};
	 USART_RX_STA = 0;
	 SetFcolor(0xffff);
	 GUI_RectangleFill(0, 63, 15, 539);

	 err = ReadErrInfo();// & (~0x40);//����ǰ����/������Ϣ,���ε���ȫ����
	 lines = GetLinesCurrentProgram();//��ȡ��ǰ����������
	 if(err)
	 {
		  i=0;
		  if(err&0x02)	{SetTextValue(7,85+i*2,(char*)SERVO_ALARM);i++;}
		  if(err&0x01)	{SetTextValue(7,85+i*2,(char*)SERVO_OFF);i++;}
		  if(err&0x04)	{SetTextValue(7,85+i*2,(char*)LOST_ORIGIN);i++;}
		  if(err&0x380) {SetTextValue(7,85+i*2,(char*)BRAKES_CLOSING); i++;}
		  if(err&0x30)	{SetTextValue(7,85+i*2,(char*)AXIS_LIMITED);i++;}
		  if(err&0x08)	{SetTextValue(7,85+i*2,(char*)EMERGENCY_STOP);i++;}
		  if(err&0x40)	{SetTextValue(7,85+i*2,(char*)KEEP_SAVE_BTN);i++;}
		  for(; i<6; i++)
		  {
			  SetTextValue(7, 85+i*2, "");
		  }
		  return;
	 }

	 if( ((*text_num) + ((*page_num)-1)*15 - 1) > lines )
	 {

		 SetTextValue(7,85, (char*)NO_PROGRAM);
		 SetTextValue(7, 87, "");
		 SetTextValue(7, 89, "");
		 SetTextValue(7, 91, "");
		 SetTextValue(7, 93, "");
		 SetTextValue(7, 95, "");
		 return;
	 }

	 SetFcolor(0x0400);
	 GUI_RectangleFill(0, 37+(*text_num)*31, 15, 60+(*text_num)*31);

	 row = (*text_num)+((*page_num)-1)*15-1;//row��0��ʼ��
	 page_tmp = *page_num;//��¼��ǰҳ��

	 ClearStopProgramFlag();//�����ͣ���б�־λ
	 RecoverSpeed();//�ָ���ʾ���ٶ�
	 prg_err = ProgramStepRun(&row,1);

		  if(prg_err < ERR_CMD)//�ж�ִ���Ƿ�ɹ�
		  {
			   *text_num = row%15+1;//text_num��1�п�ʼ
			   *page_num = row/15+1;//page_num��1ҳ��ʼ
			   if(page_tmp != *page_num) //�ж�ǰ���ҳ���Ƿ�һ�£���һ����ˢ��
			   {
				   refresh(*page_num, 7);
			   }
			   SetFcolor(0xffff);
			   GUI_RectangleFill(0, 63, 15, 539);
			   SetFcolor(63488);
			   GUI_RectangleFill(0, 37+(*text_num)*31, 15, 60+(*text_num)*31);
			   ltoa(*page_num, str);
			   SetTextValue(7, 72, str);
			   ltoa(*text_num, str);
			   SetTextValue(7, 71, str);

			   if((prg_err==code_mov) || (prg_err==code_mvs) ||
				   (prg_err==code_mvh)|| (prg_err==code_mvr) ||
				   (prg_err==code_mvc))
			   {
				   display_sixangle(7);
			   }
			   else
			   {
				   	 SetTextValue(7, 85, (char*)RUN_SUCCEED);//ˢ��
				   	 if(prg_err == code_n_x || prg_err == code_disp ||
				   			 prg_err == code_f_x )
				   	 {
				   		//�����ִ����n��������ˢ�£���Ϊn��������ʾռ����
				   	 }
				   	 else
				   	 {
				   		 SetTextValue(7, 87, "");
				   	 }
					 SetTextValue(7, 89, "");
					 SetTextValue(7, 91, "");
					 SetTextValue(7, 93, "");
					 SetTextValue(7, 95, "");
			   }
			   (*text_num0)=(*text_num);
		  }
		  else
		  {
			   SetTextValue(7, 85, (char*)error_information[prg_err-ERR_CMD]);
			   SetTextValue(7, 87, "");
			   SetTextValue(7, 89, "");
			   SetTextValue(7, 91, "");
			   SetTextValue(7, 93, "");
			   SetTextValue(7, 95, "");
			   SetFcolor(63488);
			   GUI_RectangleFill(0, 37+(*text_num)*31, 15, 60+(*text_num)*31);
		  }

	 /*if(row == lines)
	 {
			 SetTextValue(7, 85, (char*)RUN_SUCCEED);
			 SetTextValue(7,87, (char*)PROG_IS_OVER);
			 SetTextValue(7, 89, "");
			 SetTextValue(7, 91, "");
			 SetTextValue(7, 93, "");
			 SetTextValue(7, 95, "");
	 }*/
	 USART_RX_STA = 0;
}

/********************************************************************************
����:ȫ�����к���-(8.58)
����:
***text_num0��������ڵ��ı��ؼ���ǰһ�����
***text_num ��������ڵ��ı��ؼ������±��
***sign_edit���ı��ؼ��Ƿ��б༭�ı�־λ��1Ϊ�б༭����0Ϊû�б༭��
***page_num ����¼��ǰ��ҳ����
*/
static void screen6(u8 *text_num,u8 *text_num0,u8 *page_num,const u8 *sign_edit, u8 build_program_sign)
{
	u8 err,i,ovrd;
	char str[15];
	char str_tmp[6];
	long hour,min;
	unsigned int prog_row = 0;
	u8  net_mode=0;
	USART_RX_STA = 0;
	G_BUTTON.CONTROL_TYPE=0;
	if(build_program_sign)
	{

		if((net_control==0x88)||(prompt_dialog_box(27,7)))
        {
        	net_mode=0x66;
        }
		if(net_mode==0x66)
		{

			err = ReadErrInfo() & (~0x40);//����ǰ����/������Ϣ�����ε���ȫ����
		    //G_BUTTON.SCREEN_ID=6;//ҳ������Ϊ6
		    if(err)
		    {
				   i=0;
				   if(err&0x02)	{SetTextValue(7,85+i*2,(char*)SERVO_ALARM);i++;}
				   if(err&0x01)	{SetTextValue(7,85+i*2,(char*)SERVO_OFF);i++;}
				   if(err&0x04)	{SetTextValue(7,85+i*2,(char*)LOST_ORIGIN);i++;}
				   if(err&0x380) { SetTextValue(7,85+i*2,(char*)BRAKES_CLOSING); i++;}
				   if(err&0x30)	{SetTextValue(7,85+i*2,(char*)AXIS_LIMITED);i++;}
				   if(err&0x08)	{SetTextValue(7,85+i*2,(char*)EMERGENCY_STOP);i++;}
				   for(;i<5;i++)
				   {
						SetTextValue(7,85+i*2,"");
				   }
		    }
		    else
		    {
				 SetScreen(6);//��ת����������
				 G_BUTTON.SCREEN_ID = 6;
				 SetButtonValue(6, 18, 1);
				 refresh((*page_num), 6);//ˢ����ʾ����

				 ovrd = GetOvrd();
				 ltoa(ovrd,str);
				 SetTextValue(6, 19, str);//������ʾovrd

				 SetTextValue(6, 83, s_program_name_num.program_name) ;//��ʾ������

				  prog_row = (*text_num)+((*page_num)-1)*15-1;//�������е���
				 //ִ������ĺ�����prog_row��ֵ�����޸ģ�������Ϊ��һ�����е���
				 err = ProgramRunWithPointer(display_current_runing_program_position, &prog_row);

				 ErrCheck(7);
				 delay_1ms(850);//��ʱ���ȴ�������ʾ��

					TEACH_MODEL = GetModeState();
					if(TEACH_MODEL >= 2)
					{
						TEACH_MODEL = 0;
						SetModeState(TEACH_MODEL);
					}
					delay_1ms(50);
					SetModeState(TEACH_MODEL);
					delay_1ms(50);
					SetModeState(TEACH_MODEL);

				 if(err>=ERR_CMD)
				 {
					 	SetTextValue(7, 85, (char*)error_information[err-ERR_CMD]);
						min = TIMER0_CNT/60;
						hour = min/60;
						min = min%60;
						strcpy(str,"Time: ");
						ltoa(hour,str_tmp);
						strcat(str,str_tmp);
						ltoa(min,str_tmp);
						strcat(str,":");
						strcat(str,str_tmp);
						SetTextValue(7,87,str);
				 }
				 else
				 {
						min = TIMER0_CNT/60;
						hour = min/60;
						min = min%60;
						strcpy(str,"Time: ");
						ltoa(hour,str_tmp);
						strcat(str,str_tmp);
						ltoa(min,str_tmp);
						strcat(str,":");
						strcat(str,str_tmp);
						SetTextValue(7,85,str);
						SetTextValue(7, 87, "");
				 }
				 SetTextValue(7, 89, "");
				 SetTextValue(7, 91, "");
				 SetTextValue(7, 93, "");
				 SetTextValue(7, 95, "");

				 SetScreen(7);//�˳�ȫ��ִ�У����ص��߽���
				 delay_1ms(450);
				 SetScreen(7);//�˳�ȫ��ִ�У����ص��߽���
				 G_BUTTON.SCREEN_ID = 7;//ҳ������Ϊ7
				 (*text_num) = (prog_row+1)%15;//���������ı��ؼ��ı��
				 (*page_num) = (prog_row)/15+1;//��������ҳ��ı��
				 (*text_num0) = (*text_num);
				 delay_1ms(20);
				 ltoa(s_program_name_num.num,str);
				 SetTextValue(7, 82, str);
				 SetTextValue(7, 83, s_program_name_num.program_name) ;//��ʾ������
				 refresh((*page_num), 7);//ˢ����ʾ����
		    }
		}
	}
	else
	{
		SetTextValue(7, 85, "Program must be build before run!") ;
	    for(i=0; i<5; i++)
	    {
			 SetTextValue(7, 87+i*2, "");
	    }
	}
	net_mode=0x00;//���������־������
	G_BUTTON.CONTROL_ID = 0;
	USART_RX_STA = 0;
}

/**************************************************************
 * �������ܣ���������ָʾ-(8.581)
 * ���������text_num0��������ڵ��ı��ؼ���ǰһ�����
 * 		   text_num ��������ڵ��ı��ؼ������±��
 */
static void display_current_runing_program_position(u8 text_num)
{
	char str[3] = {0};
	static u8 s_page = 0;
	u8 page_tmp = 0;

	page_tmp = (text_num)/15+1;//��ǰҳ��
	if(page_tmp != s_page)//��ǰҳ����ϴε�ҳ�벻һ����ˢ��
	{
		s_page = page_tmp;
		refresh(s_page, 6);
		ltoa(s_page, str);
		SetTextValue(6, 72, (char*)str);
	}

	text_num = (text_num)%15;
	ltoa((text_num+1), str);
	SetTextValue(6, 71, str);
	SetFcolor(0xffff);
	GUI_RectangleFill(0, 63, 15, 539);
	//��ɫ
	SetFcolor(0xece0);
	GUI_RectangleFill(0, 70+(text_num)*31, 15, 90+(text_num)*31);
}

/**************************************************************
 * �������ܣ���ʾ������ĽǶ�-(8.582)
 * ���������screen_id,���ĸ�ҳ����ʾ��6��7
 */

void display_sixangle(u8 screen_id)
{
	u8 i=0;
	char string[58],angle_str[10];//xxx.xxx mm\t X: xxxx.xxx\t mm
	long hour;
	long min;
	double angle[6],radian[6],r;
	PositionGesture p_g;

	GetCurrentRadian(radian);//���ÿ�ᵱǰ����
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
		  *(angle+i) = *(radian+i)*PI_MULTI_180_INVERSE;//����ת�Ƕ�
	 }
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
		if(ROBOT_PARAMETER.AXIS_ROTATE.all & (0x0001<<i))
		{
			angle[i] = -angle[i];
		}
	 }
//	if(ROBOT_PARAMETER.SYS == FOUR_AXIS_C)
//	{
//		angle[2] = -angle[2];
//	}
	if(screen_id == 7)
	{

		p_g = GetPositionGesture(radian);
		//�������룬����3λС��
//		if(p_g.px<0.0)
//		{
//			p_g.px = -p_g.px;
//			p_g.px += 0.0005;
//			p_g.px = -p_g.px;
//		}
//		else
//		{
//			p_g.px += 0.0005;
//		}
//		if(p_g.py<0.0)
//		{
//			p_g.py = -p_g.py;
//			p_g.py += 0.0005;
//			p_g.py = -p_g.py;
//		}
//		else
//		{
//			p_g.py += 0.0005;
//		}
//		if(p_g.pz<0.0)
//		{
//			p_g.pz = -p_g.pz;
//			p_g.pz += 0.0005;
//			p_g.pz = -p_g.pz;
//		}
//		else
//		{
//			p_g.pz += 0.0005;
//		}
		//screen7(����༭����)
		My_DoubleToString(angle[0],string,9,3,0);//������ת��Ϊ�ַ�����С�����3λ
		My_DoubleToString(p_g.px,angle_str,10,3,0);
		strcat(string,"  X:");//���Ƶ�string
		strcat(string,angle_str);//���Ƶ�string
		SetTextValue(screen_id, 85, string);

		My_DoubleToString(angle[1],string,9,3,0);//������ת��Ϊ�ַ�����С�����3λ
		My_DoubleToString(p_g.py,angle_str,10,3,0);
		strcat(string,"  Y:");//���Ƶ�string
		strcat(string,angle_str);//���Ƶ�string
		SetTextValue(screen_id, 87, string);

		My_DoubleToString(angle[2],string,9,3,0);//������ת��Ϊ�ַ�����С�����3λ
		My_DoubleToString(p_g.pz,angle_str,10,3,0);
		strcat(string,"  Z:");//���Ƶ�string
		strcat(string,angle_str);//���Ƶ�string
		SetTextValue(screen_id, 89, string);

		My_DoubleToString(angle[3],string,9,3,0);//������ת��Ϊ�ַ�����С�����3λ
		if(ROBOT_PARAMETER.AXIS_NUM == 4)
		{
			r = (radian[0]+radian[1]+radian[3])*PI_MULTI_180_INVERSE;
			My_DoubleToString(r,angle_str,10,3,0);
			strcat(string,"  R:");//���Ƶ�string
			strcat(string,angle_str);//���Ƶ�string
			SetTextValue(screen_id, 93, "");
			SetTextValue(screen_id, 95, "");
		}
		SetTextValue(screen_id, 91, string);

		if(ROBOT_PARAMETER.AXIS_NUM==6 || ROBOT_PARAMETER.AXIS_NUM==5)
		{
			My_DoubleToString(angle[4],string,9,3,0);//������ת��Ϊ�ַ�����С�����3λ
			SetTextValue(screen_id, 93, string);
		}

		if(ROBOT_PARAMETER.AXIS_NUM==6 || ROBOT_PARAMETER.AXIS_NUM==5)
		{
			My_DoubleToString(angle[5],string,9,3,0);//������ת��Ϊ�ַ�����С�����3λ
			SetTextValue(screen_id, 95, string);
		}
	}
	else
	{
		//screen6(ȫ�����н���)
		if(ROBOT_PARAMETER.AXIS_NUM == 4)
		{
			for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
			{
				My_DoubleToString(angle[i],string,9,3,0);//������ת��Ϊ�ַ�����С�����3λ
				SetTextValue(screen_id, i*2+85, string);
			}
		}
		else
		{
			for(i=0;i<6;i++)
			{
				My_DoubleToString(angle[i],string,9,3,0);//������ת��Ϊ�ַ�����С�����3λ
				SetTextValue(screen_id, i*2+85, string);
			}
		}

		min = TIMER0_CNT/60;
		hour = min/60;
		min = min%60;
		strcpy(string,"Time: ");
		ltoa(hour,angle_str);
		strcat(string,angle_str);
		ltoa(min,angle_str);
		strcat(string,":");
		strcat(string,angle_str);
		SetTextValue(6,28,string);
	}
}

/**************************************************************
����:����λ����Ϣ-(8.59)
����:
***text_num0��������ڵ��ı��ؼ���ǰһ�����
***text_num ��������ڵ��ı��ؼ������±��
***sign_edit���ı��ؼ��Ƿ��б༭�ı�־λ��1Ϊ�б༭����0Ϊû�б༭��
***page_num ����¼��ǰ��ҳ����
*/
static void save_position(u8 text_num, u8 page_num)
{
	 u8 return_value = 0;
	 char str[4]={0};
	 u16 position_num = 0;
	 u32 t=0;
	 USART_RX_STA = 0;

	 return_value = GetPositionNumInRow((text_num+(page_num-1)*15-1),&position_num);//�жϱ�����һ��

	 ltoa(position_num, str);
	 SetTextValue(45, 6, str);
	 if(0 == return_value)//��λ��û��������Ϣ
	 {
		 SetTextValue(45, 2, "Sure to save this position?");
	 }
	 if(0x91 == return_value)//��λ������������Ϣ����ʾ�Ƿ񸲸Ǳ���
	 {
		 SetTextValue(45, 2, "Sure to overwrite this position?") ;
		 return_value = 0;
	 }
	 if(return_value > 0)
	 {
		   if(return_value>=ERR_CMD)
		   {
			   SetTextValue(7, 85, (char*)error_information[return_value-ERR_CMD]);
		   }
		   else
		   {
			   SetTextValue(7, 85, "Must be mov,mvs,mvh command!");
		   }
		   SetTextValue(7, 87, "");
		   SetTextValue(7, 89, "");
		   SetTextValue(7, 91, "");
		   SetTextValue(7, 93, "");
		   SetTextValue(7, 95, "");
		   return ;
	 }


	 if(prompt_dialog_box(45,7))
	 {
		 GetControlValue(7, text_num+2);
		 while(1)
		{
			  if(USART_RX_STA&0x8000)
			  {
				  GetCodeStrings( (text_num+(page_num-1)*15-1), USART_RECEIVE_STRING );//�����һ�α༭����Ϣ���浽�ڴ���
				  return_value = CapturePositionDataInRow((text_num+(page_num-1)*15-1));
				  if(return_value == 0)
				  {
					 SetTextValue(7, 85, "save position successfully!") ;

				  }
				  else
				  {
					   if(return_value>=ERR_CMD)
					   {
						   SetTextValue(7, 85, (char*)error_information[return_value-ERR_CMD]);
					   }
					   else
					   {
						   SetTextValue(7, 85, "Must be mov,mvs,mvh command!");
					   }
				  }
				  SetTextValue(7, 87, "");
				  SetTextValue(7, 89, "");
				  SetTextValue(7, 91, "");
				  SetTextValue(7, 93, "");
				  SetTextValue(7, 95, "");
				  USART_RX_STA = 0;
				  break;
			  }
			 delay_1us(1);
			 if(t++ == 2000000) break;
		 }
		 USART_RX_STA=0;
	 }
}

/**************************************************************
����:��ʾ��-(8.6)
*/
static u8 prompt_dialog_box(u8 screen_id1, u8 screen_id2)
{
	u8 tmp;
	tmp = ErrCheck(2);//����֮ǰ������û�б���
	USART_RX_STA = 0;
	G_BUTTON.CONTROL_ID = 0;
	if(screen_id1 > 0)
	{
		SetScreen(screen_id1);
		G_BUTTON.SCREEN_ID = screen_id1;
	}
	delay_1ms(20);
	USART_RX_STA = 0;

	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_TYPE == 15)//�л�����ʱ������������û�������ı��ؼ����༭���Ļ����Զ��ϴ��ı�ֵ
			{
				G_BUTTON.CONTROL_TYPE = 20;
				G_BUTTON.CONTROL_ID = 0;
				USART_RX_STA = 0;
			}
			if(G_BUTTON.CONTROL_ID > 0)//���°�ť��ѡ���Ƿ�ִ�в���
			{
				USART_RX_STA = 0;
				break;
			}
			USART_RX_STA = 0;
		}
		if(ErrCheck(7)&(~tmp))	{ break;}
	}

	USART_RX_STA = 0;
	SetScreen(screen_id2);
	G_BUTTON.SCREEN_ID = screen_id2;
	if(G_BUTTON.CONTROL_ID == 3)
	{
		G_BUTTON.CONTROL_ID = 0;
		return 1;
	}
	G_BUTTON.CONTROL_ID = 0;
	return 0;
}

/**************************************************************
����:ˢ�³�����ʾ����-(8.7)
����:
***page_num :��¼��ǰ��ҳ����
***screen_id:��Ҫˢ�����ݵ�ҳ��
*/
static void refresh(u8 page_num, u8 screen_id)//��ȡ�������ݲ�ˢ����ʾҳ��
{
	u8 i = 3;
	u16 line_num = 0;

	line_num = 15*(page_num-1) + line_num;
	delay_1ms(2);

	for(; i<18; i++)
	{
 		GetRowCode(line_num++, USART_RECEIVE_STRING);//��ȡһ�е�����
 		SetTextValue(screen_id, i, (char*)USART_RECEIVE_STRING);//������ʾ����ʾ
 		//delay_1ms(1);
	}
}


//��9�£�����༭���Թ�����
/********************************************************************************
 * 9
 * 9.1
 * 9.2
 * 9.3
 * 9.4
 * 9.5
*********************************************************************************/
/*******************************************
* ��    �ƣ� program_manage-(9)
* ��    �ܣ� ���������棨�򿪣�ɾ�������ƣ�ճ���ȣ�
* ��    ����last_screen,��ʾ�����ҳ��ǰ��ҳ��ı��
*/
static void programs_manage(u8 last_screen)
{
	unsigned long key_value;
	static u8 text_num = 100;//��ǰ������ڵ��ı���ţ���ʼֵΪ100���������������ᵽ���ֵ
	u8 sign_program_num = 0;


	SetScreen(24);//�жϴ�����ҳ�������ҳ���Ƿ�һ��
	G_BUTTON.SCREEN_ID = 24;
	refresh_display_program_name(0);

	while(1)//ѭ��ɨ�裬����ҳ��ͬ��
	{
		if(USART_RX_STA&0x8000)//�ж��Ƿ��м����£�����ҳ��ͬ��
		{

			if(exit_programs_manage_screen(text_num, last_screen,sign_program_num))
			{	break;}

			display_objective_program(&text_num, &sign_program_num);

			copy_delete_rename_page(&text_num, &sign_program_num);

			if(28 == G_BUTTON.CONTROL_ID)
			{
				export_program_to_udisk(text_num , &sign_program_num);
			}

			if(29 == G_BUTTON.CONTROL_ID)
			{
				display_udisk_program();
			}

			USART_RX_STA = 0;
			G_BUTTON.CONTROL_ID = 0;
			G_BUTTON.CONTROL_TYPE = 0;
		}
#if KEY_BOARD
		key_value = ReadKeyboardValue();
		if(key_value == ReadKeyboardValue())
		{
			KeyboardDataProcessing(key_value);
		}
#endif
		ErrCheck(24);
	}
}
/*******************************************
* ��    �ƣ� programs_net_manage-(9)
* ��    �ܣ� ����ͨ�ſ���ѡ���Լ��������
* ��    ����last_screen,��ʾ�����ҳ��ǰ��ҳ��ı��
* ��    ����text_num����ʾ��ѡ��ĳ���
*/
void programs_net_manage(u8 last_screen,u8 text_num,u8* check_value)//ͨ��������г������
{
	u8 sign_program_num=1,page_num=1,text_line=1;
	u8 text_line0=1,sign_edit=0,open_program_sign=0;
	u8 program_select=0,program_page=0,i=0;
	SetScreen(24);//�жϴ�����ҳ�������ҳ���Ƿ�һ��
	G_BUTTON.SCREEN_ID = 24;
	G_BUTTON.screencoordinate_x=1;
	program_page=text_num/10;
	refresh_display_program_name(0);
    if(program_page!=0)
    {
    	for(i=0;i<program_page;i++)
    	{
    		refresh_display_program_name(1);
    	}
    }
    program_select=text_num%10;
	G_BUTTON.screencoordinate_y=program_select*30+89;//�������ѡ������λ�ò��ú�ɫ������
	display_objective_program(&program_select, &sign_program_num);
	G_BUTTON.CONTROL_TYPE = 11;
	G_BUTTON.CONTROL_ID = 0x15;
	exit_programs_manage_screen(program_select,last_screen,sign_program_num);
	open_program(&page_num, &text_line, &text_line0, &sign_edit, &open_program_sign);
	Software_name_check=Software_name_check&0x00ff;
	if((Software_name_check-*check_value)==0)
	{
		build_program();
		build_program_sign=1;
	}
	Software_name_check=0;
}


void DisplayUdiskError(unsigned char err)
{
	switch (err)
	{
	case 1:
			SetTextValue(34,2,(char*)FLASH_ERROR);
			break;
	case 2:
			SetTextValue(34,2,(char*)SAVE_FAILED);
			break;
	case 3:
			SetTextValue(34,2,(char*)NAME_REPETITION);
			break;
	case 4:
			SetTextValue(34,2,(char*)READ_EX_ERR);
			break;
	case 5:
			SetTextValue(34,2,(char*)INTERNAL_COMM_ERR);
			break;
	case 6:
			SetTextValue(34,2,(char*)NAME_EMPTY);
			break;
	case 7:
			SetTextValue(34,2,(char*)NAME_TOO_LONG);
			break;
	case 0xff:
			SetTextValue(34,2,(char*)STORAGE_FULL);
			break;
	default:
			SetTextValue(34,2,(char*)_ERROR);
			break;
	}
}
/******************************************************
 * �������ܣ���������U��
 * ���������
*/
static void export_program_to_udisk(u8 text_num , u8 *sign_program_num)
{
	u8 sign_return=0,err;
	u32 t=0;
	if(1 == *sign_program_num)
	{
		err = save_program_to_flash();//�Ƿ���Ҫ�������
		if(err == 1 || err == 2)//����ʧ��
		{
			return;
		}
		USART_RX_STA = 0;
		*sign_program_num = 0;
		G_BUTTON.CONTROL_ID = 0;

		GetControlValue(24, 2*(text_num+1));//���Ͷ�ȡ�ı��ؼ�����ָ��
		while(1)//�ȴ����մ������������ı��ؼ���Ϣ
		{
			if(USART_RX_STA&0x8000)//���յ��������ϴ���Ϣ���������
			{
				SetScreen(37);//������
				sign_return = ExportProgram((char*)USART_RECEIVE_STRING,(char*)USART_RECEIVE_STRING,0);
				export_judge(sign_return);
				G_BUTTON.SCREEN_ID = 24;
				break;
			}
			delay_1us(1);
			if(t++ == 2000000) break;
		}
	}
	SetScreen(24);
	G_BUTTON.SCREEN_ID = 24;
	G_BUTTON.CONTROL_ID = 0;
	USART_RX_STA = 0;
}

/******************************************************
 * �������ܣ��жϵ����Ƿ�ɹ�
*/
static void export_judge(u8 sign_return)
{
	u8 i=0;
	char USART_RECEIVE_STRING0[USART_REC_LEN] = {0};
	G_BUTTON.SCREEN_ID = 34;

//**************��1����*****************//���򵼳��ɹ�

	if(sign_return == 0)
	{
		SetTextValue(34,2,(char*)EXPORT_SUCCESSFUL);
		SetScreen(34);
		delay_1ms(1000);
		return;
	}

//**************��2����*****************//����ͬ������Ĵ���

	if(sign_return == 3)
	{
		USART_RX_STA = 0;
		for(i=0;i<USART_REC_LEN;i++)//2.1���浱ǰ��������
		{
			USART_RECEIVE_STRING0[i] = USART_RECEIVE_STRING[i];
		}

		//2.2ѡ�񸲸Ǳ������������
		G_BUTTON.SCREEN_ID = 42;
		SetTextValue(42,5,"Name repeat!YES overwrite,NO rename!");
		SetScreen(42);//����ѡ�񸲸ǻ����������Ĵ���
		while(1)//�ȴ��û�ѡ�񸲸Ǳ������������
		{
			if(USART_RX_STA&0x8000)
			{
				if(3==G_BUTTON.CONTROL_ID)//���Ǳ���
				{
					SetScreen(37);//������
					sign_return = ExportProgram(USART_RECEIVE_STRING0,(char*)USART_RECEIVE_STRING,1);
					if(sign_return == 0)
					{
						SetTextValue(34,2,(char*)EXPORT_SUCCESSFUL);
					}
					else
					{
						DisplayUdiskError(sign_return);
					}
					SetScreen(34);
					delay_1ms(1000);
					return ;
				}
				else if(4==G_BUTTON.CONTROL_ID)//������
				{
					USART_RX_STA = 0;
					break;
				}
				else if(2 == G_BUTTON.CONTROL_ID)//cancel
				{
					USART_RX_STA = 0;
					return;
				}
				USART_RX_STA = 0;
			}
			ErrCheck(42);
		}

		//2.3����������ʵ�ֲ���
		SetTextValue(35,5,(char*)USART_RECEIVE_STRING);
		SetScreen(35);
		G_BUTTON.SCREEN_ID = 35;
		G_BUTTON.CONTROL_ID = 0;
		while(1)//�ȴ�������
		{
			if(USART_RX_STA&0x8000)
			{
				if(3==G_BUTTON.CONTROL_ID)//ȷ��������
				{
					SetScreen(37);//������
					sign_return = ExportProgram(USART_RECEIVE_STRING0,(char*)USART_RECEIVE_STRING,0);
					if(sign_return == 0)
					{
						SetTextValue(34,2,(char*)EXPORT_SUCCESSFUL);
					}
					else
					{
						DisplayUdiskError(sign_return);
					}
					SetScreen(34);
					delay_1ms(1000);
					return;
				}
				if(4==G_BUTTON.CONTROL_ID){return;}//ȡ��������
				USART_RX_STA = 0;
				G_BUTTON.CONTROL_ID = 0;
			}
			ErrCheck(35);
		}
	}

//**************��3����*****************//���򵼳������������

	DisplayUdiskError(sign_return);
	SetScreen(34);
	delay_1ms(1000);
}

/******************************************************
 * �������ܣ���ʾ�Ƿ��ȱ����ڴ��еĳ���
 * ����ֵ��0->����ɹ�
 * 		   1->����ʧ��
 * 		   2->����ʧ��
 * 		   0xff->ѡ����NO
*/
static u8 save_program_to_flash()
{
	 u8 return_value = 0;

	if(2 == MEMORY_TO_FLASH_SIGN)
	{
		 USART_RX_STA = 0;
		 SetTextValue(32, 1, "Program changed,save now? ") ;

		 if( prompt_dialog_box(32,32) )
		 {
			   USART_RX_STA = 0;
			   return_value = SaveProgram((unsigned char *)s_program_name_num.program_name);
			   if(return_value == 0)
			   {
					 SetTextValue(32, 1, "Save program successfully!") ;
					 delay_1ms(550);
					 MEMORY_TO_FLASH_SIGN = 1;
			   }
			   else
			   {
					 SetTextValue(32, 1, "Fail to save program!") ;
					 delay_1ms(1100);
			   }
		 }
		 else return 0xff;
	}
	return return_value;
}


/******************************************************
 * �������ܣ��ж�U�������������ʾU�̳���
*/
static void display_udisk_program()
{
	u8 i=0,j=0,err;
	u8 sign_return=0;
	long prg_num=0;//U������������
	char str[3]={0};
	char USART_RECEIVE_STRING0[USART_REC_LEN] = {0};

	sign_return = GetExStorageState(&prg_num);
	if(sign_return == 0) //U�̿�ʹ��
	{
		err = save_program_to_flash();//�Ƿ���Ҫ�������
		if(err == 1 || err == 2)//����ʧ��
		{
			return;
		}
		G_BUTTON.SCREEN_ID=41;
		SetScreen(41);
		if(prg_num>=11)//�ж�Ҫ��ʾ�ĳ������
			j=11;
		else
			j=prg_num;

		for(i=0;i<11;i++)
		{
			ltoa(i+1, str);
			SetTextValue(41,2*(i+1),str);
		}
		for(i=0;i<j;i++)//��ʾ��ҳ�ĳ���
		{
			sign_return = GetExPrgName(i,USART_RECEIVE_STRING0);
			if(0==sign_return)
			{
				SetTextValue(41,2*(i+1)+1,USART_RECEIVE_STRING0);
			}
			else
			{
				SetTextValue(41,2*(i+1)+1," ");
			}
			//delay_1ms(100);
		}
		for(i=j;i<11;i++)//��ʾ��ҳ�ĳ���
		{
			SetTextValue(41,2*(i+1)+1,"--------");
			delay_1ms(2);
		}
		manage_udisk_program();
		G_BUTTON.SCREEN_ID = 24;
		SetTextValue(32, 1, "save last program ? ") ;
		return;
	}

	if(sign_return == 1) //1->U��δ����
	{
		SetTextValue(34,2,"External storage disconnected!");
	}
	if(sign_return == 5) //5->DSP��STM32ͨ�ų���
	{
		SetTextValue(34,2,"Connection error!");
	}
	SetScreen(34);
	delay_1ms(1000);
	SetTextValue(32, 1, "save last program ? ") ;
	SetScreen(24);
	G_BUTTON.SCREEN_ID=24;
}


/******************************************************
 * �������ܣ���U�̳�����̵Ĳ�������ҳ�������
*/
static void manage_udisk_program()
{
	u32 i=0;
	u8 sign_return=0;
	u8 text_num=0;
	u8 sign_program_num=0;
	long pro_num=0;
	u8 page_num = 1;//��ǰҳ��


	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.screencoordinate_x > 0)//��ʾѡ�еĳ���
			{
				SetFcolor(0xffff);
				GUI_Rectangle(270, 119+(text_num)*30, 570, 149+(text_num)*30);
				text_num = (G_BUTTON.screencoordinate_y-119) / 30;

				SetFcolor(63488);
				GUI_Rectangle(270, 119+(text_num)*30, 570, 149+(text_num)*30);
				G_BUTTON.screencoordinate_x = 0;
				sign_program_num = 1;
			}

			if( (G_BUTTON.CONTROL_ID>23) && (G_BUTTON.CONTROL_ID<26) )//���·�ҳ
			{
				change_page(pro_num,&page_num);
			}

			if( (26==G_BUTTON.CONTROL_ID) && (1==sign_program_num) )//�������
			{
				import_program(&sign_program_num, text_num, page_num);
				G_BUTTON.SCREEN_ID = 41;
			}

			if(G_BUTTON.CONTROL_ID == 27)//�˳�U�̽���
			{break;}

			USART_RX_STA = 0;
			G_BUTTON.CONTROL_ID = 0;
		}

		if(100 == i++)//��ʱ���U�̵�����״̬
		{
			sign_return = GetExStorageState(&pro_num);
			if(0 != sign_return)
				break;
			i=0;
		}
		ErrCheck(G_BUTTON.SCREEN_ID);
	}

	SetScreen(24);//���ص�����������
	refresh_display_program_name(0);
}

/******************************************************
 * �������ܣ��������
*/
static void import_program(u8 *sign_program_num, u8 text_num ,u8 page_num)
{
	u8 read_num;
	u8 sign_return=0;
	u16 t=0;
	G_BUTTON.SCREEN_ID=37;
	USART_RX_STA = 0;

	SetScreen(37);
	GetControlValue(41, 2*((text_num)+1)+1);//���Ͷ�ȡ�ı��ؼ�����ָ��
	while(1)//�ȴ����մ������������ı��ؼ���Ϣ
	{
		if(USART_RX_STA&0x8000)//���յ��������ϴ���Ϣ���������
		{
			read_num = 11*(page_num-1) + text_num;
			sign_return = ImportProgram(read_num,(char*)USART_RECEIVE_STRING,0);

			SetScreen(34);
			if(sign_return == 0)
			{
				SetTextValue(34,2,(char*)IMPORT_SUCCESSFUL);
			}
			else if(sign_return == 3)//�����ظ��������
			{
				SetTextValue(42,5,"Name repeat,YES overwrite��NO rename.");
				SetScreen(42);//����ѡ�񸲸ǻ����������Ĵ���
				G_BUTTON.SCREEN_ID = 42;
				USART_RX_STA = 0;
				while(1)//�ȴ��û�ѡ�񸲸Ǳ������������
				{
					if(USART_RX_STA&0x8000)
					{
						if(3==G_BUTTON.CONTROL_ID)//���Ǳ���
						{
							SetScreen(37);
							sign_return = ImportProgram(read_num,(char*)USART_RECEIVE_STRING,1);
							if(sign_return == 0)
							{
								SetTextValue(34,2,(char*)IMPORT_SUCCESSFUL);
							}
							else
							{
								DisplayUdiskError(sign_return);
							}
							SetScreen(34);
							delay_1ms(1000);
							SetScreen(41);
							return ;
						}
						else if(4==G_BUTTON.CONTROL_ID)//������
						{
							USART_RX_STA = 0;
							break;
						}
						else if(2==G_BUTTON.CONTROL_ID)//cancel
						{
							SetScreen(41);//U�̽���
							USART_RX_STA = 0;
							return;
						}
						USART_RX_STA = 0;
					}
					ErrCheck(42);
				}

				//2.3����������ʵ�ֲ���
				SetTextValue(35,5,(char*)USART_RECEIVE_STRING);
				SetScreen(35);
				G_BUTTON.SCREEN_ID = 35;
				G_BUTTON.CONTROL_ID = 0;
				while(1)//�ȴ�������
				{
					if(USART_RX_STA&0x8000)
					{
						if(3==G_BUTTON.CONTROL_ID)//ȷ��������
						{
							SetScreen(37);
							sign_return = ImportProgram(read_num,(char*)USART_RECEIVE_STRING,0);
							if(sign_return == 0)
							{
								SetTextValue(34,2,(char*)IMPORT_SUCCESSFUL);
							}
							else
							{
								DisplayUdiskError(sign_return);
							}
							SetScreen(34);
							delay_1ms(1000);
							SetScreen(41);
							return;
						}
						if(4==G_BUTTON.CONTROL_ID){	SetScreen(41);	return;}//ȡ��������
						USART_RX_STA = 0;
						G_BUTTON.CONTROL_ID = 0;
					}
					ErrCheck(35);
				}
			}
			else
			{
				DisplayUdiskError(sign_return);
			}

			delay_1ms(1000);
			SetScreen(41);
			break;
		}
		ErrCheck(41);
		delay_1us(1);
		if(t++ == 2000000) break;
	}
	USART_RX_STA=0;
	*sign_program_num = 0;
	G_BUTTON.SCREEN_ID=41;

}
/******************************************************
 * �������ܣ����·�ҳ
*/
static void change_page(long pro_num,u8 *page_num)
{
	u8 i=0,j=0;
	u8 sign_return=0;
	char str[3]={0};
	char USART_RECEIVE_STRING0[USART_REC_LEN]={0};
	static u8 change_sign=0;//��ҳ��Ч��־λ

	if( (*page_num>1) && (G_BUTTON.CONTROL_ID == 24) )//�Ϸ�ҳ
	{
		(*page_num)--;
		change_sign = 1;
	}
	if( ((pro_num+1)>(*page_num)*11) && (G_BUTTON.CONTROL_ID == 25) )//�·�ҳ
	{
		(*page_num)++;
		change_sign = 1;
	}

	if( (pro_num>0) && (1==change_sign) )
	{
		change_sign = 0;
		if(pro_num>=(*page_num)*11)//�ж��ڵ�ǰҳ����Ҫ��ʾ�ĳ������
			j=11;
		else
			j=pro_num-11*((*page_num)-1);

		for(i=0;i<11;i++)//��ʾ��ǰҳ��ĳ�����
		{
			ltoa( (*page_num-1)*11+i+1, str );
			SetTextValue(41,2*(i+1),str);
		}
		for(i=0;i<j;i++)//��ʾ��ǰҳ��ĳ���
		{
			sign_return = GetExPrgName(11*((*page_num)-1)+i,USART_RECEIVE_STRING0);
			if(0==sign_return)
			{
				SetTextValue(41,2*(i+1)+1,USART_RECEIVE_STRING0);
			}
			else
			{
				SetTextValue(41,2*(i+1)+1,"  ");
			}
		}
		for(i=j;i<11;i++)//��ǰҳ��û�г���ı���ı���ʾΪ���
		{
			SetTextValue(41,2*(i+1)+1,"--------");
		}
	}
	else//��ҳ��Ч����
	{
//		SetScreen(34);
//		SetTextValue(34,2,"�ѵ���ͷ��");
//		delay_1ms(1000);
//		SetScreen(41);
	}
	G_BUTTON.CONTROL_ID = 0;
}

/******************************************************
 * �������ܣ��˳�����������-(9.1)
 * ��������� text_num,ѡ�еĳ���ı��
 * 	       last_screen,�������������ǰ���Ǹ�ҳ��ı��
*/
static u8 exit_programs_manage_screen(u8 text_num, u8 last_screen, u8 sign_program_num)
{
	unsigned char err;
	if(G_BUTTON.CONTROL_TYPE == 11)//���µ�Ϊ�л�ҳ��İ�ť
	{
		switch(G_BUTTON.CONTROL_ID)
		{
			case 21:
					//text_num == 100,����ɾ�������ʱ�򣬱���ֵΪ100�ģ�
					//��ʼֵҲΪ100�����Գ����������ܳ�100���������ֵ�޸ı��
					SCREEN_SIGN=0;
					if(sign_program_num == 1)
					{
						if(text_num == 100)
						{
							OPEN_PROGRAM_SIGN = 0;
						}
						else
						{
							err = save_program_to_flash();
							if(err == 1 || err == 2)//����ʧ��
							{
								OPEN_PROGRAM_SIGN = 0;
							}
							else OPEN_PROGRAM_SIGN = text_num + 1;

						}
						 SCREEN_SIGN = 7;
					}
					 break;//
			case 22:
					 SCREEN_SIGN=7;
					 break;
			default: SCREEN_SIGN=0; break;
		}
		if(SCREEN_SIGN > 0)
		{
			if(last_screen == 7)	{ SetScreen(7);SetTextValue(32, 1, "save last edit?") ;}
			if(last_screen == 13)	{ SCREEN_SIGN = 0; USART_RX_STA = 0;}
			return 1;
		}
		return 0;
	}
	return 0;
}

/******************************************************
 * �������ܣ�����ǰѡ�еĳ�����Ϻ�ɫ��ͼ�����±�־λ-(9.2)
 * ��������� text_num,ѡ�еĳ���ı��
 * 	       sign_program_num,��־λ�������ǰ�в���ѡ�г�������Ϊ1������������ʱ����
*/
static void display_objective_program(u8 *text_num, u8 *sign_program_num)
{
	if(G_BUTTON.screencoordinate_x > 0)//��ʾѡ��ĳ���
	{
		SetFcolor(0xffff);
		GUI_Rectangle(48, 90+(*text_num)*30, 725, 120+(*text_num)*30);
		*text_num = (G_BUTTON.screencoordinate_y-89) / 30;

		SetFcolor(63488);
		GUI_Rectangle(48, 90+(*text_num)*30, 725, 120+(*text_num)*30);
		G_BUTTON.screencoordinate_x = 0;
		*sign_program_num = 1;
	}
}

/******************************************************
 * �������ܣ�ˢ�³�������ʾ����-(9.3)
 * ��������� text_num,ѡ�еĳ���ı��
 * 	       sign_program_num,��־λ�������ǰ�в���ѡ�г�������Ϊ1������������ʱ����
*/
static void copy_delete_rename_page(u8 *text_num, u8 *sign_program_num)
{
	if(G_BUTTON.CONTROL_TYPE == 11)//�����������ճ����ɾ���ȣ�
	{
		switch(G_BUTTON.CONTROL_ID)
		{
			case 23: copy_or_rename_program(sign_program_num, text_num, CopyProgram); break;//���Ƴ���
			case 24: copy_or_rename_program(sign_program_num, text_num, RenameProgram); break;//����������
			case 25: display_previouspage_program_name();break;//�Ϸ�ҳ
			case 26: display_nextpage_program_name();break;//�·�ҳ
			case 27: delete_program(text_num, sign_program_num);break;//ɾ������
			default:break;
		}
	}
}

/******************************************************
 * �������ܣ����Ƴ���-(9.31)
 * ���������text_num,ѡ�еĳ���ı��
 * 	       sign_program_num,��־λ�������ǰ�в���ѡ�г�������Ϊ1������������ʱ����
 * 	       f ����ָ��
*/
static void copy_or_rename_program(u8 *sign_program_num, u8 *text_num, u8 f(u8 *old_name, u8 *new_name))
{
	u8 i = 0,err;
	u8 sign_return = 0;
	u8 program_name[USART_REC_LEN] = {0};
	Uint32 t=0;
	if(*sign_program_num == 1)//������ѡ�г���
	{
		err = save_program_to_flash();
		if(err == 1 || err == 2)//����ʧ��
		{
			return;
		}
		G_BUTTON.SCREEN_ID = 35;
		while(1)
		{
			 if(USART_RX_STA&0x8000)//���յ��������ϴ���Ϣ���������
			 {
				USART_RX_STA = 0;
				break;
			 }
			 if(t++ == 5000000) break;
		}

		GetControlValue(24, 2*((*text_num)+1));//���Ͷ�ȡ�ı��ؼ�����ָ��
	    while(1)//�ȴ����մ������������ı��ؼ���Ϣ
	    {
		    if(USART_RX_STA&0x8000)//���յ��������ϴ���Ϣ���������
		    {
		    	for(i=0; i<PROGRAM_NAME_LENGTH; i++)
		    	{
		    		program_name[i] = USART_RECEIVE_STRING[i];
		    	}
		    	USART_RX_STA = 0;
		    	break;
		    }
			delay_1us(1);
			if(t++ == 2000000) break;
		}

		SetTextValue(35,5,(char*)program_name);
		SetScreen(35);//���������ֵĿ�
		G_BUTTON.SCREEN_ID=35;

	    USART_RX_STA=0;
	    G_BUTTON.CONTROL_ID = 0;
	    USART_RECEIVE_STRING[0] = 0;

		if(prompt_dialog_box(0, 24))
		{
			 G_BUTTON.SCREEN_ID = 24;
			if(USART_RECEIVE_STRING[0] > 0)
			{
				SetScreen(37);//������
				err = f(program_name, USART_RECEIVE_STRING);
				if( err == 0 )
				{
					//������������ĳ��������ڴ򿪵ģ��������ʾ���ڴ򿪵ĳ���
					if(f == RenameProgram)//�жϺ���ָ���Ƿ�ΪRenameProgram
					{
						sign_return = StringCmp(PROGRAM_NAME_LENGTH,program_name,(u8*)s_program_name_num.program_name);
						if(sign_return == 0)
						{
							for(i=0; i<PROGRAM_NAME_LENGTH; i++)
							{
								s_program_name_num.program_name[i] = USART_RECEIVE_STRING[i];
							}
							SetTextValue(7, 83,(char*)s_program_name_num.program_name);
						}
						refresh_display_program_name(4);//ˢ�µ�ǰҳ
					}
					else refresh_display_program_name(3);//ˢ�����һҳ
				}
				else
				{
					DispalyProgEditInfo(err);
				}
				SetScreen(G_BUTTON.SCREEN_ID);
			}
		}
		*sign_program_num = 0;
	}
}

/******************************************************
 * �������ܣ�ˢ�³�������ʾ����-(9.33)
 * ���������pages,��Ҫˢ�½���ķ�ʽ���Ϸ�ҳ���·�ҳ��ճ��ɾ���ȡ�0��ʾ��ʾ��һҳ��1�·�ҳ��2��ʾ�Ϸ�ҳ
*/
static void display_nextpage_program_name()
{
	refresh_display_program_name(1);

}

/******************************************************
 * �������ܣ�ˢ�³�������ʾ����-(9.34)
 * ���������pages,��Ҫˢ�½���ķ�ʽ���Ϸ�ҳ���·�ҳ��ճ��ɾ���ȡ�0��ʾ��ʾ��һҳ��1�·�ҳ��2��ʾ�Ϸ�ҳ
*/
static void display_previouspage_program_name()
{
	refresh_display_program_name(2);
}

/******************************************************
 * �������ܣ�ˢ�³�������ʾ����-(9.35)
 * ���������pages,��Ҫˢ�½���ķ�ʽ���Ϸ�ҳ���·�ҳ��ճ��ɾ���ȡ�0��ʾ��ʾ��һҳ��1�·�ҳ��2��ʾ�Ϸ�ҳ
 * 	      sign_program_num����ʾѡ�еĳ���ı��
*/
static void delete_program(u8 *text_num, u8 *sign_program_num)
{
	unsigned char err;
	Uint32 t=0;
	if(*sign_program_num)
	{
		if(prompt_dialog_box(27,24))
		{
		   USART_RX_STA = 0;
		   GetControlValue( 24, 2*((*text_num)+1) );//���Ͷ�ȡ�ı��ؼ�����ָ��
		   while(1)//�ȴ����մ������������ı��ؼ���Ϣ
		   {
			   if(USART_RX_STA&0x8000)//���յ��������ϴ���Ϣ���������
			   {
				   //�����ɾ���ĳ������ڴ�״̬
				   if(!StringCmp(PROGRAM_NAME_LENGTH,USART_RECEIVE_STRING,(unsigned char*)s_program_name_num.program_name))
				   {
			    		SetTextValue(43,2,"The program is opened,cannot be operated��");
			    		SetScreen(43);
			    		delay_1ms(1100);
			    		G_BUTTON.SCREEN_ID = 24;
			    		SetScreen(24);
			    		USART_RX_STA = 0;
			    		G_BUTTON.CONTROL_ID = 0;
					    break;
				   }
				   err = DeleteProgram(USART_RECEIVE_STRING);
				   if(err)
				   {
					   DispalyProgEditInfo(err);
				   }
				   else
				   {
					   refresh_display_program_name(3);//ˢ����ʾ���ҳ�������
				   }
				   *text_num = 100;
				   break;
			   }
				delay_1us(1);
				if(t++ == 2000000) break;
		   }
		   USART_RX_STA=0;
		}
		*sign_program_num = 0;
		G_BUTTON.SCREEN_ID = 24;
	}
}

/******************************************************
 * �������ܣ�ˢ�³�������ʾ����-(9.4)
 * ���������pages,��Ҫˢ�½���ķ�ʽ��0��ʾ��ʾ��һҳ��
 * 									   1�·�ҳ��
 * 									   2��ʾ�Ϸ�ҳ��
 * 									   3��ʾ���һҳ
 * 									   others:ˢ�µ�ǰҳ
 */
static void refresh_display_program_name(u8 page_sign)
{
	u8 i=0, j=0;
	u8 program_num = 0;
	char str[3] = {0};
	static u8 page_num = 1;//��ǰҳ��

	program_num = GetProgramNum();
	if(program_num == 0)//û�г���
    {
		for(i=0; i<10; i++)
		{
			 ltoa(i+1, str);
			 SetTextValue(24, 2*(i%10+1)-1, str);
			 SetTextValue(24, 2*(i%10+1), " ");
		}
    }
    else
    {
    	if( page_sign == 0 )	{ page_num = 1;}
		if( page_sign == 3 )	{ page_num = (program_num-1)/10 + 1;}
		if( (page_sign==2) && (page_num>1) )
		{ page_num -= 1;}	//�Ϸ�ҳ
		if( (page_sign==1) && (page_num < ((program_num-1)/10+1)) )
		{ page_num += 1;}	//�·�ҳ

		if( (program_num-(page_num-1)*10) < 10 )//�жϵ�ǰҳ����Ҫ��ʾ�ĳ�������
		{
			for(i=(page_num-1)*10; i<program_num; i++)
			{
				  j = GetProgramName(i, USART_RECEIVE_STRING);
				  if(j == 0)
				  {
					  ltoa(i+1, str);
					  SetTextValue(24, 2*(i%10+1)-1, str);
					  SetTextValue(24, 2*(i%10+1), (char*)USART_RECEIVE_STRING );
				  }
			}
			for(; i<10*page_num; i++)
			{
				  ltoa(i+1, str);
				  SetTextValue(24, 2*(i%10+1)-1, str);
				  SetTextValue(24, 2*(i%10+1), " ");
			}
		}
		else
		{
			for(i=(page_num-1)*10; i<10*page_num; i++)
			{
				  j = GetProgramName(i, USART_RECEIVE_STRING);
				  if(j == 0)
				  {
					  ltoa(i+1, str);
					  SetTextValue(24, 2*(i%10+1)-1, str);
					  SetTextValue(24, 2*(i%10+1), (char*)USART_RECEIVE_STRING);
				  }
			}
		}
		USART_RECEIVE_STRING[0] = 0;//����
    }
}

/******************************************************
 * �������ܣ���ѡ�еĳ���-(9.5)
 * ��������� text_num,��굱ǰ�����ı��ؼ��ı��
 * 	       text_num0,�����һ�������ı��ؼ��ı��
 * 	       page_num,��ʾ�����ҳ���ҳ����
 */

static void open_program(u8 *page_num, u8 *text_num, u8 *text_num0, u8 *sign_edit, u8 *open_program_sign)
{
	u8 return_value = 0;
	char str[5] = {0};
	unsigned char i;
	unsigned int num;
	Uint32 t=0;
	if(OPEN_PROGRAM_SIGN)
	{
		*open_program_sign = 1;
		GetControlValue(24, 2*OPEN_PROGRAM_SIGN);//���Ͷ�ȡ�ı��ؼ�����ָ��
		while(1)//�ȴ����մ������������ı��ؼ���Ϣ
		{
			if(USART_RX_STA&0x8000)//���յ��������ϴ���Ϣ���������
			{
				return_value = OpenProgram(USART_RECEIVE_STRING,&num);//��Ҫ��ȡ�ĳ�������ַ��͸��ײ�����,�ѳ�������ڴ档
				if(return_value == 0)
				{
					ltoa(OPEN_PROGRAM_SIGN, str);
					s_program_name_num.num = OPEN_PROGRAM_SIGN;
					for(i=0;i<PROGRAM_NAME_LENGTH;i++)
					{
						s_program_name_num.program_name[i] = USART_RECEIVE_STRING[i];
						if(USART_RECEIVE_STRING[i]==0x00) break;
						Software_name_check = Software_name_check +USART_RECEIVE_STRING[i];//������//2018/06/01
					}
					SetTextValue(7, 82, str);
					SetTextValue(7, 83, (char*)USART_RECEIVE_STRING);
					refresh(1, 7);//ˢ�³���༭����
					*page_num = 1;//ҳ������Ϊ1
					*text_num = 1;
					*text_num0 = 1;
					*sign_edit = 0;
					USART_RX_STA = 0;//���ڱ�־λ���㣬�ȴ���һ�δ�����Ϣ��
					MEMORY_TO_FLASH_SIGN = 1;//�򿪳���󣬾�û�г�����Ҫ���棬�ʸ�ֵΪ1
					for(i=0;i<PROGRAM_NAME_LENGTH;i++)
					{
						USART_RECEIVE_STRING[i]=0x00;
					}
					break;
				}
				else
				{
					DispalyProgEditInfo(return_value);
					//ltoa(return_value, str);
					//SetTextValue(7, 83, str);
					break;
				}
			}
			delay_1us(1);
			if(t++ == 2000000) break;
		}
		USART_RX_STA=0;
		ERROR_LINE[5]=0;//���������Ϣ����
		DISPLAY_ERROR_LINE=0;
		OPEN_PROGRAM_SIGN = 0;
		G_BUTTON.CONTROL_TYPE = 0;

	}
}

/******************************************************
 * �������ܣ���ѡ�еĳ���-(9.5)
 * ��������� err	�������
 * 			  1������������ٱ���
 * 			  2���޴˳���
 * 			  3�����ֲ����ظ�
 * 			  4������������19���ַ�����
 * 			  5: ��ʽ���ɹ�
 * 			  6: ��ʽ��ʧ��
 * 			  7���������!
 * 			  0xff�������������������½�
 */
static void DispalyProgEditInfo(u8 err)
{
	SetScreen(20);
	SetTextValue(20, 4, "");
	switch(err)
	{
	case 1:
		SetTextValue(20, 3, "Flash error,try again!");
		break;
	case 2:
		SetTextValue(20, 3, "Have no this program!");
		break;
	case 3:
		SetTextValue(20, 3, "Name repeat!");
		break;
	case 4:
		SetTextValue(20, 3, "Program name limited to 19 characters!");
		break;
	case 5:
		SetTextValue(20, 3, "Formatting successful!");
		break;
	case 6:
		SetTextValue(20, 3, "Failed to format!");
		break;
	case 7:
		SetTextValue(20, 3, "Password is incorrect!");
		break;
	case 0xff:
		SetTextValue(20, 3, "Program is full,cannot new program!");
		break;
	default:
		SetTextValue(20, 3, "Other ERROR!");
		break;
	}
	USART_RX_STA = 0;
	while(1)
	{
	    if((USART_RX_STA&0x8000) || KeyBoard_PressOK())
	    { break;}
	    ErrCheck(G_BUTTON.SCREEN_ID);
	    delay_1ms(1);
	}
	USART_RX_STA = 0;
	SetScreen(G_BUTTON.SCREEN_ID);
	G_BUTTON.KEY_VALUE = 0;
	G_BUTTON.CONTROL_TYPE = 0;
	G_BUTTON.screencoordinate_x = 0;
	G_BUTTON.screencoordinate_y = 0;
	USART_RX_STA = 0;
}

//��10�£�����༭���Թ�����
/********************************************************************************
 * 10.1
 * 10.2
*********************************************************************************/

/*******************************************
* ��    �ƣ� screen3-(10.1)
* ��    �ܣ� ���˵�3���ж�����һ����ť����,�л�����Ӧҳ�����
*/
static void screen3()
{
	unsigned long key_value;
	char stm32_version[8] = "ERROR";
	u8 err;
	SetScreen(3);//�жϴ�����ҳ�������ҳ���Ƿ�һ��
	G_BUTTON.SCREEN_ID = 3;

	//��ǰ�Ļ�е���ͺ�
	SetTextValue(3,14,ROBOT_PARAMETER.NAME);

	//stm32 ����汾
	SetTextValue(3, 8, "IO version");
	err = GetStm32Version((u8*)&stm32_version[1]);
	if(err)
	{
		SetTextValue(3, 11, stm32_version);
	}
	else
	{
		stm32_version[0] = 'V';
		stm32_version[5] = 0;
		SetTextValue(3, 11, stm32_version);
	}

	//dsp����汾
	SetTextValue(3, 12, CURRENT_VERSION);

	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_ID == 7)//IPE logo
			{
				USART_RX_STA = 0;
			}
			else
			{
				switch(G_BUTTON.CONTROL_ID)//�ж�����һ����ť����,�����������л�����Ӧҳ��
				{
					case 1: SCREEN_SIGN=1; break;
					case 2: SCREEN_SIGN=2; break;
					case 3: SCREEN_SIGN=4; break;
					case 4: SCREEN_SIGN=5; break;
				   default:		    break;
				}
				USART_RX_STA = 0;
				break;
			}

		}
#if KEY_BOARD
		key_value = ReadKeyboardValue();
		if(key_value == ReadKeyboardValue())
		{
			if(1 == switch_teach_model(key_value, 10)){return;}
			KeyboardDataProcessing(key_value);
		}
#endif
		ErrCheck(3);
	}
	//s_times=0;
}

/*****************************************
* ��    �ƣ� screen5-(10.2)
* ��    �ܣ� ���˵�5���ж�����һ����ť����,�л�����Ӧҳ�����
*/
static void screen5()
{
	unsigned long key_value;
	SetScreen(5);//�жϴ�����ҳ�������ҳ���Ƿ�һ��
	G_BUTTON.SCREEN_ID = 5;

	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			switch(G_BUTTON.CONTROL_ID)
			{
				case 1: SCREEN_SIGN=1; break;
				case 2: SCREEN_SIGN=2; break;
				case 3: SCREEN_SIGN=3; break;
				case 4: SCREEN_SIGN=4; break;
			   default:		    break;
			}
			USART_RX_STA = 0;
			break;
		}
#if KEY_BOARD
		key_value = ReadKeyboardValue();
		if(key_value == ReadKeyboardValue())
		{
			if(1 == switch_teach_model(key_value, 10)){return;}
			KeyboardDataProcessing(key_value);
		}
#endif
		ErrCheck(5);
	}
}

/*****************************************
 * �������ܣ���ʾ��ǰ����
 * ���������NONE
 * ���������NONE
 */
void SetCurrentScreen(void)
{
	char str[6]= "SVO";
	unsigned char i,ovrd;
	unsigned int IOState;
	SetScreen(G_BUTTON.SCREEN_ID);//��ʾ��ǰ����
	//�ָ�����/ֱ��ģʽ��ť
	TEACH_MODEL = GetModeState();
	SetModeState(TEACH_MODEL);

	if(G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG)//�����ʱ�б���
	{
		SetTextValue(20, 4, str);
		SetTextValue(20, 3, "ALARM");
	}
/**** �ָ����������� ****/

	//ʾ���ٶ�ֵ�ָ�
	ltoa(SPEED_VALUE,str);
	SetSliderValue(10,5,SPEED_VALUE);
	SetTextValue(10,18,str);
	SetSliderValue(22,5,SPEED_VALUE);
	SetTextValue(22,19,str);
	SetSliderValue(23,5,SPEED_VALUE);
	SetTextValue(23,18,str);

	//���IOͼ��ָ�
	//if(G_BUTTON.SCREEN_ID == 12)
	{
		IOState = ReadPoutState();		// ��ȡIO״̬
		for(i=0;i<8;i++)
		{
			if(IOState & (0x01<<i))
			{
				SetButtonValue(12,i+2,0);//IO�ر�״̬
			}
			else
			{
				SetButtonValue(12,i+2,1);//IO��״̬
			}
		}

		//ɲ��ͼ��ָ�
		IOState = ReadBrakeState();
		for(i=0;i<3;i++)
		{
			if(IOState & (0x01<<i))
			{
				SetButtonValue(12,i+15,0);//ɲ���ر�״̬
			}
			else
			{
				SetButtonValue(12,i+15,1);//ɲ����״̬
			}
		}
	}

	//�ָ��ŷ�����ͼ��
	if(G_CTRL_BUTTON.CTRL_BUTTON.SERVO_ON == 0)
	{
		SetButtonValue(2, 13, 1);//�����ŷ����ذ�ť����Ϊ��״̬
	}
	else
	{
		SetButtonValue(2, 13, 0);//�����ŷ����ذ�ť����Ϊ�ر�״̬
	}

	//�ָ�ȫ������ʱ��ȡ����ť��ͼ��
	SetButtonValue(6, 18, 1);

	//�ָ�ȫ������ʱ��ȫ���ٶȵ�ֵ
	ovrd = GetOvrd();
	ltoa(ovrd,str);
	SetTextValue(6, 19, str);//������ʾovrd

//	SetTextValue(6, 83, s_program_name_num.program_name);

	ltoa(s_program_name_num.num, str);
	//�ָ�����༭����ҳ��7��
	if(G_BUTTON.SCREEN_ID != 6)//ȫ������ʱ����Ҫ�ָ����˳�ȫ�����л�ˢ��
	{
		//�ָ��������ֺͳ�����
		SetTextValue(7, 82, str);
		SetTextValue(7, 83, &s_program_name_num.program_name[0]);
		refresh(page_num,7);
	}
	else
	{
		if(ProgramRuningFlag(0) == 1)
		{
			SetButtonValue(6,2,1);//�������ͣ״̬�򽫰�ť�ָ�Ϊ���������С�
		}
		//�ָ���������
		SetTextValue(6, 83, &s_program_name_num.program_name[0]);
		//�ָ�����༭����ҳ��6��ȫ������ҳ�棩
		refresh(page_num,6);
	}

	//�ָ��������ҳ��
	if(G_BUTTON.SCREEN_ID == 24)
		refresh_display_program_name(0);

	keyboard_speed_meter(3);//�ٶ��Ǳ�������Ǵ�״̬�͹ر�

	//�ָ���������ͼ��

	//�ָ���λ����ͼ��


}



//===========================================================================
// No more.
//===========================================================================
