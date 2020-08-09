; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; Lab 7 requirement is for at least one local variable on the stack with symbolic binding
LCD_OutDec
	CMP R0, #9
    BHI notEnd
    ADD R0, R0, #48
    PUSH {R2, LR}
    BL ST7735_OutChar
    POP {R2, LR}
    BX LR
notEnd
    MOV R2, #10
    MOV R1, R0
    UDIV R0, R0, R2
    MUL R2, R2, R0
    SUB R1, R1, R2
    SUB SP, SP, #8
    STR R1, [SP, #0]
    PUSH {R2, LR}
    BL LCD_OutDec
    POP {R2, LR}
    LDR R1, [SP, #0]
    ADD SP, SP, #8
    MOV R0, R1
    ADD R0, R0, #48
    PUSH {R2, LR}
    BL ST7735_OutChar
    POP {R2, LR}
    BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.01, range 0.00 to 9.99
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.00 "
;       R0=3,    then output "0.03 "
;       R0=89,   then output "0.89 "
;       R0=123,  then output "1.23 "
;       R0=999,  then output "9.99 "
;       R0>999,  then output "*.** "
; Invariables: This function must not permanently modify registers R4 to R11
; Lab 7 requirement is for at least one local variable on the stack with symbolic binding
LCD_OutFix
	LDR R2, =999
	CMP R0, R2
	BHI Invalid
	CMP R0, #100
	BLO smaller
	MOV R1, R0
	MOV R2, #100
	UDIV R0, R1, R2
	
	SUB SP, SP, #16
	STR R0, [SP, #0]
	STR R1, [SP, #8]
	ADD R0, R0, #48
	PUSH {R2, LR}
	BL ST7735_OutChar
	MOV R0, #0x2E 
	BL ST7735_OutChar
	
	POP {R2, LR}
	LDR R0, [SP, #0]
	LDR R1, [SP, #8] 
	
	MUL R2, R0, R2
	SUB R0, R1, R2
	MOV R1, R0
	MOV R2, #10
	UDIV R0, R1, R2
	
	STR R0, [SP, #0]
	STR R1, [SP, #8]
	
	ADD R0, R0, #48
	PUSH {R2, LR}
	BL ST7735_OutChar
	POP {R2, LR}
	LDR R0, [SP, #0]
	LDR R1, [SP, #8] 
	
	MUL R2, R0, R2
	SUB R0, R1, R2
	
	STR R0, [SP, #0]
	STR R1, [SP, #8]
	
	ADD R0, R0, #48
	PUSH {R2, LR}
	BL ST7735_OutChar
	POP {R2, LR}
	LDR R0, [SP, #0]
	LDR R1, [SP, #8] 
	
	ADD SP, SP, #16
	BX LR
Invalid
	B invalid
smaller
	SUB SP, SP, #16
	STR R0, [SP, #0]
	STR R1, [SP, #8]
	MOV R0, #0x30
	PUSH {R2, LR}
	BL ST7735_OutChar
	MOV R0, #0x2E 
	BL ST7735_OutChar
	
	POP {R2, LR}
	LDR R0, [SP, #0]
	LDR R1, [SP, #8] 
	
	MOV R2, #10
	MOV R1, R0
	UDIV R0, R1, R2
	
	STR R0, [SP, #0]
	STR R1, [SP, #8]
	
	ADD R0, R0, #48
	PUSH {R2, LR}
	BL ST7735_OutChar
	POP {R2, LR}
	LDR R0, [SP, #0]
	LDR R1, [SP, #8] 
	
	MUL R2, R0, R2
	SUB R0, R1, R2
	
	STR R0, [SP, #0]
	STR R1, [SP, #8]
	
	ADD R0, R0, #48
	PUSH {R2, LR}
	BL ST7735_OutChar
	POP {R2, LR}
	LDR R0, [SP, #0]
	LDR R1, [SP, #8] 
	
	ADD SP, SP, #16
	BX LR
singledigit
	SUB SP, SP, #8
	STR R0, [SP, #0]
	MOV R0, #0x30
	PUSH {R2, LR}
	BL ST7735_OutChar
	MOV R0, #0x2E 
	BL ST7735_OutChar
	MOV R0, #0x30
	BL ST7735_OutChar
	LDR R0, [SP, #0]
	ADD R0, R0, #48
	BL ST7735_OutChar
	POP {R2, LR}
	
	ADD SP, SP, #8
	BX LR
invalid
	MOV R0, #0x2A
	PUSH {R2, LR}
	BL ST7735_OutChar
	POP {R2, LR}
	MOV R0, #0x2E
	PUSH {R2, LR}
	BL ST7735_OutChar
	POP {R2, LR}
	MOV R0, #0x2A
	PUSH {R2, LR}
	BL ST7735_OutChar
	POP {R2, LR}
	MOV R0, #0x2A
	PUSH {R2, LR}
	BL ST7735_OutChar
	POP {R2, LR}
     BX   LR
	
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN          ; make sure the end of this section is aligned
     END            ; end of file
