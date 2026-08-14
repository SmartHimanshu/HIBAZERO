.PHONY: all clean run
all:
	$(MAKE) -C HIBABLZERO
clean:
	rm -f HIBABLZERO/bin/*
	rm -f HIBABLZERO/build/*

run: HIBABLZERO/build/disk.img
	qemu-system-i386 -hda HIBABLZERO/build/disk.img


#NOTE: I love making purely excellent help tools for my make.
deploy: HIBABLZERO/build/disk.img
	@lsblk
	@printf "Please choose a drive in sdx form:"
	@read drive;\
	lsblk /dev/"$$drive";\
	printf "Are you sure? [y/N]: ";\
	read ans; \
	if [ "$$ans" != "y" ] && [ "$$ans" != "Y" ]; then \
		echo "Deploy aborted. Exiting..."; \
		exit 1; \
	fi; \
	printf "What would you like to load today? (A)KERNEL (B)BOOT (C)BOTH "; \
	read payload; \
	if [ "$$payload" == "B" ] || [ "$$payload" == "b" ]; then \
		dd if=HIBABLZERO/build/disk.img of=/dev/"$$drive" bs=1M count=1 conv=notrunc; \
		echo "Loading bootloader in..."; \
		exit 0; \
	elif [ "$$payload" == "A" ] || [ "$$payload" == "a" ]; then \
		sudo mount /dev/"$$drive" /mnt/hibausb; \
		cp /HIBAKERNZERO/build/kernel.bin /mnt/hibausb/KERNEL.bin; \
		sync; \
		sudo unmount /dev/"$$drive" \
		echo "Loading kernel in... "; \
		exit 0; \
	elif [ "$$payload" == "C" ] || [ "$$payload" == "c" ]; then \
		sudo mount /dev/"$$drive" /mnt/hibausb; \
		cp HIBAKERNZERO/build/kernel.bin /mnt/hibausb/KERNEL.bin; \
		sync; \
		sudo unmount /dev/"$$drive" \
		dd if=HIBABLZERO/build/disk.img of=/dev/"$$drive" bs=1M count=1 conv=notrunc; \
		echo "Loading bootloader and kernel in... "; \
		exit 0; \
	else \
		echo "Command not recognized!" \
		exit 1; \
	fi
	