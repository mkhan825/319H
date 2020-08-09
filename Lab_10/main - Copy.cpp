// main.cpp
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10 in C++

// Last Modified: 1/17/2020 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2017

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2017

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
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Random.h"
#include "SlidePot.h"
#include "Images.h"
#include "UART.h"
#include "Timer0.h"
#include "Timer1.h"
#include "Sound.h"
#include "LCD.h"

#define BLACK   0x0000
#define BLUE    0x008F
#define RED     0xF100
#define GREEN   0x0E70
#define CYAN    0x0EFF
#define MAGENTA 0xF18F
#define YELLOW  0xFF70
#define WHITE   0xFFFF

SlidePot my(1500,0);

extern "C" void DisableInterrupts(void);
extern "C" void EnableInterrupts(void);
extern "C" void SysTick_Handler(void);

void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x07;      // enable SysTick with core clock
}
// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(uint32_t delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}
// The delay parameter is in ms.
// 80000 bus cycles equals 1ms
void SysTick_Wait1ms(uint32_t delay){
  uint32_t i;
  for(i=0; i<delay; i++){
    SysTick_Wait(80000);  // wait 10ms
  }
}

// 800000 bus cycles equals 10ms
void SysTick_Wait10ms(uint32_t delay){
  uint32_t i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}

void Wait1ms(uint32_t delay) {
	delay = delay*80000;
	for(int i = delay; i >= 0; i--) {}
}

void ST7735_Init(void) {
	Output_Init();
	SYSCTL_RCGCGPIO_R |= 0x20;
	__nop();
	__nop();
	GPIO_PORTF_DIR_R |= 0x4;
	GPIO_PORTF_DEN_R |= 0x4;
	PLL_Init(Bus80MHz);
}

void DrawBoard(){
	ST7735_FillScreen(0x000F);
	
	ST7735_DrawCircle(10,17,0);
	ST7735_DrawCircle(30,17,0);
	ST7735_DrawCircle(50,17,0);
	ST7735_DrawCircle(70,17,0);
	ST7735_DrawCircle(90,17,0);
	ST7735_DrawCircle(110,17,0);
	ST7735_DrawCircle(130,17,0);
	
	ST7735_DrawCircle(10,34,0);
	ST7735_DrawCircle(30,34,0);
	ST7735_DrawCircle(50,34,0);
	ST7735_DrawCircle(70,34,0);
	ST7735_DrawCircle(90,34,0);
	ST7735_DrawCircle(110,34,0);
	ST7735_DrawCircle(130,34,0);
	
	ST7735_DrawCircle(10,51,0);
	ST7735_DrawCircle(30,51,0);
	ST7735_DrawCircle(50,51,0);
	ST7735_DrawCircle(70,51,0);
	ST7735_DrawCircle(90,51,0);
	ST7735_DrawCircle(110,51,0);
	ST7735_DrawCircle(130,51,0);
	
	ST7735_DrawCircle(10,68,0);
	ST7735_DrawCircle(30,68,0);
	ST7735_DrawCircle(50,68,0);
	ST7735_DrawCircle(70,68,0);
	ST7735_DrawCircle(90,68,0);
	ST7735_DrawCircle(110,68,0);
	ST7735_DrawCircle(130,68,0);
	
	ST7735_DrawCircle(10,85,0);
	ST7735_DrawCircle(30,85,0);
	ST7735_DrawCircle(50,85,0);
	ST7735_DrawCircle(70,85,0);
	ST7735_DrawCircle(90,85,0);
	ST7735_DrawCircle(110,85,0);
	ST7735_DrawCircle(130,85,0);
	
	ST7735_DrawCircle(10,102,0);
	ST7735_DrawCircle(30,102,0);
	ST7735_DrawCircle(50,102,0);
	ST7735_DrawCircle(70,102,0);
	ST7735_DrawCircle(90,102,0);
	ST7735_DrawCircle(110,102,0);
	ST7735_DrawCircle(130,102,0);
}

int colpre;

