/*
 *  版权声明：广州新豪五金精密制品有限公司研发部BU28拥有所有最终解析权，未经批准，不得转载，违者必究
 *  文件名称：Touch_screen.c
 *  摘	 要： 第1章：头文件声明区
 *  		第2章：全局变量定义区
 *  		第3章：内部函数声明区域
 *  		第4章：外部函数实现区
 *  		第5章：公共功能区域
 *  		第6章：机械手示教和IO口管理区域
 *  		第7章：属性设置区域
 *  		第8章：程序编辑调试管理区
 *  		第9章：程序管理界面区域
 *  		第10章：使用说明区域和关机区域
 *  当前版本：
 *  作	 者：王文东
 *  创建日期：2015-1-10
 *  完成日期：2015-4-27
 *  修改内容：格式类
 *
 *  	         程序类：删除掉了manage_usart_return_value()函数中按下切换页面时获取返回值中当前页面编号的信息
 *  孔工修改：step by step
 *
 */


//第1章：头文件声明区
/********************************************************************************
* 名    称： 头文件
* 功    能： 外部接口声明
* 说	   明：头文件排序方式2，首字母升序排序（方式1：不稳定到稳定排序）
 ********************************************************************************/
#include "BuildProgram.h"
#include "DSP2833x_DefaultIsr.h"
#include "EditProgram.h"
#include "KeyboardDriver.h"
#include "kinematic_explain.h"
#include "Man_DriverScreen.h"
#include "Man_Nandflash.h"//修正于2018/06/01
#include "Man_MCX31xAS.h"
#include "My_Project.h"
#include "ScreenApi.h"
#include "stdlib.h"
#include "stdio.h"
#include "Touch_screen.h"
#include"DSP28335_Spi.h"

//第2章：全局变量定义区
/********************************************************************************
* 名    称： 定义区
* 功    能： 定义变量和一些宏定义等
 ********************************************************************************/
#define error_information_length  50
#define PAGE_NUMBER (unsigned int)(ROWS_IN_PROGRAM/15)
static u8 SCREEN_SIGN = 1;//页面编号标志位
static u8 USART_RECEIVE_STRING[USART_REC_LEN] = {0};//用于保存待处理的字符串
static u8 SPEED_VALUE = 5;//示教速度值
static u8 OPEN_PROGRAM_SIGN = 0;//是否打开程序标志位，1确认打开，0表示不打开
static u8 MEMORY_TO_FLASH_SIGN = 1;//内存与闪存同步的标志,1->没有程序需要保存  2->有程序需要保存
static u8 TEACH_MODEL=0;
static u16 ERROR_LINE[6]={0};//0~4存储编译后错误代码所在的行数，5存储编译信息：0表示编译成功，1表示编译失败,
static u8 DISPLAY_ERROR_LINE=0; //存储按下触摸屏后错误信息所在的行数
static u8 DEVELOPER_MODE=0;//0表示客户模式，1表示开发者模式
extern unsigned char Teach_mode;//修正于//2018/06/01
extern long TIMER0_CNT;
extern struct program_name_num s_program_name_num;
static u8 net_control=0;//修正于//2018/06/01
static u16 Software_name_check=0;
static u8 build_program_sign = 0;//修正于//2018/06/01
struct button//声明按钮结构体，用于保存各种控件的值
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
const char SAVE_FAILED[]="Failed to save！";
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



//第3章：内部函数声明区域
/********************************************************************************
* 名    称：内部函数声明
* 功    能：调用函数
* 说	   明：
 ********************************************************************************/
//公共功能区域5-3
u8 DisplayErrInfo(unsigned char enable);//5.1
static void manage_button_slider_text(u8 usart_receive_value[32]);//5.2
static void manage_touch_screen_coordinate(u8 usart_receive_value[32]);//5.3

//机械手示教和IO口管理区域6-5
static void screen1(void);//6
static void screen12(char screen_id);//6.1
static u8 switch_teach_model(u32 key_value, u8 screen_sign);
static void screen10(void);//6.2
static void screen22(void);//6.3
static void screen23(void);//6.4
//属性设置区域7-20
static void screen2(void);//7
static void origin_sound_alarm(void);//7.1
static void locked_origin(void);//7.11
static void sure_locked_origin(void);//7.111
static void return_home(void);//7.12
static void noparameter(u8 screen_id);//7.121
static void sure_return_home(u8 speed);//7.122
static void sound_settings(void);//7.13声音设置
static void display_open_servo_fail_inform(u8 servo_inform);//7.151
static void formatting_sd(void);//7.16
static void developer_mode(void);//7.17
static void switch_alarm_signal_mode(void);
static void switch_limit_mode(void);
static void switch_teach_speed_mode(void);
static void switch_mode(void);//修正于2018/06/01
static void screen8(void);//7.2
static void screen11(void);//7.3
static void screen13(void);//7.4
static u16 array_to_date(void);//7.41
static void reset_text(u8 clocks[2], u8 minutes[2]);//7.42
static void start_text(u8 clocks[2], u8 minutes[2]);//7.43
static u8 check_real_time(u8 clocks[2], u8 minutes[2]);//7.431
static void display_runing_test_screen(u8 clocks[2], u8 minutes[2]);//7.432
static void run_intime(u8 clocks[2], u8 minutes[2], u8 program_name[18]);//7.433

//程序编辑调试管理区8-22
static void screen4(void);//
static void screen7(void);//8
static void display_ten_page(unsigned long sign);//翻10页
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

//程序管理界面区域9-6
static void programs_manage(u8 last_screen);//程序管理//9
static void programs_net_manage(u8 last_screen,u8 text_num,u8* check_value);//通过网络进行程序管理
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

//使用说明区域和关机区域10-2
static void screen3(void);//10.1
static void screen5(void);//10.2


