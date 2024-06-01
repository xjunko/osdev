.set IRQ_BASE, 0x20

.section .text
.extern _ZN16InterruptManager16handle_interruptEhj

.macro HandleException num
.global _ZN16InterruptManager16handle_exception_\num\()Ev
_ZN16InterruptManager16handle_exception_\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN16InterruptManager29handle_interrupt_request_\num\()Ev
_ZN16InterruptManager29handle_interrupt_request_\num\()Ev:
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
    call _ZN16InterruptManager16handle_interruptEhj
    movl %eax, %esp

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

    iret

.global _ZN16InterruptManager24ignore_interrupt_requestEv
_ZN16InterruptManager24ignore_interrupt_requestEv:
    iret

.data 
    interruptnumber: .byte 0