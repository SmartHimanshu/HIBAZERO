.PHONY: all clean run
all:
	$(MAKE) -C HIBABLZERO
clean:
	rm -f HIBABLZERO/bin/*
	rm -f HIBABLZERO/build/*

run: HIBABLZERO/build/disk.img
	qemu-system-i386 -hda HIBABLZERO/build/disk.img