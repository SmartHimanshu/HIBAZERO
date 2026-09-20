/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

.code32

.file "int.S"

.section .text


/*
You wanna go fast? let's go fast!
I am gonna use regparm to pass the struct in edx and int no. in al for speed.
*/
.globl bios_interrupt
bios_interrupt:
    /*
    We will have to be careful and mindful about the way by which we will fill registers.
    First off, we know that we will have to load ds es edx and flags with utmost
    precision and care.
    */

    /*
    We are preserving the exact state of the caller,
    here. In order to prevent wiping out of any runtime flags.
    */
    pushf       
    cli
    popf

    pushl %eax
    pushl %ebx
    pushl %ecx
    pushl %edi
    pushl %esi
    pushl %ebp

    pushl %edx  /* Preserve bios regs addr */

    movb %al, INTNO_filler
    
    movl (%edx), %eax
    movl %eax, EAX_filler
    movw 28(%edx), %ax
    movw %ax, DS_filler
    movw 26(%edx), %ax
    movw %ax, ES_filler
    movw 24(%edx), %ax
    movw %ax, FLAGS_filler

    movl 4(%edx), %ebx
    movl 8(%edx), %ecx
    movl 16(%edx), %edi
    movl 20(%edx), %esi
    movl 12(%edx), %edx

via_centaur_workaround1:
    .byte 0x90, 0x90

    call prot_to_real
.code16
    pushf
    cli

    movw %ds, %ax
    pushw %ax

/* movw imm16, %ax */
    .byte 0xb8
DS_filler:
    .word 0x00
    movw %ax, %ds

    .byte 0xb8
ES_filler:
    .word 0x00
    movw %ax, %es

/* movw imm16, %ax */
    .byte 0xb8
FLAGS_filler:
    .word 0x00
    pushw %ax
    popf

/* movl imm32, %eax*/
    .byte 0x66, 0xb8
EAX_filler:
    .long 0x00

/* int imm8 */
    .byte 0xCD
INTNO_filler:
    .byte 0x00

    /* 
    We are going to preserve all the sensitive things in previous 
    self-modifying code slots so we don't waste memory and become
    bloat.
    */
    movl %eax, %cs:EAX_filler
    
    movw %ds, %ax
    movw %ax, %cs:DS_filler
    popw %ax
    movw %ax, %ds /* We clear DS */

    movw %es, %ax
    movw %ax, ES_filler /* We can now refer without overhead. */
    
    pushf
    popw %ax
    movw %ax, FLAGS_filler

    popf

via_centaur_workaround2:
    .byte 0x90, 0x90

    calll real_to_prot
    .code32

    popl %eax /* Get the bios regs addr */

    movl %ebx, 4(%eax)
    movl %ecx, 8(%eax)
    movl %edx, 12(%eax)
    movl %edi, 16(%eax)
    movl %esi, 20(%eax)

    movl %eax, %edx

    movl EAX_filler, %eax
    movl %eax, (%edx)
    movw FLAGS_filler, %ax
    movw %ax, 24(%edx)
    movw ES_filler, %ax
    movw %ax, 26(%edx)
    movw DS_filler, %ax
    movw %ax, 28(%edx)

    popl %ebp
    popl %esi
    popl %edi
    popl %ecx
    popl %ebx
    popl %eax

    ret
    