void DrawArrow(int col, sprite cursor) {
	if(col == 0) {
		if(col != colpre){
			cursor.move(10,10);
			ST7735_FillRect(0,0,160,15,0x000F);
			cursor.print();
			colpre = col;
		}
	}
  if(col == 1) {
		if(col != colpre){
			cursor.move(30,10);
			ST7735_FillRect(0,0,160,15,0x000F);
			cursor.print();
			colpre = col;
		}
	}
	if(col == 2) {
		if(col != colpre){
			cursor.move(50,10);
			ST7735_FillRect(0,0,160,15,0x000F);
			cursor.print();
			colpre = col;
		}
	}
	if(col == 3) {
		if(col != colpre){
			cursor.move(70,10);
			ST7735_FillRect(0,0,160,15,0x000F);
			cursor.print();
			colpre = col;
		}
	}
	if(col == 4) {
		if(col != colpre){
			cursor.move(90,10);
			ST7735_FillRect(0,0,160,15,0x000F);
			cursor.print();
			colpre = col;
		}
	}
	if(col == 5) {
		if(col != colpre){
			cursor.move(110,10);
			ST7735_FillRect(0,0,160,15,0x000F);
			cursor.print();
			colpre = col;
		}
	}
	if(col == 6) {
		if(col != colpre){
			cursor.move(130,10);
			ST7735_FillRect(0,0,160,15,0x000F);
			cursor.print();
			colpre = col;
		}
	}
}

//main to test Sound and DAC
/*
int main(void){
	Sound_Init();
	while(1){
	Sound_lose();
		while(1){
		}
	}
}
*/
/*
//main to test SlidePot
int data = 0;
int mainSP(void){
	SlidePot sp(6, 0);
	ADC_Init();
	while(1){
		data = sp.Convert(ADC_In());
		for(int x = 0; x< 400000; x++){
		}
	}
}
*/
/*
//main to test buttons
int F4Pressed = 0;
int F0Pressed = 0;
int mainButton(void){
	Button_Init();
	Flag_Init(&F0Pressed, &F4Pressed);
	GPIO_PORTF_DIR_R |= 0x4;
	GPIO_PORTF_DEN_R |= 0x4;
	EnableInterrupts();
	while(1){
		if(F4Pressed == 1){
			GPIO_PORTF_DATA_R ^= 0x4;
			F4Pressed = 0;
		}
		if(F0Pressed == 1){
			GPIO_PORTF_DATA_R ^= 0x4;
			F0Pressed = 0;
		}
	}
}
*/

//main to test LCD
/*
int main(void){
	PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);
  Output_Init();
	
	ST7735_OutUDec(10);
	for(int x = 0; x<4000000; x++){
	}
	sprite test(0, 9, SmallEnemy10pointA);
	test.print(16,10);
	
	return 0;
}
*/
const unsigned short cursor[] = {
 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0xF820, 0xF820,
 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0xF820, 0xF820, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F,
 0x000F, 0xF820, 0xF820, 0xF820, 0xF820, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0xF820, 0xF820, 0xF820, 0xF820, 0x000F,
 0x000F, 0x000F, 0x000F, 0x000F, 0xF820, 0xF820, 0xF820, 0xF820, 0xF820, 0xF820, 0x000F, 0x000F, 0x000F, 0x000F, 0xF820, 0xF820,
 0xF820, 0xF820, 0xF820, 0xF820, 0x000F, 0x000F, 0x000F, 0xF820, 0xF820, 0xF820, 0xF820, 0xF820, 0xF820, 0xF820, 0xF820, 0x000F,
 0x000F, 0xF820, 0xF820, 0xF820, 0xF820, 0xF820, 0xF820, 0xF820, 0xF820, 0x000F, 0xF820, 0xF820, 0xF820, 0xF820, 0xF820, 0xF820,
 0xF820, 0xF820, 0xF820, 0xF820,
};

