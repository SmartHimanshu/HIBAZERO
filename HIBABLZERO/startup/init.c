#include <startup/init.h>
#include <hb0/types.h>

void NORETURN kern_startup(void)
{
    u16* vga_mem = (u16*)0xb8000;
    vga_mem[1] = 0x0F41;
    while(1) { ; };
};