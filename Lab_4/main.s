;****************** main.s ***************
; Program written by: **-UUU-*Masaad Khan Ravi Avalkotkar**update this***
; Date Created: 2/14/2017
; Last Modified: 3/2/2020
; You are given a simple stepper motor software system with one input and
; four outputs. This program runs, but you are asked to add minimally intrusive
; debugging instruments to verify it is running properly. 
; The system outputs in one of three modes:
; 1) cycles through 10,6,5,9,... at a constant rate
; 2) cycles through 5,6,10,9,... at a constant rate
; 3) does not change the output, but still outputs at a constant rate
; When PA4 goes high and low again, the system cycles through these modes
; The output rate will be different on the simulator versus the real board
;   Insert debugging instruments which gather data (state and timing)
;   to verify that the system is functioning as expected.
; Hardware connections (External: One button and four outputs to stepper motor)
;  PA4 is Button input  (1 means pressed, 0 means not pressed)
;  PE3-0 are stepper motor outputs 
;  PF2 is Blue LED on Launchpad used as a heartbeat
; Instrumentation data to be gathered is as follows:
; After every output to Port E, collect one state and time entry. 
; The state information is the 5 bits on Port A bit 4 and Port E PE3-0
;   place one 8-bit entry in your Data Buffer  
; The time information is the 24-bit time difference between this output and the previous (in 12.5ns units)
;   place one 32-bit entry in the Time Buffer
;    24-bit value of the SysTick's Current register (NVIC_ST_CURRENT_R)
;    you must handle the roll over as Current goes 3,2,1,0,0x00FFFFFF,0xFFFFFE,
; Note: The size of both buffers is 100 entries. Once you fill these
;       entries you should stop collecting data
; The heartbeat is an indicator of the running of the program. 
; On each iteration of the main loop of your program toggle the 
; LED to indicate that your code(system) is live (not stuck or dead).

SYSCTL_RCGCGPIO_R  EQU 0x400FE608
NVIC_ST_CURRENT_R  EQU 0xE000E018
GPIO_PORTA_DATA_R  EQU 0x400043FC
GPIO_PORTA_DIR_R   EQU 0x40004400
GPIO_PORTA_DEN_R   EQU 0x4000451C
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_DEN_R   EQU 0x4002451C
GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_DEN_R   EQU 0x4002551C
; RAM Area
          AREA    DATA, ALIGN=2
Index     SPACE 4 ; index into Stepper table 0,1,2,3
Direction SPACE 4 ; -1 for CCW, 0 for stop 1 for CW

;place your debug variables in RAM here
DataBuffer SPACE 100  
TimeBuffer SPACE 400
DataPt	   SPACE 4
TimePt	   SPACE 4
Previous   SPACE 4
; ROM Area
        IMPORT TExaS_Init
        IMPORT SysTick_Init
;-UUU-Import routine(s) from other assembly files (like SysTick.s) here
        AREA    |.text|, CODE, READONLY, ALIGN=2
        THUMB
Stepper DCB 5,6,10,9
        EXPORT  Start

Start
 ; TExaS_Init sets bus clock at 80 MHz
; PA4, PE3-PE0 out logic analyzer to TExasDisplay
      LDR  R0,=SendDataToLogicAnalyzer
      ORR  R0,R0,#1
      BL   TExaS_Init ; logic analyzer, 80 MHz
 ;place your initializations here
	  LDR R0, =SYSCTL_RCGCGPIO_R
	  LDR R1, [R0]
	  ORR R1, #0x20
	  STR R1, [R0]
	  NOP
	  NOP
	  LDR R0, =GPIO_PORTF_DIR_R
	  LDR R1, [R0]
	  ORR R1, #0x4
	  STR R1, [R0]
	  LDR R0, =GPIO_PORTF_DEN_R
	  LDR R1, [R0]
	  ORR R1, #0x4
	  STR R1, [R0]
	  LDR R0, =Previous
	  LDR R1, =0x00FFFFFF
	  STR R1, [R0]
	  BL   Heartbeat
      BL   Stepper_Init ; initialize stepper motor
      BL   Switch_Init  ; initialize switch input
