/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#ifndef HB0_LIBC_H
#define HB0_LIBC_H

#include "types.h"

static inline void memcpy(void* src, void* dest, size_t len)
{
    u8* src_bytes = (u8*) src;
    u8* dest_bytes = (u8*) dest;
    for(size_t i = 0; i < len; i++)
    {
        dest_bytes[i] = src_bytes[i];
    }
}

#endif