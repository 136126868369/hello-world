// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:46:06 $
//###########################################################################
//
// FILE:	DSP2833x_DefaultIsr.c
//
// TITLE:	DSP2833x Device Default Interrupt Service Routines.
//
// This file contains shell ISR routines for the 2833x PIE vector table.
// Typically these shell ISR routines can be used to populate the entire PIE 
// vector table during device debug.  In this manner if an interrupt is taken
// during firmware development, there will always be an ISR to catch it.  
//
// As develpment progresses, these ISR rotuines can be eliminated and replaced
// with the user's own ISR routines for each interrupt.  Since these shell ISRs
// include infinite loops they will typically not be included as-is in the final
// production firmware. 
//
//###########################################################################
// $TI Release: DSP2833x Header Files V1.01 $
// $Release Date: September 26, 2007 $
//###########################################################################

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "My_Project.h"
#include "Touch_screen.h"
#include "stdlib.h"
#include "crc.h"
extern unsigned int USART_RX_STA;   //串口a的接收状态标记
unsigned int USART_RX_STA_B=0x0ff;		// 串口b的接收状态标记
extern int RX_POSITION_STA;		//是否接收到position的标志，0:未接收到  1:已接收到
static u8 USART_RX_BUF[32];
char USART_B_RX_BUF[32];//-xxxx.xxx 9*3
extern struct EX_Position_stru EX_POSITION;
#define USART_REC_LEN  		32  	//定义最大接收字节数 32
#define USART_B_RX_LEN  	32  	//定义串口B最大接收字节数 32
long TIMER0_CNT = 0;

#define CRC16_ENABLE 1  // 如果需要CRC16校验功能，修改此宏为1(此时需要在VisualTFT工程中配CRC校验)
static u16 i=0,data_length=0;//修正于//2018/06/01
static unsigned char crc_buffer[32]={0,0,0,0,0,0,0,0,0,0};//修正于//2018/06/01


// Connected to INT13 of CPU (use MINT13 mask):
// Note CPU-Timer1 is reserved for TI use, however XINT13
// ISR can be used by the user. 
interrupt void INT13_ISR(void)     // INT13 or CPU-Timer1
{
  // Insert ISR Code here
  
  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// Note CPU-Timer2 is reserved for TI use.
interrupt void INT14_ISR(void)     // CPU-Timer2
{
  // Insert ISR Code here
  
  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void DATALOG_ISR(void)   // Datalogging interrupt
{
   // Insert ISR Code here
 
   // Next two lines for debug only to halt the processor here
   // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

interrupt void RTOSINT_ISR(void)   // RTOS interrupt
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void EMUINT_ISR(void)    // Emulation interrupt
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void NMI_ISR(void)       // Non-maskable interrupt
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void ILLEGAL_ISR(void)   // Illegal operation TRAP
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm("          ESTOP0");
  for(;;);

}


interrupt void USER1_ISR(void)     // User Defined trap 1
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

interrupt void USER2_ISR(void)     // User Defined trap 2
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);


}

interrupt void USER3_ISR(void)     // User Defined trap 3
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER4_ISR(void)     // User Defined trap 4
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER5_ISR(void)     // User Defined trap 5
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER6_ISR(void)     // User Defined trap 6
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER7_ISR(void)     // User Defined trap 7
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER8_ISR(void)     // User Defined trap 8
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER9_ISR(void)     // User Defined trap 9
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER10_ISR(void)    // User Defined trap 10
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER11_ISR(void)    // User Defined trap 11
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER12_ISR(void)     // User Defined trap 12
{
 // Insert ISR Code here
 
  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

// -----------------------------------------------------------
// PIE Group 1 - MUXed into CPU INT1
// -----------------------------------------------------------

// INT1.1 
interrupt void SEQ1INT_ISR(void)   //SEQ1 ADC
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code

  asm ("      ESTOP0");
  for(;;);

}     

// INT1.2 
interrupt void SEQ2INT_ISR(void)  //SEQ2 ADC
{

  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  
  asm("	  ESTOP0");
  for(;;);
	
}
// INT1.3 - Reserved

// INT1.4
interrupt void  XINT1_ISR(void)
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}     

