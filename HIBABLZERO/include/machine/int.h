/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#ifndef MACHINE_INT_H
#define MACHINE_INT_H

#include <hb0/types.h>

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

#define DEFAULT_INT_FLAGS 0x200

#define CF_FLAG 1


void __attribute__((regparm(3))) bios_interrupt(u8 intno, struct bios_regs *regs);

#endif