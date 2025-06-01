.section .text
.align 4

.macro ISR_NOERR n
    .global _isr\n
    _isr\n:
        cli
        push $0
        push $\n
        jmp isr_common
.endm

.macro ISR_ERR n
    .global _isr\n
    _isr\n:
        cli
        push $\n
        jmp isr_common
.endm

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_NOERR 10


.extern isr_handler
.type isr_handler, @function

isr_common:
    pusha // push all

    // save registers
    push %ds
    push %es
    push %fs
    push %gs
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    cld

    // c for handler
    push %esp
    call isr_handler
    add $4, %esp

    // restore registers
    pop %gs
    pop %fs
    pop %es
    pop %ds

    popa // pop all
    add $8, %esp // pop error code and return address
    iret // pop cs, eip, eflags, ss and esp