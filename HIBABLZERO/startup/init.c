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
    struct serial_port port = {PORT_COM1};
    struct serial_interface interface = {0};
    interface.port = &port;
    
    int res = serial_interface_init(&interface, 1, PORT_COM1);

    struct bios_regs regs = {0};
    
    regs.eax = 0x0E00 | 'A';
    regs.ebx = 0x000F;
    
    while(interface.putc(&interface, 'A'))
    {

    }

    bios_interrupt(0x10, &regs);

    while(interface.putc(&interface, 'A'))
    {

    }

    while(1) { ; };
};