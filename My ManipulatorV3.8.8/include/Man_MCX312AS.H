#ifndef MAN_MCX312AS_H_
#define MAN_MCX312AS_H_

//******************************mcx314 常用变量名称和数值***********************************/

#define Q_LENGTH        1024                   //BP插补中循环队列的长度
#define Q_PROT          14                     //在循环队列中开始和结尾中保留数据的长度
#define BPHEADER        0x0F0F                 //BP插补中数据文件的头指针
#define INIT_SPEED      SDLIST.Init_Speed      //供参考的初始速度，全局变量
#define ACCELERATION    SDLIST.Deceleration    //供参考的初始加速度，全局变量
#define ACC_RATE        SDLIST.Accelerated_Rate//供参考的初始减加速度，全局变量
#define SHIFT_PULSE     SDLIST.Shift_Pulse;    //供参考的加速度变化率，全局变量
#define HW_COUNTER      0x8                    //首轮正向脉冲计数，利用U轴反馈计时器
#define SPINDLE_COUNTER 0x4                    //首轮负向脉冲计数，利用Z轴反馈计时器
#define H_SEARCHING_L   2000                   //机械原点搜索距离，2mm


#endif
