/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#include <hb0/err.h>
#include <machine/int.h>
#include <hb0/types.h>
#include "main.h"

/*
BIOS CALL. INT 0x10 AH = 0x00
AL = VIDEO_MODE.
this function sets the video mode and does
not have any standard failure indicator.
*/

static void set_vga_mode(u8 video_mode)
{
    struct bios_regs regs = {0};
    regs.eax = (u64)video_mode;
    
    bios_interrupt(0x10, &regs);
}

/*
BIOS CALL INT 0x10 AH = 0x0F
NO ARGUMENTS.
this function will try to get the currect set vga mode
from the bios. And return the video mode and number of 
colunns. Does not have a failure indicator.
*/

static void get_vga_mode(u8* video_mode, u8* char_cols)
{
    struct bios_regs regs = {0};
    regs.eax = 0x0F00;
    
    bios_interrupt(0x10, &regs);

    *video_mode = (u8)(regs.eax & 0xFF);
    *char_cols = (u8)((regs.eax & 0xFF00)>>8);
}

/*
Simple helper function. Takes a vga mode, and then 
tries to set it. If BIOS successfully sets the mode
then it returns SUCCESS along with num columns.
*/
static inline int check_vga_mode(u8 video_mode, u8* char_cols)
{
    if(char_cols==NULL) return -ERRINV;

    set_vga_mode(video_mode);  // Set bvm

    // we now check if it got set.
    u8 set_bvm;
    u8 set_char_cols;

    get_vga_mode(&set_bvm, &set_char_cols);

    if(set_bvm!=video_mode) return -ERRNS;  //Check whether it got set.

    *char_cols = set_char_cols; //Successfully set the vga mode.
    return SUCCESS;
}

/*
This function iterates over all the vga modes in bvm configuration.
Then it checks whether the BIOS will accept each bvm number.
Chooses the best vga mode. The order of the modes is in main.h
*/

int iterate_vga_modes(u8* bvm, u8* char_cols)
{
    if ((bvm==NULL)||(char_cols==NULL)) return -ERRINV;
    for(s8 i = bvm_config_size; i >= 0; i--)
    {
        u8 _char_cols;
        
        int failed = check_vga_mode(bvm_config[i].bios_num, &_char_cols);
        if(failed) continue;

        *char_cols = _char_cols;
        *bvm = (u8)i;
        
        break;
    }
    return SUCCESS;
}