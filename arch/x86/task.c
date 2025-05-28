#include <arch/x86/task.h>

#include <string.h>

#include <arch/x86/gdt.h>
#include <arch/x86/memory.h>

uint8_t task0_stack[4096] = { 0 };
uint8_t task1_stack[4096] = { 0 };

uint8_t ustack0[4096] = { 0 };
uint8_t ustack1[4096] = { 0 };

extern uint32_t task0_entry;
extern uint32_t task1_entry;

extern uint32_t page_directory[PAGE_ENTRIES]
	__attribute__((aligned(PAGE_SIZE)));

#define MAX_TASKS 2

struct task tasks[MAX_TASKS];
uint8_t current_task = 0;

void tss_install(void)
{
	gdt_set_tss_gate(5, &tasks[0].tss);
	gdt_set_tss_gate(6, &tasks[1].tss);
}

#include <kernel/printk.h>
void tasking_init(uint32_t task0_entry, uint32_t task1_entry)
{
	memset(tasks, 0, sizeof(tasks));

	tasks[0].tss.eip = task0_entry;
	tasks[0].tss.esp = (uint32_t)&task0_stack[4096 - 4];
	tasks[0].tss.ebp = tasks[0].tss.esp;
	tasks[0].tss.esp0 = tasks[0].tss.esp;
	tasks[0].tss.ss0 = 0x10;
	tasks[0].tss.es = 0x10;
	tasks[0].tss.cs = 0x08;
	tasks[0].tss.ss = 0x10;
	tasks[0].tss.ds = 0x10;
	tasks[0].tss.fs = 0x10;
	tasks[0].tss.gs = 0x10;
	tasks[0].tss.iomap_base = sizeof(struct tss_entry);
	tasks[0].tss.eflags = 0x202;
	tasks[0].tss.cr3 = (uint32_t)page_directory;
	tasks[0].next = &tasks[1];

	tasks[1].tss.eip = task1_entry;
	tasks[1].tss.esp = (uint32_t)&task1_stack[4096 - 4];
	tasks[1].tss.ebp = tasks[1].tss.esp;
	tasks[1].tss.esp0 = tasks[1].tss.esp;
	tasks[1].tss.ss0 = 0x10;
	tasks[1].tss.es = 0x10;
	tasks[1].tss.cs = 0x08;
	tasks[1].tss.ss = 0x10;
	tasks[1].tss.ds = 0x10;
	tasks[1].tss.fs = 0x10;
	tasks[1].tss.gs = 0x10;
	tasks[1].tss.iomap_base = sizeof(struct tss_entry);
	tasks[1].tss.eflags = 0x202;
	tasks[1].tss.cr3 = (uint32_t)page_directory;
	tasks[1].next = &tasks[0];
}

void yield(void)
{
	current_task = (current_task + 1) % MAX_TASKS;
	switch_to(current_task);
}

void start_scheduler(void)
{
	current_task = 0;
	ltr(0);
	switch_to(0);
}
