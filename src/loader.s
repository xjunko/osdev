/* constants */
.set ALIGN,     1 << 0
.set MEMINFO,   1 << 1
.set FLAGS,     ALIGN | MEMINFO
.set MAGIC,     0x1BADB002
.set CHECKSUM,  -(MAGIC + FLAGS)

/* header */
.section .multiboot
    .align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM


.section .text
.extern kernel_entry
.extern call_constructors
.global loader

loader:
    mov $kernel_stack, %esp

    /* because we're using cpp, so had to call this first. */
    call call_constructors
    
    /* parameters to kernel_entry, not required, but we still accept it. */
    push %eax
    push %ebx
    call kernel_entry /* let's fucking go */

_stop:
    cli
    hlt
    jmp _stop

.section .bss
.space 2*1024*1024 # 2 MiB
kernel_stack:
