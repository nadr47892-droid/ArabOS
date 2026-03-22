ENTRY(_start)

SECTIONS
{
    . = 1M;

    /* يجب أن يكون multiboot2 header في أول 32KB من الملف */
    .multiboot2 :
    {
        KEEP(*(.multiboot2))
    }

    .text :
    {
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(COMMON)
        *(.bss*)
    }
}
