#include <stddef.h>
#include <stdint.h>

#include <arch/x86/memory.h>
#include <arch/x86/vga_print.h>
#include <arch/x86/task.h>
#include <kernel/printk.h>

extern void gdt_init(void);
extern void idt_init(void);
extern void pit_init(void);
extern void tss_flush(void);
extern void tss_switch_to(uint32_t);

#define SWITCH_COUNT 10000

static uint64_t leave_time0[SWITCH_COUNT] = { 0 };
static uint64_t entry_time0[SWITCH_COUNT] = { 0 };
static uint64_t leave_time1[SWITCH_COUNT] = { 0 };
static uint64_t entry_time1[SWITCH_COUNT] = { 0 };

static inline uint64_t rdtsc(void)
{
	uint32_t lo, hi;
	__asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
	return ((uint64_t)hi << 32) | lo;
}

void task0_func()
{
	static uint32_t count = 0;
	printk("task0 enter\n");

	while (1) {
		if (count < SWITCH_COUNT) {
			leave_time0[count] = rdtsc();
			yield();
			entry_time0[count] = rdtsc();
			count++;
		} else {
			yield();
		}
	}
}

void task1_func()
{
	static uint32_t count = 0;
	static bool printed_results = false;
	printk("task1 enter\n");

	while (1) {
		if (count < SWITCH_COUNT) {
			leave_time1[count] = rdtsc();
			yield();
			entry_time1[count] = rdtsc();
			count++;
		} else {
			if (!printed_results) {
				uint32_t average = 0;
				for (uint32_t i = 0; i < SWITCH_COUNT; ++i) {
					average += (uint32_t)(entry_time1[i] - leave_time0[i]);
					average += (uint32_t)(entry_time0[i] - leave_time1[i]);
				}
				average /= 2 * SWITCH_COUNT;

				printk("Average cycles for switch: %d\n", average);
				printed_results = true;
			}
			yield();
		}
	}
}

void kmain(void)
{
	vga_terminal_initialize();
	gdt_init();
	idt_init();
	pit_init();
	paging_init();
	tasking_init((uint32_t)task0_func, (uint32_t)task1_func);
	start_scheduler();

	vga_set_color(VGA_COLOR_GREEN);
	printk("\
    __                         ____\n\
   / /_____  _________  __  __/ / /\n\
  / //_/ _ \\/ ___/ __ \\/ / / / / /\n\
 / ,< /  __/ /  / / / / /_/ / / /  \n\
/_/|_|\\___/_/  /_/ /_/\\__,_/_/_/\n\
");
	for (;;)
		__asm__ __volatile__("hlt");
}
