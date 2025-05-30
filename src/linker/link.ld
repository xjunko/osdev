ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
    . = 1M;

    .text BLOCK(4K) : ALIGN(4K) 
    {
        *(.multiboot2)
        *(.text*)
    }

    .rodata BLOCK(4K) : ALIGN(4K) 
    {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K) 
    {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        _bss_start = .;
        *(.bss)
        *(COMMON)
        _bss_end = .;
    }

    . = ALIGN(0x1000);
    __heap_start = .;

    .heap (NOLOAD) :
    {
        . = . + 8M; 
    }

    __heap_end = .;


    _end = .;
}