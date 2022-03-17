#include "1986ve8_lib/cm4ikmcu.h"
#include <string.h>
#include <stdio.h>
#include "sysinit.h"
#include "eerom.h"
#include "mko.h"
#include "adc.h"
#include "wdt.h"
#include "uarts.h"
#include "timer.h"


#define MKO_Id      22

int i,n;
uint8_t Buff[256];
extern uint16_t ADCData[CHAN_NUM][WVFRM_LENG];
extern uint16_t ADCDataPtr;
int shift = 0;


int main() {
	uint8_t leng;
  uint16_t adc_data[1*1024];
	
  System_Init();
	Timers_Init();
	UART0_Init();
	ADC_Init();
	
//	PORTE->SRXTX = (1<<17);
//	PORTE->CRXTX = (1<<17);
	
	
	sprintf((char*)Buff, "START\n\r");
	UART0_SendPacket(Buff, strlen((char*)Buff), 0);
	
	sprintf((char*)Buff, "Time is set\n\r");
	UART0_SendPacket(Buff, strlen((char*)Buff), 0);
	
	Timers_Start(3, 1000000/5); //	1Hz(Buff[2]), 255Hz(Buff[255]) 

while(1) {
    WDRST;

    if(UART0_GetPacket(Buff, &leng)) {
      if(Buff[0] == 'A') {
				//ADCDataPtr = 0;
				i=0;
				n=Buff[1];
				memcpy(adc_data, ADCData[n], sizeof(adc_data));		
				while (i<16){
					memcpy(Buff,&adc_data[i*64], 128);		
					UART0_SendPacket(Buff, 128, 0);
					i=i+1;
					}
				ADCDataPtr = 0;
			}			
      else if(Buff[0] == 'B') {
        sprintf((char*)Buff, "%X\n\r", EXT_BUS_CNTR->RGN0_CNTRL);
        UART0_SendPacket(Buff, strlen((char*)Buff), 0);
      }
      else if(Buff[0] == 'R') {
        PORTE->SRXTX = 0x10000;
      }
			else if(Buff[0] == 'T') {
				if(Buff[1] == '0'){
					Timers_Stop(3);
					sprintf((char*)Buff, "PWM is disable\n\r");
					UART0_SendPacket(Buff, strlen((char*)Buff), 0);
				}
				else if(Buff[1] == '2'){
					Timers_Start(3, 1000000/Buff[2]); //	1Hz(Buff[2]), 255Hz(Buff[255]) 
					sprintf((char*)Buff, "Time is set\n\r");
					UART0_SendPacket(Buff, strlen((char*)Buff), 0);
				}
			}	
		}
	}
}
