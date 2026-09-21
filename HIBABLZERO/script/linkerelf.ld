ENTRY(_start)
SECTIONS
{
    . = 0x8000;
    .startup :
    {
        *(.startup*)
    }

    .text :
    {
        *(.text*)
    }

    .bss :
    {
        *(.bss*)
    }

    .data :
    {
        *(.data*)
    }
    
    .rodata :
    {
        *(.rodata*)
    }
}