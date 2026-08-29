/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#include <drivers/serial.h>
#include <hb0/io/serial_io.h>
#include <hb0/err.h>

#include "uart.h"

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
        *detected_chip_version = &uart_standard_chips_config[U8520];
        return SUCCESS;
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

int serial_interface_init(struct serial_interface* interface, u16 divisor, u16 port)
{
    interface->port->iobase = port;
    int status = serial_get_chip(interface->port, &interface->chip);

    if(status) return status;

    // Set baud rate.
    serial_out(interface->port->iobase, 3, 0x80);
    serial_set_dl(interface->port, divisor);

    /* 
    No stop bit, 8 bit data length for simple communication. Our bootloader
    doesn't need advanced features right now. 
    */

    serial_out(interface->port->iobase, 3, 0x3);

    if(interface->chip->fifo_size==1)
    {
        interface->can_read = can_serial_recv;
        interface->can_write = can_serial_send;
        interface->backspace = serial_backspace8250;
        interface->newline = serial_newline8250;
        interface->putc = serial_send8250;
        interface->readc = serial_recv8250;
    }

    return status;
}

int serial_chip_test(struct serial_port* port)
{
    serial_out(port->iobase, 4, 1<<4);
    serial_out(port->iobase, 0, ARBRITRARY_VALUE);
    u8 val = serial_in(port->iobase, 0);
    if(val==ARBRITRARY_VALUE)
    {
        serial_out(port->iobase, 4, 0);
        return SUCCESS;
    }
    else
    {
        return -ERRFAULTY;
    }
}

bool can_serial_recv(struct serial_port* port)
{
    return (serial_in(port->iobase, 5) & 1);
}

int serial_recv8250(struct serial_interface* chip, u8* data)
{
    if(can_serial_recv(chip->port))
    {
        *data = serial_in(chip->port->iobase, 0);
        return SUCCESS;
    }
    else
    {
        return -FAILED;
    }
}

bool can_serial_send(struct serial_port* port)
{
    return (serial_in(port->iobase, 5) & (1<<5));
}

int serial_send8250(struct serial_interface* chip, u8 data)
{
    if(can_serial_send(chip->port))
    {
        serial_out(chip->port->iobase, 0, data);
        return SUCCESS;
    }
    else
    {
        return -FAILED;
    }
}

int serial_newline8250(struct serial_interface* chip)
{
    int status = serial_send8250(chip, '\r');
    if(status)
    {
        return status;
    }
    else
    {
        while(serial_send8250(chip, '\n'))
        {
            ;
        }
    }
    return SUCCESS;
}

int serial_backspace8250(struct serial_interface* chip)
{
    int status = serial_send8250(chip, 0x08);
    if(status)
    {
        return status;
    }
    else
    {
        while(serial_send8250(chip, 0x20))
        {
            ;
        }
        while(serial_send8250(chip, 0x08))
        {
            ;
        }
    }

    return SUCCESS;
}