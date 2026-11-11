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
    call a20_enable_main // Enable A20 mode.

    movb %al, a20_on

    calll real_to_prot

.code32
    jmp kern_startup

die:
    jmp die

.global a20_on
a20_on:
        .byte 0
#include "modeswitch.S"

#include "int.S"
#include "a20.S"
