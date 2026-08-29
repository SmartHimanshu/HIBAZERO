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
bios_interrupt:
    /*
    We will have to be careful and mindful about the way by which we will fill registers.
    First off, we know that we will have to load ds es edx and flags with utmost
    precision and care.
    */

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
    .word 0x00

/* int imm8 */
    .byte 0xCD
INTNO_filler:
    .byte 0x00



    



