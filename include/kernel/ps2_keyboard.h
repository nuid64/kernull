#pragma once

#include <kernel/types.h>

#define PS2_KBD_COM_SET_LED      0xED
#define PS2_KBD_COM_ECHO         0xEE
#define PS2_KBD_COM_SET_SCAN_SET 0xF0

void ps2_kbd_command_arg(u8 command, u8 arg);
void ps2_kbd_command(u8 command);
u8   ps2_kbd_get_scan_code_set();
