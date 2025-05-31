# rinux - a monolithic kernel written in c
It's nothing much atm, just your average unfinished kernel.

## preview
![image](https://github.com/user-attachments/assets/6932ecb6-fc45-4c1d-a512-51dbab5564b6)

it aint much but its honest work

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
- [x] vfs
- [x] devfs
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
