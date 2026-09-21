ENTRY(_start)
SECTIONS
{
    . = 0x6000;
    .text :
    {
        *(.text)
    }

    .bss :
    {
        *(.bss)
    }

    .data :
    {
        *(.data)
    }

    .rodata :
    {
        *(.rodata)
    }
}