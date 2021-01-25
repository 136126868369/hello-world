/**********************************************************************
 * �ŷ�����14400������ÿȦ
 * Ƶ��=Ȧ��*14400/60
 * 3000r/min��Ӧ720kHz  240HZ=1r/mis
 * 1C��Ӧ40P
 * ���ӶԾ�������ļ�أ��㶯���⣬���5ms�жϳ����ŷ����������⡣
 **********************************************************************/
#include "EditProgram.h"
#include "My_Project.h"
#include "ReadEncoder.h"
#include "Man_MCX31xAS.h"
#include "Man_Nandflash.h"
#include "Man_DriverScreen.h"
#include "EditProgram.h"
#include "ScreenApi.h"
#include "kinematic_explain.h"
#include "KeyboardDriver.h"
#include "Touch_screen.h"
#include "Sanxie_Servo.h"
// ======================================================================
// ��    �ܣ� �Ӻ�������**
// ======================================================================
void InitMcu (void);
void InitData(void);
//PositionGesture pg1,pg2;
#define len	50
char str[9];
unsigned char Teach_mode=0;//������//2018/06/01
/*************************************
 * �������ܣ���ʼ����е�ֲ���
 * ���������axis_num ����� 5��6
 * ���������NONE
 */
void InitRobotPara(unsigned char axis_num);

/*************************************
 * �������ܣ���ʼ����е��״̬
 * ���������NONE
 * ���������NONE
 */
void InitRobotState(void);
/*======================================================================
* ��    �ƣ� void main(void)
* ��    �ܣ� ������
* ��ڲ����� ��
* ���ڲ����� ��
======================================================================*/
//long pulse[6]={100000,-200000,300000,-400000,500000,-600000};
//long pulse_tmp[6],i;

 void main(void)

 {
//	 u8 data[4],err;
//	 u16 tmp;
	/***********************
	 *
	 *
	 */
     InitMcu();

     //������ָ��
//      unsigned char code_err,err;
//      char data0[54]="part ( -0.222 , +123.123, +0 ,-456, -0.234, +55.432)";
//      code_err = CheckGrammar((PrgCode*)data0);//����﷨
//      err = ProgramStepRun_Code(code_err,((PrgCode*)data0));
//
//      char data[35]="part.px= -5.67";
//      code_err = CheckGrammar((PrgCode*)data);//����﷨
//      err = ProgramStepRun_Code(code_err,((PrgCode*)data));
//
//      char data1[35]="part.rz = +12.34";
//      code_err = CheckGrammar((PrgCode*)data1);//����﷨
//      err = ProgramStepRun_Code(code_err,((PrgCode*)data1));

     InitMcx31x();

	 MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);//������//2018/06/01 for Extend RAM
	 InitFlash();

     CloseBrake(1);//�ر�ɲ��

//�ٴγ�ʼ�����ڣ���Ȼ����������ղ�������
     delay_1ms(300);
	 DINT;   				     //��CPU�ж�
	 InitSci();				     //��ʼ��SCIB�Ĵ���
	 EINT;   				     //Enable INTM ʹ��ȫ���ж�
	 ERTM;					     //Enable DBGM ʹ��ʵʱ�ж�


	 delay_1ms(5500);
	 SetScreen(1);
	 delay_1ms(100);
//
//	 err = GetServoPARAM(1,SX_UNLOCK_PARAM_ALL,0,data,0);
//	 tmp = data[0]<<8;
//	 tmp |= data[1];
//	 err = SetServoPARAM(1,SX_SAVE_PARAM_ALL,tmp,0);
     InitData();

