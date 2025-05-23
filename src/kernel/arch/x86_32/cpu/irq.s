.set IRQ_BASE, 0x20

.section .text
.extern idt_handle_interrupt

.macro HandleException num
.global handle_interrupt_exception_\num
handle_interrupt_exception_\num:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global handle_interrupt_request_\num
handle_interrupt_request_\num:
    movb $\num + IRQ_BASE, (interruptnumber)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C

int_bottom:
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    pushl %esp
    push (interruptnumber)
    call idt_handle_interrupt
    movl %eax, %esp

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

    iret

.data 
    interruptnumber: .byte 0