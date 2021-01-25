/*
 *  ��Ȩ������ �����º��������Ʒ���޹�˾�з���BU28ӵ���������ս���Ȩ��δ����׼������ת�أ�Υ�߱ؾ�
 *  �ļ����ƣ�Touch_screen.h
 *  ժ	 Ҫ�� ��1�£�ͷ�ļ�������
 *  		��2�£�ȫ�ֱ���������
 *  		��3�£��ڲ�������������
 *  		��4�£���е�����˶�����
 *  		��5�£���е�����˶�����
 *  		��6�£�������ŽⲢת��Ϊ�Ƕ�
 *  		��7�£��ⲿӦ�ú���
 *  ��ǰ�汾��
 *  ��	 �ߣ����Ķ�
 *  �������ڣ�2015-1-10
 *  ������ڣ�2015-4-27
*/


//��1�£�ͷ�ļ�������
/********************************************************************************
* ��    �ƣ� ͷ�ļ�
* ��    �ܣ� �ⲿ�ӿ�����
* ˵	   ����ͷ�ļ�����ʽ2������ĸ�������򣨷�ʽ1���ֲ��ȶ����ȶ�����
 ********************************************************************************/
#include "kinematic_explain.h"
#include "math.h"
#include <C28x_FPU_FastRTS.h>

//��2�£�ȫ�ֱ���������
/********************************************************************************
* ��    �ƣ� ������
* ��    �ܣ� ���������һЩ�궨���
 ********************************************************************************/
#define ERROR 1
#define EPSINON 0.000001
#define TOOL_LENGTH		89
#define AXIS6_LENGTH	89	//5���6��Ľ��㣬��6��ĩ�˵ĳ���

static double c[6];
static double s[6];
static double s_sin_1_plus_2,s_cos_1_plus_2;
//static double a2 = 253.0, d3 = 0.0,a3 = 98,d4 = 213.5;//���ǵ�С��е�ֹ�����
static double a2 = 253, d3 = 0.0,a3 = 98,d4 = 211.5;//���ǵ�С��е�ֹ�����
static double a2_a3_d4_len;
//static double a2 = 250.0, d3 = 0.0,a3 = 0,d4 = 166;//�����5���е�ֲ���
static double s_best_angle[6];
static double ag3_0;
static double fp[9],sp[9],tp[9],fop[9];
static double o[4];

static double s_ARM_A;
static double s_ARM_B;
static double s_ARM_C;
static double s_ARM_LUOJU;
static PositionGesture s_pos_ges={1,0,0,0,1,0,0,0,1,0,0,0};//λ�ú���̬


//��3�£��ڲ�������������
/********************************************************************************
* ��    �ƣ��ڲ���������
* ��    �ܣ����ú���
* ˵	   ����
 ********************************************************************************/
//��е�����˶�����4-13

//#pragma CODE_SECTION(getNx, "ramfuncs");//������//2018/06/01 for Extend RAM
double getNx(void);//4.21
//#pragma CODE_SECTION(getNy, "ramfuncs");//������//2018/06/01 for Extend RAM
double getNy(void);//4.22
//#pragma CODE_SECTION(getNz, "ramfuncs");//������//2018/06/01 for Extend RAM
double getNz(void);//4.23
//#pragma CODE_SECTION(getOx, "ramfuncs");//������//2018/06/01 for Extend RAM
double getOx(void);//4.31
//#pragma CODE_SECTION(getOy, "ramfuncs");//������//2018/06/01 for Extend RAM
double getOy(void);//4.32
//#pragma CODE_SECTION(getOz, "ramfuncs");//������//2018/06/01 for Extend RAM
double getOz(void);//4.33
//#pragma CODE_SECTION(getAx, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAx(void);//4.41
//#pragma CODE_SECTION(getAy, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAy(void);//4.41
//#pragma CODE_SECTION(getAz, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAz(void);//4.41
//#pragma CODE_SECTION(getx, "ramfuncs");//������//2018/06/01 for Extend RAM
double getx(void);//4.51
//#pragma CODE_SECTION(gety, "ramfuncs");//������//2018/06/01 for Extend RAM
double gety(void);//4.52
//#pragma CODE_SECTION(getz, "ramfuncs");//������//2018/06/01 for Extend RAM
double getz(void);//4.53


//��е�����˶�����5-16
//#pragma CODE_SECTION(getAngle1_1, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle1_1(void);//5.11
//#pragma CODE_SECTION(getAngle1_2, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle1_2(void);//5.12
//#pragma CODE_SECTION(getAngle3_1, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle3_1(void);//5.21
//#pragma CODE_SECTION(getAngle3_2, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle3_2(void);//5.22
//#pragma CODE_SECTION(getAngle2, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle2(void);//5.3
//#pragma CODE_SECTION(getAngle2_1, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle2_1(void);//5.31
//#pragma CODE_SECTION(getAngle2_2, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle2_2(void);//5.32
//#pragma CODE_SECTION(getAngle2_3, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle2_3(void);//5.33
//#pragma CODE_SECTION(getAngle2_4, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle2_4(void);//5.34
//#pragma CODE_SECTION(getAngle4_1, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle4_1(void);//5.41
//#pragma CODE_SECTION(getAngle4_2, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle4_2(void);//5.42
//#pragma CODE_SECTION(getAngle5_1, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle5_1(void);//5.51
//#pragma CODE_SECTION(getAngle5_2, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle5_2(void);//5.52
//#pragma CODE_SECTION(getAngle6_1, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle6_1(void);//5.61
//#pragma CODE_SECTION(getAngle6_2, "ramfuncs");//������//2018/06/01 for Extend RAM
double getAngle6_2(void);//5.62
//#pragma CODE_SECTION(getAngle4_61, "ramfuncs");//������//2018/06/01 for Extend RAM
void getAngle4_61(double *a);//5.71
//#pragma CODE_SECTION(getAngle4_62, "ramfuncs");//������//2018/06/01 for Extend RAM
void getAngle4_62(double *a);//5.72
//#pragma CODE_SECTION(first_position, "ramfuncs");//������//2018/06/01 for Extend RAM
void first_position(unsigned char gesture);//5.81
//#pragma CODE_SECTION(second_position, "ramfuncs");//������//2018/06/01 for Extend RAM
void second_position(unsigned char gesture);//5.82
//#pragma CODE_SECTION(third_position, "ramfuncs");//������//2018/06/01 for Extend RAM
void third_position(unsigned char gesture);//5.83
//#pragma CODE_SECTION(fourth_position, "ramfuncs");//������//2018/06/01 for Extend RAM
void fourth_position(unsigned char gesture);//5.84
//#pragma CODE_SECTION(location_gesture, "ramfuncs");//������//2018/06/01 for Extend RAM
void location_gesture(u8 a,u8 b,u8 c,u8 d);//5.9

//������ŽⲢת��Ϊ�Ƕ�6-5
//#pragma CODE_SECTION(best_one_ag, "ramfuncs");//������//2018/06/01 for Extend RAM
u8 best_one_ag(void);//6.1
//#pragma CODE_SECTION(best_two_ag, "ramfuncs");//������//2018/06/01 for Extend RAM
u8 best_two_ag(void);//6.2
//#pragma CODE_SECTION(best_four_ag, "ramfuncs");//������//2018/06/01 for Extend RAM
u8 best_four_ag(void);//6.3
//#pragma CODE_SECTION(change_angle, "ramfuncs");//������//2018/06/01 for Extend RAM
void change_angle(double *a,double *b,u8 sign);//6.4
//#pragma CODE_SECTION(reset_angle, "ramfuncs");//������//2018/06/01 for Extend RAM
void reset_angle(double *a,u8 sign,double* best_ag);//6.5
//�����е��
unsigned char backward_explain();//�����е�����


//��4�£���е�����˶�����
/********************************************************************************
* ��    �ܣ����ݻ�е�ֲ����͵�ǰ�Ƕ����λ��
* ˵	   ����ֻ��Ψһ��һ����
 ********************************************************************************/

/**************************************
 * �������ܣ��������Ǻ�����-4.1
 * �����������ǰ������ĽǶ�ֵ
 * �����������
*/
void update_sin_cos_data(double *angle)
{
	u8 i;

	if(ROBOT_PARAMETER.AXIS_NUM == 4)
	{
		for(i=0; i<4; i++)
		{
			 s_best_angle[i] = *(angle + i);
		}
	}
	else
	{
		if(ROBOT_PARAMETER.AXIS_NUM == 5)
		{
			*(angle + 3) = 0;
		}

		for(i=0; i<6; i++)
		{
			 s_best_angle[i] = *(angle + i);
			 s[i] = sin( *(angle + i) );
			 c[i] = cos( *(angle + i) );
		}

		s_sin_1_plus_2 = sin( *(angle + 1)  +  *(angle + 2) );//ע��
		s_cos_1_plus_2 = cos( *(angle + 1)  +  *(angle + 2) );//ע��
	}
}

/******************************************
 * �������ܣ����»�е��������־λ4.11
 * ���������sign����е������
*/
void update_kinematic_parameter(u8 sign)
{
	if(ROBOT_PARAMETER.AXIS_NUM == 4)
	{
		s_ARM_A = ROBOT_PARAMETER.ROBOT_PARA[0];
		s_ARM_B = ROBOT_PARAMETER.ROBOT_PARA[1];
		s_ARM_C = ROBOT_PARAMETER.ROBOT_PARA[2];
		s_ARM_LUOJU = ROBOT_PARAMETER.ROBOT_PARA[3];
	}
	else
	{
		a2 = ROBOT_PARAMETER.ROBOT_PARA[0];
		d3 = ROBOT_PARAMETER.ROBOT_PARA[1];
		a3 = ROBOT_PARAMETER.ROBOT_PARA[2];
		d4 = ROBOT_PARAMETER.ROBOT_PARA[3];
		a2_a3_d4_len = sqrt(a3*a3+d4*d4) + a2;	//
	}
}

/**************************************
 * �������ܣ�����̬����Nx-4.21
*/
double getNx(void)
{
	double d1, d2, nx;
	d1 = c[0] * ( s_cos_1_plus_2*(c[3]*c[4]*c[5]-s[3]*s[5]) - s_sin_1_plus_2*s[4]*c[5] );
	d2 = s[0] * ( s[3]*c[4]*c[5] + c[3]*s[5] );
	nx = d1 + d2;
	return nx;
}

/**************************************
 * �������ܣ�����̬����Ny-4.22
*/
double getNy(void)
{
	double d1, d2, ny;
	d1 = s[0] * ( s_cos_1_plus_2*(c[3]*c[4]*c[5]-s[3]*s[5]) - s_sin_1_plus_2*s[4]*c[5] );
	d2 = c[0] * ( s[3]*c[4]*c[5] + c[3]*s[5] );
	ny = d1 - d2;
	return ny;
}

/**************************************
 * �������ܣ�����̬����Nz-4.23
*/
double getNz(void)
{
	long double nz;
	nz = -s_sin_1_plus_2*(c[3]*c[4]*c[5]-s[3]*s[5]) - s_cos_1_plus_2*s[4]*c[5];
	return nz;
}

