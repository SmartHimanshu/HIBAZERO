/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#ifndef DRIVER_INTERNAL_SERIAL_INTERNAL_H
#define DRIVER_INTERNAL_SERIAL_INTERNAL_H

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


int serial_backspace8250(struct serial_interface* chip);
int serial_get_chip(struct serial_port* port, const struct serial_chip** detected_chip_version);
void serial_set_dl(struct serial_port* port, u16 value);
u16 serial_get_dl(struct serial_port* port);
int serial_chip_test(struct serial_port* port);
bool can_serial_recv(struct serial_port* port);
int serial_recv8250(struct serial_interface* chip, u8* data);
bool can_serial_send(struct serial_port* port);
int serial_send8250(struct serial_interface* chip, u8 data);
int serial_newline8250(struct serial_interface* chip);

#endif