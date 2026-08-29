#ifndef MACHINE_IO_H
#define MACHINE_IO_H
#include "../hb0/types.h"

static inline u8 _inb(u16 port)
{
    u8 res;
    asm volatile("inb %1, %0" : "=a"(res) : "Nd"(port));
    return res;
};

static inline u16 _inw(u16 port)
{
    u16 res;
    asm volatile("inw %1, %0" : "=a"(res) : "Nd"(port));
    return res;
};

static inline u32 _inl(u16 port)
{
    u32 res;
    asm volatile("inl %1, %0" : "=a"(res) : "Nd"(port));
    return res;
};

static inline void _outb(u16 port, u8 data)
{
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
    return;
};

static inline void _outw(u16 port, u16 data)
{
    asm volatile("outw %0, %1" : : "a"(data), "Nd"(port));
    return;
};

static inline void _outl(u16 port, u32 data)
{
    asm volatile("outl %0, %1" : : "a"(data), "Nd"(port));
    return;
};

#define inb _inb
#define inw _inw
#define inl _inl
#define outb _outb
#define outw _outw
#define outl _outl

#endif