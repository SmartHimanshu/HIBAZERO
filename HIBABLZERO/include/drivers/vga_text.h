/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#ifndef DRIVER_VGA_TEXT_H
#define DRIVER_VGA_TEXT_H

#include <hb0/types.h>

struct bios_video_mode_conf_t
{
    u8      bios_num;       // BIOS video mode number

    u8      text_rows;      // Text mode number of rows
    u8      text_cols;      // Text mode number of cols
    
    bool    monochrome;     // Is the mode monochrome
    bool    color;          // Does the BVM support color graphics

};

typedef struct vga_text_console_t
{
    volatile u16* _console_addr;
    struct bios_video_mode_conf_t _local_config;
    
    u8 _cursor_x;
    u8 _cursor_y;

    

    void (*puts)(struct vga_text_console_t*, u8* s, u8 color);
    void (*newline)(struct vga_text_console_t*);
    void (*backspace)(struct vga_text_console_t*);
    
    // Set by init caller
    void (*restore)(void);
} vga_text_console;

// VGA Text color mode helpers.

#define VGA_TEXT_COLOR_BLACK            0x0
#define VGA_TEXT_COLOR_BLUE             0x1
#define VGA_TEXT_COLOR_GREEN            0x2
#define VGA_TEXT_COLOR_CYAN             0x3
#define VGA_TEXT_COLOR_RED              0x4
#define VGA_TEXT_COLOR_MAGNETA          0x5
#define VGA_TEXT_COLOR_BROWN            0x6
#define VGA_TEXT_COLOR_LIGHT_GRAY       0x7
#define VGA_TEXT_COLOR_DARK_GRAY        0x8
#define VGA_TEXT_COLOR_LIGHT_BLUE       0x9
#define VGA_TEXT_COLOR_LIGHT_GREEN      0xA
#define VGA_TEXT_COLOR_LIGHT_CYAN       0xB
#define VGA_TEXT_COLOR_LIGHT_RED        0xC
#define VGA_TEXT_COLOR_LIGHT_MAGNETA    0xD
#define VGA_TEXT_COLOR_YELLOW           0xE
#define VGA_TEXT_COLOR_WHITE            0xF

static inline u8 vga_make_color(u8 background_color, u8 text_color)
{
    return ((u16)background_color<<8)|(u16)text_color;
};

int vga_text_init(vga_text_console* handle);


#endif