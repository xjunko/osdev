/* constants */
.set ALIGN,     1 << 0
.set MEMINFO,   1 << 1
.set FLAGS,     ALIGN | MEMINFO /* couldn't i just do it here without declaring? */
.set MAGIC,     0x1BADB002
.set CHECKSUM,  -(MAGIC + FLAGS)

/* header */
.section .multiboot
    .align 4 /* long is 4 bytes (at least that's what Miku said) */
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

/* stack for us to run c on, I assumed. */
.section .bss
    .align 16
    
    stack_bottom:
        .skip 16384 /* 16kb */
    stack_top:
        

.section .text 
.global _start
.type _start, @function

_start:
    mov $stack_top, %esp /* if I'm not wrong, this is where we move the "top" part of the stack to somewhere along the line of [address of stack_bottom - 16kb]
                           because memory grows downward in 32-bit. I'm not sure if it's legit though... */

    /* enter the c world */
    call kernel_main

    /* infinite loop if our kernel ever dies. */
    cli
1:  hlt
    jmp 1b

.size _start, . - _start
