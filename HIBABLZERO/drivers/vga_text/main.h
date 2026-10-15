/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#ifndef LOCAL_VGA_TEXT
#define LOCAL_VGA_TEXT


#include <hb0/types.h>
#include <drivers/vga_text.h>
#include <hb0/libc.h>


// bios.c
int iterate_vga_modes(u8* bvm, u8* char_cols);

// config.c
extern const struct bios_video_mode_conf_t bvm_config[];    // Share the configuration locally
extern u8 bvm_config_size;  // Configuration size

// Simple helper function to load bvm configuration.

static inline void load_config(const struct bios_video_mode_conf_t* source, struct bios_video_mode_conf_t* destination)
{
    memcpy((void*)source, (void*)destination, sizeof(struct bios_video_mode_conf_t));
}

// The following is the order of the bvm from worst to best:
enum bios_video_mode_num
{
    T_GREY_SMALL = 0,
    T_COLOR_SMALL,
    T_MONOCHROME,
    T_GREY_BIG,
    T_COLOR_BIG,
};

void vga_puts(vga_text_console* handle, u8* s, u8 color);
void vga_backspace(vga_text_console* handle);
void vga_newline(vga_text_console* handle);
int vga_text_init(vga_text_console* handle);

#endif