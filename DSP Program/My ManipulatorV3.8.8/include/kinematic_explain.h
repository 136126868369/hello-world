/*
 *  版权声明： 广州新豪五金精密制品有限公司研发部BU28拥有所有最终解析权，未经批准，不得转载，违者必究
 *  文件名称：Touch_screen.h
 *  摘	 要：全局结构体变量声明
 *  	         机械手走特殊轨迹函数模块
 *  	         比较位姿的函数模块
 *  当前版本：
 *  作	 者：王文东
 *  创建日期：2015-1-10
 *  完成日期：2015-4-27
 */

#ifndef KINEMATIC_EXPLAIN_H_
#define KINEMATIC_EXPLAIN_H_
#include "My_Project.h"
//全局结构体变量声明
struct PositionGestureStru{
	 double r11;	//Nx
	 double r21;	//Ny
	 double r31;	//Nz
	 double r12;	//Ox
	 double r22;	//Oy
	 double r32;	//Oz
	 double r13;	//Ax
	 double r23;	//Ay
	 double r33;	//Az
	 double px;
	 double py;
	 double pz;
};
typedef struct PositionGestureStru PositionGesture;

struct PositionEulerStru{
	 double Px;		//
	 double Py;		//
	 double Pz;		//
	 double Rx;		//
	 double Ry;		//
	 double Rz;		//
};
typedef struct PositionEulerStru PositionEuler;

struct PositionStruct{
	double px;
	double py;
	double pz;
};
typedef struct PositionStruct PositionStru;


struct ToolParameterStru{
	PositionEuler PosEuler;	//工具末端坐标相对于机械手末端法兰面中心的位置关系的参数
	double ToolLength;		//刀具长度
};
typedef struct ToolParameterStru ToolParameter;


//工件基坐标相对于机械手基坐标的位置关系的参数
struct PartParameterStru{
	PositionEuler PosEuler;
};
typedef struct PartParameterStru PartParameter;


//更新机械手轴数标志位
void update_kinematic_parameter(unsigned char sign);

//#pragma CODE_SECTION(update_sin_cos_data, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void update_sin_cos_data(double* angle);//4.1

//正解和最优解
//#pragma CODE_SECTION(GetPositionGesture, "ramfuncs");//修正于//2018/06/01 for Extend RAM
PositionGesture GetPositionGesture(double* angle);//4.6
//#pragma CODE_SECTION(GetBestSolution, "ramfuncs");//修正于//2018/06/01 for Extend RAM
u8 GetBestSolution(double *a);//6.4

//姿态修改比较区域
//#pragma CODE_SECTION(UpdatePositionGesture, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void UpdatePositionGesture(double* angle);//7.11
//#pragma CODE_SECTION(ModifyPositionGesture, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void ModifyPositionGesture(PositionGesture p_g);//7.112
//#pragma CODE_SECTION(CompareCurrentGestureByAngle, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char CompareCurrentGestureByAngle(double* next_angle);//7.12
//#pragma CODE_SECTION(CompareGestureByTowAngle, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char CompareGestureByTowAngle(double* angle1,double* angle2);//7.13
//#pragma CODE_SECTION(CompareTowGesture, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char CompareTowGesture(PositionGesture pos_ges1,PositionGesture pos_ges2);//7.14

//#pragma CODE_SECTION(FourAxisRobot_GetZ_LimitedFlg, "ramfuncs");//修正于//2018/06/01 for Extend RAM
unsigned char FourAxisRobot_GetZ_LimitedFlg(double* angle);

//沿着基座标平移px-pz
//#pragma CODE_SECTION(MovePxyz, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void MovePxyz(double delta_x,double delta_y,double delta_z);//7.21
//#pragma CODE_SECTION(MovePx, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void MovePx(double delta_x);//7.22
//#pragma CODE_SECTION(MovePy, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void MovePy(double delta_y);//7.23
//#pragma CODE_SECTION(MovePz, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void MovePz(double delta_z);//7.24

//修改px-pz的值
//#pragma CODE_SECTION(ModifyPositionValue, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void ModifyPositionValue(double px,double py,double pz);//7.31
//#pragma CODE_SECTION(ModifyPxValue, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void ModifyPxValue(double px);//7.32
//#pragma CODE_SECTION(ModifyPyValue, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void ModifyPyValue(double py);//7.33
//#pragma CODE_SECTION(ModifyPzValue, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void ModifyPzValue(double pz);//7.34

//沿着姿态座标平移px-pz
//#pragma CODE_SECTION(straight_x, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void straight_x(float x1);//7.41
//#pragma CODE_SECTION(straight_y, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void straight_y(float y1);//7.42
//#pragma CODE_SECTION(straight_z, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void straight_z(float z1);//7.43

//平面圆弧
//#pragma CODE_SECTION(circle_parameter, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void circle_parameter(double move_x,double move_y);//7.51
//#pragma CODE_SECTION(circle_xy, "ramfuncs");//修正于//2018/06/01 for Extend RAM
void circle_xy(double precision,double delta_height);//7.52

//三维画圆
//#pragma CODE_SECTION(drive_arc, "ramfuncs");//修正于//2018/06/01 for Extend RAM
u8 drive_arc(const PositionGesture* p1, u8 flag);


/***************************************************7.52
 * 函数功能：把结构体里面的值转换到数组中去
 * 输入参数：p1：
*/
void structrue_to_array(const PositionGesture *p1, double *p);
//判断3点能否成圆
u8 judge_three_point(double x[9]);
#endif /* KINEMATIC_EXPLAIN_H_ */

void ToolPointToEndPoint(PositionEuler p_e, PositionGesture* p_g, double toolLength);
PositionGesture PartPosToToolPos(ToolParameter tool_end,PartParameter part_rob,PositionEuler part,double len);




