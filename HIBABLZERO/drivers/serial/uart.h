/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#ifndef DRIVER_SERIAL_INTERNAL_H
#define DRIVER_SERIAL_INTERNAL_H

#define PORT_COM1 0x3F8 
#define PORT_COM2 0x2F8 
#define PORT_COM3 0x3E8 
#define PORT_COM4 0x2E8 
#define PORT_COM5 0x5F8
#define PORT_COM6 0x4F8
#define PORT_COM7 0x5E8
#define PORT_COM8 0x4E8

#include <hb0/types.h>
#include <hb0/bool.h>

struct serial_chip
{
    const char* chip_name;
    bool fifo_supported;
    u8 fifo_size;
};

enum chip_type
{
    UNKNOWN = 0,
    U8520,
    U16450,
    U16550,
    U16550A,
    U16750
};

struct serial_port
{
    u32 iobase;
};

struct serial_interface
{
    struct serial_port* port;
    struct serial_chip* chip;
    void (*putc)(struct serial_interface console, char c);
    void (*backspace)(struct serial_interface console);
    void (*newline)(struct serial_interface console);
    char (*readc)(struct serial_interface console);
    bool (*can_read)(void);
    bool (*can_write)(void);
};

u8 get_lsr(struct serial_port *port);
void set_lsr(struct serial_port *port, u8 lsr);
u8 get_dl(struct serial_port *port);
void set_dl(struct serial_port *port, u8 dl);

#endif