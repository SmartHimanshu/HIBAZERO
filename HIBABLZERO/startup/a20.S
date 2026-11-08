/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

.code16

.file "a20.S"

/*
 *	Function: test A20 line.
 *	Destroys: ax
 *	Returns: 0 for ax disabled and 1 for ax enabled.
 */

.global a20_test
a20_test:
	pushf
	pushw %ds
	pushw %es
	pushw %si
	pushw %di

	xorw %ax, %ax
	movw %ax, %ds		/* DS = 0x0000 */

	notw %ax
	movw %ax, %es		/* ES = 0xFFFF */

	movw $0x7B00, %si	/* SI = 0x7B00 */
	movw $0x7B10, %di	/* DI = 0x7B10 */

	/*
	 * ES:DI = 0xFFFF * 16 + 0x7B10 = 0x107B00 (free memory if a20 enabled. wrapped to 0x7B00 if not) 
	 * DS:SI = 0x0000 * 16 + 0x7B00 = 0x7B00 (scratch space) 
	 */

	movw $0xFFFF, %ds:(%si)
	movw $0x0000, %es:(%di)

	movw %ds:(%si), %ax
	cmpw %ax, $0x0000	/* Check for change */

	movw $0x0000, %ax

	/*
	 *	We check for change in the value at 0x7B00.
	 *	If it is 0x0000, we failed and the written value looped back.
	 */
	je .a20_exit

	movw $0x0001, %ax	/* A20 is enabled. */

.a20_exit:
	popw %di
	popw %si
	popw %es
	popw %ds

	popf

	ret

/* A20 Enable FAST: Destroys AL. There's no confirmed check for it so no check here */
a20_enable_fast:
	inb $0x92, %al
	orb $0x2, %al
	outb %al, $0x92

	ret

/*
 * A20 enable using BIOS:
 * Returns 0 for success, else (status of A20 support OR 1<<2) in AX.
 * or if Status of A20 support was clobbered, return 1<<3.
 */
a20_enable_bios:

        /*
         * BIOS SYSTEM - later PS/2s - QUERY A20 GATE SUPPORT
         * AX = 0x2403 INT = 0x15
         * AH = 0x00, CF clear on success and AH != 0, CF set on failure.
         */
        movw $0x2403, %ax
        int $0x15

        jc .a20_enable_bios_fail_bx_clobber

        testb %ah, %ah
        jnz .a20_enable_bios_fail_bx_clobber

        pushw %bx /* Save A20 support value from clobbering */
        /*
         * BIOS SYSTEM - later PS/2s - GET A20 GATE SUPPORT
         * AX = 0x2402 INT = 0x15
         * AL = 0x00 for disabled and AL = 0x01 for enabled
         * AH = 0x00, CF clear on success and AH != 0, CF set on failure.
         */

        movw $0x2402, %ax
        int $0x15

        jc .a20_enable_bios_fail

        testb %ah, %ah
        jnz .a20_enable_bios_fail

        testb %al, %al
        jnz .a20_enable_bios_success

        /*
         * BIOS SYSTEM - later PS/2s - ENABLE A20 GATE
         * AX = 0x2401 INT = 0x15
         * AH = 0x00, CF clear on success and AH != 0, CF set on failure.
         */

        movw $0x2401, %ax
        int $0x15

        jc .a20_enable_bios_fail

        testb %ah, %ah
        jnz .a20_enable_bios_fail

        /*
         * Retry - Some BIOSes return success while not enabling the A20,
         * so, we check the A20 status again to see if we set it.
         */

        movw $0x2402, %ax
        int $0x15

        jc .a20_enable_bios_fail

        testb %ah, %ah
        jnz .a20_enable_bios_fail

        testb %al, %al
        jz .a20_enable_bios_fail

.a20_enable_bios_success:
        popw %ax
        movw $0x0000, %ax
        ret

.a20_enable_bios_fail_bx_clobber:
        movw $(1<<3), %ax
        ret

.a20_enable_bios_fail:
        popw %ax
        orw $(1<<2), %ax
        ret

/*
 * A20 enable using keyboard controller
 * 
 */
a20_enable_keyboard:
        
