ENTRY(_start)
SECTIONS
{
    . = 0x7c00;
    .text:
    {
        *(.text)
    }

    .bss:
    {
        *(.bss)
    }

    .data:
    {
        *(.data)
    }
    
    .rodata:
    {
        *(.rodata)
    }
}