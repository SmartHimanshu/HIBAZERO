/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

.code16

.file "diskboot.S"

.section .text

.globl _start
_start:
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es

    push %dx

    movw %ax, %dx

    pushw %si
    
    movw $welcome_msg, %si
    call print
    
    popw %si

    /*
    Since diskboot is all about loading core as fast as possible,
    we will use the maximum sector limit for BIOS read. (127)
    */

    xorw %dx, %dx
    movw core_size, %ax
    movw $127, %bx
    divw %bx
    movw %ax, scratch   //Scratch stores number of times to do 127 sector loads
    movw %dx, scratch+2 //Scratch+2 stores remainder



    movb 16(%si), %al
    testb %al, %al
    jnz chs_setup

    /* 
    At this offset, we have defined the kind of mode we have come from.
    1 for chs and 0 for lba.
    
    dap:
        dap_size: .byte 0x10
        dap_rsrv: .byte 0
        dap_sectors: .word 2
        dap_transfer_offset: .word 0x0000
        dap_transfer_segment: .word 0x7000
        dap_lba_addr: .quad 0 // Filled at runtime
    addressing_mode: .byte 0
    */

lba_setup:
    /*
    Since we are at lba over here, this must mean that we have booted from 
    lba. Which means that we can freely do our bidding and load core.
    */

    movw scratch, %ax
    testw %ax, %ax
    jnz 1f

    movw scratch+2, %ax
    movw %ax, 2(%si)    //Load num of sectors for dap.

    jmp 2f
1:
    movw $127, %ax
    movw %ax, 2(%si)

2:
    movw scratch, %bx //Move the amount of times we have to read 127 sectors.

    // Now that we have filled no.of sectors, its time to fill lba addr.
    movl core_lba_low, %eax
    movl %eax, 8(%si)
    movl core_lba_high, %eax
    movl %eax, 12(%si)

    pop %dx /* Restore disk number*/

    /*
    Now its time to fully commit to our goals and 
    design the lba read.
    */
lba:
    /*
    BIOS INT 13h BIOS EXTENDED READ:
        AH = 42h
        DL = drive number
        DS:SI -> disk address packet
    */

    movw $0x4200, %ax

    int $0x13
    
    jc error //Honestly this is majorly for bug catching

    

    // Now we transfer 127 sectors 

    cld

    push %es
    push %ds
    push %si

    movw $32512, %cx
    xorw %ax, %ax
    movw %ax, %di
    movw %ax, %si
    movw core_load_addr_seg, %ax //Load core_load_addr
    movw %ax, %es
    movw $0x7000, %ax
    movw %ax, %ds
    

    rep
    movsw

    pop %si
    pop %ds
    pop %es

    addw $4064, core_load_addr_seg

    cmpw $1, %bx    
    jb jump_to_core // If this is remainder sector read then simply jump to core.
    jne 1f          // If this is the last 127 read then we will load remainder sectors here

    xorl %eax, %eax
    movw scratch+2, %ax
    movw %ax, 2(%si)
    addl $127, 8(%si) 
    jnc 2f


    addl $1, 12(%si)
    jmp 2f

1:
    addl $127, 8(%si)
    jnc 2f

    addl $1, 12(%si)

2:
    decw %bx
    jmp lba 

chs_setup:
    /*
    Since we are here, it just means that we are going for 
    CHS addressing.
    */

    popw %dx

    movw scratch, %ax
    testw %ax, %ax
    jnz 1f

    movw scratch+2, %ax

    jmp 2f
1:
    movw $127, %ax

2:
    //The count of the times to do 127 read stored in scratch.


chs:
    movb %dl, current_drive
    pushw %ax
    movl core_lba_low, %eax
    movl core_lba_high, %edx

    call lba_to_chs

    popw %ax
    movb current_drive, %dl
    

    movw $0x7000, %bx
    movw %bx, %es
    xorw %bx, %bx

    movb $0x02, %ah

    int $0x13

    jc error

    cld

    push %ax
    push %es
    push %ds
    push %si

    movw $32512, %cx
    xorw %ax, %ax
    movw %ax, %di
    movw %ax, %si
    movw core_load_addr_seg, %ax //Load core_load_addr
    movw %ax, %es
    movw $0x7000, %ax
    movw %ax, %ds

    rep
    movsw

    pop %si
    pop %ds
    pop %es
    pop %ax

    addw $4064, core_load_addr_seg

    movw scratch, %bx
    cmpb $1, %bl
    jb jump_to_core
    jne 1f

    xorl %eax, %eax
    movw scratch+2, %ax
1:
    addl $127, core_lba_low
    subw $1, scratch
    jmp lba 



/* 
Stores chs address such that its easy to just call bios interrupt.
Takes lba addr at register edx:eax and then translates it onto registers for chs address.
and %si should point to disk dimensions.
*/
lba_to_chs:
    pushw %dx
    xorl %ecx, %ecx
    movb 1(%si), %cl //Take sectors per track in ecx.

    divl %ecx //Now dap % sectors in edx and dap / sectors in eax

    incl %edx
    movb %dl, %cl //Store sector start in scratch+4

    xorl %edx, %edx
    movl %edx, %ebx
    movb (%si), %bl //Take head num

    divl %ebx //Now cylinder in eax and head in edx

    movb %dl, %ch
    popw %dx
    movb %ch, %dh //Now dh has head.

    movb %al, %ch
    shrw $2, %ax
    andb $0b11000000, %al
    orb %al, %cl // Now cylinder loaded.

    ret

jump_to_core:
    ljmp $0, $0x8000


error:
    movw $error_msg, %si
    call print
    hlt

    jmp die
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
error_msg: .asciz "A serious error has occured during disk read of core.\n"


scratch:
    .quad 0
    .quad 0

core_load_addr_seg:
    .word 0x800

current_drive:
    .byte 0

//This is HB0 core. (HB0 is for HIBABOOTLOADERVZERO) addr and size.

core: 
    core_size: 
        .word 256
    core_lba_low:
        .long 11
    core_lba_high:
        .long 0

.org 0x400