//
//	 PositionGesture pos_ges;
//	 PositionEuler tool_end = {
//			 -15.642,0,175.447,180,-45,0
//	 };
//	 PositionEuler part_rob = {
//			 0,449.998,122.504,0,0,0
//	 };
//	 PositionEuler part = {
//			 //0,0,132.503,90,0,0
//			 45.367,-69.718,127.504,0,0,83.2005
//	 };
//	 tool_end.Rx = tool_end.Rx*PI_DIVIDE_180;
//	 tool_end.Ry = tool_end.Ry*PI_DIVIDE_180;
//	 tool_end.Rz = tool_end.Rz*PI_DIVIDE_180;
//	 part_rob.Rx = part_rob.Rx*PI_DIVIDE_180;
//	 part_rob.Ry = part_rob.Ry*PI_DIVIDE_180;
//	 part_rob.Rz = part_rob.Rz*PI_DIVIDE_180;
//	 part.Rx = part.Rx*PI_DIVIDE_180;
//	 part.Ry = part.Ry*PI_DIVIDE_180;
//	 part.Rz = part.Rz*PI_DIVIDE_180;
//	 double length=89;
//	 pos_ges = PartPosToToolPos(tool_end,part_rob,part,length);
//
//	 double ag_tmp[6] = {90,-56.8,-8.66,0,20.52,0};
//	 double angle[6];
//	 int i;
//	  for(i=0;i<6;i++)
//	  {
//		  *(ag_tmp+i) = *(ag_tmp+i)*PI_DIVIDE_180;//
//	  }
//	  update_sin_cos_data(ag_tmp);			//���¿�
//	  ModifyPositionGesture(pos_ges);			//��������
//	  GetBestSolution(angle);
//	  for(i=0;i<6;i++)
//	  {
//		  *(angle+i) = *(angle+i)*PI_MULTI_180_INVERSE;//����ת�Ƕ�
//	  }
//
//
/*****************************************/
     StandbyOutput();	//���Ƶ�

//     DisableServoAlarm();
//     G_CTRL_BUTTON.CTRL_BUTTON.SAVE_BTN = 0;
//     ExecuteRS232(*((PrgCode*)"rs232"));
//     double ag_tmp[6] = {
////    		 0.0001409623,
////    		 -47.97009,
////    		-6.465645,
////    		0.000001,
////    		57.60002,
////    		-179.9999
//    		 0,-47.9706,-6.4647,0.0001,57.5986,0
//     };
//     PositionGesture  p_g;
//     int i;
//	 for(i=0;i<6;i++)
//	 {
//		 ag_tmp[i] = ag_tmp[i]*0.01745329252;
//	 }
//     p_g = GetPositionGesture(ag_tmp);
//     G_CTRL_BUTTON.CTRL_BUTTON.SERVO_ON = 0;
     ReadAndSendProgramName();
	 while(1)
	 {
		 scan_touch_screen();//ȫ��ɨ��
	 }
}
//*******************************MUC��ʼ��************************************/
void InitMcu(void)
{
	 InitSysCtrl();			     //��ʼ��ϵͳ
	 //MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	 //InitFlash();

	 //ADCʱ��
     //EALLOW;
     //SysCtrlRegs.HISPCP.all = 0x3;	//High-speed peripheral clock pre-scaler
     //EDIS;

	 DINT;   				     //��CPU�ж�

	 IER = 0x0000;
	 IFR = 0x0000;
	 InitPieCtrl();			     //��ʼ��PIE�ж�
	 InitPieVectTable();         //��ʼ��PIE�ж�ʸ����
     InitGpio();                 //����I/O��
     InitSci();				     //��ʼ��SCIB�Ĵ���
	 InitXintf();
	 InitSpiaGpio();
	 InitSpi();

//	 InitAdc();
//	 AdcRegs.ADCTRL1.bit.ACQ_PS = 0x01;	//ADCģ�������е�S/H��� = 2��ADC����
//
//	 AdcRegs.ADCTRL3.bit.ADCCLKPS = 0;//ADCʱ��ģ��=HSPCLK/1=25.5MHz/(1)=25MHz
//	 AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;	//1����ģʽ
//	 AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
//	 AdcRegs.ADCTRL1.bit.CONT_RUN = 1;	//������������
//	 AdcRegs.ADCTRL1.bit.SEQ_OVRD = 1;	//ʹ�����������ǹ���
//	 AdcRegs.ADCCHSELSEQ1.all = 0x00;	//��ʼ�����е�ADCͨ����A0
//	 AdcRegs.ADCCHSELSEQ2.all = 0x00;
//	 AdcRegs.ADCCHSELSEQ3.all = 0x00;
//	 AdcRegs.ADCCHSELSEQ4.all = 0x00;
//	 AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0x7;	//ת�����洢��8������Ĵ���

	 //InitXIntrupt();
	 //InitXintf16Gpio();
// Step 4. Initialize the Device Peripheral. This function can be
//         found in DSP2833x_CpuTimers.c
	 InitCpuTimers();   // For this example, only initialize the Cpu Timers


	 ConfigCpuTimer(&CpuTimer0, 15000, 10000);//1000,000us
	 //ConfigCpuTimer(&CpuTimer1, 150, 1000000);
	 //ConfigCpuTimer(&CpuTimer2, 150, 1000000);



// Enable CPU int1 which is connected to CPU-Timer 0, CPU int13
// which is connected to CPU-Timer 1, and CPU int 14, which is connected
// to CPU-Timer 2:
	 IER |= M_INT1;
	//IER |= M_INT13;
	//IER |= M_INT14;

// Enable TINT0 in the PIE: Group 1 interrupt 7 //����ʱ��0�ж�
	 PieCtrlRegs.PIEIER1.bit.INTx7 = 1;


	 IER |= M_INT8;//SCIC

	 IER |= M_INT9;//SCIB

// �ⲿ�ж�����
	 /*IER |= M_INT12;//�ⲿ�ж�
	 GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 2;		//����6���������ڿ������
	 GpioCtrlRegs.GPBCTRL.bit.QUALPRD0 = 30;	//�������� = 32 x TSYSCLKOUT
	 EALLOW;
	 GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 1;//�ⲿ�ж�7ѡ��GPIO33����Ϊ�ж�Դ
	 EDIS;
	 XIntruptRegs.XINT3CR.bit.POLARITY = 0;	//0:�½��ش����ж� 1:������
	 XIntruptRegs.XINT3CR.bit.ENABLE = 1;		//�ⲿ�ж�7ʹ��
	 PieCtrlRegs.PIEIER12.bit.INTx1 = 1; 		//Group12 INTx1ʹ��
*/
	 // GPIO29 are inputs
/*	 EALLOW;
	 GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 0;         // GPIO
	 GpioCtrlRegs.GPADIR.bit.GPIO29 = 0;          // input
	 GpioCtrlRegs.GPAQSEL2.bit.GPIO29 = 2;        // Xint1 Synch to SYSCLKOUT only
	 GpioCtrlRegs.GPACTRL.bit.QUALPRD0 = 0xFF;   // Each sampling window is 510*SYSCLKOUT
	 GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL = 29;//�ⲿ�ж�2ѡ��GPIO29����Ϊ�ж�Դ
	 EDIS;
	 XIntruptRegs.XINT2CR.bit.POLARITY = 0;	//0:�½��ش����ж� 1:������
	 XIntruptRegs.XINT2CR.bit.ENABLE = 1;		//�ⲿ�ж�2ʹ��
	 PieCtrlRegs.PIEIER1.bit.INTx5 = 1; 		//Group1 INTx5ʹ��
*/
	 EINT;   				     //Enable INTM ʹ��ȫ���ж�
	 ERTM;					     //Enable DBGM ʹ��ʵʱ�ж�
	 //ConfigCpuTimer(&CpuTimer0, 150, 50000);//����CPU  (150M   50ms��
	 //StartCpuTimer0();
	 //SendStm32DataFlag = 1;
	 //ReadStm32DataFlag = 1;
	 //delay_1ms(500);             //��ʱ500MS

	 //StartCpuTimer0();

	 GpioDataRegs.GPBDAT.bit.GPIO57=1;delay_200ns();//SPITEA

	 GpioDataRegs.GPADAT.bit.GPIO21=0;delay_200ns();//delay_200ns();//speaker

	 GpioDataRegs.GPADAT.bit.GPIO25=1;delay_200ns();//delay_1us(1);//DIR
	 GpioDataRegs.GPADAT.bit.GPIO24=1;delay_200ns();//delay_1us(1);//MCXRET

	 GpioDataRegs.GPBDAT.bit.GPIO48=1;delay_200ns();//delay_1us(1);//ABSM1
	 GpioDataRegs.GPBDAT.bit.GPIO49=1;delay_200ns();//delay_1us(1);
	 GpioDataRegs.GPBDAT.bit.GPIO50=1;delay_200ns();//delay_1us(1);
	 GpioDataRegs.GPBDAT.bit.GPIO51=1;delay_200ns();//delay_1us(1);
	 GpioDataRegs.GPBDAT.bit.GPIO52=1;delay_200ns();//delay_1us(1);
	 GpioDataRegs.GPBDAT.bit.GPIO53=1;delay_200ns();//delay_1us(1);//ABSM6
	 GpioDataRegs.GPADAT.bit.GPIO27=1;delay_200ns();//delay_1us(1);//ABSR

	 GpioDataRegs.GPBDAT.bit.GPIO59=1;delay_200ns();//delay_1us(1);//HC_LD
	 GpioDataRegs.GPBDAT.bit.GPIO60=0;delay_200ns();//HC_CLK

	 GpioDataRegs.GPADAT.bit.GPIO20=1;delay_200ns();//RS485DIR = 1;Ĭ�Ϸ���
}
/**************
 * ��ʼ������
 */
