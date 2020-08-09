;****************** main.s ***************
; Program written by: ***Masaad Khan and **update this***
; Date Created: 2/14/2020
; Last Modified: 1/17/2020
; Brief description of the program
;   The LED toggles at 2 Hz and a varying duty-cycle
; Hardware connections (External: One button and one LED)
;  PE1 is Button input  (1 means pressed, 0 means not pressed)
;  PE2 is LED output (1 activates external LED on protoboard)
;  PF4 is builtin button SW1 on Launchpad (Internal) 
;        Negative Logic (0 means pressed, 1 means not pressed)
; Overall functionality of this system is to operate like this
;   1) Make PE2 an output and make PE1 and PF4 inputs.
; 
;   2) The system starts with the the LED toggling at 2Hz,
;      which is 2 times per second with a duty-cycle of 30%.
;      Therefore, the LED is ON for 150ms and off for 350 ms.
;   3) When the button (PE1) is pressed-and-released increase
;      the duty cycle by 20% (modulo 100%). Therefore for each
;      press-and-release the duty cycle changes from 30% to 70% to 70%
;      to 90% to 10% to 30% so on
;   4) Implement a "breathing LED" when SW1 (PF4) on the Launchpad is pressed:
;      a) Be creative and play around with what "breathing" means.
;         An example of "breathing" is most computers power LED in sleep mode
;         (e.g., https://www.youtube.com/watch?v=ZT6siXyIjvQ).
;      b) When (PF4) is released while in breathing mode, resume blinking at 2Hz.
;         The duty cycle can either match the most recent duty-
;         cycle or reset to 30%.
;      TIP: debugging the breathing LED algorithm using the real board.
; PortE device registers
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_AFSEL_R EQU 0x40024420
GPIO_PORTE_DEN_R   EQU 0x4002451C
; PortF device registers
GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_AFSEL_R EQU 0x40025420
GPIO_PORTF_PUR_R   EQU 0x40025510
GPIO_PORTF_DEN_R   EQU 0x4002551C
GPIO_PORTF_LOCK_R  EQU 0x40025520
GPIO_PORTF_CR_R    EQU 0x40025524
GPIO_LOCK_KEY      EQU 0x4C4F434B  ; Unlocks the GPIO_CR register
SYSCTL_RCGCGPIO_R  EQU 0x400FE608
	
       IMPORT  TExaS_Init
       THUMB
       AREA    DATA, ALIGN=2
;global variables go here 

       AREA    |.text|, CODE, READONLY, ALIGN=2
	   THUMB
       EXPORT  Start
		   
delay 	NOP
dloop	SUBS R1, R1, #1
		BNE dloop
		BX LR
NegReg
		LDR R9, =-1000
		B come
PosReg
		LDR R9, =1000
		AND R5, R5, #0
		B come

dloop2p
		ADD R5, R5, #1
		BNE dloop2
		BX LR
ondelay 
		CMP R8, R7
		BGE NegReg
		CMP R8, #1000
		BLE PosReg
come    ADDS R8, R9
    	ADDS R5, R8
		SUBS R6, R7, R5
dloop2	
		CMP R5, #0
		BLE dloop2p
		SUBS R5, R5, #1
		BNE dloop2
		BX LR
offdelay 	
		CMP R6, #0
		BEQ equal
		SUBS R6, R6, #1
		BNE offdelay
equal
		BX LR
		
LEDON	
		LDR R0, =GPIO_PORTE_DATA_R
		LDR R1, [R0]
		ORR R1, #0x4
		STR R1, [R0]
		BX LR
		
LEDOFF	LDR R0, =GPIO_PORTE_DATA_R
		LDR R1, [R0]
		BIC R1, #0x4
		STR R1, [R0]
		BX LR
		
Checkswitch 
		LDR R0, =GPIO_PORTE_DATA_R
		LDR R1, [R0]
		AND R2, R1, #0x2
		CMP R2, #0x0
		BEQ leave
		LDR R4, =2000000 
		ADD R3, R4
		LDR R5, =11000000
		SUBS R5, R3
		CMP R5, #0
		BNE Checkloop
		LDR R3, =1000000
Checkloop		
		LDR R0, =GPIO_PORTE_DATA_R
		LDR R1, [R0]
		AND R2, R1, #0x2 
		CMP R2, #0x2
		BEQ Checkloop		
leave
		BX LR

Checkswitch2
		LDR R5, =0 ; This will be on delay
again
		LDR R0, =GPIO_PORTF_DATA_R
		LDR R1, [R0]
		AND R2, R1, #0x10
		CMP R2, #0x10
		BEQ leave
		MOV R10, R14
		BL LEDON
		BL ondelay
		BL LEDOFF
		BL offdelay
		MOV R14, R10
		B again
		
Start
 ; TExaS_Init sets bus clock at 80 MHz
     BL  TExaS_Init ; voltmeter, scope on PD3
 ; Initialization goes here
 LDR R0, =SYSCTL_RCGCGPIO_R
 LDRB R1, [R0]
 ORR R1, #0x30
 STRB R1, [R0]
 NOP
 NOP
 LDR R0, =GPIO_PORTF_LOCK_R
 LDR R1, =GPIO_LOCK_KEY
 STR R1, [R0]
 LDR R0, =GPIO_PORTF_CR_R
 LDR R1, [R0]
 ORR R1, #0xFF
 STR R1, [R0]
 LDR R0, =GPIO_PORTF_DIR_R
 LDR R1, [R0]
 AND R1, #0xFF
 STR R1, [R0]
 LDR R0, =GPIO_PORTF_DEN_R
 LDR R1, [R0]
 ORR R1, #0x10
 STR R1, [R0]
 LDR R0, =GPIO_PORTF_PUR_R
 LDR R1, [R0]
 ORR R1, #0x10
 STR R1, [R0]
 LDR R0, =GPIO_PORTE_DIR_R
 LDR R1, [R0]
 BIC R1, #0x2
 ORR R1, #0x4
 STR R1, [R0]
 LDR R0, =GPIO_PORTE_DEN_R
 LDR R1, [R0]
 ORR R1, #0x6
 STR R1, [R0]
 LDR R0, =GPIO_PORTE_DATA_R
 LDR R1, [R0]
 ORR R1, #0x4
 STR R1, [R0]
 LDR R3, =3000000 ;R3 holds the value of the duty cycle, do not change
 LDR R7, =100000 ; Holds max value for breathe
 LDR R8, =2000 ; Counter-ish
 LDR R9, =1000


     CPSIE  I    ; TExaS voltmeter, scope runs on interrupts
loop  
; main engine goes here
	 BL LEDON
	 MOV R1, R3
	 BL delay
	 BL LEDOFF
	 LDR R1, =10000000
	 SUB R1, R3
	 BL delay
	 BL Checkswitch
	 BL Checkswitch2
     B    loop

     ALIGN      ; make sure the end of this section is aligned
     END        ; end of file

