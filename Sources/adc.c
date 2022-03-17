#include "1986ve8_lib/cm4ikmcu.h"
#include "adc.h"




uint16_t ADCSample[6];
uint16_t ADCData[CHAN_NUM][WVFRM_LENG];
uint16_t ADCDataPtr = 0;


void ADC_Init() {
  // PC[8]   ADC2    ch0
  // PC[9]   ADC3    ch1
  // PC[10]  ADC4    ch2
  // PC[11]  ADC5    ch3
  // PC[12]  ADC6    ch4
  // PC[13]  ADC7    ch5
  // PC[14]  ADC8    ch6
  int i, sm;
  CLK_CNTR->KEY = _KEY_;
  CLK_CNTR->PER0_CLK |= (1<<24);  //clock TIMER1
  CLK_CNTR->TIM1_CLK = (1<<16)|79;  //timer clock = 1 MHz @ 80MHz
  CLK_CNTR->PER1_CLK |= (1<<23);  //clock ADC0
 
	/* TIMER0 */
  MDR_TMR1->PSG = 0;
  MDR_TMR1->ARR = 40 - 1;  //25 kHz Freq
//MDR_TMR1->ARR = 500 - 1;
  MDR_TMR1->IE = 1;  //interrupt enable
  MDR_TMR1->CNTRL = 1;  //enable timer   /*******/
  NVIC_EnableIRQ(IRQn_TMR1);

  ADC0->KEY = _KEY_;
//  ADC0->CONFIG1 = (255<<12)|(511<<3);  //��� ����� ���������
  ADC0->CONFIG1 = (8<<12)|(12<<3);  //��� ����� ���������
//  ADC0->CONFIG1 = (5<<12)|(5<<3);  //��� ����� ���������
  /* ���������� */
	
  ADC0->FIFOEN1 = 0xF0000000;
  ADC0->CONFIG0 = 1;  //enable ADC0
	
  sm = 0;
  for(i=0; i<4; i++) {
    ADC0->CONTROL = (60 + i)<<8 | 1;
    while((ADC0->STATUS & 1)==0);
    sm = sm + (ADC0->RESULT & 0xFFF);
    }
  sm = sm >> 2;
  ADC0->FIFOEN1 = 0;
  ADC0->CONFIG1 = (ADC0->CONFIG1 & 0xFFFFF) | (sm << 20);
  /**/
  ADC0->CHSEL0 =  0x0000003F;
//  ADC0->FIFOEN0 = 0x0000003F;
  ADC0->CONFIG0 = (1<<5) | 1 | (2<<1);  //SELMODE=1, enable ADC0
  MDR_TMR1->CNTRL = 1;  //enable timer
}


uint32_t ChList;

void INT_TMR1_Handler(void) {
  int i;
  uint32_t rslt;
  uint32_t *resultchx;
  PORTE->SRXTX = (1<<22);
  MDR_TMR1->STATUS &= ~1;
  resultchx = (uint32_t*)0x400AA01C;
  ChList = ADC0->STATRCH0;
  for(i=0; i<6; i++) {
    rslt = resultchx[i];
    ADCSample[i] = rslt;
    if(ADCDataPtr < WVFRM_LENG) 
      ADCData[i][ADCDataPtr] = *((uint16_t*)&rslt);
    }
		if(ADCDataPtr < WVFRM_LENG) ADCDataPtr++;
		PORTE->CRXTX = (1<<22);
}
		

