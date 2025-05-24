// multiboot magic numbers
.set ALIGN,     1 << 0
.set MEMINFO,   1 << 1
.set FLAGS,     ALIGN | MEMINFO
.set MAGIC,     0x1BADB002
.set CHECKSUM,  -(MAGIC + FLAGS)

// multiboot
.section .multiboot
    .align 16, 0
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

    # aout kludge (unused)
    .long 0,0,0,0,0

    # Video mode
    .long   1       # Linear graphics please?
    .long   1024       # Preferred width
    .long   768      # Preferred height
    .long   32      # Preferred pixel depth

.section .text
.global _start
.type _start, @function
.extern kmain

_start:
    mov $kernel_stack, %esp
    
    /* parameters to kernel_entry, not required, but we still accept it. */
    push %eax
    push %ebx
    call kmain /* let's fucking go */
loop:
    cli
    hlt
    jmp loop

.section .bss
.space 2*1024*1024 # 2 MiB
kernel_stack:
