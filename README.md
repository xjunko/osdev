# rinOS - a monolithic kernel written in c
it's nothing much atm, just your average unfinished kernel

## work in progress
- [x] boots into the system
- [x] serial (com1) logging
- [x] global descriptor table
- [x] interrupt descriptor table
- [x] ps/2 inputs
- [x] basic memory allocator (malloc/free)
- [x] kernel printf (kprintf)
- [x] programmable interrupt timer (PIT) support
- [x] keyboard IRQ handler
- [x] PIC remapping and EOI
- [x] PCI device enumeration
- [ ] paging and virtual memory
- [ ] user mode support
- [ ] file system support
- [ ] multitasking
- [ ] basic shell
- [ ] process management
- [ ] system calls
- [ ] VGA text output improvements
- [ ] more device drivers
- [ ] ACPI support
- [ ] SMP/multicore support
- [ ] USB support
- [ ] networking stack
- [ ] ELF loading
- [ ] userland programs

## inspirations
[[ToaruOS]](https://github.com/klange/toaruos) [[osakaOS]](https://github.com/pac-ac/osakaOS) [[cavOS]](https://github.com/malwarepad/cavOS)

## license
[[LICENSE]](LICENSE)