;**********************
      BL   Debug_Init ;(you write this)
;**********************
      CPSIE  I    ; TExaS logic analyzer runs on interrupts
      MOV  R5,#0  ; last PA4
loop  
      LDR  R1,=GPIO_PORTA_DATA_R
      LDR  R4,[R1]  ;current value of switch
      AND  R4,R4,#0x10 ; select just bit 4
      CMP  R4,#0
      BEQ  no     ; skip if not pushed
      CMP  R5,#0
      BNE  no     ; skip if pushed last time
      ; this time yes, last time no
      LDR  R1,=Direction
      LDR  R0,[R1]  ; current direction
      ADD  R0,R0,#1 ;-1,0,1 to 0,1,2
      CMP  R0,#2
      BNE  ok
      MOV  R0,#-1  ; cycles through values -1,0,1
ok    STR  R0,[R1] ; Direction=0 (CW)  
no    MOV  R5,R4   ; setup for next time			   ;82 cycles inside Debug Capture ASK
      BL   Stepper_Step                                ;19 instructions in this loop, 14 instructions to get to Debug_Capture, (19+14)*2 = 66 cycles                
      LDR  R0,=1600000                                 ;66 cycles * 12.5ns = 0.825ms
      BL   Wait  ; time delay fixed but not accurate   ;1.025/(1.025+0.825)*100 = 55% downtime between Debug_Capture
	  BL Heartbeat
      B    loop 
;Initialize stepper motor interface
Stepper_Init
      MOV R0,#1
      LDR R1,=Direction
      STR R0,[R1] ; Direction=0 (CW)
      MOV R0,#0
      LDR R1,=Index
      STR R0,[R1] ; Index=0
    ; activate clock for Port E
      LDR R1, =SYSCTL_RCGCGPIO_R
      LDR R0, [R1]
      ORR R0, R0, #0x10  ; Clock for E
      STR R0, [R1]
      NOP
      NOP                 ; allow time to finish activating
    ; set direction register
      LDR R1, =GPIO_PORTE_DIR_R
      LDR R0, [R1]
      ORR R0, R0, #0x0F    ; Output on PE0-PE3
       STR R0, [R1]
    ; enable digital port
      LDR R1, =GPIO_PORTE_DEN_R
      LDR R0, [R1]
      ORR R0, R0, #0x0F    ; enable PE3-0
      STR R0, [R1]
      BX  LR
;Heartbeat function
Heartbeat
      PUSH {R0, R1, LR, R2}
	  LDR R0, =GPIO_PORTF_DATA_R
	  LDR R1, [R0]
	  EOR R1, #0x4
	  STR R1, [R0]
	  POP {R0, R1, PC, R2}
;Initialize switch interface
Switch_Init
    ; activate clock for Port A
      LDR R1, =SYSCTL_RCGCGPIO_R
      LDR R0, [R1]
      ORR R0, R0, #0x01  ; Clock for A
      STR R0, [R1]
      NOP
      NOP                 ; allow time to finish activating
    ; set direction register
      LDR R1, =GPIO_PORTA_DIR_R
      LDR R0, [R1]
      BIC R0, R0, #0x10    ; Input on PA4
      STR R0, [R1]
    ; 5) enable digital port
      LDR R1, =GPIO_PORTA_DEN_R
      LDR R0, [R1]
      ORR R0, R0, #0x10    ; enable PA4
      STR R0, [R1]
      BX  LR
; Step the motor clockwise
; Direction determines the rotational direction
; Input: None
; Output: None
Stepper_Step
      PUSH {R4,LR}
      LDR  R1,=Index
      LDR  R2,[R1]     ; old Index
      LDR  R3,=Direction
      LDR  R0,[R3]     ; -1 for CCW, 0 for stop 1 for CW
      ADD  R2,R2,R0
      AND  R2,R2,#3    ; 0,1,2,3,0,1,2,...
      STR  R2,[R1]     ; new Index
      LDR  R3,=Stepper ; table
      LDRB R0,[R2,R3]  ; next output: 5,6,10,9,5,6,10,...
      LDR  R1,=GPIO_PORTE_DATA_R ; change PE3-PE0
      STR  R0,[R1]
      BL   Debug_Capture
      POP {R4,PC}
; inaccurate and inefficient time delay
Wait 
      SUBS R0,R0,#1  ; outer loop
      BNE  Wait
      BX   LR
      
Debug_Init 
      PUSH {R0-R4,LR}
; you write this
	  BL SysTick_Init
	  LDR R0, =DataBuffer
	  MOV R1, #100
	  LDR R3, =DataPt
	  STR R0, [R3]
	  LDR R2, =0xFF
	  LDR R3, =TimeBuffer
	  LDR R4, =TimePt
	  STR R3, [R4]
	  LDR R4, =0xFFFFFFFF
loop2
	  STRB R2, [R0]
	  STR R4, [R3]
	  ADD R0, R0, #1
	  ADD R3, R3, #4
	  SUB R1, R1, #1
	  CMP R1, #0
	  BNE loop2
      POP {R0-R4,PC}

;Debug capture      
Debug_Capture 
      PUSH {R0-R6,LR} ;1.
; you write this
	  AND R3, #0 
	  LDR R0, =DataPt
	  LDR R2, [R0]
	  LDR R1, =DataBuffer
	  ADD R1, #100
	  CMP R2, R1
	  BEQ Return
	  LDR R0, =TimePt
	  LDR R2, [R0]
	  LDR R1, =TimeBuffer
	  ADD R1, #400
	  CMP R2, R1
	  BEQ Return ;2.
	  
	  LDR R0, =GPIO_PORTA_DATA_R
	  LDR R1, [R0]
	  AND R2, R1, #0x10
	  ADD R3, R2
	  LDR R0, =GPIO_PORTE_DATA_R
	  LDR R1, [R0]
	  AND R2, R1, #0xF
	  ADD R3, R2 ; Don't need R3 and R2 After we store this 4.
	  
	  LDR R0, =DataPt ;5.
	  LDR R1, [R0]
	  STR R3, [R1] ; Dump data into data buffer
	  
	  ADD R1, R1, #1 ;6.
	  STR R1, [R0]
	  
	  
	  LDR R0, =NVIC_ST_CURRENT_R ;What do we do with this 3.
	  LDR R1, [R0]
	  LDR R2, =TimePt
	  LDR R3, [R2]
	  LDR R5, =Previous
	  LDR R4, [R5]
	  SUB R6, R4, R1
	  AND R6, R6, #0x00FFFFFF
	  STR R6, [R3]
	  ADD R3, #4
	  STR R3, [R2]
	  STR R1, [R5]
Return	  
      POP  {R0-R6,PC}
      
; edit the following only if you need to move pins from PA4, PE3-0      
; logic analyzer on the real board
PA4  equ  0x40004040   ; bit-specific addressing
PE30 equ  0x4002403C   ; bit-specific addressing
UART0_DR_R equ 0x4000C000 ;write to this to send data
SendDataToLogicAnalyzer
     LDR  R1,=PA4  
     LDR  R1,[R1]  ; read PA4
     LDR  R0,=PE30 ; read PE3-PE0
     LDR  R0,[R0]
     ORR  R0,R0,R1 ;combine into one 5-bit value
     ORR  R0,R0,#0x80 ;bit7=1 means digital data
     LDR  R1,=UART0_DR_R
     STR  R0,[R1] ; send data at 10 kHz
     BX   LR


     ALIGN    ; make sure the end of this section is aligned
     END      ; end of file

