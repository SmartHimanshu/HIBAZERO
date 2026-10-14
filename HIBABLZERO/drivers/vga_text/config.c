/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#include "main.h"

/*
IMPORTANT NOTE: BVM configuration is to be strictly taken as a reference only.
Generally, with emulators we can say that 00h BVM would guarantee that we will
have 40 columns. However, with real world firmware, variations and buggy BIOSes
can exist. Therefore, we are going to take BVM config as strictly as a reference.
*/

const struct bios_video_mode_conf_t BVM_config[] = 
{
    [T_GREY_SMALL] = 
    {
        .bios_num=0x00,
        .text_rows=25,
        .text_cols=40,
        .monochrome=false    
    },

    [T_COLOR_SMALL] = 
    {
        .bios_num=0x00,
        .text_rows=25,
        .text_cols=40,
        .monochrome=false    
    },

    [T_GREY_BIG] = 
    {
        .bios_num=0x00,
        .text_rows=25,
        .text_cols=80,
        .monochrome=false    
    },

    [T_COLOR_BIG] = 
    {
        .bios_num=0x00,
        .text_rows=25,
        .text_cols=80,
        .monochrome=false    
    },

    [T_MONOCHROME] = 
    {
        .bios_num=0x00,
        .text_rows=25,
        .text_cols=80,
        .monochrome=true    
    },
};