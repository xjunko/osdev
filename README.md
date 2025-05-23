# rinOS - a monolithic kernel written in c
it's nothing much atm, just your average unfinished kernel

## work in progress
- [x] boots into the system
- [x] serial (com1) logging
- [x] global descriptor table (gdt) initialization
- [x] interrupt descriptor table (idt) initialization and activation
- [x] interrupt handler registration
- [x] ps/2 inputs (keyboard and mouse)
- [x] basic memory allocator (malloc/free)
- [x] kernel printf (kprintf)
- [x] programmable interrupt timer (pit) support
- [x] timer sleep (pit_sleep)
- [x] keyboard irq handler
- [x] pic remapping and eoi
- [x] pci device enumeration
- [x] vga graphics mode (320x200x256)
- [~] ps2 devices with callback
- [ ] paging and virtual memory
- [ ] user mode support
- [ ] file system support
- [ ] multitasking
- [ ] basic shell
- [ ] process management
- [ ] system calls
- [ ] vga text output improvements
- [~] more device drivers (pci, ps/2, serial, vga implemented)
- [ ] acpi support
- [ ] smp/multicore support
- [ ] usb support
- [ ] networking stack
- [ ] elf loading
- [ ] userland programs

## inspirations
[[ToaruOS]](https://github.com/klange/toaruos) [[osakaOS]](https://github.com/pac-ac/osakaOS) [[cavOS]](https://github.com/malwarepad/cavOS)

## license
[[LICENSE]](LICENSE)