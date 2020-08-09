// SlidePot.cpp
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 and use a slide pot to measure distance
// Created: 3/28/2018 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "SlidePot.h"
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void){ 
//*** students write this ******
	SYSCTL_RCGCADC_R |= 1;
	SYSCTL_RCGCGPIO_R |= 0x8;
	__nop();
	__nop();
	__nop();
	__nop();
	GPIO_PORTD_DIR_R &= ~(4);
	GPIO_PORTD_DEN_R |= 4;
	GPIO_PORTD_AFSEL_R |= 4;
	GPIO_PORTD_AMSEL_R |= 4;
	ADC0_PC_R |= 1;
	ADC0_SSPRI_R = 0x123;
	ADC0_ACTSS_R &= ~8;
	ADC0_EMUX_R &= ~(0xF000);
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R & 0xFFFFFFF0) + 5;
	ADC0_SSCTL3_R = 6;
	ADC0_IM_R &= ~(8);
	ADC0_ACTSS_R |= 8;
	ADC0_SAC_R = 6;
}

//------------ADCIn------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
//*** students write this ******
	int result;
	ADC0_PSSI_R = 0x0008;
	while((ADC0_RIS_R&8) == 0) {}
	//read data and do what you want with it
	result = ADC0_SSFIFO3_R&0xFFF;
	ADC0_ISC_R &= 8;
  return result; // remove this, replace with real code
}

// constructor, invoked on creation of class
// m and b are linear calibration coeffients 
SlidePot::SlidePot(uint32_t m, uint32_t b){
//*** students write this ******
// initialize all private variables
// make slope equal to m and offset equal to b
    slope = m;
    offset = b;
}

void SlidePot::Save(uint32_t n){
//*** students write this ******
// 1) save ADC sample into private variable
// 2) calculate distance from ADC, save into private variable
// 3) set semaphore flag = 1
    data = n;
    distance = (slope*n)/4096+offset;
    flag = 1;
}
uint32_t SlidePot::Convert(uint32_t n){
  //*** students write this ******
  // use calibration data to convert ADC sample to distance
    
  return slope*n/4096+offset; // replace this with solution
}

void SlidePot::Sync(void){
// 1) wait for semaphore flag to be nonzero
// 2) set semaphore flag to 0
    while(flag == 0){
    }
    flag = 0;
}

uint32_t SlidePot::ADCsample(void){ // return ADC sample value (0 to 4095)
  //*** students write this ******
  // return last calculated ADC sample
  return data; // replace this with solution
}

uint32_t SlidePot::Distance(void){  // return distance value (0 to 2000), 0.001cm
  //*** students write this ******
  // return last calculated distance in 0.001cm
  return distance; // replace this with solution
}


