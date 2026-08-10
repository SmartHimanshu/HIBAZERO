.code16

.file "diskboot.S"

.section .text

.globl _start
_start:
    xorw %ax, %ax
    xorw %bx, %bx
    movw welcome_msg, %si
    call print

die:
    jmp die

print:
    movb $0x0E, %ah
1:
    lodsb
    cmp $0, %al
    je 2f
    int $0x10
    jmp 1b
2:
    ret

welcome_msg: .asciz "Disk boot has been successfully loaded!"

.org 0x1000