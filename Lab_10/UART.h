// UART.h
// Runs on LM4F120/TM4C123
// This code runs on UART0 with interrupts and a simple FIFO
// EE319K tasks
// 1) Convert to UART1 PC4 PC5
// 2) Implement the FIFO as a class
// 3) Run transmitter with busy-wait synchronization
// 4) Run receiver with 1/2 full FIFO interrupt
// Daniel and Jonathan Valvano
// 1/17/2020


/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2017
   Program 5.11 Section 5.6, Program 3.10

 Copyright 2018 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// Initial code uses UART0
// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

// EE319K Lab 9, change to U1Rx connected to PC4 interrupt
// EE319K Lab 9, change to U1Tx connected to PC5 busy wait
// EE319K Lab 9 hardware
// System 1        System 2
//   PC4 ----<<----- PC5
//   PC5 ---->>----- PC4
//   Gnd ----------- Gnd
#ifndef __UART_H__
#define __UART_H__
#include <stdint.h>
// standard ASCII symbols
struct sprite{
	private:
  uint32_t x;      // x coordinate
  uint32_t y;      // y coordinate
	uint32_t length; //in pixels
	uint32_t width;
  const unsigned short *image; // ptr->image
	public:
		sprite(uint32_t xc, uint32_t yc, const unsigned short *imagec, uint32_t length, uint32_t width);
		void move(uint32_t xnew, uint32_t ynew);
		void print(void);
};


#endif 
