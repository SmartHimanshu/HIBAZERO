#ifndef HB0_IO_SERIAL_IO_H
#define HB0_IO_SERIAL_IO_H

#include "../../machine/io.h"

static inline u8 serial_in(u16 port, const u8 offset) 
{
    return inb(port+offset);
}

static inline void serial_out(u16 port, const u8 offset, u8 data) 
{
    outb(port+offset, data);
}

#define serial_inb inb
#define serial_inw inw
#define serial_inl inl
#define serial_outb outb
#define serial_outw outw
#define serial_outl outl

#endif