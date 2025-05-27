# rinOS - a monolithic kernel written in C

It's nothing much atm, just your average unfinished kernel.

## Features

- Boots into the system via Multiboot2 (GRUB)
- Serial (COM1) logging ([serial.c](src/kernel/arch/x86_32/driver/serial.c))
- Global Descriptor Table (GDT) initialization ([gdt.h](include/kernel/gdt.h))
- Interrupt Descriptor Table (IDT) initialization and activation ([idt.h](include/kernel/idt.h))
- Interrupt handler registration
- PS/2 inputs (keyboard and mouse) with callback support ([ps2hid.h](include/kernel/ps2hid.h))
- Basic memory allocator (malloc/free) ([kmalloc.c](src/libc/stdlib/kmalloc.c))
- Kernel printf (kprintf) ([printf.c](src/libc/stdio/printf.c))
- Programmable Interrupt Timer (PIT) support ([pit.h](include/kernel/pit.h))
- Timer sleep (pit_sleep)
- Keyboard IRQ handler
- PIC remapping and EOI ([pic.h](include/kernel/pic.h))
- PCI device enumeration ([pci.h](include/kernel/pci.h))
- VGA graphics mode (320x200x256) ([vga.c](src/kernel/video/vga.c))
- Framebuffer graphics mode (linear framebuffer via GRUB) ([framebuffer.c](src/kernel/video/framebuffer.c))
- ATA PIO disk driver ([ata.c](src/kernel/fs/ata.c))
- Multiboot2 tag parsing and callback system ([multiboot.h](include/kernel/multiboot.h))
- System call infrastructure ([syscall.h](include/kernel/syscall.h))
- Modular device initialization (PCI, PS/2, ATA, etc.)

## Work in Progress

- [~] Paging and virtual memory
- [~] User mode support
- [~] File system support
- [~] Multitasking
- [~] Basic shell
- [~] Process management
- [~] System calls (infrastructure present, not many syscalls implemented)
- [~] VGA text output improvements
- [~] More device drivers (PCI, PS/2, serial, VGA implemented)
- [ ] ACPI support
- [ ] SMP/multicore support
- [ ] USB support
- [ ] Networking stack
- [ ] ELF loading
- [ ] Userland programs

## Building & Running

- Cross-compiler required: i686-elf-gcc
- Build with `make`
- Run with `make run` (uses QEMU, see [Makefile](Makefile))

## Inspirations

- [ToaruOS](https://github.com/klange/toaruos)
- [osakaOS](https://github.com/pac-ac/osakaOS)
- [cavOS](https://github.com/malwarepad/cavOS)

## License

See [LICENSE](LICENSE)