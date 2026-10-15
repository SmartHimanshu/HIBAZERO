/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#include "main.h"
#include <hb0/err.h>
#include <hb0/libc.h>

// Helper function to get character at a point.

static inline u8 character_at(vga_text_console* handle, u8 cursor_x, u8 cursor_y)
{
    return (u8)(handle->_console_addr[cursor_x+(cursor_y*handle->_local_config.text_cols)]&0xFF);
}

// Helper function to scroll over the screen 

static inline void scroll(vga_text_console* handle, u8 scroll)
{
    volatile u16* addr = handle->_console_addr;
    u8 cols = handle->_local_config.text_cols;
    u8 rows = handle->_local_config.text_rows;
    while(scroll--)
    {
        memcpy((void*)(addr+cols), (void*)addr, sizeof(u16)*cols*(rows-1));
        handle->_cursor_y--;
    }
    for(u8 i = 0; i < cols; i++)
    {
        addr[(rows-1)*cols + i] = (u16)' ';
    }
}

// Helper function to advance the cursor

static inline void advance_ptr(vga_text_console* handle)
{
    handle->_cursor_x++;
    if(handle->_cursor_x==handle->_local_config.text_cols) 
    {
        handle->_cursor_y++;
        handle->_cursor_x = 0;
    }
}

// Helper function to retract the cursor

static inline void preceed_ptr(vga_text_console* handle)
{
    u8 cols = handle->_local_config.text_cols;
    if(handle->_cursor_x==0)
    {
        handle->_cursor_y--;
        for(u8 i = cols-1; i > 0; i--)
            if(character_at(handle, i, handle->_cursor_y)!=' ')
            {
                handle->_cursor_x = i;
            }
    }
}

// A non monochrome vga putc function.

static inline void vga_putc(vga_text_console* handle, u8 c, u8 color)
{
    u8 cursor_x = handle->_cursor_x;
    
    volatile u16* addr = handle->_console_addr;
    u8 cols = handle->_local_config.text_cols;
    u8 rows = handle->_local_config.text_rows;

    if(handle->_cursor_y >= rows) 
    {
        scroll(handle, handle->_cursor_y - rows + 1);
    }
    u8 cursor_y = handle->_cursor_y;
    addr[(cursor_y*cols) + cursor_x] = (((u16)color<<8)|(u16)c);
    
    advance_ptr(handle);
}

// A non monochrome vga puts function.

void vga_puts(vga_text_console* handle, u8* s, u8 color)
{
    while(*s)
    {
        vga_putc(handle, *s, color);
        s++;
    }
}

// Helper function to perform a backspace operation on vga console

void vga_backspace(vga_text_console* handle)
{
    u8 cursor_x = handle->_cursor_x;
    u8 cursor_y = handle->_cursor_y;
    volatile u16* addr = handle->_console_addr;
    u8 cols = handle->_local_config.text_cols;
    
    if(!cursor_x && !cursor_y) return;

    preceed_ptr(handle);

    addr[(cursor_y*cols) + cursor_x] = ((u16)' '<<8);
}

// Helper function to perform a newline operation on vga console

void vga_newline(vga_text_console* handle)
{
    u8 cursor_y = handle->_cursor_y;
    u8 rows = handle->_local_config.text_rows;

    handle->_cursor_x = 0;
    handle->_cursor_y++;

    if(cursor_y >= rows) 
    {
        scroll(handle, cursor_y - rows + 1);
        handle->_cursor_y -= cursor_y - rows + 1;
    }
}

/*
Selects the best vga mode.
*/

int vga_text_init(vga_text_console* handle)
{
    u8 best_bvm = 0; // Set it to default 0 as the worst case scenario.
    u8 system_cols = 40; // Default lowest possible text columns. 
    
    int status = iterate_vga_modes(&best_bvm, &system_cols);

    // Highly unlikely but it would just mean stack failure.
    if(status) return -FAILED;

    // Load the selected configuration into the handle.
    load_config(&bvm_config[best_bvm], &handle->_local_config);

    // Set the number of columns given by the bios.
    handle->_local_config.text_cols = system_cols;

    handle->_cursor_x = 0;
    handle->_cursor_y = 0;

    bool monochrome = handle->_local_config.monochrome;

    handle->puts = vga_puts;
    handle->backspace = vga_backspace;
    handle->newline = vga_newline;

    handle->_console_addr = (volatile u16*) (monochrome ? 0xb0000 : 0xb8000);
    return SUCCESS;
}