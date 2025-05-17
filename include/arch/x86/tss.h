#pragma once

#include <stdint.h>

void write_tss(int index, uint16_t kernel_ss, uint32_t kernel_esp);
void tss_set_kernel_stack(uint32_t stack);
