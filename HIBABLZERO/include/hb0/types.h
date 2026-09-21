/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#ifndef TYPES_H
#define TYPES_H

typedef unsigned char u8;
typedef signed char s8;

typedef unsigned short u16;
typedef signed short s16;

typedef unsigned int u32;
typedef signed int s32;

typedef unsigned long long u64;
typedef signed long long s64;

typedef u64 size_t;

#define NULL ((void*)0)

#define NORETURN __attribute__((noreturn))

#define PACKED __attribute__((packed))

#endif