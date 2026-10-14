/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#include <hb0/types.h>

struct bios_video_mode_conf_t
{
    u8      bios_num;       // BIOS video mode number

    u8      text_rows;      // Text mode number of rows
    u8      text_cols;      // Text mode number of cols
    
    bool    monochrome;     // Is the mode monochrome

};

enum bios_video_mode_num
{
    T_GREY_SMALL = 0,
    T_COLOR_SMALL,
    T_GREY_BIG,
    T_COLOR_BIG,
    T_MONOCHROME,
};

extern const struct bios_video_mode_conf_t BVM_config[];