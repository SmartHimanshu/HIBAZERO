.code16

.file "boot.S"

.section .text

.globl _start
_start:
    //These are generally thrashed by bios so we have to restore them.
    
    cld

    cli

    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss

    movw $0x7c00, %sp

    sti

    ljmp $0, $.reload_cs
.reload_cs:

    movw $welcome_msg, %si
    call print

    /*
    Since some BIOSes return dl with some bogus value. We are taking into account
    that this bootloader is modern, and in a sense, never gonna be loaded by a floppy.
    Floppies start wtih 0x00 to 0x7F and hard disks start with 0x80 to 0xFF.
    Because of this, we shall check if we are on a hard disk and rely fully on which hard disk
    diskboot.S is stored on.
    */

    movb diskboot_disk_num, %dh //Checks whether bootloader installer provided a valid disk
    cmpb $0xFF, %dh
    je 1f

    movb %dh, %dl // Did find a suitable disk by the installer
1:
    cmpb $0x80, %dl // We check if we are on hard disk or not. If not, take 0x80.
    jnb 1f

    movb $0x80, %dl
1:
    /*
    Now we are going to query bios in many ways 
    to see if lba is supported and load diskboot in memory.
    */

    xorw %ax, %ax
    movb $0x41, %ah
    movw $0x55aa, %bx

    /*
    INT 0x13h Extensions Installation Check
    AH = 0x41h BX = 0x55aa DL = Drive Num.
    */

    int $0x13

    jc chs

    cmp $0x01, %ah
    je chs

    cmp $0xaa55, %bx
    jne chs 

lba:

    movw $dap, %si

    movw diskboot_lba_low, %ax
    movw %ax, 8(%si)
    movw diskboot_lba_high, %ax
    movw %ax, 12(%si)

    /* Now that we have filled dap, we can continue onto the main read.*/

    /*
    BIOS INT 13h BIOS EXTENDED READ:
        AH = 42h
        DL = drive number
        DS:SI -> disk address packet
    */

    movw $0x4200, %ax

    int $0x13

    /*
    Now we check for final errors for the lba read. If there is an error, then we 
    are for sure that there is no way of extended read and we use chs addressing instead.
    */
    jc chs

    testb %ah, %ah
    jnz chs

    jmp copy
chs:

copy:
    /*
    Now that we have loaded the sectors in disk buffer.
    We are now going to copy the 10 sectors for diskboot into addr 0x6000.
    ds:si = 0x70000 es:di = 0x6000 0x1000 bytes 0x800 words to transfer.
    */


    movw $0x7000, %ax
    movw %ax, %ds
    movw $0, %si
    movw $0x600, %ax
    movw %ax, %es
    movw $0, %di

    movw $0x800, %cx

    rep
    movsw

    //Jump to diskboot.
    ljmp $0, $0x6000
print:
    movb $0x0E, %ah
1:
    lodsb
    cmp $0, %al
    je 2f
    int $0x10
    jmp 1b
2:
    ret

welcome_msg: .asciz "Hello, world!"

// The disk address packet that is responsible for trasferring diskboot

dap:
    dap_size: .byte 0x10
    dap_rsrv: .byte 0
    dap_sectors: .word 8
    dap_transfer_offset: .word 0x0000
    dap_transfer_segment: .word 0x7000
    dap_lba_addr: .quad 0 // Filled at runtime

die:
    jmp die

.org 0x1F5

diskboot_lba_high:
    .long 0
diskboot_lba_low:
    .long 1

// 0xFF stands for not defined, go with the bios provided disk number.
diskboot_disk_num:
    .byte 0xFF 

.word 0xAA55