//第4章：外部函数实现区
/********************************************************************************
*********************************************************************************/
/*
 * 描述：当执行open指令打开一个程序的时候，必须修改s_program_name_num的内容使其
 * 与打开的程序名同步，然后刷新程序行页面。
 * 函数功能：修改s_program_name_num
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
		SetTextValue(6, 83, &s_program_name_num.program_name[0]);//刷新显示正在运行的程序名
		refresh(1,6);//刷新程序页面
	}
	else
	{
		ltoa(num,str);
		SetTextValue(7, 82, str);
		SetTextValue(7, 83, &s_program_name_num.program_name[0]);//刷新显示正在运行的程序名
		refresh(1,7);
	}
}

/***************************************
* 名    称： scan_touch_screen(4.1)
* 功    能： 全盘扫描并行的页面，实现并行页面之间的切换
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
	SCREEN_SIGN = 1; //页面标志位设定为1，开机时默认进入主菜单一的界面
	G_BUTTON.SCREEN_ID = 1; //屏幕标号设为1

	SetTextValue(13, 12, "00"); //定时任务时间清零
	SetTextValue(13, 13, "00");
	SetTextValue(13, 14, "00");
	SetTextValue(13, 15, "00");
	//delay_1ms(500); //等待开机动态图显示完

    while(1)
    {
		USART_RX_STA = 0; //每次切换页面时清空控件信息和串口接收中断标志位
		G_BUTTON.KEY_VALUE = 0;
		G_BUTTON.CONTROL_ID = 0;
		G_BUTTON.CONTROL_TYPE = 0;
		G_BUTTON.BUTTON_STATUS = 0;

		switch(SCREEN_SIGN)//切换到对应标志位的页面
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
* 名       称： s_button_slider(4.2)
* 功       能： 处理串口接收回来的数值
* 入口参数： 传递串口中断缓冲值
*/
void manage_usart_return_value(u8 usart_receive_value[USART_REC_LEN])
{
	if( (usart_receive_value[1]==0xB1) && (usart_receive_value[2]==0x11) )//按下的为按钮，滑块或者返回文本值
	{
		manage_button_slider_text(usart_receive_value);
	}

	else
	{
		if( usart_receive_value[1] == 0xf7 )//返回RTC控件内容
		{
			USART_RECEIVE_STRING[0] = usart_receive_value[6];//0001 0101
			USART_RECEIVE_STRING[1] = usart_receive_value[7];//0010 0000
			USART_RECEIVE_STRING[0] = ( (USART_RECEIVE_STRING[0]>>4)*10 + (USART_RECEIVE_STRING[0]&0x0f) );//BCD编码转码
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
* 名    称： GetScreenId(4.3)
* 功    能： 获得当前页面ID
*/
u16 GetScreenId(void)
{
	 return G_BUTTON.SCREEN_ID;
}

/****************************************************
* 名       称： display_servo_alarm(4.4)
* 功       能： 显示伺服报警信息
* 入口参数： axis，出错轴编号，范围1~6
* 		  error，错误编号信息
* 		  screen_id，当前触摸屏所在的屏幕编号
*/
void display_servo_alarm(u8 axis,u8 error,u8 screen_id)
{
	char str[] = {"Jx:"};
	unsigned int time_flg=0;
	unsigned char alarm_flg = 0;
	str[1] = axis + '0';
	OpenBuzzer();
	if( ((error>>4) & 0x01) == 1 )//伺服报警
    {
		SetScreen(20);
		G_BUTTON.SCREEN_ID = 20;
		SetTextValue(20, 4, str);
		SetTextValue(20, 3, "ALARM");
		alarm_flg = 1;
	}
	else if( ((error>>5) & 0x01) == 1 )//急停
	{
		SetScreen(33);
		delay_1ms(200);
	}
	else if((error&0x01) == 1)//正限位
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
	else if( ((error>>1) & 0x01) == 1 )//负限位
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
		SetTextValue(20, 3, "ERROR #001");//xxx：如果伺服报警没有进入这里，那么就改为急停！
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
 * 名称：RecoverSpeed-4.5
 * 功能：恢复速度，每次直线插补或者程序运行完之后，速度都要恢复到速度滑动条的值
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
	//调速变化范围1~100
	ModifyAllSpeed(tmp_speed);
	return speed;
}

/*********************************************
 * 名称：RecoverSpeed-4.6
 * 功能：仪表盘显示当前速度值
 * 参数：sign:20显示速度仪表盘，23增加速度，22减小速度，3关闭显示速度仪表盘
 */
void keyboard_speed_meter(u8 sign)
{
	static u8 select_sign=0;
	static u8 screen_id=0;

	if( (select_sign == 1) && ((sign==22)||(sign==23)) )//这个按得多，所以放在最前面提高效率
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
 * 名称：RecoverSpeed-4.61
 * 功能：仪表盘显示当前速度值
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
 * 名称：按键增减速度值-4.62
 * 功能：仪表盘显示当前速度值
 * 参数：speed：23增加速度，22减小速度
 */
void key_change_speed_value(u8 speed)
{
	static u8 sign = 0;
	char str[4]={0};

	if(!GetSpeedMode())//如果是低速模式
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

	SetSliderValue(10,5,SPEED_VALUE);//单轴虚拟键盘页面速度条更新
	SetTextValue(10,18,str);//速度条的值更新
	SetSliderValue(22,5,SPEED_VALUE);//直线插补虚拟键盘页面速度条更新
	SetTextValue(22,19,str);//速度条的值更新
	SetSliderValue(23,5,SPEED_VALUE);//末端坐标虚拟键盘页面速度条更新
	SetTextValue(23,18,str);//速度条的值更新

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


//第5章：公共功能区域
/********************************************************************************
 * 5.1
 * 5.2
 * 5.3
 * 5.4
 * 5.5
*********************************************************************************/

/******************************************
 * 名        称：DisplayErrInfo(5.1)
 * 功        能：显示错误信息
 * 输入参数 ：enable，相应bit为1就使能相应的报警
 * 			bit0，伺服on
 * 			bit1，警报
 * 			bit2，有无原点
 * 			bit3，急停
 * 			bit4, 正限位
 * 			bit5, 负限位
 * 			bit6, 安全开关未按下
 * 			bit7, 1轴刹车未打开
 * 			bit8, 2轴刹车未打开
 * 			bit9, 3轴刹车未打开
 * 返回值    ：0：无警报/错误信息	1：有警报/错误信息
*/
u8 DisplayErrInfo(u8 enable)
{
	 u16 err;
	 unsigned char i,err_axis;
	 char str[]={"Jx:"};
	 err = ReadErrInfo()&enable;
	 err_axis=ReadErrState();//bit0~bit5:J1~6,读轴错误状态
	 for(i=0;i<6;i++)
	 {
		 if(err_axis>>i)
		 {
			 str[1] = i + '0';
			 break;
		 }

	 }
	 if(err)//屏蔽无原点
	 {
		  SetDriCmd(0x3F,0x27);
		  delay_1ms(1);
		  SetScreen(20);
		  SetTextValue(20, 4, "");

		  if(err&0x02)//伺服报警
		  {
			   SetTextValue(20, 3, "Servo Alarm!");
		  }
		  else if(err&0x01)//伺服off
		  {
			   SetTextValue(20, 3, "Servo OFF!");
		  }
		  else if( (err&0x10) == 1 )//正限位
		  {
			   SetTextValue(20, 4, str);
			   SetTextValue(20, 3, "+Limited!");
		  }
		  else if( (err&0x20) == 1 )//-限位
		  {
			   SetTextValue(20, 4, str);
			   SetTextValue(20, 3, "-Limited!");
		  }
		  else if(err&0x380)
		  {
			  SetTextValue(20, 3, "Brake is closing!");
		  }
		  else if(err&0x08)//急停
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
* 名       称： manage_button_slider_text(5.2)
* 功       能： 处理当触摸屏串口接收回来的是按钮，滑块和文本信息的数据
* 入口参数： 串口接收值
*/
static void manage_button_slider_text(u8 usart_receive_value[USART_REC_LEN])
{
	u8 i = 8;//通常i用在循环中
	G_BUTTON.CONTROL_ID = (usart_receive_value[5]<<8) + usart_receive_value[6];

	if(usart_receive_value[7] == 0x10)//表示按下的为按钮控件
	{
		switch(usart_receive_value[8])
		{
			case 0x00://切换画面功能
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
			case 0x01://开关模式
				G_BUTTON.CONTROL_TYPE = 11;
				G_BUTTON.BUTTON_STATUS = usart_receive_value[9];
				if(G_BUTTON.SCREEN_ID == 6)
				{
					if(G_BUTTON.CONTROL_ID == 18)
					{
						ProgramRuningFlag(3); //程序退出运行
						StopProgramRunning();
					}
					if((G_BUTTON.CONTROL_ID==2) && (G_BUTTON.BUTTON_STATUS==1))//按下暂停键
					{
						StopProgramRunning();//此处调用让伺服电机立即停止的函数
						//SetButtonValue(6,2,1);
//						ProgramRuningFlag(2);
					}
					if((G_BUTTON.CONTROL_ID==2) && (G_BUTTON.BUTTON_STATUS==0))//按下继续运行键
					{
						ProgramRuningFlag(1);
						OpenFan();//打开风扇
					}
					USART_RX_STA = 0;
				}
				break;
			case 0x02://按键模式
				G_BUTTON.CONTROL_TYPE = 12;
				G_BUTTON.KEY_VALUE = usart_receive_value[9];
				break;
			default  : 	break;
		}
	}//end if
	if(usart_receive_value[7] == 0x13)//返回滑块控件的内容
	{
		G_BUTTON.SLIDER_VALUE = usart_receive_value[11];
		G_BUTTON.CONTROL_TYPE = 13;
	}
	if(usart_receive_value[7] == 0x14)//返回仪表控件的内容
	{
		G_BUTTON.METER_VALUE = usart_receive_value[11];
		G_BUTTON.CONTROL_TYPE = 14;
	}
	if(usart_receive_value[7] == 0x11)//返回文本控件内容，并以0结束
	{
		G_BUTTON.CONTROL_TYPE = 15;
		do
		{
			USART_RECEIVE_STRING[i-8] = usart_receive_value[i];
		}
		while(usart_receive_value[i++] > 0);//因为接收到的字符串以'0'结尾,当ASCII码为零时跳出循环
	}
}

/****************************************************************
* 名       称： manage_touch_screen_coordinate(5.3)
* 功       能： 处理触摸屏返回的坐标值
* 入口参数： 串口接收值
*/
static void manage_touch_screen_coordinate(u8 usart_receive_value[USART_REC_LEN])
{
	u16 i;
	if( (usart_receive_value[1]==0x01)//若当前为界面7或者24或者41，则求出当前按下的坐标点信息
	    && ( (G_BUTTON.SCREEN_ID==7) || (G_BUTTON.SCREEN_ID==24) || (G_BUTTON.SCREEN_ID==41) ) )
	{
		G_BUTTON.screencoordinate_x = (usart_receive_value[2]<<8) + usart_receive_value[3];
		G_BUTTON.screencoordinate_y = (usart_receive_value[4]<<8) + usart_receive_value[5];

		if( (G_BUTTON.CONTROL_ID!=97)//若按在界面7的文本控件显示区域，则保留当前坐标值，并且标志位不清零
			 && (G_BUTTON.SCREEN_ID==7) && (G_BUTTON.screencoordinate_y>63)
			 && (G_BUTTON.screencoordinate_y<530) && (G_BUTTON.screencoordinate_x<238)
			 && (G_BUTTON.screencoordinate_x>16) )
		{	return;}

		if( (G_BUTTON.SCREEN_ID==7) //按下enter键
			 &&(G_BUTTON.screencoordinate_x>705) && (G_BUTTON.screencoordinate_x<787)//如果按在编辑区域
			 && (G_BUTTON.screencoordinate_y>484) && (G_BUTTON.screencoordinate_y<525) )
		{
			USART_RX_STA = 0;
			return;
		}
		if( (G_BUTTON.SCREEN_ID==7) //按下键盘区
			 &&(G_BUTTON.screencoordinate_x>250) && (G_BUTTON.screencoordinate_x<790)//如果按在编辑区域
			 && (G_BUTTON.screencoordinate_y>250) && (G_BUTTON.screencoordinate_y<525) )
		{
			return;
		}
		if( (G_BUTTON.SCREEN_ID==24) //如果按在编辑区域
			&&(G_BUTTON.screencoordinate_x<720) && (G_BUTTON.screencoordinate_y>89)
			&&(G_BUTTON.screencoordinate_y<390) && (G_BUTTON.screencoordinate_x>100))
		{	return;}//如果按在编辑区域，保存返回坐标值用于求解光标位置

		if( (G_BUTTON.SCREEN_ID==41) //如果按在编辑区域
			&&(G_BUTTON.screencoordinate_x<570) && (G_BUTTON.screencoordinate_y>119)
			&&(G_BUTTON.screencoordinate_y<449) && (G_BUTTON.screencoordinate_x>270))
		{	return;}//如果按在编辑区域，保存返回坐标值用于求解光标位置

		if( (1==ERROR_LINE[5]) && (G_BUTTON.screencoordinate_y>89) && (G_BUTTON.screencoordinate_y<214)
			 && (G_BUTTON.screencoordinate_x>244) && (G_BUTTON.screencoordinate_x<794) )
		 {
			 DISPLAY_ERROR_LINE=(G_BUTTON.screencoordinate_y-89)/25+1;//取值为1~5的整数
			 for(i=0;i<5;i++)
			 {
				 if(ERROR_LINE[i]==0)break;//计算有多少个编译错误信息
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
***********程序运行时网络通信控制函数//修正于//2018/06/01**********************
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
			ProgramRuningFlag(3); //程序退出运行
			StopProgramRunning();
		}
		if((G_BUTTON.CONTROL_ID==2) && (G_BUTTON.BUTTON_STATUS==1))//按下暂停键
		{
			StopProgramRunning();//此处调用让伺服电机立即停止的函数
			SetButtonValue(6,2,1);
	        //ProgramRuningFlag(2);
		}
		if((G_BUTTON.CONTROL_ID==2) && (G_BUTTON.BUTTON_STATUS==0))//按下继续运行键
		{
			ProgramRuningFlag(1);
			SetButtonValue(6,2,0);
			OpenFan();//打开风扇
		}
		USART_RX_STA = 0;
	}
}

//第六章：机械手示教和IO口管理区域
/*******************************************************************************
 * 6
 * 6.1
 * 6.2
 * 6.3
 * 6.4
********************************************************************************/

/*****************************************
* 名    称： screen1(6)
* 功    能： 主菜单一，判断是哪一个按钮按下,切换对应的页面
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
			switch(G_BUTTON.CONTROL_ID)//判断是哪一个按钮按下,切换到对应页面
			{
				case 1: SCREEN_SIGN = 2;  break;
				case 2: SCREEN_SIGN = 3;  break;
 				case 3: SCREEN_SIGN = 4;  break;
				case 4: SCREEN_SIGN = 5;  break;
				case 5: SCREEN_SIGN = 10; break;
			   default:		       break;
			}

			USART_RX_STA = 0;			//标志位清零

			if(G_BUTTON.CONTROL_ID == 6)//如果是页面12则直接内部调用，其它页面的话跳出此函数
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
* 名    称： screen12(6.1)
* 功    能： 判断是哪一个按钮按下,打开或者关闭对应的IO口
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

			//G_BUTTON.CONTROL_ID范围为2~9，分别对应按钮的0~7; ID15~17分别对应刹车J1~J3的刹车
			if(G_BUTTON.BUTTON_STATUS == 1)///G_BUTTON.BUTTON_STATUS为1的时候表示打开，0的时候表示关闭
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
			else//关闭
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
			input_state = ReadPinState();	//获取当前输入IO状态
			for(i=0;i<MAX_PIN_NUM;i++)
			{
				DisGifFrame(48,i+50,(input_state>>i)&((u32)0x01));
			}
			t=0;
		}
	}
}

/*******************************
* 名    称：
* 功    能： 切换示教类型
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
* 名    称： screen10(6.2)
* 功    能： 单轴示教画面
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
							 AxisSingleRun(0,1,1);//停止驱动
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
					 display_sixangle(7);//全速运行时显示六个轴的角度
				}
				//SetButtonValue(10,G_BUTTON.CONTROL_ID,0);//按钮弹起来
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
		ErrCheck(10);//扫描是否有报警
	}
}

/************************************
* 名    称： screen22(6.3)
* 功    能： 基座标系直线与旋转运动
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
					 switch(G_BUTTON.CONTROL_ID)//6(J1-)-11(J6-)，12(J1+)-17(J6+)
					 {
						 case 6:
							   while(!USART_RX_STA)
							   {
								    if(BaseCoordSlineRun(G_BUTTON.BUTTON_STATUS,1))//X-
								    {
									    break;
								    }
							   }
							   BaseCoordSlineRun(0,0);//按钮松开处理
							   break;


						 case 7:
							  while(!USART_RX_STA)
							  {
									if(BaseCoordSlineRun(G_BUTTON.BUTTON_STATUS,3))//Y-
									{
										break;
									}
							  }
							  BaseCoordSlineRun(0,0);//按钮松开处理
							  break;

						 case 8:
							  while(!USART_RX_STA)
							  {
								   if(BaseCoordSlineRun(G_BUTTON.BUTTON_STATUS,5))//Z-
								   {
									   break;
								   }
							  }
							  BaseCoordSlineRun(0,0);//按钮松开处理
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
							  BaseCoordSlineRun(0,0);//按钮松开处理
							 break;

						 case 13:
							  while(!USART_RX_STA)
							  {
									if(BaseCoordSlineRun(G_BUTTON.BUTTON_STATUS,4))//Y+
									{
										break;
									}
							  }
							  BaseCoordSlineRun(0,0);//按钮松开处理
							  break;

						 case 14:
							  while(!USART_RX_STA)
							  {
									if(BaseCoordSlineRun(G_BUTTON.BUTTON_STATUS,6))//Z+
									{
										break;
									}
							  }
							  BaseCoordSlineRun(0,0);//按钮松开处理
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
					 display_sixangle(7);//全速运行时显示六个轴的角度
				}
				 SetButtonValue(22,G_BUTTON.CONTROL_ID,0);//按钮弹起来
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
* 名    称： screen23(6.4)
* 功    能： 末端坐标系座标系
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
				switch(G_BUTTON.CONTROL_ID)//6(J1-)-11(J6-)，12(J1+)-17(J6+)
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

//第七章：属性设置区域
/********************************************************************************
 * 7
 * 7.1
 * 7.2
 * 7.3
 * 7.4
*********************************************************************************/

/********************************************
* 名    称： screen2(7)
* 功    能： 主菜单2，判断是哪一个按钮按下,切换到对应页面程序
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
			if(G_BUTTON.CONTROL_TYPE == 11)//按钮类型，实现页面切换
			{
				switch(G_BUTTON.CONTROL_ID)//判断是哪一个按钮按下,切换到对应页面
				{
					case 1: SCREEN_SIGN=1; break;
					case 2: SCREEN_SIGN=3; break;
					case 3: SCREEN_SIGN=4; break;
					case 4: SCREEN_SIGN=5; break;
					case 5: screen8(); SCREEN_SIGN=0; break;	//P.位置管理(无用)
					case 6: screen11(); SCREEN_SIGN=0; break;	//旧的限位设置页面(无用)
					case 7: screen13(); SCREEN_SIGN=0; break;	//定时任务
				  default : origin_sound_alarm(); SCREEN_SIGN=0; break;
				}
				if(SCREEN_SIGN > 0) break;
			}

			if(G_BUTTON.CONTROL_TYPE == 13)//滑块类型，实现背光调节
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
功能:原点声音警告等的设定(7.1)
*/
static void origin_sound_alarm()
{
	USART_RX_STA = 0;

	switch(G_BUTTON.CONTROL_ID)
	{
		case 8: locked_origin(); break;//按下界面为一2.11
		case 9: return_home(); break;//同理2.12
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
功能:原点锁定选择界面(7.11)
*/
static void locked_origin()
{
	u8 tmp;
	tmp = ErrCheck(2);//进来之前看看有没有报警
	if(DisplayErrInfo(0x02))//有报警不能锁定原点
		return;

	SetScreen(15);//原点锁定界面
	G_BUTTON.SCREEN_ID = 15;
	G_BUTTON.CONTROL_ID = 0;

	while(1)
	{
		if(G_BUTTON.CONTROL_ID > 1)//有键按下
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
		}//之前有报警的就屏蔽掉
	}
	G_BUTTON.SCREEN_ID = 2;

}

/*********************************
功能:原点锁定(7.111)
参数:无
*/
static void sure_locked_origin()
{
	u8 tmp;
	u8 return_value = 0;
	u8 sign_password = 0;
	tmp = ErrCheck(2);//进来之前看看有没有报警
	USART_RX_STA = 0;
	SetTextValue(30,1,"Enter the password:");
	SetScreen(30);
	G_BUTTON.SCREEN_ID = 30;

	while(1)//等待输入密码并检验密码，执行锁定原点任务的功能模块
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_ID == 4)		//直接取消
			{	break;}

			if(G_BUTTON.CONTROL_ID == 3)		//确定键，核对密码，显示是否正确
			{
				sign_password = 1;				//设定为1，如果密码错误则置为0
				if((USART_RECEIVE_STRING[0]-'0') != 1) sign_password = 0;
				if((USART_RECEIVE_STRING[1]-'0') != 2) sign_password = 0;
				if((USART_RECEIVE_STRING[2]-'0') != 3) sign_password = 0;
				if((USART_RECEIVE_STRING[3]-'0') != 4) sign_password = 0;
				if((USART_RECEIVE_STRING[4]-'0') != 5) sign_password = 0;
				if((USART_RECEIVE_STRING[5]-'0') != 6) sign_password = 0;
				if(USART_RECEIVE_STRING[6] != 0) sign_password = 0;

				if(sign_password == 1)//密码正确
				{
					return_value = ClearOrigin();//调用锁定原点函，返回处理信息给sign_return
					if(return_value == 0)//锁定原点成功，页面显示原点锁定成功
					{
						G_BUTTON.SCREEN_ID = 16;
						SetScreen(16);
					}
					else/*原点锁定失败*/
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
				if(sign_password == 0)//密码错误
				{
					SetTextValue(30,2,"");
					DispalyProgEditInfo(7);//密码错误
				}
				break;//返回主界面
			}
			USART_RX_STA = 0;
		}
	}

	USART_RECEIVE_STRING[0] = 0;
	USART_RX_STA = 0;
	G_BUTTON.SCREEN_ID = 2;
}
/********************************
功能:返回原点(7.12)
//static return_home 修正为//2018/06/1
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
			if(G_BUTTON.CONTROL_ID == 4) //按下返回原点键并且速度参数为有效值
			{
				sure_return_home(speed);
				break;
			}

			if(G_BUTTON.CONTROL_ID == 3)//按下取消键，跳出函数返回主菜单界面二
			{	break;	}

			if(G_BUTTON.CONTROL_TYPE == 15)//获取界面输入的返回原点时的速度参数
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
功能:通过网络控制机械手返回原点(7.12)
函数： return_home_fornetwork()
 修正于//2018/06/1
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
*功能:无速度警告界面(7.121)
*/
static void noparameter(u8 screen_id)
{
	u8 tmp;
	tmp = ErrCheck(2);//进来之前看看有没有报警
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
功能:确认返回原点(7.122)
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
		 //xxx 动态显示当前角度
		if(ErrCheck(2))
		{
			break;//除了限位
		}

		if(G_BUTTON.CONTROL_ID == 2)
		{
			break;
		}
	}*/
}

/******************************
功能:声音设置(7.13)
*/
static void sound_settings()
{
	if(G_BUTTON.BUTTON_STATUS)
	{
		delay_1ms(10);
		SetTouchScreen(1);//关闭声音
		delay_1ms(10);
		SetButtonValue(2, 10, 1);
		SetButtonValue(44, 10, 1);
		ROBOT_PARAMETER.SOUND = 0;
		SaveRobotPara(ROBOT_PARAMETER);//保存机械手参数
	}

	if(!G_BUTTON.BUTTON_STATUS)
	{
		delay_1ms(10);
		SetTouchScreen(3);//打开声音
		delay_1ms(10);
		SetButtonValue(2, 10, 0);
		SetButtonValue(44, 10, 0);
		ROBOT_PARAMETER.SOUND = 1;
		SaveRobotPara(ROBOT_PARAMETER);//保存机械手参数
	}
}

/******************************
功能:解除报警(7.14)
*/
void clear_alarm(void)
{
	 ClearAlarm();
	 G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG=0;//清除伺服报警标志
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
功能:伺服开关(7.15)
参数：flag  0:关闭伺服	1：打开伺服
*/
void servo_motor(u8 flag)
{
	u8 return_value = 0;
	USART_RX_STA = 0;

	if(DisplayErrInfo(0x02))
	{
		SetButtonValue(2, 13, 0);//按钮设为 伺服关闭 状态
		SetButtonValue(44, 13, 0);
		return;
	}

	if(flag == 1)//打开伺服
	{
		SetScreen(37);
		return_value = ServoON(0);
		SetScreen(G_BUTTON.SCREEN_ID);
		//SetButtonValue(2, 13, 1);//这个已经放到了ServoON里面了
	    if(return_value > 0)
	    {
	    	display_open_servo_fail_inform(return_value);
	    	//开发者模式下，即使脉冲读取异常，也会打开伺服
	    	if(DEVELOPER_MODE)
	    	{
	    		SetScreen(44);
	    		delay_1ms(10);
	    		SetScreen(37);
	    		ServoON(1);	//强制打开伺服
	    		SetScreen(44);
	    	}
	    }
	}

	if(flag == 0)//关闭伺服
	{
		ServoOFF();
		//SetButtonValue(2, 13, 0);//这个已经放到了ServoOFF里面了
	}

	USART_RX_STA = 0;
}

/*****************************
功能:显示打开伺服失败信息(7.151)
*/
static void display_open_servo_fail_inform(u8 servo_inform)
{
	char str[60],i;
	USART_RX_STA = 0;
	SetScreen(20);//xxx 要显示第几轴脉冲数据异常

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
功能:格式化sd卡(7.16)
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

	tmp = ErrCheck(2);//进来之前看看有没有报警
	SetTextValue(30,1,"Enter the password!");
	SetScreen(30);
	G_BUTTON.SCREEN_ID = 30;
	USART_RX_STA = 0;
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.CONTROL_ID == 4)		//直接取消
			{	break;}

			if(G_BUTTON.CONTROL_ID == 3)		//确定键，核对密码，显示是否正确
			{
				sign_password = 1;
				if((USART_RECEIVE_STRING[0]-'0') != 1) sign_password = 0;
				if((USART_RECEIVE_STRING[1]-'0') != 2) sign_password = 0;
				if((USART_RECEIVE_STRING[2]-'0') != 3) sign_password = 0;
				if((USART_RECEIVE_STRING[3]-'0') != 4) sign_password = 0;
				if((USART_RECEIVE_STRING[4]-'0') != 5) sign_password = 0;
				if((USART_RECEIVE_STRING[5]-'0') != 6) sign_password = 0;
				if(USART_RECEIVE_STRING[6] != 0) sign_password = 0;

				if(sign_password == 1)//密码正确
				{
					SetScreen(37);
					err_num = FormatFlash();
					if(err_num < 100)
					{
						/*SetScreen(29);//显示是否格式化成功
						USART_RX_STA = 0;
						while(1)
						{
							if(USART_RX_STA&0x8000)
							{	break;}
						}
						*/
						DispalyProgEditInfo(5);//显示格式化成功
					}
					else
					{
						DispalyProgEditInfo(6);//显示格式化失败
					}

				}
				if(sign_password == 0)//密码错误
				{
					SetTextValue(30,2,"");
					DispalyProgEditInfo(7);//密码错误
				}
				USART_RECEIVE_STRING[0] = 0;
				break;//返回主界面
			}
			USART_RX_STA = 0;
		}

		if(ErrCheck(2)&(~tmp))
		{break;}
	}
	USART_RX_STA = 0;
}

/***********************
功能:选择开发模式或者用户模式
*/
static void developer_mode(void)
{
	u8 sign_password=0;
	char str[5];
	if(!G_BUTTON.BUTTON_STATUS)/*0表示弹起状态，1表示按下状态*/
	{
		SetScreen(2);
		SetButtonValue(2,16,0);
		G_BUTTON.SCREEN_ID = 2;
		DEVELOPER_MODE = 0;
		DeveloperMode_Off();
		SPEED_VALUE = 5;//关掉开发者模式后，示教速度恢复默认值
		RecoverSpeed();
		ltoa(SPEED_VALUE,str);

		SetSliderValue(10,5,SPEED_VALUE);//单轴虚拟键盘页面速度条更新
		SetTextValue(10,18,str);//速度条的值更新
		SetSliderValue(22,5,SPEED_VALUE);//直线插补虚拟键盘页面速度条更新
		SetTextValue(22,19,str);//速度条的值更新
		SetSliderValue(23,5,SPEED_VALUE);//末端坐标虚拟键盘页面速度条更新
		SetTextValue(23,18,str);//速度条的值更新
	}

	if(G_BUTTON.BUTTON_STATUS)
	{
		SetTextValue(30,1,"Enter the password!");
		SetScreen(30);//显示输入密码界面
		G_BUTTON.SCREEN_ID = 30;

		while(1)//输入密码操作界面
		{
			if(USART_RX_STA&0x8000)
			{
				if(G_BUTTON.CONTROL_ID == 4)		//直接取消
				{	break;}

				if(G_BUTTON.CONTROL_ID == 3)		//确定键，核对密码，显示是否正确
				{
					sign_password = 1;
					if((USART_RECEIVE_STRING[0]-'0') != 1) sign_password = 0;
					if((USART_RECEIVE_STRING[1]-'0') != 2) sign_password = 0;
					if((USART_RECEIVE_STRING[2]-'0') != 3) sign_password = 0;
					if((USART_RECEIVE_STRING[3]-'0') != 4) sign_password = 0;
					if((USART_RECEIVE_STRING[4]-'0') != 5) sign_password = 0;
					if((USART_RECEIVE_STRING[5]-'0') != 6) sign_password = 0;
					if(USART_RECEIVE_STRING[6] != 0) sign_password = 0;

					if(sign_password == 0)//密码错误
					{
						SetTextValue(30,2,"");
						DispalyProgEditInfo(7);//密码错误
					}
					USART_RECEIVE_STRING[0] = 0;
					break;
				}
				USART_RX_STA = 0;
			}
		}

		if(1 == sign_password)//密码正确
		{
			SetButtonValue(44,20,0);
			SetButtonValue(44,21,0);
			SetButtonValue(44,22,0);
			SetButtonValue(44,16,1);
			SetButtonValue(44,24,0);//修正于//2018/06/01
			DEVELOPER_MODE = 1;
			G_BUTTON.SCREEN_ID = 44;
			DeveloperMode_On();
			SetScreen(44);
		}
	}
}

/***********************
功能:选择报警模式或者屏蔽报警模式
*/
static void switch_alarm_signal_mode(void)
{
	if(1 == DEVELOPER_MODE)
	{
		if(!G_BUTTON.BUTTON_STATUS)/*0表示弹起状态，1表示按下状态*/
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
功能:选择限位模式或者不限位模式
*/
static void switch_limit_mode(void)
{
	if(1 == DEVELOPER_MODE)
	{
		if(!G_BUTTON.BUTTON_STATUS)/*0表示弹起状态，1表示按下状态*/
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
功能:选择示教的高速模式或者低速模式
*/
static void switch_teach_speed_mode(void)
{
	char str[5];
	if(1 == DEVELOPER_MODE)
	{
		if(!G_BUTTON.BUTTON_STATUS)/*0表示弹起状态，1表示按下状态*/
		{
			SetSpeedMode_Low();
			SPEED_VALUE = 5;//关掉开发者模式后，示教速度恢复默认值
			RecoverSpeed();
			ltoa(SPEED_VALUE,str);

			SetSliderValue(10,5,SPEED_VALUE);//单轴虚拟键盘页面速度条更新
			SetTextValue(10,18,str);//速度条的值更新
			SetSliderValue(22,5,SPEED_VALUE);//直线插补虚拟键盘页面速度条更新
			SetTextValue(22,19,str);//速度条的值更新
			SetSliderValue(23,5,SPEED_VALUE);//末端坐标虚拟键盘页面速度条更新
			SetTextValue(23,18,str);//速度条的值更新
		}

		if(G_BUTTON.BUTTON_STATUS)
		{
			SetSpeedMode_High();
		}
	}
}

//返回值:0->相等	1->不相等
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
	4,	//轴数目
	1,		//记录是否坏块	0->坏块	1->好块
	0,	//记录开机是否需要保存编码器零点偏移量 1->开机需要保存 0->不需要
	1,	//记录电机种类 1->三菱	2->三协
	0,	//1->第一次使用 0->已使用过
	1,	//声音
	1,	//保留
	FOUR_AXIS_C,		//系统代号
	8,		//电路板版本号
	0x0F,		//电机选择(bit0~bit7 分别对应1~8轴)
	5000,		//电机每分钟转速
	0x08,//001000B,		//电机旋转方向 (bit0~bit7 分别对应1~8轴)
	0x09,//001001	//决定单轴旋转时的方向
	20000,	//电机每圈对应的脉冲数
	{0,0,0,0,0,0,0,0},	//编码器零点偏移量
	{0,0,0,0,0,0,0,0},	//电机序列号
	0.1,		//插补精度(单位:mm)
	{175,230,165,16,0},	//机械手臂长之类的参数 一轴臂长，二轴臂长，螺杆长度，螺距
	{ 100, 130,0.01, 360,0,0,0,0},//正限位(单位:角度)0.01?用0的话，返回原点后会直接处于限位状态
	{-100,-130,-360,-360,0,0,0,0},	//负限位(单位是角度)
	{0,0,0,0,0,0,0,0},				//原点位置
	{80,80,12.890625,3,0,0,0,0}	//减速比 (165mm)/(16mm/r)*2(同步轮比例)=20.625
};

//H4440
const RobotParaStru ROBOT_PARA_H4440={
	"H4440",
	4,	//轴数目
	1,		//记录是否坏块	0->坏块	1->好块
	0,	//记录开机是否需要保存编码器零点偏移量 1->开机需要保存 0->不需要
	1,	//记录电机种类 1->三菱	2->三协
	0,	//1->第一次使用 0->已使用过
	1,	//声音
	1,	//保留
	FOUR_AXIS_B,		//系统代号
	8,		//电路板版本号
	0x0F,		//电机选择(bit0~bit7 分别对应1~8轴)
	5000,		//电机每分钟转速
	0x08,//001000B,		//电机旋转方向 (bit0~bit7 分别对应1~8轴)
	0x09,//001001	//决定单轴旋转时的方向
	20000,	//电机每圈对应的脉冲数
	{0,0,0,0,0,0,0,0},	//编码器零点偏移量
	{0,0,0,0,0,0,0,0},	//电机序列号
	0.1,		//插补精度(单位:mm)
	{175,230,165,16,0},	//机械手臂长之类的参数 一轴臂长，二轴臂长，螺杆长度，螺距
	{ 100, 130,0.01, 360,0,0,0,0},//正限位(单位:角度)0.01?用0的话，返回原点后会直接处于限位状态
	{-100,-130,-360,-360,0,0,0,0},	//负限位(单位是角度)
	{0,0,0,0,0,0,0,0},				//原点位置
	{120,100,20.625,80,0,0,0,0}	//减速比 (165mm)/(16mm/r)*2(同步轮比例)=20.625
};

//H4450
const RobotParaStru ROBOT_PARA_H4450={
	"H4450",
	4,	//轴数目
	1,		//记录是否坏块	0->坏块	1->好块
	0,	//记录开机是否需要保存编码器零点偏移量 1->开机需要保存 0->不需要
	1,	//记录电机种类 1->三菱	2->三协
	0,	//1->第一次使用 0->已使用过
	1,	//声音
	1,	//保留
	FOUR_AXIS_B,		//系统代号
	8,		//电路板版本号
	0x0F,		//电机选择(bit0~bit7 分别对应1~8轴)
	5000,		//电机每分钟转速
	0x08,//001000B,		//电机旋转方向 (bit0~bit7 分别对应1~8轴)
	0x09,//001001	//决定单轴旋转时的方向
	20000,	//电机每圈对应的脉冲数
	{0,0,0,0,0,0,0,0},	//编码器零点偏移量
	{0,0,0,0,0,0,0,0},	//电机序列号
	0.1,		//插补精度(单位:mm)
	{270,230,165,16,0},	//机械手臂长之类的参数
	{ 100, 130,0.01, 360,0,0,0,0},		//正限位(单位是角度)
	{-100,-130,-360,-360,0,0,0,0},	//负限位(单位是角度)
	{0,0,0,0,0,0,0,0},				//原点位置
	{120,100,20.625,80,0,0,0,0}	//减速比
};
//H4560
const RobotParaStru ROBOT_PARA_H4560={
	"H4560",
	4,	//轴数目
	1,		//记录是否坏块	0->坏块	1->好块
	0,	//记录开机是否需要保存编码器零点偏移量 1->开机需要保存 0->不需要
	1,	//记录电机种类 1->三菱	2->三协
	0,	//1->第一次使用 0->已使用过
	1,	//声音
	1,	//保留
	FOUR_AXIS_A,		//系统代号
	8,		//电路板版本号
	0x0F,		//电机选择(bit0~bit7 分别对应1~8轴)
	5000,		//电机每分钟转速
	0x08,//001000B,		//电机旋转方向 (bit0~bit7 分别对应1~8轴)
	0x09,//001001	//决定单轴旋转时的方向
	20000,	//电机每圈对应的脉冲数
	{0,0,0,0,0,0,0,0},	//编码器零点偏移量
	{0,0,0,0,0,0,0,0},	//电机序列号
	0.1,		//插补精度(单位:mm)
	{250,350,300,5,0},	//机械手臂长之类的参数
	{ 150, 140,0.01, 360,0,0,0,0},		//正限位(单位是角度)
	{-150,-140,-360,-360,0,0,0,0},	//负限位(单位是角度)
	{0,0,0,0,0,0,0,0},				//原点位置
	{160,120,75,80,0,0,0,0}	//减速比
};
//V5350
const RobotParaStru ROBOT_PARA_V5350={
	"V5350",
	6,	//轴数目
	1,		//记录是否坏块	0->坏块	1->好块
	0,	//记录开机是否需要保存编码器零点偏移量 1->开机需要保存 0->不需要
	1,	//记录电机种类 1->三菱	2->三协
	0,	//1->第一次使用 0->已使用过
	1,	//声音
	1,	//保留
	51,		//系统代号
	8,		//电路板版本号
	0x37,		//电机选择(bit0~bit7 分别对应1~8轴)
	3500,		//电机每分钟转速
	0x14,//010100B,		//电机旋转方向 (bit0~bit7 分别对应1~8轴)
	0x29,//101001	//决定单轴旋转时的方向
	20000,	//电机每圈对应的脉冲数
	{0,0,0,0,0,0,0,0},	//编码器零点偏移量
	{0,0,0,0,0,0,0,0},	//电机序列号
	0.1,		//插补精度(单位:mm)
	{253.0, 0.0, 0, 200.0, 0},	//机械手臂长之类的参数
	{165, 15,  5,   160, 90, 360, 0,0},		//正限位(单位是角度)
	{-165,-180,-190,-160,-90,-360,0,0},	//负限位(单位是角度)
	{0,-90.0,0,0,0,0,0,0},		//原点位置
	{160,240,137.5,80,80,80,0,0}	//减速比
};
//V6350
const RobotParaStru ROBOT_PARA_V6350={
	"V6350",
	6,	//轴数目
	1,		//记录是否坏块	0->坏块	1->好块
	0,	//记录开机是否需要保存编码器零点偏移量 1->开机需要保存 0->不需要
	1,	//记录电机种类 1->三菱	2->三协
	0,	//1->第一次使用 0->已使用过
	1,	//声音
	1,	//保留
	61,		//系统代号
	8,		//电路板版本号
	0x3F,		//电机选择(bit0~bit7 分别对应1~8轴)
	3500,		//电机每分钟转速
	0x14,//010100B,		//电机旋转方向 (bit0~bit7 分别对应1~8轴)
	0x29,//101001	//决定单轴旋转时的方向
	20000,	//电机每圈对应的脉冲数
	{0,0,0,0,0,0,0,0},	//编码器零点偏移量
	{0,0,0,0,0,0,0,0},	//电机序列号
	0.1,		//插补精度(单位:mm)
	{253.0,0.0,98,213.5,0},	//机械手臂长之类的参数
	{165,15,81,160,90,360,0,0},		//正限位(单位是角度)
	{-165,-180,-85,-160,-90,-360,0,0},	//负限位(单位是角度)
	{0,-90.0,0,0,0,0,0,0},				//原点位置
	{160,240,137.5,80,80,80,0,0}	//减速比
};
//SV6350
const RobotParaStru ROBOT_PARA_SV6350={
	"SV6350",
	6,	//轴数目
	1,		//记录是否坏块	0->坏块	1->好块
	0,	//记录开机是否需要保存编码器零点偏移量 1->开机需要保存 0->不需要
	2,	//记录电机种类 1->三菱	2->三协
	0,	//1->第一次使用 0->已使用过
	1,	//声音
	1,	//保留
	61,		//系统代号
	8,		//电路板版本号
	0x3F,		//电机选择(bit0~bit7 分别对应1~8轴)
	4000,		//电机每分钟转速
	0x14,//010100B,		//电机旋转方向 (bit0~bit7 分别对应1~8轴)
	0x29,//101001	//决定单轴旋转时的方向
	20000,	//电机每圈对应的脉冲数
	{0,0,0,0,0,0,0,0},	//编码器零点偏移量
	{0,0,0,0,0,0,0,0},	//电机序列号
	0.1,		//插补精度(单位:mm)
	{253.0,0.0,98,213.5,0},	//机械手臂长之类的参数
	{165,15,81,160,90,360,0,0},		//软正限位(单位是角度)
	{-165,-180,-85,-160,-90,-360,0,0},	//软负限位(单位是角度)
	{0,-90.0,0,0,0,0,0,0},				//原点位置
	{160,240,137.5,80,80,80,0,0}	//减速比
};
//GV6250
const RobotParaStru ROBOT_PARA_GV6250={
	"GV6250",
	6,	//轴数目
	1,		//记录是否坏块	0->坏块	1->好块
	0,	//记录开机是否需要保存编码器零点偏移量 1->开机需要保存 0->不需要
	2,	//记录电机种类 1->三菱	2->三协
	0,	//1->第一次使用 0->已使用过
	1,	//声音
	1,	//保留
	61,		//系统代号
	8,		//电路板版本号
	0x3F,		//电机选择(bit0~bit7 分别对应1~8轴)
	6000,		//电机每分钟转速
	0x14,//010100B,		//电机旋转方向 (bit0~bit7 分别对应1~8轴)
	0x29,//101001	//决定单轴旋转时的方向
	20000,	//电机每圈对应的脉冲数
	{0,0,0,0,0,0,0,0},	//编码器零点偏移量
	{0,0,0,0,0,0,0,0},	//电机序列号
	0.1,		//插补精度(单位:mm)
	{253.0,0.0,98,213.5,0},	//机械手臂长之类的参数
	{165,15,81,160,90,360,0,0},		//正限位(单位是角度)
	{-165,-180,-85,-160,-90,-360,0,0},	//负限位(单位是角度)
	{0,-90.0,0,0,0,0,0,0},				//原点位置
	{160,120,100,80,80,80,0,0}	//减速比
};

void display_robot_para(RobotParaStru robot_para)
{
	char i,str[17];
	//显示参数
	ltoa(robot_para.HARDWARE_REV,str);
	SetTextValue(46,29,str);	//系统

	ltoa(robot_para.SYS,str);
	SetTextValue(46,1,str);		//系统

	ltoa(robot_para.MOTOR_TYPE,str);
	SetTextValue(46,2,str);		//电机种类

	ltoa(robot_para.MOTOR_PRECISION,str);
	SetTextValue(46,3,str);		//电机精度

	ltoa(robot_para.MOTOR_SPEED,str);
	SetTextValue(46,4,str);		//电机最大转速

	My_DoubleToString(robot_para.PRECISION,str,11,3,1);
	SetTextValue(46,5,str);		//插补精度

	ltoa(robot_para.AXIS_NUM,str);
	SetTextValue(47,80,str);	//轴数目

	SetTextValue(47,75,robot_para.NAME);

	SetTextValue(47,3,(char*)ROBOT_PARA_H4440.NAME);
	SetTextValue(47,4,(char*)ROBOT_PARA_H4450.NAME);
	SetTextValue(47,5,(char*)ROBOT_PARA_H4560.NAME);
	SetTextValue(47,6,(char*)ROBOT_PARA_V5350.NAME);
	SetTextValue(47,7,(char*)ROBOT_PARA_V6350.NAME);
	SetTextValue(47,8,(char*)ROBOT_PARA_SV6350.NAME);
	SetTextValue(47,9,(char*)ROBOT_PARA_GV6250.NAME);
	SetTextValue(47,10,(char*)ROBOT_PARA_GH4440.NAME);

	//轴选择
	for(i=0;i<8;i++)
	{
		str[8-i-1] = ((robot_para.MOTOR_SELECT>>i) & 0x0001) + '0';
	}
	str[i] = 0;
	SetTextValue(47,77,str);
	//轴转向
	for(i=0;i<8;i++)
	{
		str[8-i-1] = ((robot_para.AXIS_ROTATE.all>>i) & 0x0001) + '0';
	}
	str[i] = 0;
	SetTextValue(47,78,str);
	//电机转向
	for(i=0;i<8;i++)
	{
		str[8-i-1] = ((robot_para.MOTOR_DIR.all>>i) & 0x0001) + '0';
	}
	str[i] = 0;
	SetTextValue(47,79,str);

	//机械手参数
	for(i=0;i<5;i++)
	{
		My_DoubleToString(robot_para.ROBOT_PARA[i],str,11,3,1);
		SetTextValue(46,6+i,str);
	}

	//减速比
	for(i=0;i<6;i++)
	{
		sprintf(str,"%11.6f",robot_para.RATIO[i]);
		//My_DoubleToString(robot_para.RATIO[i],str,12,6,1);
		SetTextValue(46,11+i,str);
	}

	//正限位
	for(i=0;i<6;i++)
	{
		My_DoubleToString(robot_para.LIMITED_P[i],str,9,3,0);
		SetTextValue(46,21+i,str);
	}

	//负限位
	for(i=0;i<6;i++)
	{
		My_DoubleToString(robot_para.LIMITED_M[i],str,9,3,0);
		SetTextValue(46,31+i,str);
	}

	//原点位置
	for(i=0;i<6;i++)
	{
		My_DoubleToString(robot_para.ORIGIN_POSITION[i],str,9,3,0);
		SetTextValue(47,81+i,str);
	}
}
/***********************
功能:机械手参数设置
返回值：NONE
*/
void setting_robot_para(void)
{
	char i,tmp,err,len,str[25];
	double f_value,f_tmp;
	RobotParaStru robot_para;
	RobotParaStru robot_para_tmp;

	G_BUTTON.SCREEN_ID = 46;
	SetScreen(46);	//参数设置页面
	ReadRobotPara(&robot_para);	//读机械手参数
	if(robot_para.HARDWARE_REV == 0)
		robot_para.HARDWARE_REV = 8;	//如果是之前老版本的，这个值默认是0，现在改为默认都是8

	if(robot_para.FIRST == 1)
	{
		robot_para_tmp = ROBOT_PARA_V6350;	//未设置时，将会默认赋值V6350的参数
	}
	else
	{
		robot_para_tmp = robot_para;
	}
	display_robot_para(robot_para_tmp);//显示参数
//写参数
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			f_value = atof((char*)USART_RECEIVE_STRING);
			GetControlValue(G_BUTTON.SCREEN_ID,G_BUTTON.CONTROL_ID);
			USART_RX_STA = 0;
			i=0;
			//连续两次读取到的数据相同为止，最多6次
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
			//机械手参数
			case 6: robot_para_tmp.ROBOT_PARA[0] = f_value;break;
			case 7: robot_para_tmp.ROBOT_PARA[1] = f_value;break;
			case 8: robot_para_tmp.ROBOT_PARA[2] = f_value;break;
			case 9: robot_para_tmp.ROBOT_PARA[3] = f_value;break;
			case 10: robot_para_tmp.ROBOT_PARA[4] = f_value;break;
			//减速比
			case 11: robot_para_tmp.RATIO[0] = f_value;break;
			case 12: robot_para_tmp.RATIO[1] = f_value;break;
			case 13: robot_para_tmp.RATIO[2] = f_value;break;
			case 14: robot_para_tmp.RATIO[3] = f_value;break;
			case 15: robot_para_tmp.RATIO[4] = f_value;break;
			case 16: robot_para_tmp.RATIO[5] = f_value;break;
			case 17: robot_para_tmp.RATIO[6] = f_value;break;
			case 18: robot_para_tmp.RATIO[7] = f_value;break;
			//正限位
			case 21: robot_para_tmp.LIMITED_P[0] = f_value;break;
			case 22: robot_para_tmp.LIMITED_P[1] = f_value;break;
			case 23: robot_para_tmp.LIMITED_P[2] = f_value;break;
			case 24: robot_para_tmp.LIMITED_P[3] = f_value;break;
			case 25: robot_para_tmp.LIMITED_P[4] = f_value;break;
			case 26: robot_para_tmp.LIMITED_P[5] = f_value;break;
			case 27: robot_para_tmp.LIMITED_P[6] = f_value;break;
			case 28: robot_para_tmp.LIMITED_P[7] = f_value;break;
			//负限位
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
			case 77://轴选择
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
			case 78://轴转向
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
			case 79://电机转向
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
			//原点位置
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
			case 93: SetScreen(46);G_BUTTON.SCREEN_ID=46;break;	//上一页
			case 94: SetScreen(47);G_BUTTON.SCREEN_ID=47;break;	//下一页
			case 91:	//保存
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
			case 92:	//退出
				//参数发生修改，并且未保存
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
									SetScreen(44);	//开发者模式页面
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
									SetScreen(44);	//开发者模式页面
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
					SetScreen(44);	//开发者模式页面
					G_BUTTON.SCREEN_ID = 44;
					return;
				}

			default:	break;
			}
			USART_RX_STA = 0;
			G_BUTTON.CONTROL_ID = 0;
		}
	}

	//SetScreen(44);	//2018/05/22注释掉//开发者模式页面
	//G_BUTTON.SCREEN_ID = 44;//2018/05/22注释掉
}
/***********************
功能:选择开环示教模式与闭环运行模式
*/
void switch_mode(void)
{
    if(G_BUTTON.BUTTON_STATUS==1)
    {
    	Teach_mode=1;//修正于//2018/06/01;

    }
    else
    {
    	Teach_mode=0;//修正于//2018/06/01;

    }
}
/***********************
功能:
*/
void teach_speed(void)
{
	if(!G_BUTTON.BUTTON_STATUS)//弹起，即默认状态
	{

	}

	if(G_BUTTON.BUTTON_STATUS)//按下状态
	{

	}
}
/************************************
* 名    称： screen8(7.2)	//P.位置页面
* 功    能： 判断是哪一个按钮按下,切换到对应页面程序
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
* 名    称： screen11(7.3)旧的限位设置页面
* 功    能： 判断是哪一个按钮按下,切换到对应页面程序
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
////					SetTextValue(11,29,"已开限位");
////					EnableLimited();//打开限位
////				}
////
////				if(G_BUTTON.BUTTON_STATUS)
////				{
////					SetTextValue(11,29,"已关限位");
////					DisableLimited();//关闭限位
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
* 名    称： screen13(7.4)定时任务
* 功    能： 判断是哪一个按钮按下,切换到对应页面程序
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
			if(G_BUTTON.CONTROL_ID == 1)//返回上一级
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
 * 函数名：    array_to_date  (7.41)
 * 函数功能: 将时间转换成数字
 * 输入参数: date,时分和年月日(ASCII码)
 * 输出参数: 转换好的整形数据
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
 * 函数名：    reset_text  (7.42)
 * 函数功能: 清空开关机时间
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
 * 函数名：    start_text  (7.43)
 * 函数功能: 定时执行选中的程序
 * 输入参数: clocks和minutes，开始和结束的时间
*/
static void start_text(u8 clocks[2],u8 minutes[2])//开始执行任务
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

	if(return_value == 1)//check_working_tine()检测当前时间，判断是否执行任务
	{
		run_intime(clocks, minutes, program_name);//正在执行  xxx
	}

    SetRTCMode(0x00, 0x01, 0x06,0xffff,559,101);
	SetScreen(13);
	delay_1ms(50);//延时处理
}

/******************************************************
 * 函数名：    start_text (7.431)
 * 函数功能: 检测当前时间
 * 输入参数: clocks和minutes，开始和结束的时间
     	   check_touch:是否检测键盘中断
 * 输出参数: 0表示未到执行时间，1表示到了执行时间，2表示人为退出或者输入的任务时间有误
*/
static u8 check_real_time(u8 clocks[2], u8 minutes[2])
{
	u16 real_time = 0;//北京时间
	u16 time0 = clocks[0]*60 + minutes[0];//开机时间
	u16 time1 = clocks[1]*60 + minutes[1];//关机时间
	delay_1ms(10);

	if((time0<1440) && (time1<1440))//如果任务计划的时间在正常范围内的话，继续执行，否则返回设定任务计划界面
	{
		if(USART_RX_STA&0x8000)
		{
			if(prompt_dialog_box(27,28) == 1)//选择是否退出当前任务
			{
				 OPEN_PROGRAM_SIGN = 0;
				 return 2;
			}
			USART_RX_STA = 0;
		}

		ReadTime();//发送读取时间指令 xxx
		while(1)
		{
			if(USART_RX_STA&0x8000)//等待触摸屏上传时间
			{
				real_time = USART_RECEIVE_STRING[0]*60 + USART_RECEIVE_STRING[1];//把时间转化成分钟表示
				USART_RX_STA = 0;
				break;
			}
		}
		//开始判断当前时间是否进入了任务时间
		if(time1 > time0)//当关机时间比开机时间大的时候
		{
			if( ((real_time==time0)||(real_time>time0)) && (real_time<time1) )
			{	return 1;}
		}
		if(time1 < time0)//当关机时间比开机时间小的时候
		{
			if( ((real_time==time0) || (real_time>time0)) //大于或者等于开机时间并且小于或者等于24点整
			  && ((real_time==1440) || (real_time<1440)))
			{	return 1;}
			if( ((real_time==0)||(real_time>0)) && (real_time<time1) )//大于或者等于0点整并且小于或者等于关机时间
			{	return 1;}
		}
		if(time1 == time0)//开关机时间相同，不执行任务
		{
			 OPEN_PROGRAM_SIGN = 0;
			 return 2;
		}
		return 0;
	}

	OPEN_PROGRAM_SIGN = 0;
	return 2;//输入时间有误
}

/******************************************************
* 函数名：    display_runing_test_screen  (7.432)
* 函数功能: 定时执行选中的程序
* 输入参数: clocks和minutes，开始和结束的时间
*/
static void display_runing_test_screen(u8 clocks[2],u8 minutes[2])//开始执行任务
{
	u8 str[3] = {0, 0, 0};
	SetScreen(28);
	G_BUTTON.SCREEN_ID = 28;
    u32 t=0;
	if(OPEN_PROGRAM_SIGN)
	{
		GetControlValue(24, 2*OPEN_PROGRAM_SIGN);//发送读取文本控件内容指令
		while(1)//等待接收触摸屏反馈的文本控件信息
		{
			if(USART_RX_STA&0x8000)//接收到触摸屏上传信息并处理完毕
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
 * 函数名：    run_intime  (7.433)
 * 函数功能: 定时执行选中的程序
 * 输入参数: clocks和minutes，开始和结束的时间
*/
static void run_intime(u8 clocks[2],u8 minutes[2],u8 program_name[USART_REC_LEN])
{
	 unsigned char num,return_value=0;
	 unsigned int i=0,n;
	 SetRTCMode(0x01, 0x01, 0x06,0xf800,559,101);

	 num = GetLinesCurrentProgram();	//获得程序行数
	 G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 0;//不使用安全开关
	 for(; i<num; )
	 {
	     if(ProgramStepRun(&i,0))	//执行第i行程序
	     {
	    	 break;
	     }
	     if( i >= (num-1) )
	     { i=0;}		//到尽头，重新开始

	     return_value = check_real_time(clocks, minutes);//判断任务计划是否完成
	     if( return_value != 1 )
	     {
	    	 if(OPEN_PROGRAM_SIGN)
			 {
	    		 return_value = OpenProgram(program_name,&n);//把要读取的程序的名字发送给底层驱动,把程序读入内存。
	    		 if(return_value == 0)
	    		 {
					 num = GetLinesCurrentProgram();	//获得程序行数
					 for(i=0; i<num; )
					 {
						 if(ProgramStepRun(&i,0))//执行第i行程序
						 {
							 break;
						 }
					 }
	    		 }
			 }
	    	 break;
	     }
	 }
	 G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 1;//使用安全开关
	 OPEN_PROGRAM_SIGN = 0;
}


//第8章：程序编辑调试管理区
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
* 名    称： screen4
* 功    能： 主菜单4，判断是哪一个按钮按下,切换到对应页面程序
*/
static void screen4()
{
	unsigned long key_value;
	SetScreen(4);//判断触摸屏页面与程序页面是否一致
	G_BUTTON.SCREEN_ID = 4;

	while(4)
	{
		if(USART_RX_STA&0x8000)
		{
			switch(G_BUTTON.CONTROL_ID)//判断是哪一个按钮按下,切换到对应页面
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
* 名    称： screen7-(8)
* 功    能： 编辑程序界面
*/
static u8  page_num=1;
static void screen7()
{
	unsigned long key_value;
	u8 sign_edit = 0;
	u8 open_program_sign = 0;
	char str[4] = {0};
	static unsigned long s_key_value=0;
	static u8 text_num=1, text_num0=1;//当前光标所在的文本标号与当前显示的页数
	static u16 net_comcounter=0,program_select=0;//修正于//2018/06/01(定时上报下位机状态以及接收上位机启动命令)
    static u8 warn_data=0,program_check=0,*check_value=0;

	text_num0 = text_num;//编辑中text_num保存最新的光标位置所在文本控件编号，text_num0则保存前个编号
	SetScreen(7);//判断触摸屏页面与程序页面是否一致
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
	net_comcounter=0;//修正于//2018/06/01(定时上报下位机状态以及接收上位机启动命令)
	SetScreen(7);
	SetTextValue(7,96,"     OK!");
	while(1)//循环扫描，其它页面同理
	{
		if(USART_RX_STA&0x8000)//判断是否有键按下，其它页面同理
		{
			G_BUTTON.SCREEN_ID = 7;

			if(switch_screen(text_num, &sign_edit))	{ break;}//切换页面函(7.1)

			update_program(&text_num0,&text_num, &page_num, &sign_edit);//保存更新的程序文本控件内容(7.2)

			return_keyboard_value(&text_num,&text_num0, &page_num, &sign_edit);//处理上传的键盘值(7.3)

			touch_edit(&text_num0,&text_num,&sign_edit);//触摸屏坐标有效时切换光标(7.4)

			dispose_button(&text_num0,&text_num,&sign_edit,&page_num, &open_program_sign);//处理按钮上传值(7.6)

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
		net_comcounter=net_comcounter+1;//修正于//2018/06/01

		if(net_comcounter==200)//修正于//2018/06/01
		{
			 SetScreen(7);
			 SetTextValue(7,96,"    NET!");
			 delay_1ms(100);//添加次延时看到字符串的变化，否则只能看到一个字符
			warn_data=Cycle_Query_Uppermachine(&program_select,&program_check);//修正于//2018/06/01
			if(warn_data==2)
			{
				G_BUTTON.CONTROL_TYPE = 11;
				G_BUTTON.CONTROL_ID = 0x63;
				net_control=0x88;
				dispose_button(&text_num0,&text_num,&sign_edit,&page_num, &open_program_sign);//处理按钮上传值(7.6)
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
			net_comcounter=0;//修正于//2018/06/01
			 SetScreen(7);
			 SetTextValue(7,96,"     OK!");

		}

	}
	save_last_edit_program(text_num, page_num, &sign_edit);
}

/*****************************************
 * 函数功能：上/下翻十页
 * 输入参数：sign: 0:表示上翻10页，1:表示下翻10页
 * 			 2:表示上翻1页，3:表示下翻1页
 * 输出参数：NONE
 */
static void display_ten_page(unsigned long sign)
{
	char str[4]={0};
	if(sign==0x00000400)//右键
	{
		page_num += 10;
		if(page_num>50)
		{
			page_num = 50;
		}
	}

	else if(sign==0x00000200)//左键
	{
		if(page_num>10)
			page_num -= 10;
		else
			page_num = 1;
	}

	else if(sign==0x00000100)//下键
	{
		page_num += 1;
		if(page_num>50)
		{
			page_num = 50;
		}
	}

	else if(sign==0x00000800)//上键
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
功能:跳转到错误代码所在处-(8.1)
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
功能:切换界面-(8.1)
*/
static u8 switch_screen(u8 text_num,u8 *sign_edit)
{
	if( G_BUTTON.CONTROL_TYPE == 11 )//按下的为切换页面的按钮
	{
		switch(G_BUTTON.CONTROL_ID)
		{
			case 1 : SCREEN_SIGN=1; break;//按下界面为一
			case 2 : SCREEN_SIGN=10;
					  if(1 == TEACH_MODEL)		{SCREEN_SIGN = 14; }
					  break;//同理
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
功能:保存更新的程序文本控件内容-(8.2)
参数:
***text_num0：光标所在的文本控件的前一个编号
***sign_edit：文本控件是否有编辑的标志位，1为有编辑过，0为没有编辑过
***page_num ：记录当前的页码数
*/
static void update_program(u8 *text_num0, u8 *text_num, u8 *page_num, u8 *sign_edit)
{
	if(G_BUTTON.CONTROL_TYPE == 15)//接收到的为文本控件内容时
	{
		if( (G_BUTTON.screencoordinate_x>705) && (G_BUTTON.screencoordinate_x<787)//按下了enter键后返回文本控件的内容
		 	 && (G_BUTTON.screencoordinate_y>484) && (G_BUTTON.screencoordinate_y<525) )
		{
			GetCodeStrings( ((*text_num)+((*page_num)-1)*15-1), USART_RECEIVE_STRING );//写入内存中保存
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
功能:处理上传的键盘值-(8.3)
参数:
***text_num ：光标所在的文本控件的最新编号
***sign_edit：文本控件是否有编辑的标志位，1为有编辑过，0为没有编辑过
***page_num ：记录当前的页码数
*/
static void return_keyboard_value(u8 *text_num, u8 *text_num0, u8 *page_num, u8 *sign_edit)
{
	if( (G_BUTTON.screencoordinate_x>250) && (G_BUTTON.screencoordinate_x<790)//如果按在编辑区域
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
				*sign_edit=1;//标志位置为1，说明程序被编辑
				if(MEMORY_TO_FLASH_SIGN == 1)
				{	MEMORY_TO_FLASH_SIGN = 2;}
			}
			G_BUTTON.screencoordinate_x = 0;
			G_BUTTON.screencoordinate_y = 0;
		}
	}
	if(G_BUTTON.CONTROL_TYPE == 12)//按下的为按键方式(即键盘方式)
	{
		switch(G_BUTTON.KEY_VALUE)
		{
			case 0x0d: press_enter(text_num, page_num, text_num0);
					   *sign_edit=0;
					   break;//按下按键为回车键()

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
功能:编辑程序时按下enter键的处理(插入一行)-(8.31)
参数:
***text_num ：光标所在的文本控件的最新编号
***page_num ：记录当前的页码数
*/
static void press_enter(u8 *text_num, u8 * page_num, u8 *text_num0)//按下enter键的处理
{
	char str[5] = {0};
	u8 sign_page_change = 0;
	unsigned int row;
	row = ( (*text_num) + 15*((*page_num) - 1 ));//1~750
	if(row < ROWS_IN_PROGRAM)
	{
		if((*text_num) == 15)//需要翻页
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

		if( ((*text_num)<15) && (sign_page_change==0) )//不需要翻页
		{
			(*text_num0) = (*text_num);
			IncreaseEmptyRow( row);//按下回车，在下一行增加一行空白行
			(*text_num) += 1;
			SetCursor(7, (*text_num)+2, 1);
		}

		sign_page_change = 0;
		refresh(*page_num, 7);	//整体刷新
		ltoa(*text_num, str);
		SetTextValue(7, 71, str);
	}
}

/**************************************************************
功能:编辑程序时按下删除键的处理（删除一行）-(8.32)
参数:
***text_num ：光标所在的文本控件的最新编号
***page_num ：记录当前的页码数
*/
static void delete_line(u8 *text_num, u8 *page_num)//删除一行内容，后面的补上去
{
	if( (*text_num) > 0 )
	{
		DeleteRow( (*text_num) + 15*((*page_num)-1) - 1 );//删除一行的内容
		refresh(*page_num, 7);//整体刷新
		(*text_num) += 2;
		SetCursor(7, (*text_num), 1);
		(*text_num) -= 2;
	}
}

/**************************************************************
功能:处理上传的键盘值-(8.4)
参数:
****text_num0：
****text_num:
***sign_edit：
*/
static u8 touch_edit(u8 *text_num0, u8 *text_num, u8 *sign_edit)
{
	u8 str[3] = {0};
	if( (G_BUTTON.screencoordinate_x>0) && (G_BUTTON.screencoordinate_x<240) )//点击位置为文本控间编辑处
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
功能:处理上传的键盘值-(8.5)
参数:
***text_num0：光标所在的文本控件的前一个编号
***text_num ：光标所在的文本控件的最新编号
***sign_edit：文本控件是否有编辑的标志位，1为有编辑过，0为没有编辑过
***page_num ：记录当前的页码数
*/
static void dispose_button(u8 *text_num0, u8 *text_num, u8 *sign_edit, u8 *page_num, u8 *open_program_sign)
{
	u8 control_id = 0;

	if( ((*sign_edit)==1) || (*open_program_sign==1) )	//如果程序有被编辑过，那么编译程序成功的标志位置为1，也就是说下次运行程序的前要先编译程序
	{
		build_program_sign = 0;
		*open_program_sign = 0;
	}

	if(G_BUTTON.CONTROL_TYPE == 11)//按下为开关模式
	{
		control_id = G_BUTTON.CONTROL_ID;
		if(57 == control_id)
		{
			delete_line(text_num, page_num);//删除一行，后面的补上
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
			case 73: screen7_1(page_num); break;//上翻页
			case 74: screen7_2(page_num); break;//下翻页
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
				SetButtonValue(6,2,0);//小威添加（明天调试）
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
			case 101: save_position(*text_num, *page_num); break;//保存位置信息7.65
			default : break;
		}

		SetCursor(7, (*text_num)+2, 1);
		G_BUTTON.CONTROL_ID = 0;
		G_BUTTON.CONTROL_TYPE = 0;
	}
}

/**************************************************************
功能:返回到第一页
*/
static void display_first_page(void)
{
	 refresh(1, 7);
	 page_num = 1;
}
/**************************************************************
功能:是否保存最后的编辑内容-(8.51)
*/
static void save_last_edit_program(u8 text_num,u8 page_num,u8 *sign_edit)
{
	 USART_RX_STA = 0;

	 if( (*sign_edit) == 1 )
	 {
		 while(prompt_dialog_box(32, 7))
		 {
			 GetCodeStrings( (text_num+(page_num-1)*15-1), USART_RECEIVE_STRING );//把最后编辑的程序内容写入内存中
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
功能:实现翻页显示功能,上翻页-(8.52)
参数:
***pages ：记录当前的页码数
*/
static void screen7_1(u8 * pages)
{
	u8 str[4] = {0,0,0,0};
	if( *pages > 1 )
	{
		(*pages)--;
		ltoa((*pages), (char*)str);
		SetTextValue(7, 72, (char*)str);

		refresh(*pages, 7);//整体刷新
	}
}

/**************************************************************
功能:实现翻页显示功能,下翻页-(8.53)
参数:
***pages ：记录当前的页码数
*/
static void screen7_2(u8 * pages)
{
	u8 str[4] = {0,0,0,0};
	if( *pages < (int)(ROWS_IN_PROGRAM/15) )//如果ROWS_IN_PROGRAM不是15的倍数令做处理
	{
		(*pages)++;
		ltoa((*pages), (char*)str);
		SetTextValue(7, 72, (char*)str);

		refresh(*pages, 7);//整体刷新
	}
}

/**************************************************************
功能:新建一个应用程-(8.54)
参数:
***text_num0：光标所在的文本控件的前一个编号
***text_num ：光标所在的文本控件的最新编号
***sign_edit：文本控件是否有编辑的标志位，1为有编辑过，0为没有编辑过
***page_num ：记录当前的页码数
*/
static void new_program(u8 *text_num0,u8 *text_num,u8 *page_num)//新建程序
{
	char str[5] = {0};
	unsigned char tmp,err,i;
	u8 program_number=0;//程序编号
	u8 tempory_value_for_error=ERROR_LINE[5];

	tmp = ErrCheck(2);//进来之前看看有没有报警
	G_BUTTON.CONTROL_TYPE = 0;//结构体清零
	G_BUTTON.BUTTON_STATUS = 0;
	G_BUTTON.KEY_VALUE = 0;
	G_BUTTON.CONTROL_ID = 0;
	USART_RX_STA = 0;		//标志位清零
	SetScreen(31);
	G_BUTTON.SCREEN_ID = 31;
	ERROR_LINE[5]=0;//程序错误信息清零
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
						SetTextValue(7, 83, (char*)USART_RECEIVE_STRING);//清空程序名字显示文本
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
				ERROR_LINE[5]=tempory_value_for_error;//程序错误信息复原
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
功能:编译程序-(8.55)
参数:
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

	lines = GetLinesCurrentProgram();//获得当前程序的总行数
	SetScreen(37);//开始显示编译动画
	return_value = BuildProgram(5, err, error_line);
	delay_1ms((lines+40));//显示动画的时间与行数lines成正比
	delay_1ms(40);
	G_BUTTON.SCREEN_ID = 7;
	SetScreen(7);//结束显示编译动画
	delay_1ms(10);
	SetScreen(7);
	delay_1ms(5);
	for(i=0; i<6; i++)
	{
		SetTextValue(7, 85+2*i, " ");//先清空
		ERROR_LINE[i] = 0;//清空编译错误信息
	}
	if(return_value == 0)//编译成功
	{
		TIMER0_CNT = 0;	//每次Build过程序之后，运行时间重新开始计算
		SetTextValue(7, 85, "Build program successfully!");
		return 1;
	}
	else//编译失败
	{
		 if(return_value == 0xff)
		 {
			  SetTextValue(7, 85, "Please open or new!");
			  return 0;
		 }

		 ERROR_LINE[5] = 1;
		 error_numbers[10] = return_value/10 + '0';
		 error_numbers[11] = return_value%10 + '0';
		 SetTextValue(7, 85, error_numbers);//显示错误个数

		 if(return_value > 5){	return_value = 5;}//最多显示个错误信息
		 for(i=0; i<return_value; i++)//显示编译失败信息
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
功能:根据编译错误编号转换成相关的编译失败的信息-(8.551)
参数:
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
功能:保存应用程序-(8.56)
参数:
***text_num0：光标所在的文本控件的前一个编号
***text_num ：光标所在的文本控件的最新编号
***sign_edit：文本控件是否有编辑的标志位，1为有编辑过，0为没有编辑过
***page_num ：记录当前的页码数
*/
static void save_program()//保存程序
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
功能:单步调试-(8.57)
参数:
***text_num0：光标所在的文本控件的前一个编号
***text_num ：光标所在的文本控件的最新编号
***sign_edit：文本控件是否有编辑的标志位，1为有编辑过，0为没有编辑过
***page_num ：记录当前的页码数
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

	 err = ReadErrInfo();// & (~0x40);//读当前错误/报警信息,屏蔽掉安全开关
	 lines = GetLinesCurrentProgram();//获取当前程序总行数
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

	 row = (*text_num)+((*page_num)-1)*15-1;//row从0开始的
	 page_tmp = *page_num;//记录当前页码

	 ClearStopProgramFlag();//清除暂停运行标志位
	 RecoverSpeed();//恢复到示教速度
	 prg_err = ProgramStepRun(&row,1);

		  if(prg_err < ERR_CMD)//判断执行是否成功
		  {
			   *text_num = row%15+1;//text_num从1行开始
			   *page_num = row/15+1;//page_num从1页开始
			   if(page_tmp != *page_num) //判断前后的页码是否一致，不一致则刷新
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
				   	 SetTextValue(7, 85, (char*)RUN_SUCCEED);//刷新
				   	 if(prg_err == code_n_x || prg_err == code_disp ||
				   			 prg_err == code_f_x )
				   	 {
				   		//如果是执行了n变量，则不刷新，因为n变量的显示占用了
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
功能:全速运行函数-(8.58)
参数:
***text_num0：光标所在的文本控件的前一个编号
***text_num ：光标所在的文本控件的最新编号
***sign_edit：文本控件是否有编辑的标志位，1为有编辑过，0为没有编辑过
***page_num ：记录当前的页码数
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

			err = ReadErrInfo() & (~0x40);//读当前错误/报警信息，屏蔽掉安全开关
		    //G_BUTTON.SCREEN_ID=6;//页面标号设为6
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
				 SetScreen(6);//跳转到第六界面
				 G_BUTTON.SCREEN_ID = 6;
				 SetButtonValue(6, 18, 1);
				 refresh((*page_num), 6);//刷新显示程序

				 ovrd = GetOvrd();
				 ltoa(ovrd,str);
				 SetTextValue(6, 19, str);//更新显示ovrd

				 SetTextValue(6, 83, s_program_name_num.program_name) ;//显示程序名

				  prog_row = (*text_num)+((*page_num)-1)*15-1;//程序运行的行
				 //执行下面的函数后，prog_row的值将被修改，其意义为下一次运行的行
				 err = ProgramRunWithPointer(display_current_runing_program_position, &prog_row);

				 ErrCheck(7);
				 delay_1ms(850);//延时，等待其他显示完

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

				 SetScreen(7);//退出全速执行，返回第七界面
				 delay_1ms(450);
				 SetScreen(7);//退出全速执行，返回第七界面
				 G_BUTTON.SCREEN_ID = 7;//页面标号设为7
				 (*text_num) = (prog_row+1)%15;//更新所在文本控件的标号
				 (*page_num) = (prog_row)/15+1;//更新所在页面的标号
				 (*text_num0) = (*text_num);
				 delay_1ms(20);
				 ltoa(s_program_name_num.num,str);
				 SetTextValue(7, 82, str);
				 SetTextValue(7, 83, s_program_name_num.program_name) ;//显示程序名
				 refresh((*page_num), 7);//刷新显示程序
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
	net_mode=0x00;//函数进入标志字清零
	G_BUTTON.CONTROL_ID = 0;
	USART_RX_STA = 0;
}

/**************************************************************
 * 函数功能：单步运行指示-(8.581)
 * 输入参数：text_num0：光标所在的文本控件的前一个编号
 * 		   text_num ：光标所在的文本控件的最新编号
 */
static void display_current_runing_program_position(u8 text_num)
{
	char str[3] = {0};
	static u8 s_page = 0;
	u8 page_tmp = 0;

	page_tmp = (text_num)/15+1;//当前页码
	if(page_tmp != s_page)//当前页码跟上次的页码不一样就刷新
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
	//黄色
	SetFcolor(0xece0);
	GUI_RectangleFill(0, 70+(text_num)*31, 15, 90+(text_num)*31);
}

/**************************************************************
 * 函数功能：显示六个轴的角度-(8.582)
 * 输入参数：screen_id,在哪个页面显示，6或7
 */

void display_sixangle(u8 screen_id)
{
	u8 i=0;
	char string[58],angle_str[10];//xxx.xxx mm\t X: xxxx.xxx\t mm
	long hour;
	long min;
	double angle[6],radian[6],r;
	PositionGesture p_g;

	GetCurrentRadian(radian);//获得每轴当前弧度
	 for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
	 {
		  *(angle+i) = *(radian+i)*PI_MULTI_180_INVERSE;//弧度转角度
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
		//四舍五入，保留3位小数
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
		//screen7(程序编辑界面)
		My_DoubleToString(angle[0],string,9,3,0);//浮点型转换为字符串，小数点后3位
		My_DoubleToString(p_g.px,angle_str,10,3,0);
		strcat(string,"  X:");//复制到string
		strcat(string,angle_str);//复制到string
		SetTextValue(screen_id, 85, string);

		My_DoubleToString(angle[1],string,9,3,0);//浮点型转换为字符串，小数点后3位
		My_DoubleToString(p_g.py,angle_str,10,3,0);
		strcat(string,"  Y:");//复制到string
		strcat(string,angle_str);//复制到string
		SetTextValue(screen_id, 87, string);

		My_DoubleToString(angle[2],string,9,3,0);//浮点型转换为字符串，小数点后3位
		My_DoubleToString(p_g.pz,angle_str,10,3,0);
		strcat(string,"  Z:");//复制到string
		strcat(string,angle_str);//复制到string
		SetTextValue(screen_id, 89, string);

		My_DoubleToString(angle[3],string,9,3,0);//浮点型转换为字符串，小数点后3位
		if(ROBOT_PARAMETER.AXIS_NUM == 4)
		{
			r = (radian[0]+radian[1]+radian[3])*PI_MULTI_180_INVERSE;
			My_DoubleToString(r,angle_str,10,3,0);
			strcat(string,"  R:");//复制到string
			strcat(string,angle_str);//复制到string
			SetTextValue(screen_id, 93, "");
			SetTextValue(screen_id, 95, "");
		}
		SetTextValue(screen_id, 91, string);

		if(ROBOT_PARAMETER.AXIS_NUM==6 || ROBOT_PARAMETER.AXIS_NUM==5)
		{
			My_DoubleToString(angle[4],string,9,3,0);//浮点型转换为字符串，小数点后3位
			SetTextValue(screen_id, 93, string);
		}

		if(ROBOT_PARAMETER.AXIS_NUM==6 || ROBOT_PARAMETER.AXIS_NUM==5)
		{
			My_DoubleToString(angle[5],string,9,3,0);//浮点型转换为字符串，小数点后3位
			SetTextValue(screen_id, 95, string);
		}
	}
	else
	{
		//screen6(全速运行界面)
		if(ROBOT_PARAMETER.AXIS_NUM == 4)
		{
			for(i=0;i<ROBOT_PARAMETER.AXIS_NUM;i++)
			{
				My_DoubleToString(angle[i],string,9,3,0);//浮点型转换为字符串，小数点后3位
				SetTextValue(screen_id, i*2+85, string);
			}
		}
		else
		{
			for(i=0;i<6;i++)
			{
				My_DoubleToString(angle[i],string,9,3,0);//浮点型转换为字符串，小数点后3位
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
功能:保存位置信息-(8.59)
参数:
***text_num0：光标所在的文本控件的前一个编号
***text_num ：光标所在的文本控件的最新编号
***sign_edit：文本控件是否有编辑的标志位，1为有编辑过，0为没有编辑过
***page_num ：记录当前的页码数
*/
static void save_position(u8 text_num, u8 page_num)
{
	 u8 return_value = 0;
	 char str[4]={0};
	 u16 position_num = 0;
	 u32 t=0;
	 USART_RX_STA = 0;

	 return_value = GetPositionNumInRow((text_num+(page_num-1)*15-1),&position_num);//判断保存哪一点

	 ltoa(position_num, str);
	 SetTextValue(45, 6, str);
	 if(0 == return_value)//此位置没有坐标信息
	 {
		 SetTextValue(45, 2, "Sure to save this position?");
	 }
	 if(0x91 == return_value)//此位置已有坐标信息，提示是否覆盖保存
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
				  GetCodeStrings( (text_num+(page_num-1)*15-1), USART_RECEIVE_STRING );//把最后一次编辑的信息保存到内存中
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
功能:提示框-(8.6)
*/
static u8 prompt_dialog_box(u8 screen_id1, u8 screen_id2)
{
	u8 tmp;
	tmp = ErrCheck(2);//进来之前看看有没有报警
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
			if(G_BUTTON.CONTROL_TYPE == 15)//切换界面时，如果非主机用户输入的文本控件被编辑过的话会自动上传文本值
			{
				G_BUTTON.CONTROL_TYPE = 20;
				G_BUTTON.CONTROL_ID = 0;
				USART_RX_STA = 0;
			}
			if(G_BUTTON.CONTROL_ID > 0)//按下按钮，选择是否执行操作
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
功能:刷新程序显示界面-(8.7)
参数:
***page_num :记录当前的页码数
***screen_id:需要刷新内容的页面
*/
static void refresh(u8 page_num, u8 screen_id)//读取程序内容并刷新显示页面
{
	u8 i = 3;
	u16 line_num = 0;

	line_num = 15*(page_num-1) + line_num;
	delay_1ms(2);

	for(; i<18; i++)
	{
 		GetRowCode(line_num++, USART_RECEIVE_STRING);//读取一行的内容
 		SetTextValue(screen_id, i, (char*)USART_RECEIVE_STRING);//驱动显示屏显示
 		//delay_1ms(1);
	}
}


//第9章：程序编辑调试管理区
/********************************************************************************
 * 9
 * 9.1
 * 9.2
 * 9.3
 * 9.4
 * 9.5
*********************************************************************************/
/*******************************************
* 名    称： program_manage-(9)
* 功    能： 程序管理界面（打开，删除，复制，粘贴等）
* 参    数：last_screen,表示进入此页面前的页面的编号
*/
static void programs_manage(u8 last_screen)
{
	unsigned long key_value;
	static u8 text_num = 100;//当前光标所在的文本标号，初始值为100，即程序数量不会到达的值
	u8 sign_program_num = 0;


	SetScreen(24);//判断触摸屏页面与程序页面是否一致
	G_BUTTON.SCREEN_ID = 24;
	refresh_display_program_name(0);

	while(1)//循环扫描，其它页面同理
	{
		if(USART_RX_STA&0x8000)//判断是否有键按下，其它页面同理
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
* 名    称： programs_net_manage-(9)
* 功    能： 网络通信控制选择以及编译程序
* 参    数：last_screen,表示进入此页面前的页面的编号
* 参    数：text_num，表示所选择的程序
*/
void programs_net_manage(u8 last_screen,u8 text_num,u8* check_value)//通过网络进行程序管理
{
	u8 sign_program_num=1,page_num=1,text_line=1;
	u8 text_line0=1,sign_edit=0,open_program_sign=0;
	u8 program_select=0,program_page=0,i=0;
	SetScreen(24);//判断触摸屏页面与程序页面是否一致
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
	G_BUTTON.screencoordinate_y=program_select*30+89;//计算出所选择程序的位置并用红色方框标记
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
 * 函数功能：导出程序到U盘
 * 输入参数：
*/
static void export_program_to_udisk(u8 text_num , u8 *sign_program_num)
{
	u8 sign_return=0,err;
	u32 t=0;
	if(1 == *sign_program_num)
	{
		err = save_program_to_flash();//是否需要保存程序
		if(err == 1 || err == 2)//保存失败
		{
			return;
		}
		USART_RX_STA = 0;
		*sign_program_num = 0;
		G_BUTTON.CONTROL_ID = 0;

		GetControlValue(24, 2*(text_num+1));//发送读取文本控件内容指令
		while(1)//等待接收触摸屏反馈的文本控件信息
		{
			if(USART_RX_STA&0x8000)//接收到触摸屏上传信息并处理完毕
			{
				SetScreen(37);//进度条
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
 * 函数功能：判断导出是否成功
*/
static void export_judge(u8 sign_return)
{
	u8 i=0;
	char USART_RECEIVE_STRING0[USART_REC_LEN] = {0};
	G_BUTTON.SCREEN_ID = 34;

//**************第1部份*****************//程序导出成功

	if(sign_return == 0)
	{
		SetTextValue(34,2,(char*)EXPORT_SUCCESSFUL);
		SetScreen(34);
		delay_1ms(1000);
		return;
	}

//**************第2部份*****************//程序同名情况的处理

	if(sign_return == 3)
	{
		USART_RX_STA = 0;
		for(i=0;i<USART_REC_LEN;i++)//2.1保存当前程序名字
		{
			USART_RECEIVE_STRING0[i] = USART_RECEIVE_STRING[i];
		}

		//2.2选择覆盖保存或者重命名
		G_BUTTON.SCREEN_ID = 42;
		SetTextValue(42,5,"Name repeat!YES overwrite,NO rename!");
		SetScreen(42);//弹出选择覆盖或者重命名的窗口
		while(1)//等待用户选择覆盖保存或者重命名
		{
			if(USART_RX_STA&0x8000)
			{
				if(3==G_BUTTON.CONTROL_ID)//覆盖保存
				{
					SetScreen(37);//进度条
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
				else if(4==G_BUTTON.CONTROL_ID)//重命名
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

		//2.3重命名功能实现部分
		SetTextValue(35,5,(char*)USART_RECEIVE_STRING);
		SetScreen(35);
		G_BUTTON.SCREEN_ID = 35;
		G_BUTTON.CONTROL_ID = 0;
		while(1)//等待重命名
		{
			if(USART_RX_STA&0x8000)
			{
				if(3==G_BUTTON.CONTROL_ID)//确定重命名
				{
					SetScreen(37);//进度条
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
				if(4==G_BUTTON.CONTROL_ID){return;}//取消重命名
				USART_RX_STA = 0;
				G_BUTTON.CONTROL_ID = 0;
			}
			ErrCheck(35);
		}
	}

//**************第3部份*****************//程序导出出错情况处理

	DisplayUdiskError(sign_return);
	SetScreen(34);
	delay_1ms(1000);
}

/******************************************************
 * 函数功能：提示是否先保存内存中的程序
 * 返回值：0->保存成功
 * 		   1->保存失败
 * 		   2->保存失败
 * 		   0xff->选择了NO
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
 * 函数功能：判断U盘连接情况并显示U盘程序
*/
static void display_udisk_program()
{
	u8 i=0,j=0,err;
	u8 sign_return=0;
	long prg_num=0;//U盘里程序的数量
	char str[3]={0};
	char USART_RECEIVE_STRING0[USART_REC_LEN] = {0};

	sign_return = GetExStorageState(&prg_num);
	if(sign_return == 0) //U盘可使用
	{
		err = save_program_to_flash();//是否需要保存程序
		if(err == 1 || err == 2)//保存失败
		{
			return;
		}
		G_BUTTON.SCREEN_ID=41;
		SetScreen(41);
		if(prg_num>=11)//判断要显示的程序个数
			j=11;
		else
			j=prg_num;

		for(i=0;i<11;i++)
		{
			ltoa(i+1, str);
			SetTextValue(41,2*(i+1),str);
		}
		for(i=0;i<j;i++)//显示首页的程序
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
		for(i=j;i<11;i++)//显示首页的程序
		{
			SetTextValue(41,2*(i+1)+1,"--------");
			delay_1ms(2);
		}
		manage_udisk_program();
		G_BUTTON.SCREEN_ID = 24;
		SetTextValue(32, 1, "save last program ? ") ;
		return;
	}

	if(sign_return == 1) //1->U盘未连接
	{
		SetTextValue(34,2,"External storage disconnected!");
	}
	if(sign_return == 5) //5->DSP与STM32通信出错
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
 * 函数功能：对U盘程序进程的操作：翻页，导入等
*/
static void manage_udisk_program()
{
	u32 i=0;
	u8 sign_return=0;
	u8 text_num=0;
	u8 sign_program_num=0;
	long pro_num=0;
	u8 page_num = 1;//当前页数


	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			if(G_BUTTON.screencoordinate_x > 0)//显示选中的程序
			{
				SetFcolor(0xffff);
				GUI_Rectangle(270, 119+(text_num)*30, 570, 149+(text_num)*30);
				text_num = (G_BUTTON.screencoordinate_y-119) / 30;

				SetFcolor(63488);
				GUI_Rectangle(270, 119+(text_num)*30, 570, 149+(text_num)*30);
				G_BUTTON.screencoordinate_x = 0;
				sign_program_num = 1;
			}

			if( (G_BUTTON.CONTROL_ID>23) && (G_BUTTON.CONTROL_ID<26) )//上下翻页
			{
				change_page(pro_num,&page_num);
			}

			if( (26==G_BUTTON.CONTROL_ID) && (1==sign_program_num) )//导入程序
			{
				import_program(&sign_program_num, text_num, page_num);
				G_BUTTON.SCREEN_ID = 41;
			}

			if(G_BUTTON.CONTROL_ID == 27)//退出U盘界面
			{break;}

			USART_RX_STA = 0;
			G_BUTTON.CONTROL_ID = 0;
		}

		if(100 == i++)//定时检测U盘的连接状态
		{
			sign_return = GetExStorageState(&pro_num);
			if(0 != sign_return)
				break;
			i=0;
		}
		ErrCheck(G_BUTTON.SCREEN_ID);
	}

	SetScreen(24);//返回到程序管理界面
	refresh_display_program_name(0);
}

/******************************************************
 * 函数功能：导入程序
*/
static void import_program(u8 *sign_program_num, u8 text_num ,u8 page_num)
{
	u8 read_num;
	u8 sign_return=0;
	u16 t=0;
	G_BUTTON.SCREEN_ID=37;
	USART_RX_STA = 0;

	SetScreen(37);
	GetControlValue(41, 2*((text_num)+1)+1);//发送读取文本控件内容指令
	while(1)//等待接收触摸屏反馈的文本控件信息
	{
		if(USART_RX_STA&0x8000)//接收到触摸屏上传信息并处理完毕
		{
			read_num = 11*(page_num-1) + text_num;
			sign_return = ImportProgram(read_num,(char*)USART_RECEIVE_STRING,0);

			SetScreen(34);
			if(sign_return == 0)
			{
				SetTextValue(34,2,(char*)IMPORT_SUCCESSFUL);
			}
			else if(sign_return == 3)//名字重复处理程序
			{
				SetTextValue(42,5,"Name repeat,YES overwrite，NO rename.");
				SetScreen(42);//弹出选择覆盖或者重命名的窗口
				G_BUTTON.SCREEN_ID = 42;
				USART_RX_STA = 0;
				while(1)//等待用户选择覆盖保存或者重命名
				{
					if(USART_RX_STA&0x8000)
					{
						if(3==G_BUTTON.CONTROL_ID)//覆盖保存
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
						else if(4==G_BUTTON.CONTROL_ID)//重命名
						{
							USART_RX_STA = 0;
							break;
						}
						else if(2==G_BUTTON.CONTROL_ID)//cancel
						{
							SetScreen(41);//U盘界面
							USART_RX_STA = 0;
							return;
						}
						USART_RX_STA = 0;
					}
					ErrCheck(42);
				}

				//2.3重命名功能实现部分
				SetTextValue(35,5,(char*)USART_RECEIVE_STRING);
				SetScreen(35);
				G_BUTTON.SCREEN_ID = 35;
				G_BUTTON.CONTROL_ID = 0;
				while(1)//等待重命名
				{
					if(USART_RX_STA&0x8000)
					{
						if(3==G_BUTTON.CONTROL_ID)//确定重命名
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
						if(4==G_BUTTON.CONTROL_ID){	SetScreen(41);	return;}//取消重命名
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
 * 函数功能：上下翻页
*/
static void change_page(long pro_num,u8 *page_num)
{
	u8 i=0,j=0;
	u8 sign_return=0;
	char str[3]={0};
	char USART_RECEIVE_STRING0[USART_REC_LEN]={0};
	static u8 change_sign=0;//翻页有效标志位

	if( (*page_num>1) && (G_BUTTON.CONTROL_ID == 24) )//上翻页
	{
		(*page_num)--;
		change_sign = 1;
	}
	if( ((pro_num+1)>(*page_num)*11) && (G_BUTTON.CONTROL_ID == 25) )//下翻页
	{
		(*page_num)++;
		change_sign = 1;
	}

	if( (pro_num>0) && (1==change_sign) )
	{
		change_sign = 0;
		if(pro_num>=(*page_num)*11)//判断在当前页面需要显示的程序个数
			j=11;
		else
			j=pro_num-11*((*page_num)-1);

		for(i=0;i<11;i++)//显示当前页面的程序编号
		{
			ltoa( (*page_num-1)*11+i+1, str );
			SetTextValue(41,2*(i+1),str);
		}
		for(i=0;i<j;i++)//显示当前页面的程序
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
		for(i=j;i<11;i++)//当前页面没有程序的编号文本显示为横杠
		{
			SetTextValue(41,2*(i+1)+1,"--------");
		}
	}
	else//翻页无效提醒
	{
//		SetScreen(34);
//		SetTextValue(34,2,"已到尽头！");
//		delay_1ms(1000);
//		SetScreen(41);
	}
	G_BUTTON.CONTROL_ID = 0;
}

/******************************************************
 * 函数功能：退出程序管理界面-(9.1)
 * 输入参数： text_num,选中的程序的编号
 * 	       last_screen,进入程序管理界面前的那个页面的编号
*/
static u8 exit_programs_manage_screen(u8 text_num, u8 last_screen, u8 sign_program_num)
{
	unsigned char err;
	if(G_BUTTON.CONTROL_TYPE == 11)//按下的为切换页面的按钮
	{
		switch(G_BUTTON.CONTROL_ID)
		{
			case 21:
					//text_num == 100,是在删除程序的时候，被赋值为100的，
					//初始值也为100，所以程序数量不能超100，除非这个值修改变大
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
							if(err == 1 || err == 2)//保存失败
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
 * 函数功能：给当前选中的程序加上红色框图并更新标志位-(9.2)
 * 输入参数： text_num,选中的程序的编号
 * 	       sign_program_num,标志位，如果当前有操作选中程序，则置为1，在其他操作时清零
*/
static void display_objective_program(u8 *text_num, u8 *sign_program_num)
{
	if(G_BUTTON.screencoordinate_x > 0)//显示选择的程序
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
 * 函数功能：刷新程序名显示界面-(9.3)
 * 输入参数： text_num,选中的程序的编号
 * 	       sign_program_num,标志位，如果当前有操作选中程序，则置为1，在其他操作时清零
*/
static void copy_delete_rename_page(u8 *text_num, u8 *sign_program_num)
{
	if(G_BUTTON.CONTROL_TYPE == 11)//程序管理（复制粘贴，删除等）
	{
		switch(G_BUTTON.CONTROL_ID)
		{
			case 23: copy_or_rename_program(sign_program_num, text_num, CopyProgram); break;//复制程序
			case 24: copy_or_rename_program(sign_program_num, text_num, RenameProgram); break;//程序重命名
			case 25: display_previouspage_program_name();break;//上翻页
			case 26: display_nextpage_program_name();break;//下翻页
			case 27: delete_program(text_num, sign_program_num);break;//删除程序
			default:break;
		}
	}
}

/******************************************************
 * 函数功能：复制程序-(9.31)
 * 输入参数：text_num,选中的程序的编号
 * 	       sign_program_num,标志位，如果当前有操作选中程序，则置为1，在其他操作时清零
 * 	       f 函数指针
*/
static void copy_or_rename_program(u8 *sign_program_num, u8 *text_num, u8 f(u8 *old_name, u8 *new_name))
{
	u8 i = 0,err;
	u8 sign_return = 0;
	u8 program_name[USART_REC_LEN] = {0};
	Uint32 t=0;
	if(*sign_program_num == 1)//必须先选中程序
	{
		err = save_program_to_flash();
		if(err == 1 || err == 2)//保存失败
		{
			return;
		}
		G_BUTTON.SCREEN_ID = 35;
		while(1)
		{
			 if(USART_RX_STA&0x8000)//接收到触摸屏上传信息并处理完毕
			 {
				USART_RX_STA = 0;
				break;
			 }
			 if(t++ == 5000000) break;
		}

		GetControlValue(24, 2*((*text_num)+1));//发送读取文本控件内容指令
	    while(1)//等待接收触摸屏反馈的文本控件信息
	    {
		    if(USART_RX_STA&0x8000)//接收到触摸屏上传信息并处理完毕
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
		SetScreen(35);//弹出改名字的框
		G_BUTTON.SCREEN_ID=35;

	    USART_RX_STA=0;
	    G_BUTTON.CONTROL_ID = 0;
	    USART_RECEIVE_STRING[0] = 0;

		if(prompt_dialog_box(0, 24))
		{
			 G_BUTTON.SCREEN_ID = 24;
			if(USART_RECEIVE_STRING[0] > 0)
			{
				SetScreen(37);//进度条
				err = f(program_name, USART_RECEIVE_STRING);
				if( err == 0 )
				{
					//如果被重命名的程序是正在打开的，则更新显示正在打开的程序
					if(f == RenameProgram)//判断函数指针是否为RenameProgram
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
						refresh_display_program_name(4);//刷新当前页
					}
					else refresh_display_program_name(3);//刷新最后一页
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
 * 函数功能：刷新程序名显示界面-(9.33)
 * 输入参数：pages,需要刷新界面的方式，上翻页，下翻页，粘贴删除等。0表示显示第一页，1下翻页，2表示上翻页
*/
static void display_nextpage_program_name()
{
	refresh_display_program_name(1);

}

/******************************************************
 * 函数功能：刷新程序名显示界面-(9.34)
 * 输入参数：pages,需要刷新界面的方式，上翻页，下翻页，粘贴删除等。0表示显示第一页，1下翻页，2表示上翻页
*/
static void display_previouspage_program_name()
{
	refresh_display_program_name(2);
}

/******************************************************
 * 函数功能：刷新程序名显示界面-(9.35)
 * 输入参数：pages,需要刷新界面的方式，上翻页，下翻页，粘贴删除等。0表示显示第一页，1下翻页，2表示上翻页
 * 	      sign_program_num：表示选中的程序的编号
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
		   GetControlValue( 24, 2*((*text_num)+1) );//发送读取文本控件内容指令
		   while(1)//等待接收触摸屏反馈的文本控件信息
		   {
			   if(USART_RX_STA&0x8000)//接收到触摸屏上传信息并处理完毕
			   {
				   //如果被删除的程序正在打开状态
				   if(!StringCmp(PROGRAM_NAME_LENGTH,USART_RECEIVE_STRING,(unsigned char*)s_program_name_num.program_name))
				   {
			    		SetTextValue(43,2,"The program is opened,cannot be operated！");
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
					   refresh_display_program_name(3);//刷新显示最后页码的内容
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
 * 函数功能：刷新程序名显示界面-(9.4)
 * 输入参数：pages,需要刷新界面的方式。0表示显示第一页，
 * 									   1下翻页，
 * 									   2表示上翻页，
 * 									   3表示最后一页
 * 									   others:刷新当前页
 */
static void refresh_display_program_name(u8 page_sign)
{
	u8 i=0, j=0;
	u8 program_num = 0;
	char str[3] = {0};
	static u8 page_num = 1;//当前页数

	program_num = GetProgramNum();
	if(program_num == 0)//没有程序
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
		{ page_num -= 1;}	//上翻页
		if( (page_sign==1) && (page_num < ((program_num-1)/10+1)) )
		{ page_num += 1;}	//下翻页

		if( (program_num-(page_num-1)*10) < 10 )//判断当前页数需要显示的程序数量
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
		USART_RECEIVE_STRING[0] = 0;//清零
    }
}

/******************************************************
 * 函数功能：打开选中的程序-(9.5)
 * 输入参数： text_num,光标当前所在文本控件的编号
 * 	       text_num0,光标上一次所在文本控件的编号
 * 	       page_num,显示程序的页面的页码数
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
		GetControlValue(24, 2*OPEN_PROGRAM_SIGN);//发送读取文本控件内容指令
		while(1)//等待接收触摸屏反馈的文本控件信息
		{
			if(USART_RX_STA&0x8000)//接收到触摸屏上传信息并处理完毕
			{
				return_value = OpenProgram(USART_RECEIVE_STRING,&num);//把要读取的程序的名字发送给底层驱动,把程序读入内存。
				if(return_value == 0)
				{
					ltoa(OPEN_PROGRAM_SIGN, str);
					s_program_name_num.num = OPEN_PROGRAM_SIGN;
					for(i=0;i<PROGRAM_NAME_LENGTH;i++)
					{
						s_program_name_num.program_name[i] = USART_RECEIVE_STRING[i];
						if(USART_RECEIVE_STRING[i]==0x00) break;
						Software_name_check = Software_name_check +USART_RECEIVE_STRING[i];//修正于//2018/06/01
					}
					SetTextValue(7, 82, str);
					SetTextValue(7, 83, (char*)USART_RECEIVE_STRING);
					refresh(1, 7);//刷新程序编辑界面
					*page_num = 1;//页码数置为1
					*text_num = 1;
					*text_num0 = 1;
					*sign_edit = 0;
					USART_RX_STA = 0;//串口标志位清零，等待下一次串口信息。
					MEMORY_TO_FLASH_SIGN = 1;//打开程序后，就没有程序需要保存，故赋值为1
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
		ERROR_LINE[5]=0;//程序错误信息清零
		DISPLAY_ERROR_LINE=0;
		OPEN_PROGRAM_SIGN = 0;
		G_BUTTON.CONTROL_TYPE = 0;

	}
}

/******************************************************
 * 函数功能：打开选中的程序-(9.5)
 * 输入参数： err	错误代号
 * 			  1：保存出错，请再保存
 * 			  2：无此程序
 * 			  3：名字不能重复
 * 			  4：程序名需在19个字符以内
 * 			  5: 格式化成功
 * 			  6: 格式化失败
 * 			  7：密码错误!
 * 			  0xff：程序已满，不能再新建
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

//第10章：程序编辑调试管理区
/********************************************************************************
 * 10.1
 * 10.2
*********************************************************************************/

/*******************************************
* 名    称： screen3-(10.1)
* 功    能： 主菜单3，判断是哪一个按钮按下,切换到对应页面程序
*/
static void screen3()
{
	unsigned long key_value;
	char stm32_version[8] = "ERROR";
	u8 err;
	SetScreen(3);//判断触摸屏页面与程序页面是否一致
	G_BUTTON.SCREEN_ID = 3;

	//当前的机械手型号
	SetTextValue(3,14,ROBOT_PARAMETER.NAME);

	//stm32 程序版本
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

	//dsp程序版本
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
				switch(G_BUTTON.CONTROL_ID)//判断是哪一个按钮按下,跳出函数，切换到对应页面
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
* 名    称： screen5-(10.2)
* 功    能： 主菜单5，判断是哪一个按钮按下,切换到对应页面程序
*/
static void screen5()
{
	unsigned long key_value;
	SetScreen(5);//判断触摸屏页面与程序页面是否一致
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
 * 函数功能：显示当前画面
 * 输入参数：NONE
 * 输出参数：NONE
 */
void SetCurrentScreen(void)
{
	char str[6]= "SVO";
	unsigned char i,ovrd;
	unsigned int IOState;
	SetScreen(G_BUTTON.SCREEN_ID);//显示当前画面
	//恢复单轴/直线模式按钮
	TEACH_MODEL = GetModeState();
	SetModeState(TEACH_MODEL);

	if(G_CTRL_BUTTON.CTRL_BUTTON.ALARM_FLG)//如果当时有报警
	{
		SetTextValue(20, 4, str);
		SetTextValue(20, 3, "ALARM");
	}
/**** 恢复触摸屏数据 ****/

	//示教速度值恢复
	ltoa(SPEED_VALUE,str);
	SetSliderValue(10,5,SPEED_VALUE);
	SetTextValue(10,18,str);
	SetSliderValue(22,5,SPEED_VALUE);
	SetTextValue(22,19,str);
	SetSliderValue(23,5,SPEED_VALUE);
	SetTextValue(23,18,str);

	//输出IO图标恢复
	//if(G_BUTTON.SCREEN_ID == 12)
	{
		IOState = ReadPoutState();		// 读取IO状态
		for(i=0;i<8;i++)
		{
			if(IOState & (0x01<<i))
			{
				SetButtonValue(12,i+2,0);//IO关闭状态
			}
			else
			{
				SetButtonValue(12,i+2,1);//IO打开状态
			}
		}

		//刹车图标恢复
		IOState = ReadBrakeState();
		for(i=0;i<3;i++)
		{
			if(IOState & (0x01<<i))
			{
				SetButtonValue(12,i+15,0);//刹车关闭状态
			}
			else
			{
				SetButtonValue(12,i+15,1);//刹车打开状态
			}
		}
	}

	//恢复伺服开关图标
	if(G_CTRL_BUTTON.CTRL_BUTTON.SERVO_ON == 0)
	{
		SetButtonValue(2, 13, 1);//更新伺服开关按钮，设为打开状态
	}
	else
	{
		SetButtonValue(2, 13, 0);//更新伺服开关按钮，设为关闭状态
	}

	//恢复全速运行时，取消按钮的图标
	SetButtonValue(6, 18, 1);

	//恢复全速运行时，全局速度的值
	ovrd = GetOvrd();
	ltoa(ovrd,str);
	SetTextValue(6, 19, str);//更新显示ovrd

//	SetTextValue(6, 83, s_program_name_num.program_name);

	ltoa(s_program_name_num.num, str);
	//恢复程序编辑栏（页面7）
	if(G_BUTTON.SCREEN_ID != 6)//全速运行时不需要恢复，退出全速运行会刷新
	{
		//恢复程序名字和程序编号
		SetTextValue(7, 82, str);
		SetTextValue(7, 83, &s_program_name_num.program_name[0]);
		refresh(page_num,7);
	}
	else
	{
		if(ProgramRuningFlag(0) == 1)
		{
			SetButtonValue(6,2,1);//如果是暂停状态则将按钮恢复为“继续运行”
		}
		//恢复程序名字
		SetTextValue(6, 83, &s_program_name_num.program_name[0]);
		//恢复程序编辑栏（页面6，全速运行页面）
		refresh(page_num,6);
	}

	//恢复程序管理页面
	if(G_BUTTON.SCREEN_ID == 24)
		refresh_display_program_name(0);

	keyboard_speed_meter(3);//速度仪表盘如果是打开状态就关闭

	//恢复报警开关图标

	//恢复限位开关图标


}



//===========================================================================
// No more.
//===========================================================================