/**************************************
 * �������ܣ���̬����Ox-4.31
*/
double getOx(void)
{
	double ox;
	ox = c[0]*(s[1]*s[2]-c[1]*c[2])*(c[3]*c[4]*s[5] + s[3]*c[5]);
	ox = ox + c[0]*s[4]*s[5]*( c[1]*s[2] + s[1]*c[2] );
	ox = ox + s[0]*( c[3]*c[5] - s[3]*c[4]*s[5] );
	return ox;
}

/**************************************
 * �������ܣ���̬����Oy-4.32
*/
double getOy(void)
{
	double oy;
	oy = s[0] * (s[1]*s[2] - c[1]*c[2]) * (s[3]*c[5] + c[3]*c[4]*s[5]);
	oy = oy + s[0]*s[4]*s[5]*( c[1]*s[2] + s[1]*c[2] );
	oy = oy - c[0] * ( c[3]*c[5] - s[3]*c[4]*s[5] );
	return oy;
}

/**************************************
 * �������ܣ���̬����Oz-4.33
*/
double getOz(void)
{
	double oz;
	oz = (c[1]*s[2] + s[1]*c[2]) * (c[3]*c[4]*s[5] + s[3]*c[5]);
	oz = oz - s[4]*s[5]*( s[1]*s[2] - c[1]*c[2] );
	return oz;
}

/**************************************
 * �������ܣ���̬����Ax-4.41
*/
double getAx(void)
{
	double ax;
	ax = c[0] * c[3]*s[4]*( s[1]*s[2] - c[1]*c[2] );
	ax = ax - c[0]*c[4]*( c[1]*s[2] + s[1]*c[2] );
	ax = ax - s[0]*s[3]*s[4];
	return ax;
}

/**************************************
 * �������ܣ���̬����Ay-4.42
*/
double getAy(void)
{
	double ay;
	ay = s[0]*c[3]*s[4]*( s[1]*s[2] - c[1]*c[2] );
	ay = ay - s[0]*c[4]*( c[1]*s[2] + s[1]*c[2] );
	ay = ay + c[0]*s[3]*s[4];
	return ay;
}

/**************************************
 * �������ܣ���̬����Az-4.43
*/
double getAz(void)
{
	double az;
	az = c[3]*s[4]*( c[1]*s[2] + s[1]*c[2]);
	az = az + c[4]*( s[1]*s[2] - c[1]*c[2]);
	return az;
}

/**************************************
 * �������ܣ�������λ��tx-4.51
*/
double getx(void)
{
	double px;
	px = c[0] * ( a2*c[1] + a3*s_cos_1_plus_2 - d4*s_sin_1_plus_2 ) - d3*s[0];
	return px;
}

/**************************************
 * �������ܣ�������λ��ty-4.52
*/
double gety(void)
{
	double py;
	py = s[0] * ( a2*c[1] + a3*s_cos_1_plus_2 - d4*s_sin_1_plus_2 ) + d3*c[0];
	return py;
}

/**************************************
 * �������ܣ�������λ��tz-4.53
*/
double getz(void)
{
	double pz;
	pz = -a3*s_sin_1_plus_2 - a2*s[1] - d4*s_cos_1_plus_2;
	return pz;
}

/******************************************
 * �������ܣ������3����4�ỡ�ȣ����С����z��λ���Ƿ���λ
 * ���������angle��4����Ļ���ֵ
 * ���ز�����0��û����λ	1������λ  2������λ
*/
unsigned char FourAxisRobot_GetZ_LimitedFlg(double* angle)
{
	double z;
	z = ((s_ARM_C*angle[2] - s_ARM_LUOJU*angle[3])/(PI*2));//ʵ�ʵ�z
	if(z >=0.01) return 1;
	else if(z <= (-s_ARM_C)) return 2;
	else return 0;
}

/******************************************
 * �������ܣ����ݵ�ǰ�Ƕ���λ��-4.6
 * ���������angle��������Ļ���ֵ
*/
PositionGesture GetPositionGesture(double *angle)
{
	PositionGesture pos_ges;

	update_sin_cos_data(angle);

	if(ROBOT_PARAMETER.AXIS_NUM == 4)
	{
		pos_ges.r11 = cos(s_best_angle[0]+s_best_angle[1]+s_best_angle[3]);
		pos_ges.r21 = sin(s_best_angle[0]+s_best_angle[1]+s_best_angle[3]);
		pos_ges.r31 = 0;

		pos_ges.px = s_ARM_A*cos(s_best_angle[0])+s_ARM_B*cos(s_best_angle[0]+s_best_angle[1]);
		pos_ges.py = s_ARM_A*sin(s_best_angle[0])+s_ARM_B*sin(s_best_angle[0]+s_best_angle[1]);
		if(ROBOT_PARAMETER.SYS == FOUR_AXIS_A)	//�������е��
		{
			pos_ges.pz = s_ARM_C*s_best_angle[2]/(PI*2);
		}
//		else if(ROBOT_PARAMETER.SYS == FOUR_AXIS_B)
//		{
//			pos_ges.pz = (s_ARM_C*s_best_angle[2] - s_ARM_LUOJU*angle[3])/(PI*2);//ʵ�ʵ�z
//		}
		else//if(ROBOT_PARAMETER.SYS == FOUR_AXIS_B)
		{
			pos_ges.pz = (s_ARM_C*s_best_angle[2] - s_ARM_LUOJU*angle[3])/(PI*2);//ʵ�ʵ�z
		}
	}
	else
	{
		pos_ges.r11 = getNx();
		pos_ges.r21 = getNy();
		pos_ges.r31 = getNz();
		pos_ges.r12 = getOx();
		pos_ges.r22 = getOy();
		pos_ges.r32 = getOz();
		pos_ges.r13 = getAx();
		pos_ges.r23 = getAy();
		pos_ges.r33 = getAz();
		pos_ges.px = getx();
		pos_ges.py = gety();
		pos_ges.pz = getz();
	}
	return pos_ges;
}


//��5�£���е�����˶�����
/********************************************************************************
* ��    �ܣ����ݵ�ǰ��λ������е�����п��ܵ���ĽǶ����
* ˵	   �������а���⣬������������Ҫ�ȶԷ��������н���
 ********************************************************************************/

/**************************************
 * �������ܣ���Ƕ�һ��һ����-5.11
*/
double getAngle1_1(void)
{
	double ag;
	ag = atan2(s_pos_ges.py, s_pos_ges.px) - atan2( d3, sqrt(s_pos_ges.px*s_pos_ges.px + s_pos_ges.py*s_pos_ges.py-d3*d3) );

	if( (ag-PI) > 0 )	{ ag -= 2*PI;}
	if( (ag + PI) < 0 )	{ ag += 2*PI;}

	s[0] = sin(ag);
	c[0] = cos(ag);
	return ag;
}

/**************************************
 * �������ܣ���Ƕ�һ�ڶ�����-5.12
*/
double getAngle1_2(void)
{
	double ag;
	ag = atan2(s_pos_ges.py,s_pos_ges.px) - atan2( d3, -sqrt(s_pos_ges.px*s_pos_ges.px + s_pos_ges.py*s_pos_ges.py-d3*d3) );

	if((ag-PI)>0)	{ ag -= 2*PI;}
	if((ag+PI)<0)	{ ag += 2*PI;}

	s[0] = sin(ag);
	c[0] = cos(ag);
	return ag;
}

/**************************************
 * �������ܣ���Ƕ�����һ����-5.21
*/
double getAngle3_1(void)
{
	double ag, d, d1;
	d = (s_pos_ges.px*s_pos_ges.px + s_pos_ges.py*s_pos_ges.py + s_pos_ges.pz*s_pos_ges.pz - a2*a2-a3*a3-d3*d3-d4*d4) / (2*a2);
	d1 = sqrt( a3*a3 + d4*d4 - (d*d) );
	ag = atan2(a3,d4) - atan2(d,d1);

	if( (ag-PI) > 0 )	{ ag -= 2*PI;}
	if( (ag+PI) < 0 )	{ ag += 2*PI;}

	ag3_0 = ag;
	s[2] = sin(ag);
	c[2] = cos(ag);
	return ag;
}

/**************************************
 * �������ܣ���Ƕ����ĵڶ�����-5.22
*/
double getAngle3_2(void)
{
	double ag, d, d1;
	d = (s_pos_ges.px*s_pos_ges.px + s_pos_ges.py*s_pos_ges.py + s_pos_ges.pz*s_pos_ges.pz - a2*a2-a3*a3-d3*d3-d4*d4) / (2*a2);
	d1 = sqrt( a3*a3 + d4*d4 - (d*d));
	ag = atan2(a3,d4) - atan2(d,-d1);

	if( ag > PI )	{ ag -= 2*PI;}
	if( ag < -PI )	{ ag += 2*PI;}

	ag3_0 = ag;
	s[2] = sin(ag);
	c[2] = cos(ag);
	return ag;
}
/**************************************
 * �������ܣ���Ƕȶ��ĵ�һ����-5.31
*/
double getAngle2 (void)
{
	double ag, d1, d2,ag23;
	d1 = (-a3-a2*c[2])*s_pos_ges.pz+(c[0]*s_pos_ges.px+s[0]*s_pos_ges.py)*(a2*s[2]-d4);;
	d2 = (a2*s[2]-d4)*s_pos_ges.pz+(a3+a2*c[2])*(c[0]*s_pos_ges.px+s[0]*s_pos_ges.py);
	ag23 = atan2(d1,d2);
	ag = ag23-ag3_0;
	if( ag > PI )	{ ag -= 2*PI;}
	if( ag < -PI )	{ ag += 2*PI;}
	s[1] = sin(ag);
	c[1] = cos(ag);
	s_sin_1_plus_2 = sin(ag23);
	s_cos_1_plus_2 = cos(ag23);
	return ag;
}

/**************************************
 * �������ܣ���Ƕȶ��ĵ�һ����-5.31
*/
double getAngle2_1 (void)
{

	double ag, d1, d2;
	d1 = atan2( s_pos_ges.pz,  sqrt(s_pos_ges.px*s_pos_ges.px + s_pos_ges.py*s_pos_ges.py) );
	d2 = acos((s_pos_ges.px*s_pos_ges.px + s_pos_ges.py*s_pos_ges.py + s_pos_ges.pz*s_pos_ges.pz + a2*a2-(a3*a3 + d4*d4))/(2*a2*sqrt(s_pos_ges.px*s_pos_ges.px + s_pos_ges.py*s_pos_ges.py + s_pos_ges.pz*s_pos_ges.pz)));
	ag = -(d1 + d2);

	if( ag > PI )	{ ag -= 2*PI;}
	if( ag < -PI )	{ ag += 2*PI;}

	s[1] = sin(ag);
	c[1] = cos(ag);
	s_sin_1_plus_2 = sin(ag + ag3_0);
	s_cos_1_plus_2 = cos(ag + ag3_0);
	return ag;

}

