#ifndef MACHINE_INT_H
#define MACHINE_INT_H

#include <types.h>

struct bios_regs
{
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;
    u32 edi;
    u32 esi;
    u16 flags;
    u16 es;
    u16 ds;
    u16 dummy;
} PACKED;

#endif