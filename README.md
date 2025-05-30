# rinux - a monolithic kernel written in c
It's nothing much atm, just your average unfinished kernel.

## kernel
### internal
- [x] boots
- [x] framebuffer
- [x] gdt
- [x] idt

### unix
- [x] syscalls
- [x] libc (newlib)

### driver
- [x] io
- [x] serial
- [x] ps/2 mouse+keyboard 
- [ ] ahci
- [ ] network
- [ ] speaker
- [ ] some kind of gpu accel?

### filesystem
- [x] mikufs (stub impl, will be removed once i get vfs working)
- [ ] vfs
- [ ] devfs
- [ ] fat
- [ ] ext2
- [ ] ext4

## userland
### ports
- [ ] bash
- [ ] coreutils
- [ ] doomgeneric

## build & run

- cross-compiler: i686-elf-gcc
- Build with `make`
- Run with `make run` (uses QEMU, see [Makefile](Makefile))

## inspo

- [ToaruOS](https://github.com/klange/toaruos)
- [osakaOS](https://github.com/pac-ac/osakaOS)
- [cavOS](https://github.com/malwarepad/cavOS)

## license

See [LICENSE](LICENSE)