/**************************************
 * �������ܣ���Ƕȶ��ĵڶ�����-5.32
*/
double getAngle2_2 (void)
{

	double ag, d1, d2;
	d1 = atan2( s_pos_ges.pz,  sqrt(s_pos_ges.px*s_pos_ges.px + s_pos_ges.py*s_pos_ges.py) );
	d2 = acos((s_pos_ges.px*s_pos_ges.px + s_pos_ges.py*s_pos_ges.py + s_pos_ges.pz*s_pos_ges.pz + a2*a2-(a3*a3 + d4*d4))/(2*a2*sqrt(s_pos_ges.px*s_pos_ges.px + s_pos_ges.py*s_pos_ges.py + s_pos_ges.pz*s_pos_ges.pz)));
	ag = -(d1 - d2);

	if( ag > PI )	{ ag -= 2*PI;}
	if( ag < -PI)	{ ag += 2*PI;}

	s[1] = sin(ag);
	c[1] = cos(ag);
	s_sin_1_plus_2 = sin(ag + ag3_0);
	s_cos_1_plus_2 = cos(ag + ag3_0);
	return ag;

}

/**************************************
 * �������ܣ���Ƕȶ��ĵ�������-5.33
*/
double getAngle2_3(void)
{
	double ag, ag1;
	ag1 = getAngle2_2 ();
	ag = (atan2(0,-1) - ag1);

	if( ag >  PI )	{ ag -= 2*PI;}
	if( ag < -PI )	{ ag += 2*PI;}

	s[1] = sin(ag);
	c[1] = cos(ag);
	s_sin_1_plus_2 = sin(ag + ag3_0);
	s_cos_1_plus_2 = cos(ag + ag3_0);
	return ag;

}

/**************************************
 * �������ܣ���Ƕȶ��ĵ��ĸ���-5.34
*/
double getAngle2_4(void)
{
	double ag, ag1;
	ag1 = getAngle2_1();
	ag = (atan2(0,-1) - ag1);

	if( (ag-PI) > 0 )	{ ag-= 2*PI;}
	if( (ag+PI) < 0 )	{ ag += 2*PI;}

	s[1] = sin(ag);
	c[1] = cos(ag);
	s_sin_1_plus_2 = sin(ag + ag3_0);
	s_cos_1_plus_2 = cos(ag + ag3_0);
	return ag;

}

/**************************************
 * �������ܣ���Ƕ��ĵĵ�һ����-5.41
*/
double getAngle4_1(void)
{
	double d1, d2, ag;
	d1 = -s_pos_ges.r13*s[0] + s_pos_ges.r23*c[0];
	d2 = -s_pos_ges.r13*c[0]*s_cos_1_plus_2-s_pos_ges.r23*s[0]*s_cos_1_plus_2 + s_pos_ges.r33*s_sin_1_plus_2;
	ag = atan2(d1, d2);

	if( (ag-PI) > 0 )	{ ag -= 2*PI;}
	if( (ag+PI) < 0 )	{ ag += 2*PI;}

	if(ROBOT_PARAMETER.AXIS_NUM == 5)
	{
		ag = 0;
	}

	s[3] = sin(ag);
	c[3] = cos(ag);
	return ag;
}

/**************************************
 * �������ܣ���Ƕ��ĵĵڶ�����-5.42
*/
double getAngle4_2(void)
{
	double d1,d2,ag;
	d1 = -s_pos_ges.r13*s[0] + s_pos_ges.r23*c[0];
	d2 = -s_pos_ges.r13*c[0]*s_cos_1_plus_2-s_pos_ges.r23*s[0]*s_cos_1_plus_2 + s_pos_ges.r33*s_sin_1_plus_2;
	ag = atan2(d1, d2);
	ag += PI;

	if( (ag-PI) > 0)	{ ag -= 2*PI;}
	if( (ag+PI) < 0)	{ ag += 2*PI;}

	if(ROBOT_PARAMETER.AXIS_NUM == 5)
	{
		ag = 0;
	}

	s[3] = sin(ag);
	c[3] = cos(ag);
	return ag;
}

/**************************************
 * �������ܣ���Ƕ���ĵ�һ����-5.51
*/
double getAngle5_1(void)
{
	double d1,d2,ag;
	d1 = s_pos_ges.r13 * (c[0]*s_cos_1_plus_2*c[3] + s[0]*s[3]) + s_pos_ges.r23*(s[0]*s_cos_1_plus_2*c[3]-c[0]*s[3])-s_pos_ges.r33*(s_sin_1_plus_2*c[3]);
	d2 = s_pos_ges.r13 * (-c[0]*s_sin_1_plus_2) + s_pos_ges.r23*(-s[0]*s_sin_1_plus_2) + s_pos_ges.r33*(-s_cos_1_plus_2);
	ag = atan2(-d1, d2);

	if( (ag-PI) > 0 )	{ ag -= 2*PI;}
	if( (ag+PI) < 0 )	{ ag += 2*PI;}
	s[4] = sin(ag);
	c[4] = cos(ag);
	return ag;
}

/**************************************
 * �������ܣ���Ƕ���ĵڶ�����-5.52
*/
double getAngle5_2(void)
{
	double d1, d2, ag;
	d1 = s_pos_ges.r13 * (c[0]*s_cos_1_plus_2*c[3] + s[0]*s[3]) + s_pos_ges.r23*(s[0]*s_cos_1_plus_2*c[3]-c[0]*s[3]) - s_pos_ges.r33*(s_sin_1_plus_2*c[3]);
	d2 = s_pos_ges.r13 * (-c[0]*s_sin_1_plus_2) + s_pos_ges.r23*(-s[0]*s_sin_1_plus_2) + s_pos_ges.r33*(-s_cos_1_plus_2);
	ag = atan2(-d1, d2);

	if( (ag-PI) > 0 )	{ ag -= 2*PI;}
	if( (ag+PI) < 0 )	{ ag += 2*PI;}
	s[4] = sin(ag);
	c[4] = cos(ag);
	return ag;
}

/**************************************
 * �������ܣ���Ƕ����ĵ�һ����-5.6
*/
double getAngle6_1(void)
{
	double d1, d2, d3, ag;
	d1 = s_pos_ges.r11 * ( (c[0]*s_cos_1_plus_2*c[3]+s[0]*s[3])*c[4] - c[0]*s_sin_1_plus_2*s[4] );
	d2 = s_pos_ges.r21 * ( (s[0]*s_cos_1_plus_2*c[3]-c[0]*s[3])*c[4] - s[0]*s_sin_1_plus_2*s[4] );
	d3 = s_pos_ges.r31 * ( s_sin_1_plus_2*c[3]*c[4] + s_cos_1_plus_2*s[4] );
	d2 = d1 + d2 - d3;

	d1 = -s_pos_ges.r11 * ( c[0]*s_cos_1_plus_2*s[3] - s[0]*c[3] );
	d3 = (s_pos_ges.r21 * ( s[0]*s_cos_1_plus_2*s[3] + c[0]*c[3] )) - s_pos_ges.r31*(s_sin_1_plus_2*s[3]);
	d1 = d1-d3;
	ag = atan2(d1, d2);

	s[5] = sin(ag);
	c[5] = cos(ag);
	return ag;
}
double getAngle6_2(void)
{
	double d1, d2, d3, ag;
	d1 = s_pos_ges.r11 * ( (c[0]*s_cos_1_plus_2*c[3]+s[0]*s[3])*c[4] - c[0]*s_sin_1_plus_2*s[4] );
	d2 = s_pos_ges.r21 * ( (s[0]*s_cos_1_plus_2*c[3]-c[0]*s[3])*c[4] - s[0]*s_sin_1_plus_2*s[4] );
	d3 = s_pos_ges.r31 * ( s_sin_1_plus_2*c[3]*c[4] + s_cos_1_plus_2*s[4] );
	d2 = d1 + d2 - d3;

	d1 = -s_pos_ges.r11 * ( c[0]*s_cos_1_plus_2*s[3] - s[0]*c[3] );
	d3 = (s_pos_ges.r21 * ( s[0]*s_cos_1_plus_2*s[3] + c[0]*c[3] )) - s_pos_ges.r31*(s_sin_1_plus_2*s[3]);
	d1 = d1-d3;
	ag = atan2(d1, d2);
	//ag += PI;


	s[5] = sin(ag);
	c[5] = cos(ag);
	return ag;
}
/***************************************************
 * �������ܣ����һ��456��ĽǶ�-5.71
 * ���������a������һ�鷴�⣨�ܹ������鷴�⣬4��λ�ý�*ÿ��λ�õ���2����̬��
*/
void getAngle4_61(double *a)
{
	a += 3;
	*a = getAngle4_1();
	if( (*a-PI) > 0 )	{ *a -= 2*PI;}
	if( (*a+PI) <= 0 )	{ *a += 2*PI;}

	a++ ;
	*a = getAngle5_1();
	if( (*a-PI) > 0)	{ *a -= 2*PI;}
	if( (*a+PI) <= 0)	{ *a += 2*PI;}

	a++ ;
	*a = getAngle6_1();
	if( (*a-PI) > 0 )	{ *a -= 2*PI;}
	if( (*a+PI) <= 0 )	{ *a += 2*PI;}
}

/***************************************************
 * �������ܣ���ڶ���456��ĽǶ�-5.72
 * ���������a������һ�鷴�⣨�ܹ������鷴�⣬4��λ�ý�*ÿ��λ�õ���2����̬��
*/
void getAngle4_62(double *a)
{
	a += 6;
	*a = getAngle4_1() + PI;
	if( (*a-PI) > 0 )	{ *a -= 2*PI;}
	if( (*a+PI) <= 0 )	{ *a += 2*PI;}

	a++ ;
	*a = -getAngle5_1();
	if( (*a-PI) > 0 )	{ *a -= 2*PI;}
	if( (*a+PI) <= 0 )	{ *a += 2*PI;}

	a++ ;
	*a = getAngle6_1() + PI;
	if( (*a-PI) > 0 )	{ *a -= 2*PI;}
	if( (*a+PI) <= 0 )	{ *a += 2*PI;}

	a -= 2;
	s[3] = sin(*a);
	c[3] = cos(*a);

	a++;
	s[4] = sin(*a);
	c[4] = cos(*a);

	a++;
	s[5] = sin(*a);
	c[5] = cos(*a);
}

/*********************************
 * �������ܣ���һ�鷴��-5.81
 * ���������gesture����־λ��1��ʾ���һ����̬��2��ʾ��ڶ�����̬��3��ʾͬʱ��һ������̬
*/
void first_position(u8 gesture)
{
	fp[0] = getAngle1_1();
	fp[2] = getAngle3_2();//����Ƕ�������Ƕȶ�
	fp[1] = getAngle2_2();

	if(gesture == 1)
	{
		getAngle4_61(fp);
	}
 	if(gesture == 2)
	{
		getAngle4_62(fp);
	}
	if(gesture == 3)
	{
		getAngle4_61(fp);
		getAngle4_62(fp);
	}
}

/******************************************
 * �������ܣ��ڶ��鷴��-5.82
 * ���������gesture����־λ��1��ʾ���һ����̬��2��ʾ��ڶ�����̬��3��ʾͬʱ��һ������̬
*/
void second_position(u8 gesture)
{
	sp[0] = getAngle1_1();
	sp[2] = getAngle3_1();//����Ƕ�������Ƕȶ�
	sp[1] = getAngle2_1();

	if(gesture == 1)
	{
		getAngle4_61(sp);
	}
	if(gesture == 2)
	{
		getAngle4_62(sp);
	}
	if(gesture == 3)
	{
		getAngle4_61(sp);
		getAngle4_62(sp);
	}
}

