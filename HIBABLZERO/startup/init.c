/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#include <startup/init.h>
#include <hb0/types.h>

void NORETURN kern_startup(void)
{
    u16* vga_mem = (u16*)0xb8000;
    vga_mem[1] = 0x0F41;
    while(1) { ; };
};