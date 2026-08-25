.code16

.file "modeswitch.S"

.section .text

/*
+----------------------------------------------------------------------------------+
|                             GLOBAL DESCRIPTOR TABLE                              |
+-------+------------+---------------------+---------------------------------------+
| Index | Selector   | Type                | Description                           |
+-------+------------+---------------------+---------------------------------------+
|   0   | 0x00       | Null                | Mandatory null descriptor             |
|   1   | 0x08       | PM Code             | Ring 0, 32-bit protected mode code    |
|   2   | 0x10       | PM Data             | Ring 0, 32-bit protected mode data    |
|   3   | 0x18       | RM Code             | 16-bit real mode compatible code seg  |
|   4   | 0x20       | RM Data             | 16-bit real mode compatible data seg  |
+-------+------------+---------------------+---------------------------------------+

Descriptor Layout (8 bytes)

 63                            56 55 52 51 48 47                 40
+--------------------------------+-----+-----+--------------------+
|         Base[31:24]            |Flags|Limit|      Access        |
+--------------------------------+-----+-----+--------------------+
 39                            32 31                             16
+--------------------------------+--------------------------------+
|         Base[23:16]            |          Base[15:0]            |
+--------------------------------+--------------------------------+
 15                                                               0
+-----------------------------------------------------------------+
|                     Segment Limit[15:0]                         |
+-----------------------------------------------------------------+

Selectors
---------
  Protected Mode
      CS = 0x08
      DS = ES = FS = GS = SS = 0x10

  Real Mode / 16-bit Segments
      CS = 0x18
      DS = ES = FS = GS = SS = 0x20

Memory Model
------------
  PM Code:
      Base  = 0x00000000
      Limit = 0xFFFFFFFF

  PM Data:
      Base  = 0x00000000
      Limit = 0xFFFFFFFF

  RM Code:
      Base  = 0x00000000
      Limit = 0x0000FFFF
      D/B   = 0 (16-bit)

  RM Data:
      Base  = 0x00000000
      Limit = 0x0000FFFF
      D/B   = 0 (16-bit)
*/

/*
    +------------------------------------------------------------------------------------------+
    | NOTE: We are using 0xFFFF Because we want our dear BIOS to not cry after using segments. |
    +------------------------------------------------------------------------------------------+
*/

gdt:
gdt_null:
    .quad 0x00
gdt_real_code:
    .word 0xFFFF //Limit is gonna be 64KB
    .word 0x00
    .byte 0x00
    .byte 0b10011011
    .byte 0b00000000
    .byte 0x00
gdt_real_data:
    .word 0xFFFF //Limit is gonna be 64KB
    .word 0x00
    .byte 0x00
    .byte 0b10010011
    .byte 0b00000000
    .byte 0x00
gdt_prot_code:
    .word 0xFFFF
    .word 0x00
    .byte 0x00
    .byte 0b10011011
    .byte 0b11001111
    .byte 0x00
gdt_prot_data:
    .word 0xFFFF
    .word 0x00
    .byte 0x00
    .byte 0b10010011
    .byte 0b11001111
    .byte 0x00
gdt_end:
gdt_descriptor:
    .word gdt_end - gdt - 1
    .long gdt


realidt:
    .word 0x400
    .long 0x00
protidt:
    .word 0
    .long 0
.globl real_to_prot
real_to_prot:
    xorw %ax, %ax
    movw %ax, %ds

    lgdtl gdt_descriptor

    movl %cr0, %eax
    orl $1, %eax
    movl %eax, %cr0

    ljmp $0x18, $pmcseg

.code32
pmcseg:
    movw $0x20, %ax
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    movw %ax, %ds

    sidt realidt
    lidt protidt

    

    ret

prot_to_real:
    sidt protidt
    lidt realidt

    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %fs
    movw %ax, %gs

    ljmp $0x08, $tmpcseg
.code16gcc
tmpcseg:
    
    movl %cr0, %eax
    andl $(~1), %eax
    movl %eax, %cr0

    ljmp $0, $return
return:

    xorl %eax, %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    sti

    retl