/******************************************
 * �������ܣ������鷴��-5.83
 * ���������gesture����־λ��1��ʾ���һ����̬��2��ʾ��ڶ�����̬��3��ʾͬʱ��һ������̬
*/
void third_position(u8 gesture)
{
	tp[0] = getAngle1_2();
	tp[2] = getAngle3_1();//����Ƕ�������Ƕȶ�
	tp[1] = getAngle2_3();

	if(gesture == 1)
	{
		getAngle4_61(tp);
	}
 	if(gesture == 2)
	{
		getAngle4_62(tp);
	}
	if(gesture == 3)
	{
		getAngle4_61(tp);
		getAngle4_62(tp);
	}
}

/******************************************
 * �������ܣ������鷴��-5.84
 * ���������gesture����־λ��1��ʾ���һ����̬��2��ʾ��ڶ�����̬��3��ʾͬʱ��һ������̬
*/
void fourth_position(u8 gesture)
{
	fop[0] = getAngle1_2();
	fop[2] = getAngle3_2();//����Ƕ�������Ƕȶ�
	fop[1] = getAngle2_4();

	if(gesture == 1)
	{
		getAngle4_61(fop);
	}
 	if(gesture == 2)
	{
		getAngle4_62(fop);
	}
	if(gesture == 3)
	{
		getAngle4_61(fop);
		getAngle4_62(fop);
	}
}

/******************************************
 * �������ܣ����鷴��-5.9
 * ���������a��b��c��d:��־λ��1��ʾ���һ����̬��2��ʾ��ڶ�����̬��3��ʾͬʱ��һ������̬
*/
void location_gesture(u8 a,u8 b,u8 c,u8 d)
{
	 first_position(a);
	 second_position(b);
	 third_position(c);
	 fourth_position(d);
}


//��6�£�������ŽⲢת��Ϊ�Ƕ�
/********************************************************************************
* ��    �ܣ����ݰ����͵�ǰ�Ƕ�ֵ�����һ��λ�õ�����Ž�
* ˵	   ����������У�ÿ��λ����������̬����ʵλ�ý���4�֣�ÿ�ֽ�����������̬����Ϊ�����
********************************************************************************/

/******************************************
 * �������ܣ��Ƕ�һ�����Ž�-6.1
*/
u8 best_one_ag(void)
{
	double angle[2];
	location_gesture(3, 3, 3, 3);

	angle[0] = fabs(fp[0] - s_best_angle[0]);
	angle[1] = fabs(tp[0] - s_best_angle[0]);

	if(angle[0] > PI)	{ angle[0] = 2*PI - angle[0];}
	if(angle[1] > PI)	{ angle[1] = 2*PI - angle[1];}
	if(angle[1] > angle[0])	{ return 1;}
	if(angle[1] < angle[0])	{ return 3;}
	return 0;
}

/******************************************
 * �������ܣ��Ƕȶ������Ž�-6.2
*/
u8 best_two_ag(void)
{
	u8 sign;
	double angle[2];

	sign = best_one_ag();
	if( (sign==1) || (sign==3) )
	{
		 if(sign == 1)
		 {
			 angle[0] = fabs(fp[1] - s_best_angle[1]);
			 angle[1] = fabs(sp[1] - s_best_angle[1]);
		 }
		 if(sign == 3)
		 {
			 angle[0] = fabs(tp[1] - s_best_angle[1]);
			 angle[1] = fabs(fop[1] - s_best_angle[1]);
		 }

		if(angle[0] > PI)		{ angle[0] = 2*PI - angle[0];}
	    if(angle[1] > PI)		{ angle[1] = 2*PI - angle[1];}
		if(angle[1] > angle[0])		{ return  sign;}
		if(angle[1] < angle[0])		{ return  sign+1;}
	}
	return 0;
}

/******************************************
 * �������ܣ��Ƕ��ĵ����Ž�-6.3
*/
u8 best_four_ag(void)
{
	u8 sign;
	double angle[2];

	sign = best_two_ag();
	if( (sign>0) && (sign<5) )
	{
		if(sign == 1)
		{
			angle[0] = fabs(fp[3] - s_best_angle[3]);
			angle[1] = fabs(fp[6] - s_best_angle[3]);
		}
		if(sign == 2)
		{
			angle[0] = fabs(sp[3] - s_best_angle[3]);
			angle[1] = fabs(sp[6] - s_best_angle[3]);
		}
		if(sign == 3)
		{
			angle[0] = fabs(tp[3] - s_best_angle[3]);
			angle[1] = fabs(tp[6] - s_best_angle[3]);
		}
		if(sign == 4)
		{
			angle[0] = fabs(fop[3] - s_best_angle[3]);
			angle[1] = fabs(fop[6] - s_best_angle[3]);
		}

		if(angle[0] > PI)		{ angle[0]  =  2*PI - angle[0];}
	    if(angle[1] > PI)		{ angle[1]  =  2*PI - angle[1];}
		if(angle[1] > angle[0])		{ return  sign*10 + 1;}
		if(angle[1] < angle[0])		{ return  sign*10 + 2;}
	}
	return 0;
}

/**********************************************************
������������������е�ֵĽǶȺ����Ž⣬s_best_angle[0]~4��ʾ���Ž�
�����������
���ز�����ERROR���޽⣬0���н�
************************************************************/
unsigned char backward_explain()
{
	double ag1_1, ag1_2;
	double ag2_1, ag2_2;
	double tmpag0, tmpag1, tmpag2;

//	if(ROBOT_PARAMETER.SYS !=FOUR_AXIS_C)
	{
		if( ((sqrt(s_pos_ges.px*s_pos_ges.px+s_pos_ges.py*s_pos_ges.py)>(s_ARM_A+s_ARM_B-0.1)) ||
			 (sqrt(s_pos_ges.px*s_pos_ges.px+s_pos_ges.py*s_pos_ges.py)<(s_ARM_A-s_ARM_B)))||
			  s_pos_ges.pz<(-s_ARM_C) || s_pos_ges.pz>0)
		{
			return ERROR;
		}
	}
//	else
//	{
//		if( ((sqrt(s_pos_ges.px*s_pos_ges.px+s_pos_ges.py*s_pos_ges.py)>(s_ARM_A+s_ARM_B-0.1)) ||
//			 (sqrt(s_pos_ges.px*s_pos_ges.px+s_pos_ges.py*s_pos_ges.py)<(s_ARM_A-s_ARM_B)))||
//			  s_pos_ges.pz>(s_ARM_C) || s_pos_ges.pz<0)
//		{
//			return ERROR;
//		}
//	}

	tmpag0 = atan2(s_pos_ges.py,s_pos_ges.px);
	tmpag1 = acos( (s_ARM_A*s_ARM_A+(s_pos_ges.px*s_pos_ges.px+s_pos_ges.py*s_pos_ges.py)-s_ARM_B*s_ARM_B) /
				   (2*s_ARM_A*sqrt(s_pos_ges.px*s_pos_ges.px+s_pos_ges.py*s_pos_ges.py)) );
	if(tmpag1>=PI)
	{
		tmpag1 -= PI;
	}
	if(tmpag1<0)
	{
		tmpag1 += PI;
	}
	tmpag2 = acos( (s_ARM_B*s_ARM_B+(s_pos_ges.px*s_pos_ges.px+s_pos_ges.py*s_pos_ges.py)-s_ARM_A*s_ARM_A) /
				   (2*s_ARM_B*sqrt(s_pos_ges.px*s_pos_ges.px+s_pos_ges.py*s_pos_ges.py)) );
	if(tmpag2>=PI)
	{
		tmpag1 -= PI;
	}
	if(tmpag2 < 0)
	{
		tmpag1 += PI;
	}

	ag1_1 = tmpag0 + tmpag1;
	if( ag1_1>PI )
		ag1_1 -= 2*PI;
	ag2_1 = -(tmpag1+tmpag2);

	ag1_2 = tmpag0 - tmpag1;
	if( ag1_2<=-PI )
		ag1_2 += 2*PI;
	ag2_2 = (tmpag1+tmpag2);

	if( fabs(s_best_angle[0]-ag1_2) < fabs(s_best_angle[0]-ag1_1) )
	{
		s_best_angle[0] = ag1_2;
		s_best_angle[1] = ag2_2;
	}
	else
	{
		s_best_angle[0] = ag1_1;
		s_best_angle[1] = ag2_1;
	}

	s_best_angle[3] = atan2(s_pos_ges.r21,s_pos_ges.r11)-s_best_angle[0]-s_best_angle[1];
	if(ROBOT_PARAMETER.SYS == FOUR_AXIS_A)	//�������е��
	{
		s_best_angle[2] = s_pos_ges.pz*((PI*2)/s_ARM_C);
	}
//	else if(ROBOT_PARAMETER.SYS == FOUR_AXIS_B)
//	{
//		s_best_angle[2] = (s_pos_ges.pz+s_best_angle[3]*PI_DIVIDE_2_INVERSE*s_ARM_LUOJU)*((PI*2)/s_ARM_C);
//	}
	else //if(ROBOT_PARAMETER.SYS == FOUR_AXIS_B)
	{
		s_best_angle[2] = (s_pos_ges.pz+s_best_angle[3]*PI_DIVIDE_2_INVERSE*s_ARM_LUOJU)*((PI*2)/s_ARM_C);
	}
//	else
//	{
//		s_best_angle[2] = -(s_pos_ges.pz+s_best_angle[3]*PI_DIVIDE_2_INVERSE*s_ARM_LUOJU)*((PI*2)/s_ARM_C);
//	}

	return 0;
}

