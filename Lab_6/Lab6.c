// Lab6.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 1/17/2020 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#define C 2389
#define D 4257
#define E 3792
#define F 3579
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts


int main(void){      
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  Piano_Init();
  Sound_Init();
  // other initialization
	SYSCTL_RCGCGPIO_R |= 0x20;
	__nop();
	__nop();
	GPIO_PORTF_DIR_R |= 0x4;
	GPIO_PORTF_DEN_R |= 0x4;
	uint32_t count = 10000;
  EnableInterrupts();
  while(1){ 
		int data = Piano_In();
		if(data == 0){
			NVIC_ST_CTRL_R = 5;
		}if(data == 1){
			Sound_Play(C); 
		}else if(data == 2){
			Sound_Play(D); 
		}else if(data == 4){
			Sound_Play(E);
		}else if(data == 7){
			Sound_Play(F);
		}
		
	  if(count == 0){
			GPIO_PORTF_DATA_R ^= 0x4;
			count = 1000000;
		}else{
			count--;
		}
  }    
}


