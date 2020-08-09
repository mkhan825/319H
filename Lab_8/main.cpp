// Lab8.cpp
// Runs on LM4F120 or TM4C123
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 1/17/2020 

// Specifications:
// Measure distance using slide pot, sample at 60 Hz
// maximum distance can be any value from 1.5 to 2cm
// minimum distance is 0 cm
// Calculate distance in fixed point, 0.01cm
// Analog Input connected to PD2=ADC5
// displays distance on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats (use them in creative ways)
// must include at least one class used in an appropriate way

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "TExaS.h"
#include "PLL.h"
#include "SlidePot.h"
#include "print.h"

SlidePot Sensor(159,16);

extern "C" void DisableInterrupts(void);
extern "C" void EnableInterrupts(void);
extern "C" void SysTick_Handler(void);

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PF4       (*((volatile uint32_t *)0x40025040))
// **************SysTick_Init*********************
// Initialize Systick periodic interrupts
// Input: interrupt period
//        Units of period are 12.5ns
//        Maximum is 2^24-1
//        Minimum is determined by length of ISR
// Output: none
void SysTick_Init(unsigned long period){
  //*** students write this ******
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = period;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 7;
}

// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
  //*** students write this ******
	SYSCTL_RCGCGPIO_R |= 0x20;
  __nop();
  __nop();
  GPIO_PORTF_DIR_R |= 14;
  GPIO_PORTF_DEN_R |= 14;
}
/*
uint32_t Data;      // 12-bit ADC
uint32_t Position;  // 32-bit fixed-point 0.01 cm

int main(void){      // single step this program and look at Data
  DisableInterrupts();
  TExaS_Init();      // start scope set system clock to 80 MHz
  ADC_Init();        // turn on ADC, PD2, set channel to 5
  EnableInterrupts();
  while(1){                
    Data = ADC_In();  // sample 12-bit channel 5, PD2
  }
}
*/
/*
uint32_t time0,time1,time2,time3;
uint32_t ADCtime,OutDectime; // in usec
uint32_t Data;    	// 12-bit ADC
int main(void){
  TExaS_Init();   	// Bus clock is 80 MHz
	NVIC_ST_CTRL_R = 0;
  NVIC_ST_RELOAD_R = 0x00FFFFFF; // maximum reload value
  NVIC_ST_CURRENT_R = 0;      	// any write to current clears it
  NVIC_ST_CTRL_R = 5;
  ADC_Init();     	// turn on ADC, set channel to 5
  ADC0_SAC_R = 4;   // 16-point averaging, move this line into your ADC_Init()
  ST7735_InitR(INITR_REDTAB);
  while(1){       	// use SysTick 
    time0 = NVIC_ST_CURRENT_R;
    Data = ADC_In();  // sample 12-bit channel 5
    time1 = NVIC_ST_CURRENT_R;
    ST7735_SetCursor(0,0);
    time2 = NVIC_ST_CURRENT_R;
    LCD_OutDec(Data);
    ST7735_OutString("  ");  // spaces cover up characters from last output
    time3 = NVIC_ST_CURRENT_R;
    ADCtime = ((time0-time1)&0x0FFFFFF)/80;	// usec
    OutDectime = ((time2-time3)&0x0FFFFFF)/80; // usec
  }
}
*/

/*
uint32_t Data;      // 12-bit ADC
uint32_t Position;
double Convert(uint32_t n){
  *** students write this ******
  // use calibration data to convert ADC sample to distance
    
  return 159*Data/4096+17; // replace this with solution
}

int main(void){
  TExaS_Init();     	// Bus clock is 80 MHz
  ST7735_InitR(INITR_REDTAB);
  ADC_Init();     	// turn on ADC, set channel to 5
  ADC0_SAC_R = 4;   // 16-point averaging, move this line into your ADC_Init()
  while(1){  
    Data = ADC_In();  // sample 12-bit channel 5
    Position = Convert(Data);
    ST7735_SetCursor(0,0);
    LCD_OutDec(Data); ST7735_OutString("  ");
    ST7735_SetCursor(6,0);
    LCD_OutFix(Position); // your Lab 7 solution
  }
}
*/



// final main program to create distance meter
int Data;
int main(void){ 
    //*** students write this ******
  DisableInterrupts();
  TExaS_Init();    // bus clock at 80 MHz
  ST7735_InitR(INITR_REDTAB); 
  ADC_Init();        // turn on ADC, PD2, set channel to 5
  PortF_Init();
	SysTick_Init(8000000);
  EnableInterrupts();
  while(1){
    Sensor.Sync(); // wait for semaphore
    // can call Sensor.ADCsample, Sensor.Distance, Sensor.Convert as needed 
    ST7735_SetCursor(0,0);
		LCD_OutFix(Sensor.Distance());
    ST7735_OutString(" cm");
  }
}

void SysTick_Handler(void){ // every sample
    //*** students write this ******
// should call ADC_In() and Sensor.Save
	GPIO_PORTF_DATA_R ^= 4; // Turns the led on and off 5Hz
	//Connect PF2 to PD3
  Sensor.Save(ADC_In());
}