/***************************************************
 * ��������:����������Ž�-6.4
 * ���������best_ag�����Ž�
 * ���ز�����0->�н�	1->�޽�
*/
u8 GetBestSolution(double *best_ag)
{
	static unsigned short sign,err,i;
	double cp_angle[20];
	double temp1,temp2,sc_angle[20];
	if(ROBOT_PARAMETER.AXIS_NUM == 4)
	{
		err = backward_explain();
		for(i=0;i<4;i++)
		{
			best_ag[i] = s_best_angle[i];
		}
		if(err == 0)
			return 0;
		else
			return 1;
	}
	else
	{
		if( (sqrt(s_pos_ges.px*s_pos_ges.px+s_pos_ges.py*s_pos_ges.py+
				s_pos_ges.pz*s_pos_ges.pz)>a2_a3_d4_len) )
		{
			return ERROR;
		}

/*
	sign = best_four_ag();
	switch(sign)
	{
		case 11: reset_angle(fp, sign, best_ag);	break;
		case 12: reset_angle(fp, sign, best_ag);	break;
		case 21: reset_angle(sp, sign, best_ag);	break;
		case 22: reset_angle(sp, sign, best_ag);	break;
		case 31: reset_angle(tp, sign, best_ag);	break;
		case 32: reset_angle(tp, sign, best_ag);	break;
		case 41: reset_angle(fop, sign, best_ag);	break;
		case 42: reset_angle(fop, sign, best_ag);	break;
		default: sign = 0;				         	break;
	}
*/
		//������2018��06��01��
		cp_angle[0] = getAngle3_1();
		temp1=ag3_0;
		sc_angle[0]=s[2];
		sc_angle[1]=c[2];
		cp_angle[1] = fabs(cp_angle[0] - s_best_angle[2]);

		cp_angle[2] = getAngle3_2();
		temp2=ag3_0;
		sc_angle[2]=s[2];
		sc_angle[3]=c[2];
		cp_angle[3] = fabs(cp_angle[2] - s_best_angle[2]);

		if(cp_angle[1]  > PI)	{cp_angle[1] = 2*PI - cp_angle[1];}
		if(cp_angle[3]  > PI)	{cp_angle[3] = 2*PI - cp_angle[3];}
		if(cp_angle[1] <= cp_angle[3])
		{
			best_ag[2] = cp_angle[0];
			ag3_0= temp1;
			s[2] = sc_angle[0];
			c[2] = sc_angle[1];
		}
		else
		{
			best_ag[2] = cp_angle[2];
			ag3_0= temp2;
			s[2] = sc_angle[2];
			c[2] = sc_angle[3];
		}

		cp_angle[0] = getAngle1_1();
		sc_angle[4]=s[0];
		sc_angle[5]=c[0];
		cp_angle[1] = fabs(cp_angle[0] - s_best_angle[0]);

		cp_angle[2] = getAngle1_2();
		sc_angle[6]=s[0];
		sc_angle[7]=c[0];
		cp_angle[3] = fabs(cp_angle[2] - s_best_angle[0]);

		if(cp_angle[1]  > PI)	{cp_angle[1] = 2*PI - cp_angle[1];}
		if(cp_angle[3]  > PI)	{cp_angle[3] = 2*PI - cp_angle[3];}
		if(cp_angle[1] <= cp_angle[3])
		{
			best_ag[0] = cp_angle[0];
			s[0] = sc_angle[4];
			c[0] = sc_angle[5];
		}
		else
		{
			best_ag[0] = cp_angle[2];
			s[0] = sc_angle[6];
			c[0] = sc_angle[7];
		}

		best_ag[1]=getAngle2();

		cp_angle[8] = getAngle4_1();
		sc_angle[8] = s[3];
		sc_angle[9] = c[3];
		cp_angle[9]  = fabs(cp_angle[8]-s_best_angle[3]);

		cp_angle[10] = getAngle4_2();
		sc_angle[10] = s[3];
		sc_angle[11] = c[3];
		cp_angle[11] = fabs(cp_angle[10]-s_best_angle[3]);

		if(cp_angle[9]  > PI)	{ cp_angle[9] = 2*PI - cp_angle[9];}
		if(cp_angle[11] > PI)	{ cp_angle[11] = 2*PI - cp_angle[11];}
		if(cp_angle[9] <= cp_angle[11])
		{
			best_ag[3] = cp_angle[8];
			s[3] = sc_angle[8];
			c[3] = sc_angle[9];
			best_ag[4] = getAngle5_1();
			cp_angle[12] = getAngle6_1();
		}
		else
		{
			best_ag[3] = cp_angle[10];
			s[3] = sc_angle[10];
			c[3] = sc_angle[11];
			best_ag[4] = getAngle5_2();
			cp_angle[12] = getAngle6_2();
		}

		if(( s_best_angle[5] > (PI-0.4))&&(cp_angle[12]<=0))
		{
			cp_angle[12] += 2*PI;
		}
		if(( s_best_angle[5] <= (-PI+0.4))&&(cp_angle[12]>0))
		{
			cp_angle[12] -= 2*PI;
		}
		if(cp_angle[12]>=0)
		{
			cp_angle[12] = fabs(cp_angle[12]-s_best_angle[5])<fabs(cp_angle[12]-2*PI-s_best_angle[5])?cp_angle[12]:cp_angle[12]-2*PI;
		}
		else
		{
			cp_angle[12] = fabs(cp_angle[12]-s_best_angle[5])<fabs(cp_angle[12]+2*PI-s_best_angle[5])?cp_angle[12]:cp_angle[12]+2*PI;
		}

		best_ag[5] = cp_angle[12];
		for(i=0; i<6; i++)
		{
			s_best_angle[i] =  *(best_ag + i);
		}

	}
	return 0;
}

/***************************************************
 * �������ܣ�����s_ag-6.5
 * ��������� a���е�һ�����
 * 		   sign�����Ž�ı��
 * 		   best_ag�����Ž�
*/
void reset_angle(double *a, u8 sign, double *best_ag)
{
	u8 i;
	for(i=0; i<3; i++)
	{
		s_best_angle[i] =  *(a+i);
		*(best_ag + i) =  *(a+i);
	}

	if( ( s_best_angle[5] > (PI-0.4) )
		 && ( (a[5]<=(0)) || (a[8]<=(0)) ) )
	{
		if(a[5]<=(0))
		{
			a[5] += 2*PI;
		}
		if(a[8]<=(0))
		{
			a[8] += 2*PI;
		}
	}
	if( ( s_best_angle[5] <= (-PI+0.4) )
		  && ( (a[5]>(0)) || (a[8]>(0)) ) )
	{
		if(a[5]>=(0))
		{
			a[5] -= 2*PI;
		}
		if(a[8]>=(0))
		{
			a[8] -= 2*PI;
		}
	}

	//
	if(a[5]>=0){
		a[5] = fabs(a[5]-s_best_angle[5])<fabs(a[5]-2*PI-s_best_angle[5])?a[5]:a[5]-2*PI;
	}
	else// if(a[5]<0)
	{
		a[5] = fabs(a[5]-s_best_angle[5])<fabs(a[5]+2*PI-s_best_angle[5])?a[5]:a[5]+2*PI;
	}
	//
	if(a[8]>=0)
	{
		a[8] = fabs(a[8]-s_best_angle[5])<fabs(a[8]-2*PI-s_best_angle[5])?a[8]:a[8]-2*PI;
	}
	else//if(a[8]<0)
	{
		a[8] = fabs(a[8]-s_best_angle[5])<fabs(a[8]+2*PI-s_best_angle[5])?a[8]:a[8]+2*PI;
	}

	if( sign%10 == 1 )
	{
		for(i=3; i<6; i++)
		{
			s_best_angle[i] =  *(a+i);
			*(best_ag + i) =  *(a+i);
		}
	}
	if( sign%10 == 2 )
	{
		for(i=3; i<6; i++)
		{
			s_best_angle[i] =  *(a+i+3);
			*(best_ag + i) =  *(a+i+3);
		}
	}
}



//��7�£��ⲿӦ�ú���
/********************************************************************************
* ��    �ܣ�ʵ��ǰ��������λ�˶Ա��Լ�һЩ����켣�ĳ���
* ˵	   ����
********************************************************************************/

/*************************************7.11
 * �������ܣ�����Ƕȣ����λ�ˣ�λ����̬��
*/
void UpdatePositionGesture(double *angle)
{
	 s_pos_ges = GetPositionGesture(angle);//���� �����̬λ��
}

/******************************************7.112
 * �������ܣ���ȡλ��
 */
void ModifyPositionGesture(PositionGesture p_g)
{
	 s_pos_ges = p_g;
}

/***************************************************7.12
 * �������ܣ���̬�Ƚϣ�������ĽǶȼ������̬�������ڵ���̬�Ƚ�
 * ���������double���͵�ָ��Ƕ�ֵ��ָ�룬�����Ƕ�
 * ���������0����̬һ��   1����̬��һ��
*/
unsigned char CompareCurrentGestureByAngle(double *next_angle)
{
	 PositionGesture new_pos_ges;
	 new_pos_ges = GetPositionGesture(next_angle);
	 return CompareTowGesture(new_pos_ges, s_pos_ges);
}

/*********************************************7.13
 * �������ܣ���̬�Ƚϣ��Ƚ���������ĽǶȼ������̬���Ƚ�
 * ���������double���͵�ָ��Ƕ�ֵ��ָ�룬�����Ƕ�
 * ���������0����̬һ��   1����̬��һ��
*/
unsigned char CompareGestureByTowAngle(double *angle1, double *angle2)
{
	 PositionGesture pos_ges1, pos_ges2;
	 pos_ges1 = GetPositionGesture(angle1);
	 pos_ges2 = GetPositionGesture(angle2);
	 return CompareTowGesture(pos_ges1, pos_ges2);
}

/*********************************************7.14
 * �������ܣ���̬�Ƚϣ��Ƚ������������̬�Ƿ���ͬ
 * ���������PositionGesture���͵���̬
 * ���������0����̬һ��   1����̬��һ��
*/
unsigned char CompareTowGesture(PositionGesture pos_ges1, PositionGesture pos_ges2)
{
	 unsigned char i,k;
	 double differ[9];

	 differ[0]  =  pos_ges1.r11 - pos_ges2.r11;
	 differ[1]  =  pos_ges1.r21 - pos_ges2.r21;
	 differ[2]  =  pos_ges1.r31 - pos_ges2.r31;
	 k = 3;
	 if(ROBOT_PARAMETER.AXIS_NUM != 4)
	 {
		 differ[3]  =  pos_ges1.r12 - pos_ges2.r12;
		 differ[4]  =  pos_ges1.r22 - pos_ges2.r22;
		 differ[5]  =  pos_ges1.r32 - pos_ges2.r32;
		 differ[6]  =  pos_ges1.r13 - pos_ges2.r13;
		 differ[7]  =  pos_ges1.r23 - pos_ges2.r23;
		 differ[8]  =  pos_ges1.r33 - pos_ges2.r33;
		 k = 9;
	 }

	 for(i=0; i<k; i++)
	 {
		  if( (differ[i]<-0.026) || (differ[i]>0.026) )//3000����ĽǶ�0.84375(���ٱ�64)
		  {
			   return 1;
		  }
	 }
	 return 0;
}

/*********************************************************7.21
 * �������ܣ�ƽ��px,py,pz
 * ���������delta_x  double���ͣ���Ҫ��x���ƶ��ľ��� ����λ��mm
 * 			 delta_y  double���ͣ���Ҫ��y���ƶ��ľ��� ����λ��mm
 * 			 delta_z  double���ͣ���Ҫ��z���ƶ��ľ��� ����λ��mm
 * ����ֵ��	 none
*/
void MovePxyz(double delta_x, double delta_y, double delta_z)
{
	 s_pos_ges.px  =  s_pos_ges.px + delta_x;
	 s_pos_ges.py  =  s_pos_ges.py + delta_y;
	 s_pos_ges.pz  =  s_pos_ges.pz + delta_z;
}

/******************************************7.22
 * �������ܣ��ƶ�px
 * ���������delta_x  double���ͣ���Ҫ�ƶ��ľ��� ����λ��mm
 * ����ֵ��	 none
*/
void MovePx(double delta_x)
{
	 s_pos_ges.px = s_pos_ges.px + delta_x;
}

/******************************************7.23
 * �������ܣ��ƶ�py
 * ���������delta_x  double���ͣ���Ҫ�ƶ��ľ��� ����λ��mm
 * ����ֵ��	 none
*/
void MovePy(double delta_y)
{
	 s_pos_ges.py = s_pos_ges.py + delta_y;
}

