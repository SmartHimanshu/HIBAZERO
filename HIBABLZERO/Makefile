# First and foremost, we compile the boot sector.
CC := i686-elf-gcc
LD := i686-elf-ld
STARTUPASFLAGS := -g -Iinclude
ASFLAGS := $(STARTUPASFLAGS) -Wa,--divide -ffreestanding -nostdlib -nostdinc -Wall -Wextra
CFLAGS := $(ASFLAGS)
OBJS := bin/startup_boot.S.o bin/init.c.o bin/serial_uart_chip.c.o

all: build/disk.img

build/core.bin: $(OBJS)
	$(CC) $(ASFLAGS) -Tscript/linker.ld -o $@ $(OBJS)
	$(CC) $(ASFLAGS) -Tscript/linkerelf.ld -o build/coreelf.o $(OBJS)

build/boot.bin: boot/boot.S
	$(CC) $(ASFLAGS) -m32 -Tscript/bootlinker.ld -o $@ $<
	$(CC) $(ASFLAGS) -m32 -Tscript/bootlinkerelf.ld -o build/boot.o -c $<

build/diskboot.bin: boot/diskboot.S
	$(CC) $(ASFLAGS) -m32 -Tscript/diskbootlinker.ld -o $@ $<
	$(CC) $(ASFLAGS) -m32 -Tscript/bootlinkerelf.ld -o build/diskboot.o -c $<

bin/startup_boot.S.o: startup/startup_boot.S
	$(CC) $(ASFLAGS) -o $@ -c $<

bin/init.c.o: startup/init.c
	$(CC) $(CFLAGS) -o $@ -c $<

bin/serial_uart_chip.c.o: drivers/serial/uart_chip.c
	$(CC) $(CFLAGS) -o $@ -c $<

build/disk.img: build/boot.bin build/diskboot.bin build/core.bin
	truncate -s 32M $@
	dd if=build/boot.bin of=build/disk.img bs=512 count=1 seek=0 conv=notrunc
	dd if=build/diskboot.bin of=build/disk.img bs=512 count=10 seek=1 conv=notrunc
	dd if=build/core.bin of=build/disk.img bs=512 count=256 seek=11 conv=notrunc