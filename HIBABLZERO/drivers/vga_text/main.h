/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#include <hb0/types.h>




// bios.c
int iterate_vga_modes(u8* bvm, u8* char_cols);

// config.c

struct bios_video_mode_conf_t
{
    u8      bios_num;       // BIOS video mode number

    u8      text_rows;      // Text mode number of rows
    u8      text_cols;      // Text mode number of cols
    
    bool    monochrome;     // Is the mode monochrome
    bool    color;          // Does the BVM support color graphics

};

// The following is the order of the bvm from worst to best:

enum bios_video_mode_num
{
    T_GREY_SMALL = 0,
    T_COLOR_SMALL,
    T_MONOCHROME,
    T_GREY_BIG,
    T_COLOR_BIG,
};

extern const struct bios_video_mode_conf_t bvm_config[];    // Share the configuration locally
extern u8 bvm_config_size;  // Configuration size

// main.c

typedef struct vga_text_console_t
{
    volatile u16* _console_addr;
    struct bios_video_mode_conf_t _local_config;
    
    u8 _cursor_x;
    u8 _cursor_y;

    

    void (*puts)(u8* s);
    void (*newline)(struct vga_text_console_t);
    void (*backspace)(struct vga_text_console_t);
} vga_text_console;