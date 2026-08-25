ENTRY(_start)
OUTPUT_FORMAT(binary)
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