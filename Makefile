ARCH = i686
OS   = fullmoon
CC = ~/.cross/$(ARCH)-$(OS)/bin/$(ARCH)-$(OS)-gcc
AS = ~/.cross/$(ARCH)-$(OS)/bin/$(ARCH)-$(OS)-as
LD = ~/.cross/$(ARCH)-$(OS)/bin/$(ARCH)-$(OS)-ld

C_FLAGS = -m32 -ffreestanding \
	      -fno-stack-protector \
		  -Wall -Wextra -Werror -Wno-error=unused-parameter -Wno-error=unused-variable -Wno-error=address-of-packed-member \
		                        -Wno-error=unused-const-variable -Wno-error=int-to-pointer-cast -Wno-error=implicit-fallthrough= -Wno-error=expansion-to-defined\
								-Wno-error=type-limits -Wno-error=unused-but-set-parameter -Wno-error=enum-conversion -Wno-error=missing-field-initializers\
		  -g -c -fcommon -mno-mmx -mno-sse -mno-red-zone -Wno-error=sign-compare -Wno-error=char-subscripts -Wno-error=unused-but-set-variable\
		  -fno-lto -fno-exceptions -nostartfiles -nostdlib -fno-builtin
		  
C_FLAGS += -Iinclude/ -Isysroot/usr/include

AS_FLAGS = --32 
LD_FLAGS = -melf_i386 -nostdlib
LD_FLAGS += -Lsysroot/usr/lib

SRC_DIR  := src
OBJ_DIR  := obj
BOOT_DIR := boot

ASM_SOURCES := $(shell find $(SRC_DIR) -name '*.s')
C_SOURCES   := $(shell find $(SRC_DIR) -name '*.c')
SOURCES     := $(C_SOURCES) $(ASM_SOURCES)

ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
C_OBJECTS   := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
OBJECTS     := $(C_OBJECTS) $(ASM_OBJECTS)

OS_FOLDER      := build
STORAGE_FORMAT := raw
STORAGE_SIZE   := 8M


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(C_FLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	mkdir -p $(@D)
	$(AS) $(AS_FLAGS) -o $@ $<

build/kernel.bin: src/linker/link.ld $(OBJECTS)
	mkdir -p build/
	echo $(LD) $(LD_FLAGS) -T $< -o $@ ${OBJECTS}
	$(LD) $(LD_FLAGS) -T $< -o $@ ${OBJECTS}

install: build/kernel.bin
	sudo cp $< /boot/kernel.bin

build/kernel.iso: build/kernel.bin
	mkdir -p iso/
	mkdir -p iso/boot/
	mkdir -p iso/boot/grub/
	cp $< iso/boot/
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'set timeout=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "RinOS" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot2 /boot/kernel.bin' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso/

	rm -rf iso
	rm -rf obj

	rm build/kernel.bin

build/master.img:
	qemu-img create -f $(STORAGE_FORMAT) $@ $(STORAGE_SIZE)

build/slave.img:
	qemu-img create -f $(STORAGE_FORMAT) $@ $(STORAGE_SIZE)

run: build/kernel.iso build/master.img build/slave.img
	$(info Running the kernel...)
	qemu-system-i386 \
		-drive file=build/master.img,if=none,id=hd0,index=0 \
		-drive file=build/slave.img,if=none,id=hd1,index=1 \
		-drive file=build/kernel.iso,if=none,id=bt0,index=2 \
		-device ide-hd,drive=hd0,bus=ide.0,unit=0 \
 		-device ide-hd,drive=hd1,bus=ide.0,unit=1 \
		-device ide-cd,drive=bt0,bus=ide.1 \
		-boot d \
		-cpu pentium3 \
		-smp 1 \
		-m 256M \
		-vga virtio \
		-serial stdio \
		-enable-kvm \
		-no-reboot -no-shutdown



.PHONY: clean
clean:
	rm -rf obj kernel.bin build/kernel.iso
