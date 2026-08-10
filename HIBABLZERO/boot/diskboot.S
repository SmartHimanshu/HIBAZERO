.code16

.file "diskboot.S"

.section .text

.globl _start
_start:
    
    
    movw $welcome_msg, %si
    call print

die:
    jmp die

print:
    movb $0x0E, %ah
1:
    lodsb

    cmp $0, %al
    je 2f

    cmp $0x0A, %al
    je .newline

    int $0x10

    jmp 1b
.newline:
    movb $0x0D, %al
    int $0x10

    movb $0x0A, %al
    int $0x10

    jmp 1b
2:
    ret

welcome_msg: .asciz "Disk boot has been successfully loaded!\n"

.org 0x1000