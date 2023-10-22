#include "idt.h"
#include "regs.h"

extern void idt_load(u64 idtr);

static struct idtr idtr;
static struct idt_entry IDT[256];

void init_idt()
{
    idtr.size = sizeof(struct idt_entry) * 256 - 1;
    idtr.base = (u64) &IDT;

    idt_set_gate(0, (u64) isr0, 0x08, 0, 0x8E);
    idt_set_gate(1, (u64) isr1, 0x08, 0, 0x8E);
    idt_set_gate(2, (u64) isr2, 0x08, 0, 0x8E);
    idt_set_gate(3, (u64) isr3, 0x08, 0, 0x8E);
    idt_set_gate(4, (u64) isr4, 0x08, 0, 0x8E);
    idt_set_gate(5, (u64) isr5, 0x08, 0, 0x8E);
    idt_set_gate(6, (u64) isr6, 0x08, 0, 0x8E);
    idt_set_gate(7, (u64) isr7, 0x08, 0, 0x8E);
    idt_set_gate(8, (u64) isr8, 0x08, 0, 0x8E);
    idt_set_gate(9, (u64) isr9, 0x08, 0, 0x8E);
    idt_set_gate(10, (u64) isr10, 0x08, 0, 0x8E);
    idt_set_gate(11, (u64) isr11, 0x08, 0, 0x8E);
    idt_set_gate(12, (u64) isr12, 0x08, 0, 0x8E);
    idt_set_gate(13, (u64) isr13, 0x08, 0, 0x8E);
    idt_set_gate(14, (u64) isr14, 0x08, 0, 0x8E);
    idt_set_gate(15, (u64) isr15, 0x08, 0, 0x8E);
    idt_set_gate(16, (u64) isr16, 0x08, 0, 0x8E);
    idt_set_gate(17, (u64) isr17, 0x08, 0, 0x8E);
    idt_set_gate(18, (u64) isr18, 0x08, 0, 0x8E);
    idt_set_gate(19, (u64) isr19, 0x08, 0, 0x8E);
    idt_set_gate(20, (u64) isr20, 0x08, 0, 0x8E);
    idt_set_gate(21, (u64) isr21, 0x08, 0, 0x8E);
    idt_set_gate(22, (u64) isr22, 0x08, 0, 0x8E);
    idt_set_gate(23, (u64) isr23, 0x08, 0, 0x8E);
    idt_set_gate(24, (u64) isr24, 0x08, 0, 0x8E);
    idt_set_gate(25, (u64) isr25, 0x08, 0, 0x8E);
    idt_set_gate(26, (u64) isr26, 0x08, 0, 0x8E);
    idt_set_gate(27, (u64) isr27, 0x08, 0, 0x8E);
    idt_set_gate(28, (u64) isr28, 0x08, 0, 0x8E);
    idt_set_gate(29, (u64) isr29, 0x08, 0, 0x8E);
    idt_set_gate(30, (u64) isr30, 0x08, 0, 0x8E);
    idt_set_gate(31, (u64) isr31, 0x08, 0, 0x8E);

    asm volatile (
        "lidt %0"
        : : "m"(idtr)
	);
}

void idt_set_gate(u8 idx, u64 base, u16 sel, u8 ist, u8 attrs)
{
    IDT[idx].base_low      = base & 0xFFFF;
    IDT[idx].base_mid   = (base >> 16) & 0xFFFF;
    IDT[idx].base_high     = (base >> 32) & 0xFFFFFFFF;

    IDT[idx].selector.full = sel;
    IDT[idx].ist           = ist & 0b111;
    IDT[idx].attrs.full    = attrs;
};

// FIXME dummy for now
#include "vga_print.h"
struct regs* isr_handler(struct regs* r)
{
    vga_print("received interrupt");
    return r;
}