/******************************************7.24
 * �������ܣ��ƶ�pz
 * ���������delta_x  double���ͣ���Ҫ�ƶ��ľ��� ����λ��mm
 * ����ֵ��	 none
*/
void MovePz(double delta_z)
{
	 s_pos_ges.pz = s_pos_ges.pz + delta_z;
}

/******************************************7.31
 * �������ܣ��޸�px��py��pzλ��ֵ
*/
void ModifyPositionValue(double px,double py,double pz)
{
	 s_pos_ges.px = px;
	 s_pos_ges.py = py;
	 s_pos_ges.pz = pz;
}

/******************************************7.32
 * �������ܣ��޸�pxλ��ֵ
*/
void ModifyPxValue(double px)
{
	 s_pos_ges.px = px;
}

/******************************************7.33
 * �������ܣ��޸�pyλ��ֵ
*/
void ModifyPyValue(double py)
{
	 s_pos_ges.py = py;
}

/******************************************7.34
 * �������ܣ��޸�pzλ��ֵ
*/
void ModifyPzValue(double pz)
{
	 s_pos_ges.pz = pz;
}

/******************************************7.41
 * �������ܣ�����ĩ�������x����ֱ���˶�
 * ���������һ���ƶ�����С��λ
 * ����ֵ��    none
*/
void straight_x(float x1)
{
	s_pos_ges.px += (s_pos_ges.r11*x1);
	s_pos_ges.py += (s_pos_ges.r21*x1);
	s_pos_ges.pz += (s_pos_ges.r31*x1);
}

/******************************************7.42
 * �������ܣ�����ĩ�������y����ֱ���˶�
 * ���������һ���ƶ�����С��λ
 * ����ֵ��    none
*/
void straight_y(float y1)
{
	s_pos_ges.px += s_pos_ges.r12*y1;
	s_pos_ges.py += s_pos_ges.r22*y1;
	s_pos_ges.pz += s_pos_ges.r32*y1;
}

/***********************************************7.43
 * �������ܣ�����ĩ�������z����ֱ���˶�
 * ���������һ���ƶ�����С��λ
 * ����ֵ��    none
*/
void straight_z(float z1)
{
	s_pos_ges.px  +=  s_pos_ges.r13*z1;
	s_pos_ges.py  +=  s_pos_ges.r23*z1;
	s_pos_ges.pz  +=  s_pos_ges.r33*z1;
}

/************************************************7.51
 * �������ܣ���Բ��
 * ���������delta_x  double���ͣ���Ҫ�ƶ��ľ��� ����λ��mm
 * ����ֵ��    none
*/
void circle_parameter(double move_x,double move_y)
{
	//��һ�������ݵ�ǰ�����ͱ仯��xyֵ���Բ��λ�ã������棬Բ��ֻ����һ��
	o[0]   =   s_pos_ges.px + move_x;
	o[1]   =   s_pos_ges.py + move_y;//���Բ��,o[0]����x��ֵ��o[1]����y��ֵ
	o[2]   =   sqrt( (move_y)*(move_y) + (move_x)*(move_x) );//�뾶����
	o[3]   =   atan2( -move_y, -move_x );//��ǰ�Ƕ�
}

/***************************************************7.52
 * �������ܣ���Բ��
 * ���������precision  double���ͣ�ת��һ�ε���СԲ���ȣ���λ������
 * ����ֵ��    none
*/
void circle_xy(double precision,double delta_height)//precisionΪִ��һ�α仯�Ļ��ȣ����ȣ��������ɸ���
{
	o[3]   =   o[3] + precision;
	s_pos_ges.px   =   o[2]*cos(o[3]) + o[0];
	s_pos_ges.py   =   o[2]*sin(o[3]) + o[1];
	s_pos_ges.pz   =   s_pos_ges.pz + delta_height;
}

/***************************************************7.52
 * �������ܣ��ѽṹ�������ֵת����������ȥ
 * ���������p1��
*/
void structrue_to_array(const PositionGesture *p1, double *p)
{
	*(p+0) = p1->px;
	*(p+1) = p1->py;
	*(p+2) = p1->pz;
	p1++;
	*(p+3) = p1->px;
	*(p+4) = p1->py;
	*(p+5) = p1->pz;
	p1++;
	*(p+6) = p1->px;
	*(p+7) = p1->py;
	*(p+8) = p1->pz;
}
/***************************************************7.52
 * �������ܣ��ж������Ƿ�����Բ��
 * ���������x:x[0]~x[2]:Բ����ʼ�㣬x[3]~x[5]:Բ���м�㣬x[6]~x[8]:Բ���յ�
 * ���������0���������ȷ��Բ����2��12���غϣ�3��13���غϣ�4��23���غϣ�5�������һֱ��
*/
u8 judge_three_point(double x[9])
{
	double sign;
	sign =( ( (x[4]-x[1])/(x[3]-x[0]) ) - ( (x[7]-x[4])/(x[6]-x[3]) ) );

	if( (x[3]-x[0]==0) && (x[4]-x[1]==0) && (x[5]-x[2]==0) )	{ return 2;}
	if( (x[6]-x[0]==0) && (x[7]-x[1]==0) && (x[8]-x[2]==0) )	{ return 3;}
	if( (x[6]-x[3]==0) && (x[7]-x[4]==0) && (x[8]-x[5]==0) )	{ return 4;}

	if( (sign<=EPSINON) && (sign>=-EPSINON) )
	{
		return 5;
	}

	return 0;
}

/***************************************************7.52
 * �������ܣ���Բ������
 * ���������x:x[0]~x[2]:Բ����ʼ�㣬x[3]~x[5]:Բ���м�㣬x[6]~x[8]:Բ���յ�
 * ���������P:����Բ������
*/
static void arc_parameter(double x[9], double *o)
{
	double d1,d2,d3,c1,c2,c3,c;

	d1 = (x[6]-x[0])*(x[3]-x[0])+(x[7]-x[1])*(x[4]-x[1])+(x[8]-x[2])*(x[5]-x[2]);
	d2 = (x[6]-x[3])*(x[0]-x[3])+(x[7]-x[4])*(x[1]-x[4])+(x[8]-x[5])*(x[2]-x[5]);
	d3 = (x[0]-x[6])*(x[3]-x[6])+(x[1]-x[7])*(x[4]-x[7])+(x[2]-x[8])*(x[5]-x[8]);
	c1 = d2*d3;
	c2 = d1*d3;
	c3 = d1*d2;
	c  = c1+c2+c3;

	o[0] = sqrt((d1+d2)*(d2+d3)*(d1+d3)/c) * 0.5;
	o[1] = ( (c2+c3)*x[0]+(c1+c3)*x[3]+(c1+c2)*x[6] ) / (2*c);
	o[2] = ( (c2+c3)*x[1]+(c1+c3)*x[4]+(c1+c2)*x[7] ) / (2*c);
	o[3] = ( (c2+c3)*x[2]+(c1+c3)*x[5]+(c1+c2)*x[8] ) / (2*c);
}

/***************************************************7.52
 * �������ܣ���������ϵ
 * ���������x:x[0]~x[2]:Բ����ʼ�㣬x[3]~x[5]:Բ���м�㣬x[6]~x[8]:Բ���յ�
*/
static void new_coordinate(double x[9], const double *o, double *px)
{
	double value;

	px[0]  =  x[0] - o[1];
	px[1]  =  x[1] - o[2];
	px[2]  =  x[2] - o[3];
	value  =  sqrt( px[0]*px[0] + px[1]*px[1] + px[2]*px[2] );
	px[0]  =  px[0]/value;
	px[1]  =  px[1]/value;
	px[2]  =  px[2]/value;

	px[6]  =   x[1]*x[5] - x[1]*x[8] - x[4]*x[2] + x[4]*x[8] + x[7]*x[2] - x[7]*x[5];
	px[7]  =  -x[0]*x[5] + x[0]*x[8] + x[3]*x[2] - x[3]*x[8] - x[6]*x[2] + x[6]*x[5];
	px[8]  =   x[0]*x[4] - x[0]*x[7] - x[3]*x[1] + x[3]*x[7] + x[6]*x[1] - x[6]*x[4];
	value  =  sqrt( px[6]*px[6] + px[7]*px[7] + px[8]*px[8] );
	px[6]  =  px[6]/value;
	px[7]  =  px[7]/value;
	px[8]  =  px[8]/value;

	px[3]  =  -px[1]*px[8] + px[7]*px[2];
	px[4]  =   px[0]*px[8] - px[6]*px[2];
	px[5]  =  -px[0]*px[7] + px[6]*px[1];
}

/***************************************************7.52
 * �������ܣ��л�����
 * ���������x:x[0]~x[2]:Բ����ʼ�㣬x[3]~x[5]:Բ���м�㣬x[6]~x[8]:Բ���յ�
*/
static void x_to_x_px(double x[9], const double *o, const double *px, double *x_px)
{
	double ag1,ag2;
	double current_ag1=0.0,current_ag2=0.0,current_ag3=0.0;

	x_px[0] = x[0]*px[0] + x[1]*px[1] + x[2]*px[2];
	x_px[1] = x[0]*px[3] + x[1]*px[4] + x[2]*px[5];
	x_px[2] = x[3]*px[0] + x[4]*px[1] + x[5]*px[2];
	x_px[3] = x[3]*px[3] + x[4]*px[4] + x[5]*px[5];
	x_px[4] = x[6]*px[0] + x[7]*px[1] + x[8]*px[2];
	x_px[5] = x[6]*px[3] + x[7]*px[4] + x[8]*px[5];
	x_px[6] = o[1]*px[0] + o[2]*px[1] + o[3]*px[2];
	x_px[7] = o[1]*px[3] + o[2]*px[4] + o[3]*px[5];
	x_px[8] = o[1]*px[6] + o[2]*px[7] + o[3]*px[8];

	current_ag1 = atan2(x_px[1]-x_px[7],x_px[0]-x_px[6]);
	current_ag2 = atan2(x_px[3]-x_px[7],x_px[2]-x_px[6]);
	current_ag3 = atan2(x_px[5]-x_px[7],x_px[4]-x_px[6]);

	ag1 = current_ag2-current_ag1;
	if(ag1>PI){ag1 -= 2*PI;}
	if(ag1<=-PI){ag1 += 2*PI;}

	ag2 = current_ag3-current_ag1;
	if(ag2>PI){ag2 -= 2*PI;}
	if(ag2<=-PI){ag2 += 2*PI;}

	if(ag1>0)
	{
		if((ag2>0)&&(ag2<ag1))//�����򻭻�
		{
			x_px[10] = 2*atan2(0,-1)-ag2;
			x_px[9] = 1;
		}
		else//�����򻭻�
		{
			if(ag2>0) {x_px[10] = ag2;}
			else 	  {x_px[10] = 2*atan2(0,-1)+ag2;}
			x_px[9] = 0;
		}
	}
	if(ag1<0)
	{
		if((ag2<0)&&(ag2>ag1))//�����򻭻�
		{
			x_px[10] = 2*atan2(0,-1)+ag2;
			x_px[9] = 0;
		}
		else//�����򻭻�
		{
			if(ag2<0) {x_px[10] = -ag2;}
			else 	  {x_px[10] = 2*atan2(0,-1)-ag2;}
			x_px[9] = 1;
		}
	}
}

