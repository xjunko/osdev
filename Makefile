C_FLAGS = -m32 -g -c -ffreestanding -Wall -Werror -fcommon -Iinclude/ \
	      -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -fno-stack-protector \
		  -fno-stack-check -fno-lto -fno-rtti -fno-exceptions\
		  -nostartfiles -nostdlib -fno-builtin \
		  -DPRINTF_INCLUDE_CONFIG_H=1 -DPRINTF_ALIAS_STANDARD_FUNCTION_NAMES_SOFT=1 \
		  -Wno-error=int-to-pointer-cast -Wno-error=write-strings \
		  -fno-use-cxa-atexit -fno-threadsafe-statics

AS_ARGS = --32
LD_ARGS = -melf_i386

SRC_DIR  := src
OBJ_DIR  := obj
BOOT_DIR := boot

ASM_SOURCES := $(shell find $(SRC_DIR) -name '*.s')
CPP_SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
C_SOURCES   := $(shell find $(SRC_DIR) -name '*.c')
SOURCES     := $(CPP_SOURCES) $(C_SOURCES) $(ASM_SOURCES)

ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
CPP_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_SOURCES))
C_OBJECTS   := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
OBJECTS     := $(CPP_OBJECTS) $(C_OBJECTS) $(ASM_OBJECTS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	g++ $(C_FLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	g++ $(C_FLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	mkdir -p $(@D)
	as $(AS_ARGS) -o $@ $<

kernel.bin: linker.ld $(OBJECTS)
	echo $(OBJECTS)
	ld $(LD_ARGS) -T $< -o $@ ${OBJECTS}

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
        -serial chardev:char0





.PHONY: clean
clean:
	rm -rf obj kernel.bin kernel.iso
