// Timer1.c
// Runs on LM4F120/TM4C123
// Use TIMER1 in 32-bit periodic mode to request interrupts at a periodic rate
// Daniel Valvano
// Last Modified: 1/17/2020 
// You can use this timer only if you learn how it works

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013
  Program 7.5, example 7.6

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>
#include "Timer1.h"
#include "ST7735.h"
#include "../inc/tm4c123gh6pm.h"


extern "C" void GPIOF_Handler(void);


int* F0;
int* F4;
void Flag_Init(int*x, int* y){
	F0 = x;
	F4 = y;
}

void Button_Init(void){volatile long delay;                            
  SYSCTL_RCGCGPIO_R |= 0x20;           // activate port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
  delay = 100;                  //    allow time to finish activating
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0                              // 2) GPIO Port F needs to be unlocked
  GPIO_PORTF_AMSEL_R &= ~0x11;  // 3) disable analog on PF4,0
                                // 4) configure PF4,0 as GPIO
  GPIO_PORTF_PCTL_R &= ~0x000F000F;
  GPIO_PORTF_DIR_R &= ~0x11;    // 5) make PF4,0 in
  GPIO_PORTF_AFSEL_R &= ~0x11;  // 6) disable alt funct on PF4,0
	GPIO_PORTF_PUR_R |= 0x11;     
  GPIO_PORTF_DEN_R |= 0x11;     // 7) enable digital I/O on PF4,0
  GPIO_PORTF_IS_R &= ~0x11;     //    PF4,0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    //    PF4,0 is not both edges
  GPIO_PORTF_IEV_R &= ~0x11;     //    PF4,0 falling edge event (Neg logic)
  GPIO_PORTF_ICR_R = 0x11;      //    clear flag1-0
  GPIO_PORTF_IM_R |= 0x11;      // 8) arm interrupt on PF4,0
                                // 9) GPIOF priority 2
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF0FFFFF)|0x00400000;
  NVIC_EN0_R = 1<<30;   // 10)enable interrupt 30 in NVIC
}

int Paused = 0;
void GPIOF_Handler(void){
	if ((GPIO_PORTF_RIS_R & 0x01)==0x01){ //PF0
		F0[0] = 1;
		if(!Paused){
			ST7735_FillRect(0,0,160,128,0);
			ST7735_DrawStringS(0,0,"Game Paused",0xF00F,0xF00F);
			Paused = 1;
		}
		
	}
	if ((GPIO_PORTF_RIS_R & 0x10)==0x10){ //PF4
		F4[0] = 1;
	}	
  GPIO_PORTF_ICR_R = 0x11;      // acknowledge flag4
}
