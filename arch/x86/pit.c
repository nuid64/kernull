#include <stdint.h>

#include <arch/x86/asm.h>
#include <arch/x86/idt.h>
#include <arch/x86/regs.h>

#define PIT_A		0x40
#define PIT_B		0x41
#define PIT_C		0x42
#define PIT_CONTROL 0x43

#define PIT_SET	  0x36
#define PIT_SCALE 1193180
#define PIT_MASK  0xFF

#define TIMER_IRQ 0x20

static void pit_set_timer_freq(uint32_t hz)
{
	uint32_t divisor = PIT_SCALE / hz;
	outb(PIT_CONTROL, PIT_SET);
	outb(PIT_A, divisor & PIT_MASK);
	outb(PIT_A, (divisor >> 8) & PIT_MASK);
}

extern void switch_task(void);
uint32_t pit_interrupt(struct regs *)
{
	// switch_task();

	return 0;
}

void pit_init()
{
	irq_set_handler(TIMER_IRQ, pit_interrupt);

	pit_set_timer_freq(100);
}
