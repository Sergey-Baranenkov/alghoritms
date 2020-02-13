code_seg segment
ASSUME CS:code_seg, DS:code_seg,SS:code_seg

org 100h

start:
	
	mov AH, 02h 	;argument for 21 interrupt, need for print ASCII symbols on console
	xor SI, SI 		;SI = 0, main counter and offset address for PSP segment  
	mov CL, 0 		;counter for line changing
	
	call new_line	;new line
	
lop:
	lodsb 			;this thing send one byte from DS:[SI] to AL
	mov BL, AL		;because  
	
	mov DL,BL      	;here we print one byte from PSP sector 
    rcr DL,4            
    call out_proc
	
	mov DL,BL
	call out_proc	;_______________________________________
	
	mov Dl, ' '		;backspace between bytes 
    int 21h
	
	inc CL			;counter for line changing incrementation
	cmp CL, 16		;IF NOT new line, we go to loop and print next byte from PSP
	jne lop
	
	call new_line	;new line
	
	xor CL, CL		;CL = 0, we go to new line
	
	cmp SI, 100h	;if we didn't print 256 bytes (100h = 256d) from PSP yet 
	jl lop			;we go to loop and print next byte from PSP
					
	int 20h			;else we return control to MS DOS 
	
	
out_proc proc     	;this procedure transform binary number to ASCII code of this number  
	and DL, 0Fh		;add mask because we need to get only 4 bits
    cmp DL, 0Ah		;if DL contain number from 0-9 diapason
    jb num 			;we add 30h to it
    add Dl, 07h		;if DL contain number from A-F diapason,we add 7h 
    num:			
    add Dl, 30h		;and 30h to it
    int 21h			;and print it
	ret
out_proc endp

new_line proc		;just a new line, nothing spatial
	mov Dl, 10		
    int 21h
	mov Dl, 13
    int 21h	
	ret
new_line endp
	
code_seg ends
end start
