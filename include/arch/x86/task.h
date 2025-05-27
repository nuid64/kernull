#pragma once

#include <stdint.h>

struct task_context {
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax, ecx, edx, ebx;
	uint32_t esp, ebp, esi, edi;
	uint32_t es, cs, ss, ds, fs, gs;
	uint16_t iomap_base;
};

struct context {
	uint32_t edi, esi, ebp, ebx, edx, ecx, eax;
	uint32_t eip;
	uint32_t esp;
};

struct task {
	struct context ctx;
	bool active;
	struct task *next;
};

void tasking_init(uint32_t task0_entry, uint32_t task1_entry);
void yield(void);
void start_scheduler(void);
