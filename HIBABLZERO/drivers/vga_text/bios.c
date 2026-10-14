#include "hb0/err.h"
#include <machine/int.h>
#include <hb0/types.h>

int set_vga_mode(u8 video_mode)
{
    struct bios_regs regs = {0};
    regs.eax = (u64)video_mode;
    
    bios_interrupt(0x10, &regs);

    if (regs.flags & CF_FLAG)
    {
        return -ERRNS;
    }

    return SUCCESS;
}

int get_vga_mode(u8* video_mode, u8* char_cols)
{
    struct bios_regs regs = {0};
    regs.eax = 0x0F00;
    
    bios_interrupt(0x10, &regs);

    if (regs.flags & CF_FLAG)
    {
        return -FAILED;
    }

    *video_mode = (u8)(regs.eax & 0xFF);
    *char_cols = (u8)((regs.eax & 0xFF00)>>8);

    return SUCCESS;
}