#pragma once

#include <arch/x86/gdt.h>

extern void tss_flush(void);
extern void tss_switch(uint32_t to);

#define FIRST_TSS_ENTRY 5
#define _TSS(n)			((((unsigned long)n) << 4) + (FIRST_TSS_ENTRY << 3))
#define ltr(n)			__asm__("ltr %%ax" ::"a"(_TSS(n)))
#define str(n)                  \
	__asm__("str %%ax\n\t"      \
			"subl %2,%%eax\n\t" \
			"shrl $4,%%eax"     \
			: "=a"(n)           \
			: "a"(0), "i"(FIRST_TSS_ENTRY << 3))
#define switch_to(n)                           \
	{                                          \
		struct {                               \
			long a, b;                         \
		} __tmp;                               \
		__asm__("movw %%dx, %1\n"              \
				"ljmp *%0\n"                   \
				"clts" ::"m"(*&__tmp.a),       \
				"m"(*&__tmp.b), "d"(_TSS(n))); \
	}

struct task {
	struct tss_entry tss;
	bool active;
	struct task *next;
};

void tasking_init(uint32_t task0_entry, uint32_t task1_entry);
void yield(void);
void start_scheduler(void);
