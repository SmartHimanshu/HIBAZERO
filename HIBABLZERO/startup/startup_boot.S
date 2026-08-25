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
    jmp kern_startup

die:
    jmp die

#include "modeswitch.S"