code_seg segment
assume cs:code_seg,ds:code_seg,ss:code_seg

org 100h
;---------------------------------------------------------------------
print_mes macro message
local msg, nxt

push AX

push DX

mov DX, offset msg

mov AH, 09h

int 21h

pop DX

pop AX

jmp nxt

msg DB 13, 10, message,' $', 10, 13

nxt:

endm
;---------------------------------------------------------------------
clear_screen macro
 
push ax
mov ax, 03h
int 10h
pop ax

endm
;---------------------------------------------------------------------
getch macro

push bx
push ax
mov ah, 08h
xor bx, bx
int 21h
pop ax
pop bx

endm
;---------------------------------------------------------------------
start:

clear_screen 

mov cx, 0
mov cl, es:[80h]
; Если в командную строку переданны данные
cmp cl, 0
jne getCB
jmp readFF; Иначе переходим к считыванию с консоли

; Помещем данные из буфера в стек
getCB:
    mov si, cx
    mov bl, es:[si+80h]
    push bx
    loop getCB
print_mes 'Data found in the stack.'
print_mes 'Press any key to continue...'
getch 
call getFP

push 0020h
call getFP_2

; Считываем путь до 1 файла с консоли
readFF:
	clear_screen
    print_mes 'Input first file path>>'
    mov ah, 0ah
    mov dx, offset filePath1
    int 21h

    ;Меняем Enter в конце строки на 0
    push bx
    xor bx, bx
    mov bl, filePath1[1]
    mov si, bx
    mov filePath1[si+2], 0
    pop bx
	
	
endp


; Открываем 1 файл
OpenFF:
mov AX, 3D02h
mov DX, offset filePath1+2
int 21h
jc readFF


mov Handler, ax
endp
; Считываем строку из файла
mov ah, 3fh
mov bx, Handler
mov cx, 65535
mov dx, offset bufIn
int 21h
mov cx, ax
clear_screen 0
print_mes 'File opened!'
print_mes 'File data:'
print_mes '================================================================================'
mov ah, 02h
mov si,0
xor bx, bx
xor dx, dx
print_text:
	mov dl, bufIn[si]
	int 21h
	inc si
	cmp bufIn[si], 0dh
	jne print_text
endp
	
print_mes '================================================================================'
print_mes 'Press any key to continue...'
getch
clear_screen
;Считываем строку котрую нужно найти
inputFLine:
    print_mes 'What is the line you need to find?'
    mov ah, 0ah
    mov dx, offset strF
    int 21h
endp
;Находим строку
findStr:

mov dx,-1
push dx
cmp bufIn[0], 0dh
jne startFind

    startFind:
    pop si
    inc si
    cmp bufIn[si], 0dh
    jne startFind1
    jmp endFind_error
    startFind1:
    endp
    mov bl, strF[2]
    mov dx, si
    cmp bufIn[si], 0dh
    
    jne startFndNxt
        startFndNxt:
            cmp bufIn[si], bl
            jne startFindRet
            jmp startFndNxt_1
                startFndNxt_1:
                    mov si, 0
                    push dx
                    push si
                        startFndLoop:
                        pop si
                        inc si
                        mov bl, strF[si+2]
                        cmp bl, 0dh
                        jne nxtLoop
                        jmp endFind
                            nxtLoop:
                            push si
                            add si, dx
                            cmp bufIn[si], bl
                            jne startFind
                            jmp startFndLoop
                            endp
                        endp
                startFindRet:
                    push dx
                    jmp startFind
                endp   
                    
                
                endp
            
        endp
    
    endp

endp

endFind_error:
    print_mes 'The line not found. Return to input line.'
	print_mes 'Press any key to continue...'
	getch
	clear_screen
    jmp inputFLine    
endp

endFind:
    pop dx
    mov bl, strF[1] ;длинна
    mov bh, dl ; номер начала
    print_mes 'The line found.'
	clear_screen
endp

;Считываем строку котрую нужно найти
inputCLine:
    print_mes 'Which line do you want it to be replaced by?'
    mov ah, 0ah
    mov dx, offset strC
    int 21h
endp

changeLine:

    xor cx, cx
    mov cl, bh
    mov si, cx
    dec si
    
    
    changeStep_1:
        mov bl, bufIn[si]
        mov bufOut[si], bl
        dec si        
    loop changeStep_1
    
    mov si, 0 
    mov cl, bh 
    changeStep_2:
        mov bl, strC[si+2]
        cmp bl, 0dh
        jne chS2
        jmp changeStep_3
            chS2:
                add si, cx
                mov bufOut[si], bl
                sub si, cx
                inc si
                jmp changeStep_2
            endp    
    endp
    
    changeStep_3:
        add cl, strF[1]
        mov si, cx
        chS_3:
        mov bl, bufIn[si]
        cmp bl, 0dh
        jne ch_3
        jmp readSF
            ch_3:
                
                sub cl, strF[1]
                add cl, strC[1]
                mov si, cx
                mov bufOut[si], bl
                inc cx
                sub cl, strC[1]
                add cl, strF[1]
                mov si, cx
                jmp chS_3
            endp    
    endp        
    
endp

; Считываем путь до 2 файла с консоли
readSF:
clear_screen
cmp var[0], 0
jne endChange
print_mes 'Input second file path>>'
mov ah, 0ah
mov dx, offset filePath2
int 21h

;Меняем Enter в конце строки на 0
push bx
xor bx, bx
mov bl, filePath2[1]
mov si, bx
mov filePath2[si+2], 0
pop bx
endp

;Считываем кол-во символов для записи
mov si, 0
mov cx,0

getNubrB:
inc si
cmp bufOut[si-1], 0dh
jne getNubrB

mov cx, si


endChange:
    ; Открываем 2 файл
	mov var[0], 0
    mov ax, 3D02h
    mov dx, offset filePath2+2
    int 21h
    jc readSF
	
    mov Handler, ax
    ;Записываем строку
    mov ah, 40h
    mov bx, Handler
    mov dx, offset bufOut
    int 21h
    jc fileWE
endp

print_mes 'Success!'
print_mes 'Press any key to close program...'
getch
int 20h
ret

fileWE:
print_mes 'Write_error'
print_mes 'Press any key to continue...'
getch
jmp readSF
endp
                            

;====================================================================

getFP:
    pop bx
    mov dx, bx
    pop bx
    mov si,0
    cmp bl, 20h
    jne getFPe
    jmp getFPs
        getFPs:
            pop bx
            cmp bl, 0
            jne nxt
            jmp getFPe_0 ; Если есть путь только до 1 файла
            nxt:
            endp
            cmp bl, 20h
            jne getS
            jmp getFPe
                getS:
                    mov filePath1[si+2], bl
                    inc si
                    jmp getFPs
                endp
            
        endp
    getFPe:
    mov bx, si
    mov filePath1[1], bl
    xor bx, bx
    push 20h
    jmp getFP_2
    endp
    
    getFPe_0:
    mov bx, si
    mov filePath1[1], bl
    xor bx, bx
    jmp openFF
    
    endp
ret
;---------------------------------------------------------------------
getFP_2:
    pop bx
    mov si,0
    cmp bl, 20h
    jne getFPe2
    jmp getFPs2
        getFPs2:
            pop bx
            cmp bl, 0
            jne getS2
            jmp getFPe2
                getS2:
                    mov filePath2[si+2], bl
                    inc si
                    jmp getFPs2
                endp
            
        endp
    getFPe2:
    mov bx, si
    mov filePath2[1], bl
    xor bx, bx
    push dx
    mov dx, 0
    mov var[0], 1
    jmp openFF
    endp
ret

;=====================================================================

Handler DW ?
; Буферы для файла
bufIn db 1024 dup(0dh)
bufOut db 1024 dup(0dh)
; Буфер для строки, которую нужно заменить
strF db 50, 0, 50 dup(0)
; Буфер для строки, на которую нужно заменить
strC db 50, 0, 50 dup(0)
; Пути
filePath1 db 254, 0, 254 dup(0)
filePath2 db 254, 0, 254 dup(0)

var db 1 dup(0)

code_seg	ends
end	start


