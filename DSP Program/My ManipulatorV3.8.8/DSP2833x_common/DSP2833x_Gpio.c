// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:46:25 $
//###########################################################################
//
// FILE:	DSP2833x_Gpio.c
//
// TITLE:	DSP2833x General Purpose I/O Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

//---------------------------------------------------------------------------
// InitGpio: 
//---------------------------------------------------------------------------
// This function initializes the Gpio to a known (default) state.
//
// For more details on configuring GPIO's as peripheral functions,
// refer to the individual peripheral examples and/or GPIO setup example. 
void InitGpio(void)
{
   EALLOW;
   
   // Each GPIO pin can be: 
   // a) a GPIO input/output
   // b) peripheral function 1
   // c) peripheral function 2
   // d) peripheral function 3
   // By default, all are GPIO Inputs 
   GpioCtrlRegs.GPAMUX1.all = 0x0000;     // GPIO functionality GPIO0-GPIO15
   GpioCtrlRegs.GPAMUX2.all = 0x0000;     // GPIO functionality GPIO16-GPIO31
   GpioCtrlRegs.GPBMUX1.all = 0x0000;     // GPIO functionality GPIO32-GPIO39
   GpioCtrlRegs.GPBMUX2.all = 0x0000;     // GPIO functionality GPIO48-GPIO63
   GpioCtrlRegs.GPCMUX1.all = 0x0000;     // GPIO functionality GPIO64-GPIO79
   GpioCtrlRegs.GPCMUX2.all = 0x0000;     // GPIO functionality GPIO80-GPIO95

   //output:21,24,25,26,27
   //31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16
   //0  0  0  0  1  0  1  1  0  0  1  1  0  0  0  0  //0b20
   GpioCtrlRegs.GPADIR.all = 0x0b300000;      // GPIO0-GPIO31 are inputs 0:input	1:output

   //output:35,48~53,59,61
   //      61    59                53             48                              35     32
   //31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
   //0  0  1  0  1  0  0  0  0  0  1  1  1  1  1  1  0  0  0  0  0  0  0 0 0 0 0 0 1 0 0 0//283f0000
   GpioCtrlRegs.GPBDIR.all = 0x283f0008;      // GPIO32-GPIO63 are inputs

   GpioCtrlRegs.GPCDIR.all = 0x0000;      // GPI064-GPIO95 are inputs

   // Each input can have different qualification
   // a) input synchronized to SYSCLKOUT
   // b) input qualified by a sampling window
   // c) input sent asynchronously (valid for peripheral inputs only)
   GpioCtrlRegs.GPAQSEL1.all = 0x0000;    // GPIO0-GPIO15 Synch to SYSCLKOUT 
   GpioCtrlRegs.GPAQSEL2.all = 0x0000;    // GPIO16-GPIO31 Synch to SYSCLKOUT
   GpioCtrlRegs.GPBQSEL1.all = 0x0000;    // GPIO32-GPIO39 Synch to SYSCLKOUT 
   GpioCtrlRegs.GPBQSEL2.all = 0x0000;    // GPIO48-GPIO63 Synch to SYSCLKOUT 

   // Pull-ups can be enabled or disabled. 
   GpioCtrlRegs.GPAPUD.all = 0x0000;      // Pullup's enabled GPIO0-GPIO31	0:上拉	1:禁止内部上拉
   GpioCtrlRegs.GPBPUD.all = 0x0000;      // Pullup's enabled GPIO32-GPIO63
   GpioCtrlRegs.GPCPUD.all = 0x0000;      // Pullup's enabled GPIO64-GPIO79

   //GpioCtrlRegs.GPAPUD.all = 0xFFFF;    // Pullup's disabled GPIO0-GPIO31
   //GpioCtrlRegs.GPBPUD.all = 0xFFFF;    // Pullup's disabled GPIO32-GPIO34
   //GpioCtrlRegs.GPCPUD.all = 0xFFFF;     // Pullup's disabled GPIO64-GPIO79
   //save_key	GPIO58		input	Pullup
   //HC_LD		GPIO59		output
   //HC_SO		GPIO60		input	Pullup
   //HC_CLK		GPIO61		output
   //PIN1 		GPIO33		input	Pullup
   //PIN2	 	GPIO32		input	Pullup
   //POUT1		GPIO20		output
   //SPEAKER	GPIO21		output
   //MCXRET		GPIO24		output
   //DIR		GPIO25		output
   //ABSM1~6	GPIO48~53	output
   //ABST		GPIO26		inout
   //ABSR		GPIO27		output
   //ABSX1~14	GPIO 0~13	input	Pullup
   //ABSX15~18	GPIO 16~19	input	Pullup
   //
   //PIN3		GPIO29		input	Pullup xxx
   //POUT2		GPIO35		output xxx
   EDIS;
   //DELAY_US(5000);
   /**********IO口初始化************************/
	/* GpioDataRegs.GPADAT.bit.GPIO25=1;//DIR
	 GpioDataRegs.GPADAT.bit.GPIO24=1;//MCXRET

	 GpioDataRegs.GPBDAT.bit.GPIO48=1;//ABSM1
	 GpioDataRegs.GPBDAT.bit.GPIO49=1;
	 GpioDataRegs.GPBDAT.bit.GPIO50=1;
	 GpioDataRegs.GPBDAT.bit.GPIO51=1;
	 GpioDataRegs.GPBDAT.bit.GPIO52=1;
	 GpioDataRegs.GPBDAT.bit.GPIO53=1;//ABSM6
	 GpioDataRegs.GPADAT.bit.GPIO27=1;//ABSR

	 GpioDataRegs.GPADAT.bit.GPIO21=1;//speaker

	 GpioDataRegs.GPBDAT.bit.GPIO59=1;//HC_LD
	 GpioDataRegs.GPBDAT.bit.GPIO60=0;//HC_CLK

	 GpioDataRegs.GPADAT.bit.GPIO20=1;//POUT1
*/

}	
	
//===========================================================================
// End of file.
//===========================================================================
