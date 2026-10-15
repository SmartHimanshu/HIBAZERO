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

int vga_text_init(vga_text_console* handle);


#endif