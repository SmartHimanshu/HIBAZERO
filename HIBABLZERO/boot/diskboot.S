.code16

.file "diskboot.S"

.section .text

.globl _start
_start:
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    
    pushw %si
    
    movw $welcome_msg, %si
    call print
    
    popw %si

    /*
    Since diskboot is all about loading core as fast as possible,
    we will use the maximum sector limit for BIOS read. (127)
    */

    movb 16(%si), %al
    testb %al, %al
    jnz chs

    /* 
    At this offset, we have defined the kind of mode we have come from.
    1 for chs and 0 for lba.
    */

lba:




chs:

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

.org 0x400