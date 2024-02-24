#pragma once

#include <kernel/types.h>

/* INFO: Interface between keyboard/mice drivers and specific PS/2 controller */

#define DATA_PORT    0x60
#define COMMAND_REG  0x64 // Write-only
#define STATUS_REG   0x64 // Read-only

#define KBD_IRQ      0x21

u8 ps2_data_read();

void ps2_port1_write(u8 data);
void ps2_port1_disable();
void ps2_port1_enable();

void ps2_port2_write(u8 data);
void ps2_port2_disable();
void ps2_port2_enable();
