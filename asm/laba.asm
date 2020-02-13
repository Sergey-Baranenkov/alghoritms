.386
code_seg segment use16
assume cs:code_seg,ds:code_seg,ss:code_seg
org 100h
;-------------------------------------------
print macro message
local msg, next
	push AX
	push DX
	mov DX, offset msg
	mov AH,09h
	int 21h
	pop DX
	pop AX
	jmp next
    msg db 13,10,message,'$',10,13
	next:
endm
;-------------------------------------------
add_si_1 macro
add si,1
endm
;--------------------------------------------
getch macro
	push bx
	push ax
	mov ah, 08h
	xor bx,bx
	int 21h
	pop ax
	pop bx
endm
;--------------------------------------------
clear_scr macro
    push ax
	mov ax,03h
	int 10h
	pop ax
endm
;--------------------------------------------




start:
clear_scr
;ВВОД ИМЕНИ ФАЙЛА  
;Проверка, введен ли адрес файла в ком строке
xor cx, 0
mov cl, es:[80h]
; Если в командную строку переданны данные
cmp cl, 0
jne getCS ;Считываем с коммандной строки
jmp readstr ; Иначе переходим к считыванию с консоли

getCS: ;разместим в стеке всю строку, введенную после имени программы помня, что 1 символ это пробел
mov si,cx
cmp si,1
je endloop
mov bl,[si+80h]
mov filePath[si],bl ; Хитрый цикл, который не пишет пробел, данные записываются с конца до filepath[2]
loop getCS
endloop:
jmp openfile
readstr:	
    print 'Input first file path>>'
    mov ah, 0ah ; Ввод строки
    mov dx, offset filePath ; Буфер для строки
    int 21h ;на выходе получаем [max,len,'t','e','x','t'...]
	
    ;Меняем Enter в конце строки на 0
    xor bx, bx
    mov bl, filePath[1] ;Берем длину строки
    mov si, bx
    mov filePath[si+2], 0 
;КОНЕЦ ВВОДА ИМЕНИ ФАЙЛА


;ОТКРЫТИЕ ФАЙЛА
openfile:
mov AX, 3D02h;Открытие для чтения и редактирования
mov DX, offset filePath+2 
int 21h
jc readstr ;Если ошибка-значит файл введен неправильно
mov Handler, ax ;если все правильно-сохраним дескриптор файла в Handler

;Считаем файл
mov ah, 3fh
mov bx, Handler
mov cx, 1024
mov dx, offset fileBuf
int 21h ; ax-колво символов файла
mov filelen,ax ;cохраним кол-во символов файла в filelen
xor ax,ax
xor dx,dx

print 'File opened!'
print 'File data:'
print '================================================================================'
mov ah, 02h
xor bx, bx
xor dx, dx
mov si,0
jmp @1

print_str:
add si,2
cmp filelen,si     ; ФАЙЛ ЧИТАЕТСЯ ДО ТЕХ ПОР, ПОКА НЕ ДОСТИГНУТА ЕГО ДЛИНА. 
jbe @2
print ''
@1:
cmp fileBuf[si],0dh
je print_str
print_text:
	mov dl, fileBuf[si]
	int 21h
	inc si
	cmp fileBuf[si], 0dh
	jne print_text
	je print_str
@2:
print '================================================================================'
print 'Press any key to continue...'
getch
clear_scr
print 'THIS PROGRAMM WILL ALIGN STRINGS IN YOUR FILE AND REWRITE WITH NEW NAME'
err1:
print 'PRESS y(if you want)'
print 'PRESS n(if you don`t)'
print '>>>'
xor ax,ax 
mov ah,01h
int 21h
cmp al,'y'
je want
cmp al,'n'
je closefile
jmp err1

want:
print 'INPUT NEW FILENAME'
print '>>>'
mov ah, 0ah ; Ввод строки
mov dx, offset filePath_n; Буфер для строки
int 21h ;на выходе получаем [max,len,'t','e','x','t'...]

cmp filePath_n[1],0
je oldrewrite
;Меняем Enter в конце строки на 0
xor bx, bx
mov bl, filePath_n[1] ;Берем длину строки
mov si, bx
mov filePath_n[si+2], 0 
;КОНЕЦ ВВОДА ИМЕНИ НОВОГО ФАЙЛА

;Сначала файл нужно закрыть
mov ah,3eh
mov bx,Handler
int 21h
;Создадим файл 
mov ah,3ch
mov dx, offset filePath_n[2]
mov cx,2
int 21h

mov AX, 3D02h;Открытие для чтения и редактирования
mov DX, offset filePath_n[2];
int 21h
;если все правильно-сохраним дескриптор файла в Handler
mov Handler, ax
xor si,si
clear_scr
print 'FILE WAS REWRITTEN!'
print ''
cmp filelen,0
je closefile
jmp startalign

oldrewrite:
;Сначала файл нужно закрыть
mov ah,3eh
mov bx,Handler
int 21h
;Создадим файл 
mov ah,3ch
mov dx, offset filePath[2]
mov cx,2
int 21h

mov AX, 3D02h;Открытие для чтения и редактирования
mov DX, offset filePath[2];
int 21h
mov Handler,AX
xor si,si
print 'FILE WAS REWRITTEN!'
print ''
cmp filelen,0
je closefile
jmp startalign

;ОСНОВНОЙ АЛГОРИТМ НАЧИНАЕТСЯ ТУТ================================================================================================================================
startalign:
mov bp,0
mov cx,strlen
cmp cx,0
je @zerostrlen
mov si,0
clear_strbuf:
mov strbuf[si],0
inc si
loop clear_strbuf
@zerostrlen:
mov bp,0
mov amount_space,0; Обнуляем кол-во пробелов
mov strlen,0 ;Обнуляем длину строки
mov si,start_number

findspace:
cmp fileBuf[si],20h
jne find0dh
add_si_1
inc amount_space
jmp findspace

find0dh:
cmp fileBuf[si],0dh
je enternumber
add_si_1
jmp find0dh
; В amount_space число пробелов
; в si-номер интера 

enternumber:
mov strlen,si
mov bx,start_number 
sub strlen,bx   
mov bx,amount_space
sub strlen, bx ; В strlen реальная длина строки без 0ah 0dh
mov cx, strlen

cmp fileBuf[si+1],0dh
jne @notlaststr         ; последнюю строку обрабатываем отдельно
mov flag,1
mov di, start_number
cmp fileBuf[di],0dh
je closefile
jmp @laststr
@notlaststr:
add cx,2 ; длина строки с интером и тд
@laststr:
mov bp, start_number
add bp, amount_space 
mov ax,0
mov start_number, si
add start_number,2 ; Новый старт для следующей строки
; В strlen реальная длина строки без 0ah 0dh
; В bp лежит начало строки(1 символ)
mov si,0
movtostrbuf:
mov bl, fileBuf[bp]
mov strbuf[si], bl
inc bp
inc si
loop movtostrbuf
;Записали в strbuf строку до 1 интера
cmp strlen,10
jge leftallign
mov CX,82 ;Число записываемых байт
sub CX,strlen
sub cx,2
mov DX,offset probely ;Отсюда выводить
mov AH,40h ;Функция записи
mov BX,Handler ;Дескриптор
int 21h
leftallign:
mov DX,offset strbuf ;Отсюда выводить
mov AH,40h ;Функция записи
mov BX,Handler ;Дескриптор
mov CX,strlen ;Число записываемых байт
cmp flag,1
je @dont
add cx,2 ; +0dh 0ah
@dont:
int 21h
cmp flag,1
je closefile
jmp startalign
closefile:
print 'PROGRAMM ENDS'
mov ah,3eh
mov bx,Handler
int 21h
jmp endprog
;===========================
flag dw, 0
start_number dw 0
amount_space dw 0
strbuf db 82 dup(0)
probely db 82 dup(20h)
strlen dw 0
loopstart dw 0
ans db ?
filelen dw ?
fileBuf db 1024 dup(0dh)
Handler dw ?; descriptor
filePath db 254,0, 254 dup(0)
filePath_n db 254,0, 254 dup(0)
;===========================
endprog:
mov ax,4c00h
int 21h
code_seg ends
end start