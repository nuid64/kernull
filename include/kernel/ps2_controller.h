#pragma once
#include <kernel/types.h>

/* INFO: Interface between keyboard/mice drivers and specific PS/2 controller */

const u8 DATA_PORT   = 0x60;
const u8 COMMAND_REG = 0x64; // Write-only
const u8 STATUS_REG  = 0x64; // Read-only

const u8 KBD_IRQ     = 0x21;

void ps2_port1_write(u8 data);
void ps2_port2_write(u8 data);
