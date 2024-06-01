GPP_ARGS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-threadsafe-statics -fno-leading-underscore -Wno-write-strings \
			-fno-stack-protector -Wall -Wextra # this might fuck me up later on
AS_ARGS = --32
LD_ARGS = -melf_i386

objects = obj/loader.o \
		  obj/hardware/port.o \
		  obj/hardware/interrupts.o \
		  obj/hardware/stubs/interrupts.o \
		  obj/hardware/input/keyboard.o \
		  obj/hardware/input/mouse.o \
		  obj/memory/gdt.o \
		  obj/terminal/term.o \
		  obj/commons/utility.o \
		  obj/kernel.o \
		  obj/entry.o

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	g++ $(GPP_ARGS) -o $@ -c $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(AS_ARGS) -o $@ $<

kernel.bin: linker.ld $(objects)
	ld $(LD_ARGS) -T $< -o $@ ${objects}

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
		-cpu 486 -smp 1 -m 8M -vga virtio


run_vb: kernel.iso
	(killall VirtualBoxVM && sleep 1) || true
	VirtualBoxVM --startvm "RinOS" &



.PHONY: clean
clean:
	rm -rf obj kernel.bin kernel.iso
