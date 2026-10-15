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
#include <drivers/vga_text.h>

void NORETURN kern_startup(void)
{    
    struct serial_port port = {PORT_COM1};
    struct serial_interface interface = {0};
    interface.port = &port;
    struct vga_text_console_t vga_controller;

    serial_interface_init(&interface, 1, PORT_COM1);



    vga_text_init(&vga_controller);

    vga_controller.puts(&vga_controller, (u8*)"Hello, World!", 0b00001111);    
    while(true)
    {
        char c;
        interface.readc(interface.port, (u8*)&c);
        interface.puts(interface.port, (u8*)&c, 1);
    }
};