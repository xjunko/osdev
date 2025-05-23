CC = /home/junko/Projects/cross-compiler/gcc-i686/bin/i686-elf-gcc
AS = /home/junko/Projects/cross-compiler/gcc-i686/bin/i686-elf-as
LD = /home/junko/Projects/cross-compiler/gcc-i686/bin/i686-elf-ld

C_FLAGS = -m32 -ffreestanding \
	      -fno-stack-protector \
		  -Wall -Wextra -Werror -Wno-error=unused-parameter -Wno-error=unused-variable \
		  -g -c -fcommon -mno-mmx -mno-sse -mno-red-zone \
		  -fno-lto -fno-exceptions -nostartfiles -nostdlib -fno-builtin
		  
C_FLAGS += -Iinclude/

AS_FLAGS = --32
LD_FLAGS = -melf_i386 -O2

SRC_DIR  := src
OBJ_DIR  := obj
BOOT_DIR := boot

ASM_SOURCES := $(shell find $(SRC_DIR) -name '*.s')
C_SOURCES   := $(shell find $(SRC_DIR) -name '*.c')
SOURCES     := $(C_SOURCES) $(ASM_SOURCES)

ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
C_OBJECTS   := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
OBJECTS     := $(C_OBJECTS) $(ASM_OBJECTS)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(C_FLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	mkdir -p $(@D)
	$(AS) $(AS_FLAGS) -o $@ $<

kernel.bin: src/linker/link.ld $(OBJECTS)
	echo $(OBJECTS)
	$(LD) $(LD_FLAGS) -T $< -o $@ ${OBJECTS}

install: kernel.bin
	sudo cp $< /boot/kernel.bin

kernel.iso: kernel.bin
	mkdir -p iso/
	mkdir -p iso/boot/
	mkdir -p iso/boot/grub/
	cp $< iso/boot/
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'set timeout=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "RinOS" {' >> iso/boot/grub/grub.cfg
	echo ' multiboot /boot/kernel.bin' >> iso/boot/grub/grub.cfg
	echo ' boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso/
	rm -rf iso
	rm -rf obj

	qemu-img create -f qcow2 boot.img 512M
	dd if=kernel.iso of=boot.img

	rm kernel.iso

run: kernel.iso
	$(info Running the kernel...)
	qemu-system-i386 \
		-boot menu=on \
		-drive id=disk,file=boot.img,format=raw,if=none \
		-device piix4-ide,id=piix4 \
		-device ide-hd,drive=disk,bus=piix4.0 \
		-device e1000 \
		-device ich9-ahci \
		-device usb-ehci \
		-device virtio-balloon-pci \
		-cpu 486 -smp 1 -m 8M -vga virtio \
		-chardev stdio,id=char0,logfile=system.log,signal=off \
        -serial chardev:char0 \
		-no-reboot -no-shutdown



.PHONY: clean
clean:
	rm -rf obj kernel.bin kernel.iso
