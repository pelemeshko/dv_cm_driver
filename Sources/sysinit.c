#include "1986ve8_lib/cm4ikmcu.h"


#ifdef _EXT_MEM_RLS_
const uint64_t ExtBusCfgCode[] __attribute__ ((at(0x10000400))) = {0x71, 0x71, 0,0,0,0,0,0,0,0};
#endif


extern uint32_t __Vectors;

void System_Init() {
  *((uint32_t*)0xE000ED08) = (uint32_t)&__Vectors;
	//	battery memory registers
  BKP->KEY = _KEY_;
  BKP->REG_60_TMR0 |= 1<<28; // diasable POR //Power overload is permitted
  BKP->REG_60_TMR1 |= 1<<28; //
  BKP->REG_60_TMR2 |= 1<<28; //
    
  FT_CNTR->KEY = _KEY_;
  FT_CNTR->RESET_EVENT0 = 0x0; // hide all errors
  FT_CNTR->RESET_EVENT1 = 0x0; //
  FT_CNTR->RESET_EVENT2 = 0x0; //
  FT_CNTR->RESET_EVENT3 = 0x0; //
  FT_CNTR->RESET_EVENT4 = 0x0; //

  /*-------------------- Ports Init --------------------------*/
  CLK_CNTR->KEY = _KEY_;
  CLK_CNTR->PER0_CLK |= (1<<13)|(1<<14)|(1<<15)|(1<<16)|(1<<17)|(1<<26);  //port A,B,C,D,E,timer3 clock enable
	CLK_CNTR->TIM3_CLK = (1<<16)|(39); ///clocking is enablede and equal to CoreClk
	
  /* PA[29,28,26,25,24,23,21,20]-MIL0 */
  PORTA->KEY = _KEY_;
  PORTA->SANALOG =   0xFFFFFFFF;
	PORTA->CFUNC[3]  = 0x00FF0FFF; //PA 29,28,26,25,24
  PORTA->SFUNC[3] =  0x00CC0CCC;
  PORTA->CFUNC[2] =  0xF0FF0000;	//PA 23,21,20
	PORTA->SFUNC[2] =  0xC0CC0000; 
  PORTA->SPWR[1] =   0x003C0F00; //10ns
  PORTA->SPULLDOWN = 0xFFFFFFFF;

	/* PB[10,9,3,2,1,0]-MPI_ADDDR; PB[16,15]-UART0; */
	PORTB->KEY = _KEY_;
  PORTB->SANALOG =   0xFFFFFFFF;
	PORTB->CFUNC[0]  = 0x0000FFFF;	// PB[4,3,2,1]
  PORTB->SFUNC[0]  = 0x00000000;
	PORTB->CFUNC[1]  = 0xF0000FF0;	// PB[15,10,9]
  PORTB->SFUNC[1]  = 0x50000000;
	PORTB->CFUNC[2]  = 0x0000000F;	// PB[16]
  PORTB->SFUNC[2] =  0x00000005;
	PORTB->CFUNC[3]  = 0x00000000;
  PORTB->SFUNC[3] =  0x00000000;
  PORTB->SOE =       0x000E0000;
	PORTB->SPWR[1] =   0x00000002;	//100ns
  PORTB->SPWR[0] =   0x80000000;	//100ns
	PORTB->SPULLDOWN = 0x00000000;
	PORTB->SPULLUP =   0x0001860F;
	PORTB->SPD       = 0x00000000;
  	
  /* PC[31:30]-A[1:0]; PC[21:8]-ADC */
  PORTC->KEY = _KEY_;
  PORTC->CANALOG =   0x003FFF00;
	PORTC->SANALOG =   0xFFC000FF;
	PORTC->CFUNC[3] =  0xFF000000; // PC[31,30]
  PORTC->SFUNC[3] =  0x22000000;
	PORTC->CFUNC[0] =  0xFFFFFFFF; // PC[7-0]
  PORTC->SFUNC[0] =  0x00000000;
  PORTC->SPULLDOWN = 0x3FC000FF;

  /* PD[31:30]-D[1:0]; PD[24:23]-~WE,~OE; PD[19]-~CS0;  PD[20]-~CS1; PD[13:0]-A[15:2] */  
  PORTD->KEY = _KEY_;
  PORTD->SANALOG =   0xFFFFFFFF;
  PORTD->SFUNC[2] =  0x02202000;
  PORTD->SFUNC[3] =  0x00000002;
  PORTD->SPWR[0] =   0x3FFFFFFF;
  PORTD->SPWR[1] =   0xF003FCC0;
  PORTD->SPULLDOWN = 0x3E178000;

/*PE[5:0]-D[7:2];  PE[16:19]-PWM; */
  PORTE->KEY = _KEY_;
  PORTE->SANALOG =   0xFFFFFFFF;
  PORTE->CFUNC[0] =  0xFF000000;
  PORTE->SFUNC[0] =  0x00000000;
  PORTE->CFUNC[2] =  0x0000FFFF;
  PORTE->SFUNC[2] =  0x00008888; //PWM PE[16:19]
	PORTE->SPD      =  1<<17; //PWM PE17
	PORTE->SPWR[0]  =  0x00005FFF;
  PORTE->SPWR[1]  =  0x000000FF;
//	PORTE->SOE 	    =  1<<17; //PWM PE17

	

  /*------------ enable regions -----------*/
  EXT_BUS_CNTR->KEY = _KEY_;
  EXT_BUS_CNTR->RGN0_CNTRL = 0x4000361;
  EXT_BUS_CNTR->RGN2_CNTRL = 0x321;

  /*---------- System clock, PLL0 ----------*/
  CLK_CNTR->KEY = _KEY_;
  CLK_CNTR->HSE0_CLK = (1<<27)|(1<<28); // Enable HSE0 gen
  while((CLK_CNTR->HSE0_STAT & (1 << 20)) == 0); //wait HSE0
  CLK_CNTR->PLL0_CLK = (2<<29)|(1<<28)|(1<<27)|(8<<8)|(1<<0);  //PLL0 On, Fout = 10*8/2 = 40MHz
  while((CLK_CNTR->PLL0_CLK & (1<<28)) == 0); //wait PLL ready
  CLK_CNTR->MAX_CLK = 8;  //MAX_CLOCK = PLL0 = 40 MHz
  CLK_CNTR->CPU_CLK = 0;  //Core clock: MAX_CLOCK

  /*---------- Clock for ADC0 -------------*/
  CLK_CNTR->ADC0_CLK = ((uint32_t)1<<28)|(1<<16)| 7;  //HSE0 clock for ADC, freq=0.5 MHz

#ifndef _INT_RAM_
  ICACHE->KEY = _KEY_;
  ICACHE->CNTL = 0x0B;
#endif
}