// INT1.5
interrupt void  XINT2_ISR(void)
{
  // Insert ISR Code here
//	delay_1ms(1000);
//	SetCurrentScreen();
  // To receive more interrupts from this PIE group, acknowledge this interrupt 
 //  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

// INT1.6
interrupt void  ADCINT_ISR(void)     // ADC
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; 
  
  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT1.7
interrupt void  TINT0_ISR(void)      // CPU-Timer 0
{
	TIMER0_CNT++;
  // To receive more interrupts from this PIE group, acknowledge this interrupt 
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}


// INT1.8
interrupt void  WAKEINT_ISR(void)    // WD, LOW Power
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; 
  
  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}


// -----------------------------------------------------------
// PIE Group 2 - MUXed into CPU INT2
// -----------------------------------------------------------

// INT2.1
interrupt void EPWM1_TZINT_ISR(void)    // EPWM-1
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT2.2
interrupt void EPWM2_TZINT_ISR(void)    // EPWM-2
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT2.3
interrupt void EPWM3_TZINT_ISR(void)    // EPWM-3
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
  
  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}


// INT2.4
interrupt void EPWM4_TZINT_ISR(void)    // EPWM-4
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
  
  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}


// INT2.5
interrupt void EPWM5_TZINT_ISR(void)    // EPWM-5
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT2.6
interrupt void EPWM6_TZINT_ISR(void)   // EPWM-6
{
  // Insert ISR Code here

   
  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
  
  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT2.7 - Reserved
// INT2.8 - Reserved

// -----------------------------------------------------------
// PIE Group 3 - MUXed into CPU INT3
// -----------------------------------------------------------
   
// INT 3.1         
interrupt void EPWM1_INT_ISR(void)     // EPWM-1
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT3.2
interrupt void EPWM2_INT_ISR(void)     // EPWM-2
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT3.3
interrupt void EPWM3_INT_ISR(void)    // EPWM-3
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT3.4
interrupt void EPWM4_INT_ISR(void)    // EPWM-4
{
  // Insert ISR Code here
  
  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;  

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT3.5
interrupt void EPWM5_INT_ISR(void)    // EPWM-5
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;  

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT3.6
interrupt void EPWM6_INT_ISR(void)    // EPWM-6
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT3.7 - Reserved
// INT3.8 - Reserved


// -----------------------------------------------------------
// PIE Group 4 - MUXed into CPU INT4
// -----------------------------------------------------------

// INT 4.1
interrupt void ECAP1_INT_ISR(void)    // ECAP-1
{
  // Insert ISR Code here
  
  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;  

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT4.2
interrupt void ECAP2_INT_ISR(void)    // ECAP-2
{
  // Insert ISR Code here
  
  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT4.3
interrupt void ECAP3_INT_ISR(void)    // ECAP-3
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
  
  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT4.4
interrupt void ECAP4_INT_ISR(void)     // ECAP-4
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT4.5
interrupt void ECAP5_INT_ISR(void)     // ECAP-5
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}
// INT4.6
interrupt void ECAP6_INT_ISR(void)     // ECAP-6
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}
// INT4.7 - Reserved
// INT4.8 - Reserved

// -----------------------------------------------------------
// PIE Group 5 - MUXed into CPU INT5
// -----------------------------------------------------------

// INT 5.1
interrupt void EQEP1_INT_ISR(void)    // EQEP-1
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT5.2
interrupt void EQEP2_INT_ISR(void)    // EQEP-2
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;
 
  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT5.3 - Reserved
// INT5.4 - Reserved
// INT5.5 - Reserved
// INT5.6 - Reserved
// INT5.7 - Reserved
// INT5.8 - Reserved

// -----------------------------------------------------------
// PIE Group 6 - MUXed into CPU INT6
// -----------------------------------------------------------

// INT6.1
interrupt void SPIRXINTA_ISR(void)    // SPI-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT6.2
interrupt void SPITXINTA_ISR(void)     // SPI-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT6.3 
interrupt void MRINTB_ISR(void)     // McBSP-B
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT6.4
interrupt void MXINTB_ISR(void)     // McBSP-B
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT6.5
interrupt void MRINTA_ISR(void)     // McBSP-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT6.6
interrupt void MXINTA_ISR(void)     // McBSP-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT6.7 - Reserved
// INT6.8 - Reserved




// -----------------------------------------------------------
// PIE Group 7 - MUXed into CPU INT7
// -----------------------------------------------------------

// INT7.1
interrupt void DINTCH1_ISR(void)     // DMA
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT7.2
interrupt void DINTCH2_ISR(void)     // DMA
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT7.3
interrupt void DINTCH3_ISR(void)     // DMA
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT7.4
interrupt void DINTCH4_ISR(void)     // DMA
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT7.5
interrupt void DINTCH5_ISR(void)     // DMA
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT7.6
interrupt void DINTCH6_ISR(void)     // DMA
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT7.7 - Reserved
// INT7.8 - Reserved

// -----------------------------------------------------------
// PIE Group 8 - MUXed into CPU INT8
// -----------------------------------------------------------

// INT8.1
interrupt void I2CINT1A_ISR(void)     // I2C-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT8.2
interrupt void I2CINT2A_ISR(void)     // I2C-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT8.3 - Reserved
// INT8.4 - Reserved

// INT8.5
interrupt void SCIRXINTC_ISR(void)     // SCI-C
{
	static unsigned char Res;

	if((ScicRegs.SCIRXST.bit.RXRDY == 1))//接收中断(接收到的数据必须是0xFF 0xFC 0xFF 0xFF结尾)
	{
		 Res = ScicRegs.SCIRXBUF.all & 0xff;					//读取接收到的数据
			if((USART_RX_STA&0x8000)==0)//接收未完成
			{
				if(USART_RX_STA<0x1000)//接收到了0x0d
				{
					if(Res==0xFF)
					{
						USART_RX_STA |= 0x1000;
						USART_RX_BUF[USART_RX_STA & 0X0FFF] = Res ;
						USART_RX_STA++;
					}
					else
					{
						if(((USART_RX_STA&0x0fff) == 1) && (Res==0xee))
						{
							USART_RX_STA=0;
						}
						USART_RX_BUF[USART_RX_STA&0X0FFF] = Res ;
						USART_RX_STA++;
						if( USART_RX_STA > (USART_REC_LEN-2) )
							 USART_RX_STA=0;//接收数据错误,重新开始接收
					}
				}
				else //还没收到0X0D
				{
					if(USART_RX_STA<0x2000)//接收到了0xFF
					{
						if(Res==0xFC)
						{
							USART_RX_STA |= 0x2000;
							USART_RX_BUF[USART_RX_STA&0X0FFF]=Res ;
							USART_RX_STA++;
						}

						else
						{
//考虑可能到接收到EE B1 OO OO FF (FF FC FF FF)的命令
							if(Res==0xFF)
							{
								USART_RX_STA &= 0x1fff;
								USART_RX_BUF[USART_RX_STA&0X0FFF] = Res ;
								USART_RX_STA++;
							}
//考虑可能到接收到EE B1 OO OO O1 FF FF 01 02 (FF FC FF FF)的命令
							else
							{
								USART_RX_STA &= 0X0FFF;
								USART_RX_BUF[USART_RX_STA&0X0FFF]=Res ;
								USART_RX_STA++;
								if( USART_RX_STA > (USART_REC_LEN-2) )
									 USART_RX_STA=0;//接收数据错误,重新开始接收
							}
						}

					}
					else
					{
						if(USART_RX_STA<0x4000)//
						{
							if(Res==0xFF)
							{
								USART_RX_STA|=0X4000;
								USART_RX_BUF[USART_RX_STA&0X0FFF]=Res ;
								USART_RX_STA++;

							}
							else
							{
								USART_RX_STA=0;
							}
						}
						else
						{
							if(USART_RX_STA<0x8000)//
							{
								if(Res==0xFF)
								{
									USART_RX_BUF[USART_RX_STA&0X0FFF]=Res ;
									USART_RX_STA++;
									USART_RX_STA|=0X8000;
									//提取数据长度
									data_length=(USART_RX_STA&0X0FFF);//修正于//2018/06/01

									for(; (USART_RX_STA&0X0FFF)<USART_REC_LEN; USART_RX_STA++)
									{
										USART_RX_BUF[USART_RX_STA&0X0FFF]=0;
									}
								}
								else
								{
									USART_RX_STA=0;
								}
							}
						}
					}
				}
			}
		}

		if(USART_RX_STA&0x8000)
		{
			//加入CRC代码校验的位置（CRC校验不包括数据帧的帧头0xEE和帧尾0xFF oxFC 0xFF oxFF以及CRC码本身）//2018/06/01

			for(i=1;i<=(data_length-5);i++)
            {
            	crc_buffer[i-1]=USART_RX_BUF[i];
            }

#if(CRC16_ENABLE)
				//提取数据长度，掐头去尾留中间，建立数组，得到CRC校验码//2018/06/01
            	//去掉指令头0xEE,尾0xFF 0xFC 0xFF 0xFF共计5个字节，只计算数据部分CRC
            	//if(CheckCRC16(USART_RX_BUF+1,data_length-5))              //CRC校验
			if(CheckCRC16(crc_buffer,data_length-5))              //CRC校验
            		//比对接收的CRC校验码和计算的CRC校验码，若一致，则调用manage_usart_return_value
            	{manage_usart_return_value(USART_RX_BUF);}//按下的为按钮或者滑块
			for(i=1;i<=(data_length-5);i++)
			{
			    crc_buffer[i-1]=0;
			}		//否则，抛弃数据，不进行任何处理
#endif
#if(!CRC16_ENABLE)
            	manage_usart_return_value(USART_RX_BUF);//按下的为按钮或者滑块
#endif
		}
		//ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1;	//清除溢出标志
		//ScibRegs.SCIFFRX.bit.RXFFINTCLR = 1;	//清除中断标志
		PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
	//EINT;

}

// INT8.6
interrupt void SCITXINTC_ISR(void)     // SCI-C
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

// INT8.7 - Reserved
// INT8.8 - Reserved


// -----------------------------------------------------------
// PIE Group 9 - MUXed into CPU INT9
// -----------------------------------------------------------

// INT9.1
interrupt void SCIRXINTA_ISR(void)     // SCI-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

// INT9.2
interrupt void SCITXINTA_ISR(void)     // SCI-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}


// INT9.3
interrupt void SCIRXINTB_ISR(void)     // SCI-B
{
	int i=0;
	unsigned char res,flg=0;//,i=0,flg=0;
	static unsigned char fir_len=0;	//当接收到第一个数据的时候，保存将要接收数据的长度
	u16 crc1,crc2;
	if(ROBOT_PARAMETER.HARDWARE_REV >= 7)	//三协
	{
		if((ScibRegs.SCIRXST.bit.RXRDY == 1))//接收中断
		{
			 res = ScibRegs.SCIRXBUF.all & 0xff;					//读取接收到的数据
			 if(USART_RX_STA_B == 0)	//USART_RX_STA_B初始化为0xFF,必须先设为0才可以开始接收数据
			 {
				 fir_len = 0;
			 }
			 if(fir_len == 0)//接收到第一个数据
			 {
				 if(USART_RX_STA_B == 0)
				 {
					 USART_B_RX_BUF[0] = res;
					 USART_RX_STA_B++;
					 fir_len = res&0x1F; //0001 1111 bit0~bit5 数据长度
					 fir_len += 4;	//协议标头1byte + 目的地址1byte + 校验位2byte
				 }
			 }
			 else
			 {
				 if((USART_RX_STA_B&0x00FF) < fir_len)
				 {
					 USART_B_RX_BUF[USART_RX_STA_B&0x0FFF] = res;
					 USART_RX_STA_B++;
					 if((USART_RX_STA_B&0x00FF) == fir_len)//数据接收完成，开始校验数据
					 {
						 crc1 = 0;
						 crc2 = 0;
						 crc1 = USART_B_RX_BUF[fir_len-2] << 8;
						 crc1 |= USART_B_RX_BUF[fir_len-1] & 0xff;

						 crc2 = crc_cal_by_bit((u8*)USART_B_RX_BUF,fir_len-2);
						 if(crc1 == crc2)
						 {
							 USART_RX_STA_B |= 0x8000;	//数据接收完成且正确
							 fir_len = 0;
						 }
						 else
						 {
							 USART_RX_STA_B |= 0xC000;	//数据接收错误，延时50ms才清零USART_RX_STA_B
							 fir_len = 0;
						 }
					 }
				 }
			 }
		}
	}
	if(ROBOT_PARAMETER.HARDWARE_REV <= 6)
	{
		if((ScibRegs.SCIRXST.bit.RXRDY == 1))//接收中断(接收到的数据必须是0xFF 0xFC 0xFF 0xFF结尾)
		{
			 res = ScibRegs.SCIRXBUF.all & 0xff;					//读取接收到的数据
			 if((USART_RX_STA_B&0x8000)==0)//接收未完成
			 {
				 if(USART_RX_STA_B&0x4000)//接收到了0x0d
				 {
					 if(res!=0x0a)USART_RX_STA_B=0;//接收错误,重新开始
					 else
					 {
						 USART_B_RX_BUF[USART_RX_STA_B&0X3FFF]=res ;
						 USART_RX_STA_B++;
						 USART_RX_STA_B|=0x8000;	//接收完成了
					 }
				 }
				 else //还没收到0X0D
				 {
					 if(res==0x0d)
					 {
						 USART_B_RX_BUF[USART_RX_STA_B&0X3FFF]=res ;
						 USART_RX_STA_B++;
						 USART_RX_STA_B|=0x4000;
					 }
					 else
					 {
						 USART_B_RX_BUF[USART_RX_STA_B&0X3FFF]=res ;
						 USART_RX_STA_B++;
						 if(USART_RX_STA_B>(USART_B_RX_LEN-1))
							 USART_RX_STA_B=0;//接收数据错误,重新开始接收
					 }
				 }
			 }
		}
		if(USART_RX_STA_B&0x8000)
		{
			//直到遇上数字
			while((USART_B_RX_BUF[i] != '+') && (USART_B_RX_BUF[i] != '-') &&
					(USART_B_RX_BUF[i] < '0' || USART_B_RX_BUF[i] > '9'))
			{
				i++;
				if(i>(USART_B_RX_LEN-1))break;
			}
			flg = USART_B_RX_BUF[i];//第一个数据代表是否有效,'1'有效  '0'无效
			i++;
			if(flg == '1')
			{
				//直到遇上数字
				while((USART_B_RX_BUF[i] != '+') && (USART_B_RX_BUF[i] != '-') &&
						(USART_B_RX_BUF[i] < '0' || USART_B_RX_BUF[i] > '9'))
				{
					i++;
					if(i>(USART_B_RX_LEN-1))break;
				}
				EX_POSITION.EX_POSITION[0] = atof(&USART_B_RX_BUF[i]);//提取数据
				EX_POSITION.type = 1;
				//跳过数据
				while((USART_B_RX_BUF[i] == '+') || (USART_B_RX_BUF[i] == '-') ||
						(USART_B_RX_BUF[i] == '.') ||
						(USART_B_RX_BUF[i] >= '0' && USART_B_RX_BUF[i] <= '9'))
				{
					i++;
					if(i>(USART_B_RX_LEN-1))
					{
						PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
						EX_POSITION.type = 0;
						return;
					}
				}
				//直到遇上数字
				while((USART_B_RX_BUF[i] != '+') && (USART_B_RX_BUF[i] != '-') &&
						(USART_B_RX_BUF[i] < '0' || USART_B_RX_BUF[i] > '9'))
				{
					i++;
					if(i>(USART_B_RX_LEN-1))
					{
						PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
						EX_POSITION.type = 0;
						return;
					}
				}
				EX_POSITION.EX_POSITION[1] = atof(&USART_B_RX_BUF[i]);
				EX_POSITION.type = 2;
				//跳过数据
				while((USART_B_RX_BUF[i] == '+') || (USART_B_RX_BUF[i] == '-') ||
						(USART_B_RX_BUF[i] == '.') ||
						(USART_B_RX_BUF[i] >= '0' && USART_B_RX_BUF[i] <= '9'))
				{
					i++;
					if(i>(USART_B_RX_LEN-1))
					{
						PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
						EX_POSITION.type = 0;
						return;
					}
				}
				if(USART_B_RX_BUF[i] != 0x0d)//不等于0x0d，表示还有数据
				{
					//直到遇上数字
					while((USART_B_RX_BUF[i] != '+') && (USART_B_RX_BUF[i] != '-') &&
							(USART_B_RX_BUF[i] < '0' || USART_B_RX_BUF[i] > '9'))
					{
						i++;
						if(i>(USART_B_RX_LEN-1))
						{
							PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
							return;
						}
					}
					EX_POSITION.EX_POSITION[2] = atof(&USART_B_RX_BUF[i]);
					EX_POSITION.type = 3;
				}
				USART_RX_STA_B = 0;
				if((EX_POSITION.EX_POSITION[0]==0) && (EX_POSITION.EX_POSITION[1]==0))//都等于0说明数据有问题
				{
					RX_POSITION_STA=0;
					EX_POSITION.type = 0;
				}
				else
					RX_POSITION_STA=1;
			}
			else
			{
				RX_POSITION_STA = 0;
				USART_RX_STA_B = 0;
			}
		}
	}
  // To receive more interrupts from this PIE group, acknowledge this interrupt 
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

// INT9.4
interrupt void SCITXINTB_ISR(void)     // SCI-B
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

// INT9.5
interrupt void ECAN0INTA_ISR(void)  // eCAN-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

// INT9.6
interrupt void ECAN1INTA_ISR(void)  // eCAN-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

// INT9.7
interrupt void ECAN0INTB_ISR(void)  // eCAN-B
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

// INT9.8
interrupt void ECAN1INTB_ISR(void)  // eCAN-B
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

// -----------------------------------------------------------
// PIE Group 10 - MUXed into CPU INT10
// -----------------------------------------------------------

// INT10.1 - Reserved
// INT10.2 - Reserved
// INT10.3 - Reserved
// INT10.4 - Reserved
// INT10.5 - Reserved
// INT10.6 - Reserved
// INT10.7 - Reserved
// INT10.8 - Reserved


// -----------------------------------------------------------
// PIE Group 11 - MUXed into CPU INT11
// -----------------------------------------------------------

// INT11.1 - Reserved
// INT11.2 - Reserved
// INT11.3 - Reserved
// INT11.4 - Reserved
// INT11.5 - Reserved
// INT11.6 - Reserved
// INT11.7 - Reserved
// INT11.8 - Reserved

// -----------------------------------------------------------
// PIE Group 12 - MUXed into CPU INT12
// -----------------------------------------------------------

// INT12.1
interrupt void XINT3_ISR(void)  // External Interrupt
{
  // Insert ISR Code here
	/*delay_1ms(1000);
	InitSci();

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
	//ScicRegs.SCICTL1.bit.SWRESET = 1;	//SCI-C Software reset

	delay_1us(1);
	SetCurrentScreen();
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;*/
  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

// INT12.2
interrupt void XINT4_ISR(void)  // External Interrupt
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

// INT12.3
interrupt void XINT5_ISR(void)  // External Interrupt
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}
// INT12.4
interrupt void XINT6_ISR(void)  // External Interrupt
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

// INT12.5
interrupt void XINT7_ISR(void)  // External Interrupt
{
  // Insert ISR Code here
	//SetCurrentScreen();

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  //asm ("      ESTOP0");
  //for(;;);

}

// INT12.6 - Reserved
// INT12.7
interrupt void LVF_ISR(void)  // Latched overflow
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}
// INT12.8
interrupt void LUF_ISR(void)  // Latched underflow
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt 
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);

}

//---------------------------------------------------------------------------
// Catch All Default ISRs: 
//

interrupt void EMPTY_ISR(void)  // Empty ISR - only does a return.
{

}

interrupt void PIE_RESERVED(void)  // Reserved space.  For test.
{
  asm ("      ESTOP0");
  for(;;);
}

interrupt void rsvd_ISR(void)      // For test
{
  asm ("      ESTOP0");
  for(;;);
}

//===========================================================================
// End of file.
//===========================================================================