const char Names_E[] = "By Masaad K. and Ravi A.";
const char Names_F[] = "By Masaad K. and Ravi A.";
const char Action_E[] ="Press to Continue";
const char Action_F[] ="Appuyer sur la touche";
const char SelCol_E[] = "Select your Column";
const char SelCol_F[] = "Choisir sa Colonne";
const char P1_Win_E[] = "Player 1 wins!";
const char P1_Win_F[] = "Joueur 1 a gagne!";
const char P2_Win_E[] = "CPU has won!";
const char P2_Win_F[] = "CPU a gagné!";
const char InvalMov_E[] = "Invalid Move!";
const char InvalMov_F[] = "Avance Incorrect!";
const char Language_E[] = "English";
const char Language_F[] = "Francais";
const char P1_Turn_E[] = "Player 1's turn";
const char P1_Turn_F[] = "Tour du Joueur 1"; //****Change to French
const char P2_Turn_E[] = "CPU's turn";
const char P2_Turn_F[] = "Tour du CPU";
const char Yes_E[] = "Yes";
const char Yes_F[] = "Oui";
const char No_E[] = "No";
const char No_F[] = "Non";
const char Thanks_E[] = "Thanks for Playing!";
const char Thanks_F[] = "Merci pour Jouer!";
const char Final_E[] = "Final Score: ";
const char Final_F[] = "Score Finale: ";
const char *strings[13][2] = {
														{Names_E,Names_F},
														{Action_E,Action_F},
														{SelCol_E,SelCol_F},
														{P1_Win_E,P1_Win_F},
														{P2_Win_E,P2_Win_F},
														{InvalMov_E,InvalMov_F},
														{Language_E,Language_F},
														{P1_Turn_E,P1_Turn_F},
														{P2_Turn_E,P2_Turn_F},
														{Yes_E,Yes_F},
														{No_E,No_F},
														{Thanks_E,Thanks_F},
														{Final_E,Final_F}};

int language = 0;

void selecter(int select){
	if(language == 0) {
		ST7735_FillRect(30,30,100,25,0xFF00);
		ST7735_DrawStringS(9,4,"English",0xF000,0xF000); 
		ST7735_DrawStringS(9,8,"Francais",0xF000,0xF000);
	} else {
		ST7735_DrawStringS(9,4,"English",0xF000,0xF000);
		ST7735_FillRect(30,72,100,25,0xFF00);
		ST7735_DrawStringS(9,8,"Francais",0xF000,0xF000);
	}
}


int F4Pressed = 0;
int F0Pressed = 0;
void mainMenu(void) { // Main Menu
	DisableInterrupts();
	ADC_Init();
	ST7735_SetRotation(1);
	ST7735_FillScreen(0);
	EnableInterrupts();
	while(1){
		if(ADC_In() > 2048){ //Use slidepot input here halfed
			if(language == 1) {
				ST7735_FillRect(0,0,160,128,0);
			}
			language = 0;
			selecter(language);
		} else {
			if(language == 0) {
				ST7735_FillRect(0,0,160,128,0x000);
			}
			language = 1;
			selecter(language);
		}
		if(F0Pressed == 1){
			
			Wait1ms(10);
			ST7735_FillRect(0,0,160,128,0x000);
			DrawBoard();
			ST7735_FillRect(30,30,90,70,0x000F);
			ST7735_DrawStringS(6,6,"Connect Four!",0xF00F,0xF00F);
			ST7735_DrawStringS(5,7,(char*) strings[1][language],0xF00F,0xF00F);
			Wait1ms(10);
			F0Pressed = 0;
			while(F0Pressed == 0) {}
			Wait1ms(10);
			F0Pressed = 0;
			DrawBoard();
			break;
		}
	}
}

char array[6][7];

void DrawChips(){
	for(int x = 0; x < 7; x++) {
		for(int y = 0; y < 6; y++) {
			if(array[y][x] == '0') {
			ST7735_DrawCircle((20*x+10),(17*(y+1)),BLACK);
			}else if(array[y][x] == 'r') {
				ST7735_DrawCircle((20*x+10),(17*(y+1)),RED);
			} else if(array[y][x] == 'y') {
				ST7735_DrawCircle((20*x+10),(17*(y+1)),YELLOW);
			}
		}
	}
}

