/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#include "uart.h"
#include <hb0/io/serial_io.h>
#include <hb0/err.h>

#define ARBRITRARY_VALUE 0x81

const struct serial_chip uart_standard_chips_config[] = 
{
    [UNKNOWN] = {
        "Unkown Chip",
        false,
        1
    },

    [U8520] = {
        "UART 8520",
        false,
        1
    },

    [U16450] = {
        "UART 16450",
        false,
        1
    }/*,

    We are gonna make the drivers and fifo support later for our bootloader for the
    following so marked useless for now.

    [U16550] = {
        "UART 16550",
        true,
        1
    },

    [U16550A] = {
        "UART 16550A",
        true,
        1
    },

    [U16750] = {
        "UART 16750",
        false,
        1
    },
    */

};

int serial_get_chip(struct serial_port* port, struct serial_chip** detected_chip_version)
{
    serial_out(port->iobase, 2, 0xE7); // Write to FRC
    /* 
    When we write this into FRC, we can check IIR to see if we have FIFO
    Support with us. This means that we can detect which chip we are dealing
    with and there support for FIFO. We have to now check which flags got set.
    */
    u8 IIR = serial_in(port->iobase, 2);

    if(IIR&(1<<7))
    { 
        // No FIFO support for now.
        return -FAILED;
    }
    else
    {
        /*
        Since scratch register is not available for chip 8250, We can use this to 
        distinguish between 8250 16450
        */

        serial_out(port->iobase, 7, ARBRITRARY_VALUE);
        u8 scratch = serial_in(port->iobase, 7);
        if(scratch==ARBRITRARY_VALUE)
        {
            // Scratch register exists, so we are gonna be going forward with 16450.
            *detected_chip_version = &uart_standard_chips_config[U16450];
            return SUCCESS;
        }
        else
        {
            // Scratch register doesn't exist. So we are going to have to be with 8250.
            *detected_chip_version = &uart_standard_chips_config[U8520];
            return SUCCESS;
        }
    }
}

// Set serial divisor latch

void serial_set_dl(struct serial_port* port, u16 value)
{
    serial_out(port->iobase, 0, value&0xFF);
    serial_out(port->iobase, 1, (value>>8)&0xFF);
}

// Get serial divisor latch

u16 serial_get_dl(struct serial_port* port, u16 value)
{
    u8 DLL = serial_in(port->iobase, 0);
    u8 DLH = serial_in(port->iobase, 1);
    return ((u16)DLH<<8) | DLL;
}

int serial_interface_init(struct serial_interface* interface, u16 divisor)
{
    int status = serial_get_chip(interface->port, &interface->chip);

    // Set baud rate.
    serial_out(interface->port, 3, 0x80);
    serial_set_dl(interface->port, divisor);

    /* 
    No stop bit, 8 bit data length for simple communication. Our bootloader
    doesn't need advanced features right now. 
    */

    serial_out(interface->port, 3, 0x3);
    
    return status;
}

int serial_chip_test(struct serial_port* port)
{
    serial_out(port, 4, 1<<4);
    serial_out(port, 0, ARBRITRARY_VALUE);
    u8 val = serial_in(port, 0);
    if(val==ARBRITRARY_VALUE)
    {
        return SUCCESS;
    }
    else
    {
        return -ERRFAULTY;
    }
}