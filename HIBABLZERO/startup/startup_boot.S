.code16

.file "startup_boot.S"

.globl _start
_start:
    movw $0xb800, %ax
    movw %ax, %es
    movw $0x0F41, %es:(0)
die:
    jmp die