#include <arch/x86/task.h>

#include <string.h>
#include <arch/x86/gdt.h>
#include <arch/x86/tss.h>

struct task *current = 0;

struct task task0;
struct task task1;
uint8_t task0_stack[4096];
uint8_t task1_stack[4096];

#define MAX_TASKS 2
static int current_task = 0;
struct task tasks[MAX_TASKS];

void tasking_init(uint32_t task0_entry, uint32_t task1_entry)
{
	memset(&tasks, 0, sizeof(tasks));

	tasks[0].ctx.eip = task0_entry;
	tasks[0].ctx.esp = (uint32_t)&task0_stack[4096 - 4];
	tasks[0].next = &task1;
	tasks[0].active = true;

	tasks[1].ctx.eip = task1_entry;
	tasks[1].ctx.esp = (uint32_t)&task1_stack[4096 - 4];
	tasks[1].next = &task0;
	tasks[1].active = true;
}

extern uint32_t read_eip(void);
void yield(void)
{
	int next = (current_task + 1) % MAX_TASKS;
	while (!tasks[next].active)
		next = (next + 1) % MAX_TASKS;

	if (next == current_task)
		return;

	int prev = current_task;
	current_task = next;

	uint32_t esp, ebp, eip;
	__asm__ __volatile__("mov %%esp, %0" : "=r"(esp));
	__asm__ __volatile__("mov %%ebp, %0" : "=r"(ebp));

	eip = read_eip();
	if (eip == 0xDEADBABE)
		return;

	tasks[prev].ctx.eip = eip;
	tasks[prev].ctx.esp = esp;
	tasks[prev].ctx.ebp = ebp;

	eip = tasks[current_task].ctx.eip;
	esp = tasks[current_task].ctx.esp;
	ebp = tasks[current_task].ctx.ebp;

	__asm__ __volatile__("\
		cli;\
		mov %0, %%ecx;\
		mov %1, %%esp;\
		mov %2, %%ebp;\
		mov $0xDEADBABE, %%eax;\
		sti;\
		jmp  *%%ecx"
						 :
						 : "r"(eip), "r"(esp), "r"(ebp));
}

extern void task_switch(uint32_t, uint32_t);
void start_scheduler(void)
{
	current_task = 0;
	task_switch(tasks[0].ctx.eip, tasks[0].ctx.esp);
}
