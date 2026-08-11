.code16

.file "boot.S"

.globl _start
_start:
    //These are generally thrashed by bios so we have to restore them.

    cli

    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %di

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

    movl diskboot_lba_low, %eax
    movl %eax, 8(%si)

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
    movw $0x0801, %ax
    movb %al, addressing_mode


    xorb %dh, %dh
    push %dx //Save drive

    /*
    BIOS INT 13h BIOS GET PARAMETERS:
        AH = 08h
        DL = drive number
        ES:DI -> 0x0000:0x0000
    */

    int $0x13

    //If their bios can't even do chs things then bootloader gonna mark them faulty.
    jc faulty_system

    testb %ah, %ah
    jnz faulty_system

    //Using dap as scratch memory
    movw $dap, %si

    incb %dh
    movb %dh, (%si) //Head num in first byte
    
    

    xorw %ax, %ax
    movb %cl, %al
    andb $0b00111111, %al //Extract max sector number
    incb %al
    movb %al, 1(%si) //Max sector in second byte


    movb %ch, %al //Extract low 8 bits of cylinder
    andb $0b11000000, %cl //Extract high 2 bits of cylinder
    shrb $6, %cl
    movb %cl, %ah //Now ax contains cylinder num
    incw %ax
    movw %ax, 2(%si) //Max cylinder in third two bytes


    movl diskboot_lba_low, %eax

    xorl %ebx, %ebx
    movb 1(%si), %bl //Sectors per track

    xorl %edx, %edx

    divl %ebx   //Now edx contains dap % sectors. Which is sector needed for lba.

    movb %dl, %cl //Transfer sector into cl to prep for disk read.
    


    xorl %edx, %edx

    xorl %ebx, %ebx 
    movb (%si), %bl
    divl %ebx //Divide by heads.

    incb %cl // Normalize sector start to 1 based

    /* 
    Right now %eax contains cylinder.
    and %edx contains head.
    */

    

    movb %dl, %ch
    pop %dx
    movb %ch, %dh //Now dh has head needed for next read.

    movb %al, %ch //Low 8 bits of cylinder in ch now.
    shrw $2, %ax
    andb $0b11000000, %al
    orb %al, %cl    //Load high 2 bits of cylinder in upper two bits of cl

    movw $0x7000, %bx
    movw %bx, %es
    xorw %bx, %bx

    /*
    BIOS INT 13h BIOS READ SECTORS (CHS):
        AH = 02h
        AL = number of sectors to read (must be nonzero)
        CH = low eight bits of cylinder number
        CL = sector number 1-63 (bits 0-5)
        high two bits of cylinder (bits 6-7, hard disk only)
        DH = head number
        DL = drive number (bit 7 set for hard disk)
        ES:BX -> data buffer
    */

    movw $0x0202, %ax
    int $0x13

    jc faulty_system

copy:
    /*
    Now that we have loaded the sectors in disk buffer.
    We are now going to copy the 8 sectors for diskboot into addr 0x6000.
    ds:si = 0x70000 es:di = 0x6000 0x400 bytes 0x200 words to transfer.
    */


    movw $0x7000, %ax
    movw %ax, %ds
    movw $0, %si
    movw $0x600, %ax
    movw %ax, %es
    movw $0, %di

    movw $0x200, %cx

    rep
    movsw

    movw $dap, %si

    //Jump to diskboot.
    ljmp $0, $0x6000

faulty_system:
    movw $faulty_msg, %si
    call print
    cli
    hlt
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

welcome_msg: .asciz "Welcome to HIBAZERO project!\n"
faulty_msg: .asciz "Your system bios or something is seriously faulty.\n"

// The disk address packet that is responsible for trasferring diskboot

/* 
Also we are using this dap as we lack enough memory to store cylinder etc. reliably.
first byte: max head num
second byte: max sector num
third long: max cylinder

*/
dap:
    dap_size: .byte 0x10
    dap_rsrv: .byte 0
    dap_sectors: .word 2
    dap_transfer_offset: .word 0x0000
    dap_transfer_segment: .word 0x7000
    dap_lba_addr: .quad 0 // Filled at runtime
addressing_mode: .byte 0

die:
    jmp die

diskboot_lba_low:
    .long 1
diskboot_lba_high:
    .long 0


// 0xFF stands for not defined, go with the bios provided disk number.
diskboot_disk_num:
    .byte 0xFF 

.org 0x1FE

.word 0xAA55