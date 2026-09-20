/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#include <startup/init.h>
#include <hb0/types.h>
#include <drivers/serial.h>
#include <machine/int.h>


void NORETURN kern_startup(void)
{
    u16* vga_mem = (u16*)0xb8000;
    vga_mem[1] = 0x0F41;

    
    struct serial_port port = {PORT_COM1};
    struct serial_interface interface = {0};
    interface.port = &port;
    
    int res = serial_interface_init(&interface, 1, PORT_COM1);

    struct bios_regs regs = {0};
    

    while(1) { ; };
};