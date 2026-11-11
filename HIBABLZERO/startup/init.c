/*
HIBAZERO
Copyright (C) 2026 Himanshu Bakshi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.
*/

#include <drivers/serial.h>
#include <drivers/vga_text.h>
#include <hb0/types.h>
#include <machine/int.h>
#include <startup/init.h>

void NORETURN kern_startup(void) {
  struct serial_port port = {PORT_COM1};
  struct serial_interface interface = {0};
  interface.port = &port;
  struct vga_text_console_t vga_controller;

  serial_interface_init(&interface, 1, PORT_COM1);

  vga_text_init(&vga_controller);

  if (a20_on)
    vga_controller.puts(&vga_controller, (u8 *)"A20 line is enabled!\n",
                        0b00001111);
  else
    vga_controller.puts(&vga_controller, (u8 *)"A20 line is not enabled!\n",
                        0b00001111);

  vga_controller.puts(&vga_controller, (u8 *)"Hello, World!", 0b00001111);
  while (true) {
    char c;
    interface.readc(interface.port, (u8 *)&c);
    interface.puts(interface.port, (u8 *)&c, 1);
  }
};
