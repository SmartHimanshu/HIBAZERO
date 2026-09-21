/* 
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#define HB0_MIN_SECTOR_SIZE             0x200

#define HB0_BIOS_BOOT_SECTOR_LOAD_ADDR  0x7c00
#define HB0_BIOS_BOOT_SECTOR_SIZE       0x200
#define HB0_BIOS_BOOT_SECTOR_MAGIC      0xAA55

#define HB0_DISK_BOOT_SECTOR_COUNT      2
#define HB0_DISK_BOOT_LOAD_ADDR         0x6000 
#define HB0_DISK_BOOT_LOAD_SEG          (HB0_DISK_BOOT_LOAD_ADDR >> 4)
#define HB0_DISK_BOOT_SIZE              (HB0_MIN_SECTOR_SIZE * HB0_DISK_BOOT_SECTOR_COUNT)

#define HB0_CORE_SECTOR_COUNT           256
#define HB0_CORE_LOAD_ADDR              0x8000
#define HB0_CORE_LOAD_SEG               (HB0_CORE_LOAD_ADDR>>4)

#define HB0_DISK_BUFFER_ADDR            0x70000
#define HB0_DISK_BUFFER_SEG             (HB0_DISK_BUFFER_ADDR >> 4)
#define HB0_DISK_BUFFER_SIZE            0x8000

#define HB0_MAX_SECTOR_SIZE             4096
#define HB0_MAX_SECTOR_STREAM           (HB0_DISK_BUFFER_SIZE / HB0_MAX_SECTOR_SIZE)
#define HB0_MAX_STREAM_SIZE             (HB0_MAX_SECTOR_STREAM * HB0_MIN_SECTOR_SIZE)

#define HB0_PM_STACK_ADDR               0x80000
#define HB0_PM_STACK_SIZE               ((HB0_DISK_BUFFER_ADDR - HB0_PM_STACK_ADDR) - HB0_DISK_BUFFER_SIZE)

