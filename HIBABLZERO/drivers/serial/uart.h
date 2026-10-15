/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#ifndef LOCAL_SERIAL_H
#define LOCAL_SERIAL_H

#include <hb0/types.h>
#include <drivers/serial.h>

enum chip_type
{
    UNKNOWN = 0,
    U8520,
    U16450,
    U16550,
    U16550A,
    U16750
};


void serial_get_chip(struct serial_port* port, const struct serial_chip** detected_chip_version);
int serial_chip_test(struct serial_port* port);
void serial_newline(struct serial_interface* chip);
void serial_backspace(struct serial_interface* chip);
int serial_send_fifo_batched(struct serial_port* port, u8* data, u32 length);
int serial_send_nf_batched(struct serial_port* port, u8* data, u32 length);
void serial_recv(struct serial_port* port, u8* data);
#endif