/***************************************************7.52
 * �������ܣ���Բ����
 * ���������x:x[0]~x[2]:Բ����ʼ�㣬x[3]~x[5]:Բ���м�㣬x[6]~x[8]:Բ���յ�
 * ���������flag��0λ��0:Բ����1:��Բ��
 * 			 flag��1λ��0:��ֹͣ�����ִ�У�1:��ͷ��ʼ
 * ���������0������ִ�У�1��ִ�е����һ���㣬����ִ��
 */
static u8 next_arc_point(u8 flag, double x[9], double *px, double *x_px, double *o)
{
	double x_p4,y_p4,z_p4;
	static u32 i=0, circle_num=0;
	static double precision=0.01;
	static double current_angle;

	if((1<<1) & flag)
	{
		precision = ROBOT_PARAMETER.PRECISION/o[0];
		if((1<<0) & flag) {	x_px[10] = 2*atan2(0,-1);}
		circle_num = (u32)( x_px[10] / precision );
		current_angle = 0;
		i = 0;
		return 0;
	}

	if( i == circle_num )//���һ����
	{
/*		if((1<<0) & flag)
		{
			s_pos_ges.px = x[0];
			s_pos_ges.py = x[1];
			s_pos_ges.pz = x[2];
		}
		else
		{
			s_pos_ges.px = x[6];
			s_pos_ges.py = x[7];
			s_pos_ges.pz = x[8];
		}*/
		return 1;
	}
	if( 1 == x_px[9] )//��ʱ�뻭��
	{
		current_angle -= precision;
		x_p4 = o[0]*cos(current_angle) + x_px[6] ;//+ o[1]
		y_p4 = o[0]*sin(current_angle) + x_px[7];//+ o[2]
		z_p4 = x_px[8];
	}
	if( 0 == x_px[9] )//˳ʱ�뻭��
	{
		current_angle += precision;
		x_p4 = o[0]*cos(current_angle) + x_px[6] ;//+ o[1]
		y_p4 = o[0]*sin(current_angle) + x_px[7];//+ o[2]
		z_p4 = x_px[8];
	}
	i++;
	s_pos_ges.px = x_p4*px[0] + y_p4*px[3] + z_p4*px[6];
	s_pos_ges.py = x_p4*px[1] + y_p4*px[4] + z_p4*px[7];
	s_pos_ges.pz = x_p4*px[2] + y_p4*px[5] + z_p4*px[8];

	return 0;
}

/***************************************************7.52
 * �������ܣ���Բ����
 * ���������p1[0]:Բ����ʼ�㣬p1[1]:Բ���м�㣬p[2]:Բ���յ�
 * ���������flag��0λ��0Բ����1��Բ��
 * 			 flag��1λ��0��ֹͣ�����ִ�У�1��ͷ��ʼ
 * ���������0����ִ�У�1����ִ�У�2��ʼ����м���غϣ�3��ʼ����յ��غϣ�4�м����յ��غϣ�5������ͬһֱ��
*/
u8 drive_arc(const PositionGesture* p1, u8 flag)
{
	u8 sign1;
	static u8 sign=1;
	static double o[4];
	static double x[9];
	static double px[9];
	static double x_px[11];

	if((0x01<<1) & flag)	{sign = 1;}

	if(0 == sign)//����ִ��
	{
		sign = next_arc_point(flag, x, px, x_px, o);
		return sign;
	}
	if(1 == sign)//��ͷ��ʼ
	{
		structrue_to_array(&p1[0], x);
		sign1 = judge_three_point(x);
		if(sign1 > 0){return sign1;}
		s_pos_ges = p1[0];
		arc_parameter(x, o);
		new_coordinate(x, o, px);
		x_to_x_px(x, o, px, x_px);
		next_arc_point(flag, x, px, x_px, o);
		sign = 0;
		return 0;
	}
	return 1;
}


/***************************************************7.52
 * �������ܣ����ݵ������ŷ���ǽ���ת��
 * ���������p_e:λ�ú�ŷ����
 * 			p_g:PositionGesture���͵�ָ�룬���ڱ�������
 * 			toolLength:���߳���
 * ���������NONE
*/

void ToolPointToEndPoint(PositionEuler p_e, PositionGesture* p_g, double toolLength)
{
	//�û������ʾ�ĵ��ߵ������
	//double jdx=arr[0],jdy=arr[1],jdz=arr[2];
	//double jdx,jdy,jdz;
	//jdx = arr1[0];	jdy = arr1[1];	jdz = arr1[2];
	//���߳��ȣ���λ�Ǻ���
	//double toolLength=20;
	//���ڻ������ĩ�˵�
	//double jx,jy,jz;
	//ŷ����,ag1��ʾ����x���ת��������ag2��ʾ����Y���ת��������ag3��ʾ����Z���ת������
	//�������������ֶ��򣬴�Ĵָָ��ת��������������ֵķ���Ϊŷ���ǵ�������
	//double ag1=arr[3], ag2=arr[4],ag3=arr[5];
	//double ag1,ag2,ag3;
	//ag1 = arr1[3]; ag2 = arr1[4]; ag3 = arr1[5];
	//�����н�
	double jg1,jg2;
	double length1,length2,length3;

	//ת��ag1��ag2��ag3�ķ�Χ��[-PI~PI]
	if(p_e.Rx>PI){
		p_e.Rx=p_e.Rx-2*PI;
	}
	if(p_e.Rx<-PI){
		p_e.Rx=p_e.Rx+2*PI;
	}
	if(p_e.Ry>PI){
		p_e.Ry=p_e.Ry-2*PI;
	}
	if(p_e.Ry<-PI){
		p_e.Ry=p_e.Ry+2*PI;
	}
	if(p_e.Rz>PI){
		p_e.Rz=p_e.Rz-2*PI;
	}
	if(p_e.Rz<-PI){
		p_e.Rz=p_e.Rz+2*PI;
	}

//��1��������ŷ���Ǻ�ĩ�˵�λ����N����ڻ������ϵķֽ�ֵ
//�൱�ڻ�е��ģ���е�r11,r12,r13���������浽������
	(*p_g).r11 = fabs(cos(p_e.Ry))*cos(p_e.Rz);
	(*p_g).r21 = fabs(cos(p_e.Ry))*sin(p_e.Rz);
	(*p_g).r31 = -sin(p_e.Ry);

	//��Ϊ��ߵ�ĩ������ϵ��powermill�����ĩ������ϵ��ʼ��̬��ͬ������Y����ת��180�ȿ����غϣ�
	//����x��z�պø�ԭ��Գƣ����ȡ��
	(*p_g).r11 = -(*p_g).r11;
	(*p_g).r21 = -(*p_g).r21;
	(*p_g).r31 = -(*p_g).r31;

//��2��������ŷ���Ǻ�ĩ�˵�λ����O����ڻ������ϵķֽ�ֵ
//�൱�ڻ�е��ģ���е�r21,r22,r23���������浽������
	//Oz�ĳ���
	length1=fabs(sin(p_e.Rx)*cos(p_e.Ry));
	//ͶӰ����
	length2 = sqrt(1*1-length1*length1);
	//ͶӰ�㵽Y��ľ���
	length3 = sqrt( fabs(1*sin(p_e.Rx)*1*sin(p_e.Rx))-length1*length1 );
	//printf("length3=%lf\n",length3);
	//ͶӰ�߸�Y��ļнǾ���ֵ
	jg1 = asin(length3/length2);
	//�жϼнǵľ���ָ
	jg2=0;
	//Y����X����ת������Y-Zƽ������ĵ�1����
	if(p_e.Rx>=0 && p_e.Rx<=PI*0.5){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=-jg1;
		}else{
			jg2=jg1;
		}
	}
	//Y����X����ת������Y-Zƽ������ĵ�4����
	if(p_e.Rx>=-PI*0.5 && p_e.Rx<0){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=jg1;
		}else{
			jg2=-jg1;
		}
	}
	//Y����X����ת������Y-Zƽ������ĵ�2����
	if(p_e.Rx>PI*0.5 && p_e.Rx<=PI){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=jg1-PI;
		}else{
			jg2=PI-jg1;
		}
	}
	//Y����X����ת������Y-Zƽ������ĵ�3����
	if(p_e.Rx>=-PI && p_e.Rx<-PI*0.5){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=PI-jg1;
		}else{
			jg2=jg1-PI;
		}
	}
	//���µĽ��������mdoΪ��ֵ��ʱ��Ľ�������Ϊ����������ȡ��
	(*p_g).r12 = -length2*sin(jg2+p_e.Rz);
	(*p_g).r22 = length2*cos(jg2+p_e.Rz);
	(*p_g).r32 = sin(p_e.Rx)*cos(p_e.Ry);
	//printf("jg2=%lf\n\n",jg2/PI*180);




//��3��������ŷ���Ǻ�ĩ�˵�λ����A����ڻ������ϵķֽ�ֵ
//�൱�ڻ�е��ģ���е�r31,r32,r33���������浽������
	//Oz�ĳ���
	 length1=fabs(cos(p_e.Rx)*cos(p_e.Ry)*1);
	//ͶӰ����
	 length2 = sqrt(1*1-length1*length1);
	//ͶӰ�㵽Y��ľ���
	 length3 = sqrt( fabs(1*cos(p_e.Rx)*1*cos(p_e.Rx))-length1*length1 );
	//ͶӰ�߸�Y��ļнǾ���ֵ
	jg1 = asin(length3/length2);
	//�жϼнǵľ���ָ
	if(p_e.Rx>=-PI && p_e.Rx<-PI*0.5){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=jg1;
		}else{
			jg2=-jg1;
		}
	}
	if(p_e.Rx>=-PI*0.5 && p_e.Rx<0){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=-jg1;
		}else{
			jg2=jg1;
		}
	}
	if(p_e.Rx>=0 && p_e.Rx<=PI*0.5){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=jg1-PI;
		}else{
			jg2=PI-jg1;
		}
	}
	if(p_e.Rx>PI*0.5 && p_e.Rx<=PI){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=PI-jg1;
		}else{
			jg2=jg1-PI;
		}
	}
	//
	(*p_g).r13 = -length2*sin(jg2+p_e.Rz);
	(*p_g).r23 = length2*cos(jg2+p_e.Rz);
	(*p_g).r33 = cos(p_e.Rx)*cos(p_e.Ry);

	//��Ϊ��ߵ�ĩ������ϵ��powermill�����ĩ������ϵ��ʼ��̬��ͬ������Y����ת��180�ȿ����غϣ�
	//����x��z�պø�ԭ��Գƣ����ȡ��
	(*p_g).r13 = -(*p_g).r13;
	(*p_g).r23 = -(*p_g).r23;
	(*p_g).r33 = -(*p_g).r33;
	//printf("nx=%lf\nny=%lf\nnz=%lf\n\n",Nx,Ny,Nz);
	//printf("ox=%lf\noy=%lf\noz=%lf\n\n",Ox,Oy,Oz);
	//printf("Ax=%lf\nAy=%lf\nAz=%lf\n\n",Ax,Ay,Az);

