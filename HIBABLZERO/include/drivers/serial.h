/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#ifndef DRIVER_SERIAL_H
#define DRIVER_SERIAL_H

#include <hb0/types.h>

#define PORT_COM1 0x3F8 
#define PORT_COM2 0x2F8 
#define PORT_COM3 0x3E8 
#define PORT_COM4 0x2E8 
#define PORT_COM5 0x5F8
#define PORT_COM6 0x4F8
#define PORT_COM7 0x5E8
#define PORT_COM8 0x4E8

struct serial_chip
{
    const char* chip_name;
    bool fifo_supported;
    u8 fifo_size;
};

struct serial_port
{
    u32 iobase;
};

struct serial_interface
{
    struct serial_port* port;
    const struct serial_chip* chip;
    void (*putc)(struct serial_interface *console, u8 c);
    void (*backspace)(struct serial_interface *console);
    void (*newline)(struct serial_interface *console);
    void (*readc)(struct serial_interface *console, u8* c);
    bool (*can_read)(struct serial_port* port);
    bool (*can_write)(struct serial_port* port);
};

int serial_interface_init(struct serial_interface* interface, u16 divisor, u16 port);

#endif