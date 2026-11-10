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
 * Function: test A20 line.
 * Destroys: ax
 * Returns: 0 for ax disabled and 1 for ax enabled.
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
	cmpw $0x0000, %ax /* Check for change */

	movw $0x0000, %ax

	/*
	 * We check for change in the value at 0x7B00.
	 * If it is 0x0000, we failed and the written value looped back.
	 */
	je .a20_exit

	movw $0x0001, %ax	/* A20 is enabled. */

.a20_test_exit:
	popw %di
	popw %si
	popw %es
	popw %ds

	popf

	ret

a20_test_looped:
        pushw %cx
        movw $65535, %cx

.a20_test_looped_loop:
        call a20_test
        testw %ax, %ax
        jnz .a20_test_looped_done
        loop .a20_test_looped_loop

.a20_test_looped_done:
        xorw %ax, %ax
        testw %cx, %cx
        jz .a20_test_looped_done_over

.a20_test_looped_done_over:
        popw %cx
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
 * NOTE: The result of failure or not is not reliable in case of success
 */
a20_enable_keyboard:
        /*
         * This section of code attempts to enable A20.
         * Disable interrupts and keybaord from start for
         * no interference.
         */

        cli                                             // Make sure we are good with inerrupts.

        call .a20_enable_keyboard_wait_input_buffer     // Disable keyboard
        movb $0xAD, %al
        outb %al, $0x64

        call .a20_enable_keyboard_wait_input_buffer     // Read controller output port
        movb $0xD0, %al
        outb %al, $0x64

        call .a20_enable_keyboard_wait_response_byte    // Save response byte
        inb $0x60, %al
        pushw %ax

        call .a20_enable_keyboard_wait_input_buffer     // Write next byte to controller
        movb $0xD1, %al
        outb %al, $0x64

        call .a20_enable_keyboard_wait_input_buffer     // Set controller output bit for A20
        popw %ax
        orb $2, %al
        outb %al, $0x60

        /*
         * This section of code tests whether A20 is enabled
         * using the keyboard controller itself. Ofc this is
         * not enough and the true A20 check needs to be done properly.
         */
        call .a20_enable_keyboard_wait_input_buffer     // Read controller output port 
        movb $0xD0, %al
        outb %al, $0x64

        call .a20_enable_keyboard_wait_response_byte    // Get A20 state
        inb $0x60, %al
        testb $2, %al
        jz .a20_enable_keyboard_fail                    // Go to failure case if A20 not enabled.

        call .a20_enable_keyboard_wait_input_buffer     // Reactivate keyboard
        movb $0xAE, %al
        outb %al, $0x64

        call .a20_enable_keyboard_wait_input_buffer 
        ret

.a20_enable_keyboard_wait_input_buffer:
        movw $65535, %cx
1:
        inb $0x64, %al
        testb $2, %al
        loop .a20_enable_keyboard_wait_input_buffer

        testw %cx, %cx
        jnz .a20_enable_keyboard_hard_fail
        ret

.a20_enable_keyboard_wait_response_byte:
        movw $65535, %cx
1:
        inb $0x64, %al
        testb $1, %al
        loop .a20_enable_keyboard_wait_response_byte

        testw %cx, %cx
        jnz .a20_enable_keyboard_hard_fail
        ret

.a20_enable_keyboard_fail:
        call .a20_enable_keyboard_wait_input_buffer     // Reactivate keyboard
        movb $0xAE, %al
        outb %al, $0x64

        call .a20_enable_keyboard_wait_input_buffer     // Reactivate interrupts
        ret

/* 
 * The main function responsible for enabling a20 line for us.
 * Uses all of the a20 implementations shown so far.
 * This implementation is inspired by the OSdev wiki recommended
 * implementation: https://wiki.osdev.org/A20_Line?utm_source=chatgpt.com#Recommended_Method
 */
a20_enable_main:
        call a20_test                                   // If A20 is enabled, simply do nothing.
        testw %ax, %ax
        jnz .a20_enable_main_exit_success

        // A20 not enabled here. So proceed with BIOS method.

        call a20_enable_bios
        call a20_test
        testw %ax, %ax
        jnz .a20_enable_main_exit_success               // Since BIOS resutl is bogus, use normal.

        // Now proceed with keyboard controller method.

        call a20_enable_keyboard
        call a20_test_looped
        testw %ax, %ax
        jnz .a20_enable_main_exit_success               // Since keyboard not reliable.

        // If not even the keyboard is working, we try our last method: fast a20.

        call a20_enable_fast
        call a20_test_looped
        testw %ax, %ax
        jnz .a20_enable_main_exit_success               // No test for A20 on its own anyway.


        
.a20_enable_main_exit_failure:
        xorw %ax, %ax
        ret
.a20_enable_main_exit_success:
        movw $1, %ax
        ret
