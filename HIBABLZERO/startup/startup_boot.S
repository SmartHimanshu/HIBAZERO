/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

.code16

.file "startup_boot.S"

.extern kern_startup

.globl _start
.section .startup, "ax"
_start:
    cli // Just make sure we are good with the interrupts
    movw $0xb800, %ax
    movw %ax, %es
    movw $0x0F41, %es:(0)
    calll real_to_prot
.code32
    movl $0x80000, %eax
    movl %eax, %esp
    jmp kern_startup

die:
    jmp die

#include "modeswitch.S"

#include "int.S"