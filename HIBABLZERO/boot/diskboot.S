.code16

.file "diskboot.S"

.section .text

.globl _start
_start:
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    
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

//This is HB0 core. (HB0 is for HIBABOOTLOADERVZERO) addr and size.

core: 
    core_size: 
        .word 64
    core_lba_low:
        .long 2
    core_lba_high:
        .long 0

.org 0x1000