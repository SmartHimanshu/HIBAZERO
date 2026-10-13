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
    },

    [U16550] = {
        "UART 16550",
        true,
        16
    },

    [U16550A] = {
        "UART 16550A",
        true,
        16
    },

    [U16750] = {
        "UART 16750",
        false,
        1
    },

};

// Set serial divisor latch

static inline void serial_set_dl(struct serial_port* port, u16 value)
{
    serial_out(port->iobase, 0, value&0xFF);
    serial_out(port->iobase, 1, (value>>8)&0xFF);
}

// Get serial divisor latch

static inline u16 serial_get_dl(struct serial_port* port)
{
    u8 DLL = serial_in(port->iobase, 0);
    u8 DLH = serial_in(port->iobase, 1);
    return ((u16)DLH<<8) | DLL;
}

/*
Serial chip detection mechanism that will be used
to determine which kind of UART serial chip we
are dealing with.

NOTE: This function also enables fifo for any chip
we encounter that has fifo supported with it.
*/

void serial_get_chip(struct serial_port* port, const struct serial_chip **detected_chip_version)
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
        if(IIR&(1<<6))
        {
            if(IIR&(1<<5))
            {
                // Bit 5 indicates 64 byte fifo, which is supported in U16750.
                *detected_chip_version = &uart_standard_chips_config[U16750];
            }
            else
            {
                // Ordinary fifo, which is supported in regular 16550A.
                *detected_chip_version = &uart_standard_chips_config[U16550A];
            }
        }
        else
        {
            // FIFO enabled but not usable. So we assume 16550.
            *detected_chip_version = &uart_standard_chips_config[U16550];
        }
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
        }
        else
        {
            // Scratch register doesn't exist. So we are going to have to be with 8250.
            *detected_chip_version = &uart_standard_chips_config[U8520];
        }
    }
}

/*
Parent init function. Designed to detect
the chip at port, as well as initlialize
the baud rate and set serial ops respectfully.
*/

int serial_interface_init(struct serial_interface* interface, u16 divisor, u16 port)
{
    interface->port->iobase = port;
    serial_get_chip(interface->port, &interface->chip);

    // Set baud rate.
    serial_out(interface->port->iobase, 3, 0x80);
    serial_set_dl(interface->port, divisor);

    /* 
    No stop bit, 8 bit data length for simple communication. Our bootloader
    doesn't need advanced features. 
    */

    serial_out(interface->port->iobase, 3, 0x3);

    int status = serial_chip_test(interface->port);

    interface->newline = serial_newline;
    interface->backspace = serial_backspace;
    interface->readc = serial_recv;
    interface->puts = (interface->chip->fifo_size==1) ? serial_send_nf_batched : serial_send_fifo_batched;

    return status;
}

/*
This function basically checks if serial input can be drawn.
+ve for yes.
0 for no.
*/

static inline u8 can_serial_recv(struct serial_port* port)
{
    return (serial_in(port->iobase, 5) & 1);
}

/*
A synchronus recv that waits for the next character to be
available for getting drawn. Sets data to the read character.
*/

void serial_recv(struct serial_port* port, u8* data)
{
    while(!can_serial_recv(port))
    {
        ;   // Wait for serial to be able to get input.
    }
    *data = serial_in(port->iobase, 0);
}

/*
Checks whether data can be sent through. 
1 for yes. 0 for no. (no fifo)
*/

static inline u8 can_serial_send_nf(struct serial_port* port)
{
    return (serial_in(port->iobase, 5) & (1<<5));
}

/*
Checks whether data has been emptied in fifo.
1 for yes, 0 for no. (also supports normal)
*/

static inline u8 can_serial_send_fifo(struct serial_port* port)
{
    return (serial_in(port->iobase, 5) & (1<<6));
}

/*
A synchronous send that will only send 1 to 16
bytes over. no fifo.
*/

int serial_send_nf(struct serial_port* port, u8 data[16], u8 len)
{
    if ((len > 16) || (len == 0)) return -ERRINV;
    for(u8 i = 0; i < len; i++)
    {
        while(!can_serial_send_nf(port))
        {
            ;
        }
        serial_out(port->iobase, 0, data[i]);
    }
    return SUCCESS;
}

/*
A synchronous send that will only send 1 to 16
bytes over via fifo.
*/

int serial_send_fifo(struct serial_port* port, u8 data[16], u8 len)
{
    if ((len > 16) || (len == 0)) return -ERRINV;
    while(!can_serial_send_fifo(port))
    {
        ;
    }
    for(u8 i = 0; i < len; i++)
    {
        serial_out(port->iobase, 0, data[i]);
    }
    return SUCCESS;
}

/*
This is a loopback mode serial chip test function with 
timeout that will test the chip to determine if it can be used.
*/

int serial_chip_test(struct serial_port* port)
{
    serial_out(port->iobase, 4, 1<<4);  // Set MCR to loopback mode.
    
    // First timeout supported loop
    u16 i = 10000;
    bool sent = false;
    while(i--)
    {
        if(can_serial_send_nf(port))
        {
            serial_out(port->iobase, 0, ARBRITRARY_VALUE);
            sent = true;
            break;
        }
    }
    if(!sent) return -ERRFAULTY;
    
    // Second timeout supported loop
    i = 10000;
    sent = false;    // Reuse the boolean and u16 variables
    u8 val;
    while(i--)
    {
        if(can_serial_recv(port))
        {
            val = serial_in(port->iobase, 0);
            sent = true;
            break;
        }
    }
    if(!sent) return -ERRFAULTY;

    if(val==ARBRITRARY_VALUE)
    {
        serial_out(port->iobase, 4, 0); // Disable loopback mode.
        return SUCCESS;
    }
    else
    {
        return -ERRFAULTY;
    }
}

/*
A synchronous send to serial device that sends data out 
in batches. Can transmit data of size of up to 4GB - 1.
Transmits data without the use of fifo. (NO FIFO).
*/

int serial_send_nf_batched(struct serial_port* port, u8* data, u32 length)
{
    if(length==0) return -ERRINV;
    for(u32 i = 0; i < length; i+=16)
    {
        u32 remaining = length - i;
        u8 batch = (remaining >= 16) ? 16 : (u8)remaining;
        serial_send_nf(port, data+i, batch);
    }
    return SUCCESS;
}

/*
A synchronous send to serial device that sends data out 
in batches. Can transmit data of size of up to 4GB - 1.
Transmits data via the use of fifo. (FIFO).
*/

int serial_send_fifo_batched(struct serial_port* port, u8* data, u32 length)
{
    if(length==0) return -ERRINV;
    for(u32 i = 0; i < length; i+=16)
    {
        u32 remaining = length - i;
        u32 current_batch = (remaining >= 16) ? 16 : (u8)remaining;
        serial_send_fifo(port, data+i, current_batch);
    }
    return SUCCESS;
}

// A synchronous newline function that would send newline instruction over.

void serial_newline(struct serial_interface* chip)
{
    chip->puts(chip->port, "\r\n", 2);
}

// A synchronous backspace function that would send backspace instruction over.

void serial_backspace(struct serial_interface* chip)
{
    u8 backspace_arr[3] = {0x08, 0x20, 0x08};
    chip->puts(chip->port, backspace_arr, 3);
}