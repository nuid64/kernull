#include <stdint.h>

#include <arch/x86/asm.h>
#include <arch/x86/idt.h>
#include <arch/x86/pic.h>

struct idt_entry IDT[256];
struct idt_ptr IDTR;
int_handler irq_handlers[256];

extern void idt_load(uint32_t);
extern void (*isr_stub_table[])();

void idt_set_gate(uint8_t idx, uint32_t base, uint16_t selector, uint8_t attr)
{
	IDT[idx].base_low = base & 0xFFFF;
	IDT[idx].base_high = (base >> 16) & 0xFFFF;
	IDT[idx].selector.full = selector;
	IDT[idx].attrs.full = attr;
	IDT[idx]._res = 0;
}

void idt_init(void)
{
	pic_remap();

	IDTR.limit = sizeof(IDT) - 1;
	IDTR.base = (uint32_t)&IDT;

	for (int i = 0; i < 48; i++)
		idt_set_gate(i, (uint32_t)isr_stub_table[i], 0x08, 0x8E);

	__asm__("lidt %0" : : "m"(IDTR));
}

void irq_set_handler(uint8_t irq, int_handler handler)
{
	irq_handlers[irq] = handler;
}

extern uint32_t pit_interrupt(struct regs *);
void isr_handler(struct regs *r)
{
	if (irq_handlers[r->int_no]) {
		int_handler handler = irq_handlers[r->int_no];
		handler(r);
	}

	irq_ack(r->int_no);
}