void initArray(){
    for(int x = 0; x < 6; x++){
        for(int y = 0; y<7; y++){
            array[x][y] = '0';
        }
    }
}

int placePiece(int col, int turn){
    int row = 0;
    if(array[row][col] != '0'){
        return 0;
    }
    while( row < 6 && array[row][col] == '0'){
        row++;
    }
    row--;
    if(turn == 0){
        array[row][col] = 'r';
    }
    else{
        array[row][col] = 'y';
    }
		//print board with new piece
    return 1;
}

int checkifwon(int col, int turn){
    char color;
    if(turn == 0){
        color = 'r';
    }else{
        color = 'y';
    }
    int row = 0;
    while( row < 6 && array[row][col] == '0'){
        row++;
    }
    //horizontals
    for(int x = col - 3; x <= col; x++){
        if(x >= 0 && x < 4){
            if(array[row][x] == color && array[row][x+1] == color && array[row][x+2] == color && array[row][x+3] == color){
                return 0;
            }
        }
    }
    //verticals
    for(int y = row - 3; y <= row; y++){
        if(y >= 0 && y < 3){
            if(array[y][col] == color && array[y+1][col] == color && array[y+2][col] == color && array[y+3][col] == color){
                return 0;
            }
        }
    }
    //diagonal - first
    int i = row - 3;
    int j = col - 3;
    while(i <= row){
        if(i>= 0 && i<3 && j>= 0 && j<4){
            if(array[i][j] == color && array[i+1][j+1] == color && array[i+2][j+2] == color && array[i+3][j+3] == color){
                return 0;
            }
        }
        i++;
        j++;
    }
    //diagonal - second
    i = row - 3;
    j = col + 3;
    while(i <= row){
        if(i>= 0 && i<3 && j>= 3 && j<7){
            if(array[i][j] == color && array[i+1][j-1] == color && array[i+2][j-2] == color && array[i+3][j-3] == color){
                return 0;
            }
        }
        i++;
        j--;
    }
    return 1;
}

