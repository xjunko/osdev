#!/bin/bash
set -e # fuck off the moment shit doenst work

# build bootloader
toolchains/i686-elf/bin/as src/boot/loader.s -o build/boot.o 

# build kernel
toolchains/bin/i686-elf-gcc -c src/kernel/main.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# build os
toolchains/bin/i686-elf-gcc -T src/boot/linker.ld -o build/rin_os.bin -ffreestanding -O2 -nostdlib build/boot.o build/kernel.o -lgcc

# check if shits legit
if grub-file --is-x86-multiboot build/rin_os.bin; then
  echo [Build] Done, file is bootable.
else
  echo [Build] Done, file is NOT bootable.
fi