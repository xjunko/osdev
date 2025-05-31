ARCH      = i686
OS        = fullmoon
TOOLCHAIN = ~/.cross/$(ARCH)-$(OS)/bin/

CC    = $(TOOLCHAIN)$(ARCH)-$(OS)-gcc
AS    = $(TOOLCHAIN)$(ARCH)-$(OS)-as
LD    = $(TOOLCHAIN)$(ARCH)-$(OS)-gcc
STRIP = $(TOOLCHAIN)$(ARCH)-$(OS)-strip

EMU       = qemu-system-i386
EMU_ARGS  = -cpu pentium3 -smp 1 -m 256M -vga virtio 
EMU_ARGS += -serial stdio -enable-kvm -no-reboot -no-shutdown

BASE = sysroot/usr

# for kernel and modules
KERNEL_CFLAGS  = -ffreestanding -O2 -g -static
KERNEL_CFLAGS += -Wall -Wextra -Wno-unused-function -Wno-unused-variable 
KERNEL_CFLAGS += -pedantic -Wwrite-strings

# constants
KERNEL_CFLAGS  += -D_KERNEL_ -DKERNEL_ARCH=$(ARCH) 

# kernel sources
KERNEL_OBJS  = $(patsubst %.c,%.o,$(wildcard kernel/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/*/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/*/*/*.c))

# assembly sources
KERNEL_ASMOBJS  = $(patsubst %.S,%.o,$(wildcard kernel/*/*/*.S))

# libs sources
KERNEL_LIBOBJS  = $(patsubst %.c,%.o,$(wildcard libs/*.c))
KERNEL_LIBOBJS += $(patsubst %.c,%.o,$(wildcard libs/*/*.c))
KERNEL_LIBOBJS += $(patsubst %.c,%.o,$(wildcard libs/*/*/*.c))

CRTS = $(BASE)/lib/crt0.o
LIBC = $(BASE)/lib/libc.a $(BASE)/lib/libg.a $(BASE)/lib/libm.a 

kernel/%.o: kernel/%.c
	$(CC) $(KERNEL_CFLAGS) -nostdlib -g -Iinclude -Ilibs -o $@ -c $<

kernel/%.o: libs/%.c
	$(CC) $(KERNEL_CFLAGS) -nostdlib -g -Iinclude -Ilibs -o $@ -c $<

kernel/%.o: kernel/%.S
	$(AS) -o $@ $<

OBJS = $(KERNEL_OBJS) $(KERNEL_ASMOBJS) $(KERNEL_LIBOBJS) kernel/boot/boot.o

build/boot/kernel.bin: linker/link.ld ${OBJS} ${LIBC}
	$(CC) -T $< ${KERNEL_CFLAGS} -nostdlib -static -Wl,-z,max-page=0x1000 -o $@ ${OBJS} ${LIBC}
	$(STRIP) $@

image.iso: build/boot/kernel.bin
	grub-mkrescue --output=$@ build/

.PHONY: run clean
run: image.iso
	${EMU} ${EMU_ARGS} -cdrom $< 

clean:
	-rm -f ${KERNEL_ASMOBJS}
	-rm -f ${KERNEL_OBJS}