int findbestplace(){
    int col = 0;
    bool found = false;
    for(int x = 0; x<7; x++){
        if(placePiece(x, 1) == 1){
            if(checkifwon(x, 1) == 0){
                col = x;
                found = true;
            }
            int row = 0;
            while(array[row][x] == '0'){
                row++;
            }
            array[row][x] = '0';
            if(found){
                return col;
            }

        }
    }
    for(int x = 0; x<7; x++){
        if(placePiece(x, 0) == 1){
            if(checkifwon(x, 0) == 0){
                col = x;
                found = true;
            }
            int row = 0;
            while(array[row][x] == '0'){
                row++;
            }
            array[row][x] = '0';
            if(found){
                return col;
            }
        }
    }
    col = (((int)Random()*7)/255);
    return col;
}
int Score1 = 0;
int Score2 = 0;
int Tot_Chips = 0;
int main(void){
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);
  Output_Init();
  Button_Init();
	Sound_Init();
	Flag_Init(&F0Pressed, &F4Pressed);
	SlidePot sp(7, 0);
	SysTick_Init();
	Random_Init(2);
	EnableInterrupts();
  
	////////////////////////////choose language
	//print language selector with english hovered over
	mainMenu();
	sprite arrow(10,10,cursor, 10, 10);
	bool gameisrunning = true;
	bool notQuit = true;
	int turn = 0;
  while(notQuit){
		initArray();
		DrawBoard();
    while(gameisrunning){
			int col = sp.Convert(ADC_In());
			//draw cursor on top of col 0 - 7 depending on sp.Convert(ADC_In()) which gives a number from 0 - 7
			DrawArrow(col, arrow);
			if(turn == 0){
            //print "player 1's turn" at the top
				ST7735_DrawString(0,12,(char*) strings[7][language],0xF000);
				if(F4Pressed == 1){
					if(placePiece(col, turn) == 0){
							//print that the move was invalid
						ST7735_DrawString(0,0,(char*) strings[5][language],0xF000);
					}else {
							//the updated printing should be done in the placepiece function
							Tot_Chips++;
							DrawChips();
							if(checkifwon(col, turn) == 0){
									if(turn == 0) {
											//print "Player 1 wins"
											//play win sound
										//Sound_win();
										ST7735_FillRect(0,0,160,128,0x000);
										ST7735_FillRect(15,50,100,30,0x00F);
										ST7735_DrawStringS(3,6,(char*)strings[3][language],0xF00F,0xF00F);
										ST7735_DrawStringS(15,0,"EE319K",0xF00F,0xF00F);
										ST7735_DrawStringS(14,1,(char*) strings[0][language],0xF00F,0xF00F);
										Score1++;
									} else {
											//print "CPU wins"
											//play lose sound
										//Sound_lose();
										ST7735_FillRect(0,0,160,128,0x000);
										ST7735_FillRect(30,50,100,30,0x00F);
										ST7735_DrawStringS(6,6,(char*)strings[4][language],0xF00F,0xF00F);	
										ST7735_DrawStringS(15,0,"EE319K",0xF00F,0xF00F);
										ST7735_DrawStringS(14,1,(char*) strings[0][language],0xF00F,0xF00F);
										Score2++;
									}
									gameisrunning = false;
							}
							turn = 1;
					}
					Wait1ms(20);
					F4Pressed = 0;
				}
      }else{
				col = findbestplace();
				placePiece(col, turn);
				Tot_Chips++;
				DrawChips();
				Wait1ms(20);
				F4Pressed = 0;
				turn = 0;
			}
			if(checkifwon(col, turn) == 0){
				//Sound_lose();
        ST7735_FillRect(0,0,160,128,0x000);
				ST7735_FillRect(30,50,100,30,0x00F);
				ST7735_DrawStringS(6,6,(char*)strings[4][language],0xF00F,0xF00F);    
				ST7735_DrawStringS(15,0,"EE319K",0xF00F,0xF00F);
				ST7735_DrawStringS(14,1,(char*) strings[0][language],0xF00F,0xF00F);
				gameisrunning = false;
				Score2++;
			}
		}
		//print score
		//wait a bit
		//print "play again?" with yes or no option
		int changedprev;
		bool playagainchosen = false;
		while(!playagainchosen){
			if (ADC_In() < 2048){
				if(changedprev != 0) {
					notQuit = true;
					//print screen with yes hovered over
					ST7735_DrawStringS(3,9,"Quit?",RED,RED);
					ST7735_DrawStringS(3,10,(char*) strings[9][language],RED,YELLOW);
					ST7735_DrawStringS(3,11,(char*) strings[10][language],BLUE,BLACK);
				}
					changedprev = 0;
				}
			if(ADC_In() >= 2048) {
				if(changedprev != 1){
				notQuit = false;
					//print screen with no hovered over
					ST7735_DrawStringS(3,9,"Quit?",RED,RED);
					ST7735_DrawStringS(3,10,(char*) strings[9][language],BLUE, BLACK);
					ST7735_DrawStringS(3,11,(char*) strings[10][language],RED, YELLOW);
				}
				changedprev = 1;
			}
			if(F4Pressed == 1 && changedprev == 1){
				Wait1ms(20);
				F4Pressed = 0;
				playagainchosen = true;
				gameisrunning = true;
				notQuit = true;
				//play selection noise
				//wait until noise is over using systick
				//Sound_click();
			} else if(F4Pressed == 1 && changedprev == 0){
				ST7735_FillRect(0,0,160,128,0x000);
				ST7735_DrawStringS(0,0,(char*) strings[11][language],RED, YELLOW);
				ST7735_DrawStringS(0,2,(char*) strings[12][language],RED, YELLOW);
				ST7735_SetCursor(10,4);
				ST7735_OutUDec(Score1);
				ST7735_DrawStringS(10,4,":", BLUE, BLACK);
				ST7735_SetCursor(12,4);
				ST7735_OutUDec(Score2);
				notQuit = false;
				break;
			}
		}
  }
}
