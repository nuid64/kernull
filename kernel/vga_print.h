#pragma once

#include "types.h"

void vga_terminal_initialize();

void vga_set_color(u8 color);

void vga_print(const char* s);
void vga_print_num(u64 num);
