ARCH = i686
OS   = fullmoon
CC = ~/.cross/$(ARCH)-$(OS)/bin/$(ARCH)-$(OS)-gcc
AS = ~/.cross/$(ARCH)-$(OS)/bin/$(ARCH)-$(OS)-as
LD = ~/.cross/$(ARCH)-$(OS)/bin/$(ARCH)-$(OS)-gcc

C_FLAGS = -ffreestanding        \
				-g -c           \
				-nostdlib       \
				-fno-exceptions \
				-Iinclude/ -Ilibs/

AS_FLAGS = 
LD_FLAGS = -nostdlib -ffreestanding -static \
    -Lsysroot/usr/lib \
    -lc -lm -lgcc --specs=sysroot/usr/lib/nosys.specs

SRC_DIR  := src
LIB_DIR  := libs
OBJ_DIR  := obj
BOOT_DIR := boot

ASM_SOURCES := $(shell find $(SRC_DIR) -name '*.s')
C_SOURCES   := $(shell find $(SRC_DIR) -name '*.c')
C_LIBS      := $(shell find $(LIB_DIR) -name '*.c')
SOURCES     := $(C_SOURCES) $(ASM_SOURCES) $(C_LIBS)

ASM_OBJECTS   := $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
C_OBJECTS     := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
C_LIB_OBJECTS := $(patsubst $(LIB_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_LIBS))
OBJECTS       := $(C_OBJECTS) $(C_LIB_OBJECTS) $(ASM_OBJECTS) 

OS_FOLDER      := build
STORAGE_FORMAT := raw
STORAGE_SIZE   := 64M


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(C_FLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(C_FLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	mkdir -p $(@D)
	$(AS) $(AS_FLAGS) -o $@ $<

build/kernel.bin: src/linker/link.ld $(OBJECTS)
	mkdir -p build/
	$(LD) -T $< -o $@ ${OBJECTS} $(LD_FLAGS)

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
	mkfs.fat -F 32 $@

build/slave.img:
	qemu-img create -f $(STORAGE_FORMAT) $@ $(STORAGE_SIZE)
	mkfs.fat -F 32 $@

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
