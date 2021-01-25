/*
 *  ��Ȩ������ �����º��������Ʒ���޹�˾�з���BU28ӵ���������ս���Ȩ��δ����׼������ת�أ�Υ�߱ؾ�
 *  �ļ����ƣ�Touch_screen.h
 *  ժ	 Ҫ��ȫ�ֽṹ���������
 *  	         ��е��������켣����ģ��
 *  	         �Ƚ�λ�˵ĺ���ģ��
 *  ��ǰ�汾��
 *  ��	 �ߣ����Ķ�
 *  �������ڣ�2015-1-10
 *  ������ڣ�2015-4-27
 */

#ifndef KINEMATIC_EXPLAIN_H_
#define KINEMATIC_EXPLAIN_H_
#include "My_Project.h"
//ȫ�ֽṹ���������
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
	PositionEuler PosEuler;	//����ĩ����������ڻ�е��ĩ�˷��������ĵ�λ�ù�ϵ�Ĳ���
	double ToolLength;		//���߳���
};
typedef struct ToolParameterStru ToolParameter;


//��������������ڻ�е�ֻ������λ�ù�ϵ�Ĳ���
struct PartParameterStru{
	PositionEuler PosEuler;
};
typedef struct PartParameterStru PartParameter;


//���»�е��������־λ
void update_kinematic_parameter(unsigned char sign);

//#pragma CODE_SECTION(update_sin_cos_data, "ramfuncs");//������//2018/06/01 for Extend RAM
void update_sin_cos_data(double* angle);//4.1

//��������Ž�
//#pragma CODE_SECTION(GetPositionGesture, "ramfuncs");//������//2018/06/01 for Extend RAM
PositionGesture GetPositionGesture(double* angle);//4.6
//#pragma CODE_SECTION(GetBestSolution, "ramfuncs");//������//2018/06/01 for Extend RAM
u8 GetBestSolution(double *a);//6.4

//��̬�޸ıȽ�����
//#pragma CODE_SECTION(UpdatePositionGesture, "ramfuncs");//������//2018/06/01 for Extend RAM
void UpdatePositionGesture(double* angle);//7.11
//#pragma CODE_SECTION(ModifyPositionGesture, "ramfuncs");//������//2018/06/01 for Extend RAM
void ModifyPositionGesture(PositionGesture p_g);//7.112
//#pragma CODE_SECTION(CompareCurrentGestureByAngle, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char CompareCurrentGestureByAngle(double* next_angle);//7.12
//#pragma CODE_SECTION(CompareGestureByTowAngle, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char CompareGestureByTowAngle(double* angle1,double* angle2);//7.13
//#pragma CODE_SECTION(CompareTowGesture, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char CompareTowGesture(PositionGesture pos_ges1,PositionGesture pos_ges2);//7.14

//#pragma CODE_SECTION(FourAxisRobot_GetZ_LimitedFlg, "ramfuncs");//������//2018/06/01 for Extend RAM
unsigned char FourAxisRobot_GetZ_LimitedFlg(double* angle);

//���Ż�����ƽ��px-pz
//#pragma CODE_SECTION(MovePxyz, "ramfuncs");//������//2018/06/01 for Extend RAM
void MovePxyz(double delta_x,double delta_y,double delta_z);//7.21
//#pragma CODE_SECTION(MovePx, "ramfuncs");//������//2018/06/01 for Extend RAM
void MovePx(double delta_x);//7.22
//#pragma CODE_SECTION(MovePy, "ramfuncs");//������//2018/06/01 for Extend RAM
void MovePy(double delta_y);//7.23
//#pragma CODE_SECTION(MovePz, "ramfuncs");//������//2018/06/01 for Extend RAM
void MovePz(double delta_z);//7.24

//�޸�px-pz��ֵ
//#pragma CODE_SECTION(ModifyPositionValue, "ramfuncs");//������//2018/06/01 for Extend RAM
void ModifyPositionValue(double px,double py,double pz);//7.31
//#pragma CODE_SECTION(ModifyPxValue, "ramfuncs");//������//2018/06/01 for Extend RAM
void ModifyPxValue(double px);//7.32
//#pragma CODE_SECTION(ModifyPyValue, "ramfuncs");//������//2018/06/01 for Extend RAM
void ModifyPyValue(double py);//7.33
//#pragma CODE_SECTION(ModifyPzValue, "ramfuncs");//������//2018/06/01 for Extend RAM
void ModifyPzValue(double pz);//7.34

//������̬����ƽ��px-pz
//#pragma CODE_SECTION(straight_x, "ramfuncs");//������//2018/06/01 for Extend RAM
void straight_x(float x1);//7.41
//#pragma CODE_SECTION(straight_y, "ramfuncs");//������//2018/06/01 for Extend RAM
void straight_y(float y1);//7.42
//#pragma CODE_SECTION(straight_z, "ramfuncs");//������//2018/06/01 for Extend RAM
void straight_z(float z1);//7.43

//ƽ��Բ��
//#pragma CODE_SECTION(circle_parameter, "ramfuncs");//������//2018/06/01 for Extend RAM
void circle_parameter(double move_x,double move_y);//7.51
//#pragma CODE_SECTION(circle_xy, "ramfuncs");//������//2018/06/01 for Extend RAM
void circle_xy(double precision,double delta_height);//7.52

//��ά��Բ
//#pragma CODE_SECTION(drive_arc, "ramfuncs");//������//2018/06/01 for Extend RAM
u8 drive_arc(const PositionGesture* p1, u8 flag);


/***************************************************7.52
 * �������ܣ��ѽṹ�������ֵת����������ȥ
 * ���������p1��
*/
void structrue_to_array(const PositionGesture *p1, double *p);
//�ж�3���ܷ��Բ
u8 judge_three_point(double x[9]);
#endif /* KINEMATIC_EXPLAIN_H_ */

void ToolPointToEndPoint(PositionEuler p_e, PositionGesture* p_g, double toolLength);
PositionGesture PartPosToToolPos(ToolParameter tool_end,PartParameter part_rob,PositionEuler part,double len);