//���Ĳ������ĩ������
	//��⵶�����������ߵ������ȥ�����������ɵõ�ĩ������
	(*p_g).px = p_e.Px - (toolLength+TOOL_LENGTH)*(*p_g).r13;
	(*p_g).py = (p_e.Py - (toolLength+TOOL_LENGTH)*(*p_g).r23);
	(*p_g).pz = p_e.Pz - (toolLength+TOOL_LENGTH)*(*p_g).r33;

	//printf("jx=%lf\njy=%lf\njz=%lf\n\n",jx,jy,jz);
}


//��һ������ĩ������תΪ���������ʾ
/***************************************************7.52
 * �������ܣ�����ŷ���ǽ�һ�������������λ����ת��Ϊ��һ�������ʾ
 * ���������p_e:λ�ú�ŷ����
 * 			 p_g:PositionGesture���͵�ָ�룬���ڱ�����õľŸ�����
 * ���������NONE
*/
void CoordinateConversionByEulerAngles(PositionEuler p_e, PositionGesture* p_g)
{
	double jg1,jg2;
	double length1,length2,length3;

	//ת��ag1��ag2��ag3�ķ�Χ��[-PI~PI]
//	if(p_e.Rx>PI){
//		p_e.Rx=p_e.Rx-2*PI;
//	}
//	if(p_e.Rx<-PI){
//		p_e.Rx=p_e.Rx+2*PI;
//	}
//	if(p_e.Ry>PI){
//		p_e.Ry=p_e.Ry-2*PI;
//	}
//	if(p_e.Ry<-PI){
//		p_e.Ry=p_e.Ry+2*PI;
//	}
//	if(p_e.Rz>PI){
//		p_e.Rz=p_e.Rz-2*PI;
//	}
//	if(p_e.Rz<-PI){
//		p_e.Rz=p_e.Rz+2*PI;
//	}

//��1��������ŷ���Ǻ�ĩ�˵�λ����N����ڻ������ϵķֽ�ֵ
//�൱�ڻ�е��ģ���е�r11,r12,r13���������浽������
	(*p_g).r11 = fabs(cos(p_e.Ry))*cos(p_e.Rz);
	(*p_g).r21 = fabs(cos(p_e.Ry))*sin(p_e.Rz);
	(*p_g).r31 = -sin(p_e.Ry);

//��2��������ŷ���Ǻ�ĩ�˵�λ����O����ڻ������ϵķֽ�ֵ
//�൱�ڻ�е��ģ���е�r21,r22,r23���������浽������
	//Oz�ĳ���
	length1=fabs(sin(p_e.Rx)*cos(p_e.Ry));
	//ͶӰ����
	length2 = sqrt(1*1-length1*length1);
	//ͶӰ�㵽Y��ľ���
	length3 = sqrt( fabs(1*sin(p_e.Rx)*1*sin(p_e.Rx))-length1*length1 );
	//printf("length3=%lf\n",length3);
	//ͶӰ�߸�Y��ļнǾ���ֵ
	jg1 = asin(length3/length2);
	//�жϼнǵľ���ָ
	jg2=0;
	//Y����X����ת������Y-Zƽ������ĵ�1����
	if(p_e.Rx>=0 && p_e.Rx<=PI*0.5){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=-jg1;
		}else{
			jg2=jg1;
		}
	}
	//Y����X����ת������Y-Zƽ������ĵ�4����
	if(p_e.Rx>=-PI*0.5 && p_e.Rx<0){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=jg1;
		}else{
			jg2=-jg1;
		}
	}
	//Y����X����ת������Y-Zƽ������ĵ�2����
	if(p_e.Rx>PI*0.5 && p_e.Rx<=PI){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=jg1-PI;
		}else{
			jg2=PI-jg1;
		}
	}
	//Y����X����ת������Y-Zƽ������ĵ�3����
	if(p_e.Rx>=-PI && p_e.Rx<-PI*0.5){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=PI-jg1;
		}else{
			jg2=jg1-PI;
		}
	}
	//���µĽ��������mdoΪ��ֵ��ʱ��Ľ�������Ϊ����������ȡ��
	(*p_g).r12 = -length2*sin(jg2+p_e.Rz);
	(*p_g).r22 = length2*cos(jg2+p_e.Rz);
	(*p_g).r32 = sin(p_e.Rx)*cos(p_e.Ry);

//��3��������ŷ���Ǻ�ĩ�˵�λ����A����ڻ������ϵķֽ�ֵ
//�൱�ڻ�е��ģ���е�r31,r32,r33���������浽������
	//Oz�ĳ���
	 length1=fabs(cos(p_e.Rx)*cos(p_e.Ry)*1);
	//ͶӰ����
	 length2 = sqrt(1*1-length1*length1);
	//ͶӰ�㵽Y��ľ���
	 length3 = sqrt( fabs(1*cos(p_e.Rx)*1*cos(p_e.Rx))-length1*length1 );
	//ͶӰ�߸�Y��ļнǾ���ֵ
	jg1 = asin(length3/length2);
	//�жϼнǵľ���ָ
	if(p_e.Rx>=-PI && p_e.Rx<-PI*0.5){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=jg1;
		}else{
			jg2=-jg1;
		}
	}
	if(p_e.Rx>=-PI*0.5 && p_e.Rx<0){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=-jg1;
		}else{
			jg2=jg1;
		}
	}
	if(p_e.Rx>=0 && p_e.Rx<=PI*0.5){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=jg1-PI;
		}else{
			jg2=PI-jg1;
		}
	}
	if(p_e.Rx>PI*0.5 && p_e.Rx<=PI){
		if(p_e.Ry>=0 && p_e.Ry<=PI){
			jg2=PI-jg1;
		}else{
			jg2=jg1-PI;
		}
	}
	(*p_g).r13 = -length2*sin(jg2+p_e.Rz);
	(*p_g).r23 = length2*cos(jg2+p_e.Rz);
	(*p_g).r33 = cos(p_e.Rx)*cos(p_e.Ry);

}





/*
 *
 */
void hanshuming1(PositionEuler *p_e0, PositionEuler p_e, PositionGesture p_g)
{
	(*p_e0).Px = p_e.Px*p_g.r11 + p_e.Py*p_g.r12 + p_e.Pz*p_g.r13;
	(*p_e0).Py = p_e.Px*p_g.r21 + p_e.Py*p_g.r22 + p_e.Pz*p_g.r23;
	(*p_e0).Pz = p_e.Px*p_g.r31 + p_e.Py*p_g.r32 + p_e.Pz*p_g.r33;
}


/*
 * ����һ���б��ֵ������������XYZ��ĩ������ģ��ֽ⵽���������
 */
void hanshuming2(PositionGesture* p_g3, PositionGesture p_g1, PositionGesture p_g2)
{
	//ĩ������X��λ��������������ϵı�ʾ
	(*p_g3).r11 = p_g1.r11*p_g2.r11 + p_g1.r21*p_g2.r12 + p_g1.r31*p_g2.r13;
	(*p_g3).r21 = p_g1.r11*p_g2.r21 + p_g1.r21*p_g2.r22 + p_g1.r31*p_g2.r23;
	(*p_g3).r31 = p_g1.r11*p_g2.r31 + p_g1.r21*p_g2.r32 + p_g1.r31*p_g2.r33;

	//ĩ������Y��λ��������������ϵı�ʾ
	(*p_g3).r12 = p_g1.r12*p_g2.r11 + p_g1.r22*p_g2.r12 + p_g1.r32*p_g2.r13;
	(*p_g3).r22 = p_g1.r12*p_g2.r21 + p_g1.r22*p_g2.r22 + p_g1.r32*p_g2.r23;
	(*p_g3).r32 = p_g1.r12*p_g2.r31 + p_g1.r22*p_g2.r32 + p_g1.r32*p_g2.r33;

	//ĩ������Z��λ��������������ϵı�ʾ
	(*p_g3).r13 = p_g1.r13*p_g2.r11 + p_g1.r23*p_g2.r12 + p_g1.r33*p_g2.r13;
	(*p_g3).r23 = p_g1.r13*p_g2.r21 + p_g1.r23*p_g2.r22 + p_g1.r33*p_g2.r23;
	(*p_g3).r33 = p_g1.r13*p_g2.r31 + p_g1.r23*p_g2.r32 + p_g1.r33*p_g2.r33;
}

/*
 * ���ݼӹ�����������������е��ĩ�˵�����
 * �Ӷ����ڽ�һ��������
 */
void hanshuming3(PositionEuler* p_e5, PositionEuler p_e4, PositionEuler p_e0, PositionGesture p_g5, double Length)
{
	(*p_e5).Px = p_e4.Px + p_e0.Px - Length*p_g5.r13;
	(*p_e5).Py = p_e4.Py + p_e0.Py - Length*p_g5.r23;
	(*p_e5).Pz = p_e4.Pz + p_e0.Pz - Length*p_g5.r33;
}

/***************************************************7.52
 * �������ܣ��������ת��Ϊ��е��ĩ�������ʾ
 * ���������tool_end:����ĩ�����е��ĩ�˵�λ�ù�ϵ
 * 			part_rob:���ӹ�������ο��������е�ֻ������λ�ù�ϵ
 * 			part:��������������ο�����Ĺ�ϵ(���ӹ�����ļӹ��켣����ο���������ϵģ�ĳ����,�Ʋο���)
 * 			len:5���6��Ľ��㣬��6��ƽ��ĩ������ԭ��ĳ���(����V6350С��е����˵��89����)
 * ���������ת���õ�������������̬
 */
PositionGesture PartPosToToolPos(ToolParameter tool_end,PartParameter part_rob,PositionEuler part,double len)
{
	PositionGesture p_g1,p_g2,p_g3,p_g4;
	PositionEuler p_e0,p_e4,p_e5;
	PositionGesture p_g;
	tool_end.PosEuler.Pz += tool_end.ToolLength;	//���ϵ��߳��Ⱦ��ǵ���ĩ�˵��λ��
	CoordinateConversionByEulerAngles(tool_end.PosEuler, &p_g1);
//	part.Rz = part.Rx;
//	part.Rx = 0;
	CoordinateConversionByEulerAngles(part,&p_g2);
	hanshuming2(&p_g3, p_g1, p_g2);
	p_e0 = tool_end.PosEuler;
	hanshuming1(&p_e0, p_e0, p_g2);
	CoordinateConversionByEulerAngles(part_rob.PosEuler, &p_g4);
	hanshuming2(&p_g, p_g3, p_g4);
	hanshuming1(&p_e0, p_e0, p_g4);
	hanshuming1(&p_e4, part, p_g4);
	p_e4.Px += part_rob.PosEuler.Px;
	p_e4.Py += part_rob.PosEuler.Py;
	p_e4.Pz += part_rob.PosEuler.Pz;
	hanshuming3(&p_e5, p_e4, p_e0, p_g, len);
	p_g.px = p_e5.Px;
	p_g.py = p_e5.Py;
	p_g.pz = p_e5.Pz - 280;	//���ǵĻ�е���㷨��ԭ����3D�����е�ֽ�ģ��ԭ�㲻һ����Z�������280mm
	return p_g;
}











