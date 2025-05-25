// multiboot2 stuffs
.set MULTIBOOT2_HEADER_MAGIC,      0xE85250D6
.set MULTIBOOT2_ARCHITECTURE_I386, 0
.set MULTIBOOT2_HEADER_LENGTH,     multiboot2_header_end - multiboot2_header
.set MULTIBOOT2_HEADER_CHECKSUM,   -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCHITECTURE_I386 + MULTIBOOT2_HEADER_LENGTH)

.section .multiboot2
    .align 8
multiboot2_header:
    .long MULTIBOOT2_HEADER_MAGIC
    .long MULTIBOOT2_ARCHITECTURE_I386
    .long MULTIBOOT2_HEADER_LENGTH
    .long MULTIBOOT2_HEADER_CHECKSUM

    // Tag: information request (optional, ask for memory map)
    .short 1                  // tag type: information request
    .short 0                  // reserved
    .long  8 + 4              // size of tag
    .long  6                  // mmap
    .align 8

    // framebuffer
    .short 5                 // type: framebuffer
    .short 0
    .long 20
    .long 1024               // width
    .long 768                // height
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
    mov $kernel_stack, %esp

    // Multiboot2 passes boot info in %ebx
    pushl %ebx         # Multiboot info pointer
    pushl %eax         # Multiboot magic
    call kmain

.hang:
    cli
    hlt
    jmp .hang

// Reserve stack
.section .bss
    .align 16
    .space 8*1024*1024   // 2 MiB stack
kernel_stack:
