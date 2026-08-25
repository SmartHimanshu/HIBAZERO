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