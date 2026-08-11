# First and foremost, we compile the boot sector.
CC := i686-elf-gcc
LD := i686-elf-ld
ASFLAGS := -g -ffreestanding -nostdlib -nostdinc -m32 -Wall -Iinclude

OBJS := bin/startup_boot.S.o

all: build/disk.img

build/core.bin: $(OBJS)
	$(CC) $(ASFLAGS) -Tlinker.ld -o $@ $<
	$(CC) $(ASFLAGS) -Tlinkerelf.ld -o build/coreelf.o $<

build/boot.bin: boot/boot.S
	$(CC) $(ASFLAGS) -Tboot/bootlinker.ld -o $@ $<
	$(CC) $(ASFLAGS) -Tboot/bootlinkerelf.ld -o build/boot.o -c $<

build/diskboot.bin: boot/diskboot.S
	$(CC) $(ASFLAGS) -Tboot/diskbootlinker.ld -o $@ $<
	$(CC) $(ASFLAGS) -Tboot/bootlinkerelf.ld -o build/diskboot.o -c $<

bin/startup_boot.S.o: startup/startup_boot.S
	$(CC) $(ASFLAGS) -o $@ -c $<

build/disk.img: build/boot.bin build/diskboot.bin build/core.bin
	truncate -s 32M $@
	dd if=build/boot.bin of=build/disk.img bs=512 count=1 seek=0 conv=notrunc
	dd if=build/diskboot.bin of=build/disk.img bs=512 count=10 seek=1 conv=notrunc
	dd if=build/core.bin of=build/disk.img bs=512 count=256 seek=2 conv=notrunc