void InitData(void)
{
	 SetTextValue(43,2,"Initializing system...");
	 SetScreen(43);
	 delay_1ms(100);
	 SetScreen(37);
	 delay_1ms(100);
	 InitRobotPara(6);//��ʼ����е�ֲ���
	 ClearProgramPulseData();//�������������ڴ�
}

/*************************************
 * �������ܣ���ʼ����е�ֲ���
 * ���������axis_num ����� 5��6
 * ���������NONE
 */
void InitRobotPara(unsigned char axis_num)
{
	long LimitedPositionPlus[]={0,0,0,0,0,0};
	long LimitedPositionMinus[]={0,0,0,0,0,0};
	long pulse[6],range;
	unsigned char err,err1,i;
	err = ReadRobotPara(&ROBOT_PARAMETER);
	if(err == 1)	//��ȡʧ��
	{
		SetScreen(20);
		SetTextValue(20, 4, "");
		SetTextValue(20, 3, "Read system parameter failed!");
		while(1)
		{
			if(USART_RX_STA&0x8000)
			{
				USART_RX_STA = 0;
				break;
			}
		}
		setting_robot_para();
		ReadRobotPara(&ROBOT_PARAMETER);
	}
	if(err == 2)	//����δ����
	{
		SetScreen(20);
		SetTextValue(20, 4, "");
		SetTextValue(20, 3, "Please Set system parameter!");
		while(1)
		{
			if(USART_RX_STA&0x8000)
			{
				USART_RX_STA = 0;
				break;
			}
		}
		setting_robot_para();
		ReadRobotPara(&ROBOT_PARAMETER);
	}
	 if((ROBOT_PARAMETER.ENCODER_LEFT_SAV==1) &&
			 (ROBOT_PARAMETER.MOTOR_TYPE==2))//�ж��Ƿ���Ҫ���������ԭ��ƫ������
	 {
		 if(ReadEncoderPulseAll(pulse))//���������ԭ��ƫ������
		 {
			 	//��ȡʧ��
				SetScreen(20);
				SetTextValue(20, 4, "");
				SetTextValue(20, 3, "Read encoder data failed!");
				while(1)
				{
					if(USART_RX_STA&0x8000)
					{
						USART_RX_STA = 0;
						break;
					}
				}
		 }
		 else//��ȡ�ɹ�
		 {
			 for(i=0;i<6;i++)
			 {
				 ROBOT_PARAMETER.ENCODER_LEFT[i] = pulse[i];
			 }
			 ROBOT_PARAMETER.ENCODER_LEFT_SAV = 0;
			 err1 = SaveRobotPara(ROBOT_PARAMETER);//�����е�ֲ���
			 if(err1)//����ʧ��
			 {
					SetScreen(20);
					SetTextValue(20, 4, "");
					SetTextValue(20, 3, "Save encoder data failed!");
					while(1)
					{
						if(USART_RX_STA&0x8000)
						{
							USART_RX_STA = 0;
							break;
						}
					}
			 }
		 }
	 }

	PulseOfCircle_INVERSE = (double)1/ROBOT_PARAMETER.MOTOR_PRECISION;
	range = (double)8000000/(ROBOT_PARAMETER.MOTOR_SPEED/60.0*ROBOT_PARAMETER.MOTOR_PRECISION/8000.0);
	SetRange(0x3f,range);//55000-3500ת  32000-6000ת
	SetAcac(0x3f,300);      //���ٶȱ仯�� 1~65535  (62.5*10^6)/K*(8000000/R)
	SetAcc(0x3f,400);      //���ٶ�  A*125	 1~8000
	//SetDec(0x3f,80);      //���ٶ�  A*125 1~8000
	SetStartV(0x3f,8000);      //��ʼ�ٶ�   ��Χ��1~8000
	SetSpeed(0x3f,400);//���ó�ʼ�ٶ�

	if(ROBOT_PARAMETER.SOUND == 0)
	{
		SetTouchScreen(0x21);//�ر�����
		delay_1ms(10);
		SetButtonValue(44, 10, 1);
		SetButtonValue(2, 10, 1);
		delay_1ms(10);
		SetButtonValue(2, 10, 1);
	}
	else
	{
		SetTouchScreen(0x23);//������
		delay_1ms(10);
		SetButtonValue(44, 10, 0);
		delay_1ms(10);
		SetButtonValue(2, 10, 0);
	}

	LimitedPositionPlus[0]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[0]*ROBOT_PARAMETER.LIMITED_P[0]/360.0;	//��һ������λ��165�� 	(���᣺165-5)
	LimitedPositionPlus[1]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[1]*ROBOT_PARAMETER.LIMITED_P[1]/360.0;	//�ڶ�������λ��10��	(���᣺144-4)
	LimitedPositionPlus[2]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[2]*ROBOT_PARAMETER.LIMITED_P[2]/360.0;	//����������λ��81��	(���᣺0)
	LimitedPositionPlus[3]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[3]*ROBOT_PARAMETER.LIMITED_P[3]/360.0;	//����������λ��160��	(���᣺360)
	LimitedPositionPlus[4]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[4]*ROBOT_PARAMETER.LIMITED_P[4]/360.0;	//����������λ��90��
	LimitedPositionPlus[5]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[5]*ROBOT_PARAMETER.LIMITED_P[5]/360.0;	//����������λ��360��

	LimitedPositionMinus[0]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[0]*ROBOT_PARAMETER.LIMITED_M[0]/360.0;//��һ�Ḻ��λ��-165��	(���᣺-165+5)
	LimitedPositionMinus[1]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[1]*ROBOT_PARAMETER.LIMITED_M[1]/360.0;//�ڶ��Ḻ��λ��-180��	(���᣺-144+4)
	LimitedPositionMinus[2]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[2]*ROBOT_PARAMETER.LIMITED_M[2]/360.0;//�����Ḻ��λ��-85��	(���᣺-300)
	LimitedPositionMinus[3]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[3]*ROBOT_PARAMETER.LIMITED_M[3]/360.0;//�����Ḻ��λ��-160��	(���᣺360)
	LimitedPositionMinus[4]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[4]*ROBOT_PARAMETER.LIMITED_M[4]/360.0;//�����Ḻ��λ��-90��
	LimitedPositionMinus[5]=ROBOT_PARAMETER.MOTOR_PRECISION*ROBOT_PARAMETER.RATIO[5]*ROBOT_PARAMETER.LIMITED_M[5]/360.0;//�����Ḻ��λ��-360��

	SetLimitedPosition(LimitedPositionPlus,LimitedPositionMinus); //��λ����
	update_kinematic_parameter(axis_num);
}

/*************************************
 * �������ܣ���ʼ����е��״̬
 * ���������NONE
 * ���������NONE
 */
void InitRobotState(void)
{

}
//******************************THE END*************************************/
