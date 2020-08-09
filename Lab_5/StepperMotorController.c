// StepperMotorController.c starter file EE319K Lab 5
// Runs on TM4C123
// Finite state machine to operate a stepper motor.  
// Jonathan Valvano
// 1/17/2020

// Hardware connections (External: two input buttons and four outputs to stepper motor)
//  PA5 is Wash input  (1 means pressed, 0 means not pressed)
//  PA4 is Wiper input  (1 means pressed, 0 means not pressed)
//  PE5 is Water pump output (toggle means washing)
//  PE4-0 are stepper motor outputs 
//  PF1 PF2 or PF3 control the LED on Launchpad used as a heartbeat
//  PB6 is LED output (1 activates external LED on protoboard)

#include "SysTick.h"
#include "TExaS.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

void EnableInterrupts(void);
// edit the following only if you need to move pins from PA4, PE3-0      
// logic analyzer on the real board
#define PA4       (*((volatile unsigned long *)0x40004040))
#define PE50      (*((volatile unsigned long *)0x400240FC))

#define Start 0
#define Home  1
#define WP1   2
#define WP2   3
#define WP3   4
#define WP4   5
#define WP5   6 
#define WP6   7
#define WP7   8 
#define WP8   9
#define WP9   10
#define WP10   11
#define WP11   12
#define WP12   13
#define WP13   14
#define WP14   15
#define WP15   16 
#define WP16   17
#define WP17   18 
#define WP18   19
#define WP19   20
#define WS1   21
#define WS2   22
#define WS3   23
#define WS4   24
#define WS5   25  
#define WS6   26 
#define WS7   27
#define WS8   28
#define WS9   29
#define WS10  30
#define WS11  31
#define WS12  32
#define WS13  33
#define WS14  34
#define WS15  35 
#define WS16  36
#define WS17  37 
#define WS18  38
#define WS19  39

void SendDataToLogicAnalyzer(void){
  UART0_DR_R = 0x80|(PA4<<2)|PE50;
}

struct state{
  uint8_t outputs;     // first element
  uint8_t time;     // second element
  uint8_t nextState[4];   // third element
};

typedef const struct state stateType;

stateType FSM[40]= {
	{0, 20, {Start, WP1, WS1, WS1}},
	{0, 80, {Start, Start, Start, Start}},
	{1, 50, {WP2, WP2, WP2, WP2}},
	{2, 50, {WP3, WP3, WP3, WP3}},
	{4, 50, {WP4, WP4, WP4, WP4}},
	{8, 50, {WP5, WP5, WP5, WP5}},
	{16, 50, {WP6, WP6, WP6, WP6}},
	{1, 50, {WP7, WP7, WP7, WP7}},
	{2, 50, {WP8, WP8, WP8, WP8}},
	{4, 50, {WP9, WP9, WP9, WP9}},
	{8, 50, {WP10, WP10, WP10, WP10}},
	{16, 100, {WP11, WP11, WP11, WP11}},
	{8, 50, {WP12, WP12, WP12, WP12}},
	{4, 50, {WP13, WP13, WP13, WP13}},
	{2, 50, {WP14, WP14, WP14, WP14}},
	{1, 50, {WP15, WP15, WP15, WP15}},
	{16, 50, {WP16, WP16, WP16, WP16}},
	{8, 50, {WP17, WP17, WP17, WP17}},
	{4, 50, {WP18, WP18, WP18, WP18}},
	{2, 50, {WP19, WP19, WP19, WP19}},
	{1, 50, {Home, Home, Home, Home}},
	{33, 50, {WS2, WS2, WS2, WS2}},
	{2, 50, {WS3, WS3, WS3, WS3}},
	{36, 50, {WS4, WS4, WS4, WS4}},
	{8, 50, {WS5, WS5, WS5, WS5}},
	{48, 50, {WS6, WS6, WS6, WS6}},
	{1, 50, {WS7, WS7, WS7, WS7}},
	{34, 50, {WS8, WS8, WS8, WS8}},
	{4, 50, {WS9, WS9, WS9, WS9}},
	{40, 50, {WS10, WS10, WS10, WS10}},
	{16, 100, {WS11, WS11, WS11, WS11}},
	{40, 50, {WS12, WS12, WS12, WS12}},
	{4, 50, {WS13, WS13, WS13, WS13}},
	{34, 50, {WS14, WS14, WS14, WS14}},
	{1, 50, {WS15, WS15, WS15, WS15}},
	{48, 50, {WS16, WS16, WS16, WS16}},
	{8, 50, {WS17, WS17, WS17, WS17}},
	{36, 50, {WS18, WS18, WS18, WS18}},
	{2, 50, {WS19, WS19, WS19, WS19}},
	{33, 50, {Home, Home, Home, Home}}};

uint8_t S = Start;
uint8_t input = 0;
	
int main(void){ 
  TExaS_Init(&SendDataToLogicAnalyzer);    // activate logic analyzer and set system clock to 80 MHz
  SysTick_Init();   
// you initialize your system here
	SYSCTL_RCGCGPIO_R |= 0x33;
	__nop();
	__nop();
	GPIO_PORTE_DIR_R |= 0x3F;
	GPIO_PORTA_DIR_R &= ~0x30;
	GPIO_PORTF_DIR_R |= 0x7;//
	GPIO_PORTB_DIR_R |= 0x40;
	GPIO_PORTE_DEN_R |= 0x3F;
	GPIO_PORTA_DEN_R |= 0x30;
	GPIO_PORTF_DEN_R |= 0xE;
	GPIO_PORTB_DEN_R |= 0x40;
	
  EnableInterrupts();   
	
  while(1){
// output
  GPIO_PORTE_DATA_R |= FSM[S].outputs;
	GPIO_PORTE_DATA_R &= (FSM[S].outputs + 192);
// wait
	SysTick_Wait1ms(FSM[S].time);
// input
  input = GPIO_PORTA_DATA_R;
  input = input>>4;		
	input &= 3;
// next		
	S = FSM[S].nextState[input];
  }
}


