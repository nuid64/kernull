#pragma once

#include <kernel/types.h>

/* INFO: Interface between keyboard/mice drivers and specific PS/2 controller */

#define DATA_PORT    0x60
#define COMMAND_REG  0x64 // Write-only
#define STATUS_REG   0x64 // Read-only

#define KBD_IRQ      0x21

void ps2_port1_write(u8 data);
void ps2_port2_write(u8 data);
