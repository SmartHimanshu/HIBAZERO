.code16

.file "startup_boot.S"

.globl _start
_start:
    jmp lol
die:
    jmp die
.org 0x800
lol:
    movw $0xb800, %ax
    movw %ax, %es
    movw $0x0F41, %es:(0)
    jmp die