.set MULTIBOOT2_HEADER_MAGIC,      0xE85250D6
.set MULTIBOOT2_ARCHITECTURE,      0 // 0=i386, 1=x86_64
.set MULTIBOOT2_HEADER_LENGTH,     multiboot2_header_end - multiboot2_header
.set MULTIBOOT2_HEADER_CHECKSUM,   -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCHITECTURE + MULTIBOOT2_HEADER_LENGTH)

.section .multiboot2
    .align 8
multiboot2_header:
    .long MULTIBOOT2_HEADER_MAGIC
    .long MULTIBOOT2_ARCHITECTURE
    .long MULTIBOOT2_HEADER_LENGTH
    .long MULTIBOOT2_HEADER_CHECKSUM

    .short 1                  // tag type: information request
    .short 0                  // reserved
    .long  8 + 4              // size of tag
    .long  6                  // mmap
    .align 8

    // framebuffer
    .short 5                 // type: framebuffer
    .short 0
    .long 20
    .long 1280               // width
    .long 720                // height
    .long 32                 // bpp
    .align 8, 0

    // end
    .short 0                 // tag type: end
    .short 0                 // reserved
    .long 8                  // size of tag
    .align 8
multiboot2_header_end:

// Kernel entry
.section .text
.global _start
.type _start, @function
.extern kmain

_start:
    mov $kernel_stack_end, %esp

    pushl %ebx         # mb ptr
    pushl %eax         # mb magic
    call kmain

    cli
loop:
    hlt
    jmp loop

.section .bss
    .align 16
kernel_stack:
    .space 2*1024*1024  // stack
kernel_stack_end:
