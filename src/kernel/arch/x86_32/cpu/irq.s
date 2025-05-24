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

HandleInterruptRequest 0x80

int_bottom:
    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax
    
    pushl %esp
    push (interruptnumber)
    call idt_handle_interrupt
    mov %eax, %esp

    popl %eax
    popl %ebx
    popl %ecx
    popl %edx

    popl %esi
    popl %edi
    popl %ebp


    iret

.data 
    interruptnumber: .byte 0