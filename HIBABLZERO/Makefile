# First and foremost, we compile the boot sector.
CC := i686-elf-gcc
LD := i686-elf-ld
ASFLAGS := -g -ffreestanding -nostdlib -nostdinc -Iinclude
all: build/disk.img

build/boot.bin: boot/boot.S
	$(CC) $(ASFLAGS) -Tboot/bootlinker.ld -o $@ $<

build/diskboot.bin: boot/diskboot.S
	$(CC) $(ASFLAGS) -Tboot/diskbootlinker.ld -o $@ $<

build/disk.img: build/boot.bin build/diskboot.bin
	truncate -s 32M $@
	dd if=build/boot.bin of=build/disk.img bs=512 count=1 seek=0
	dd if=build/diskboot.bin of=build/disk.img bs=512 count=10 seek=1