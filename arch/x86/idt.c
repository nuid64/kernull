#include <arch/x86/idt.h>
#include <kernel/types.h>
#include <arch/x86/asm.h>
#include <arch/x86/regs.h>
#include <arch/x86/segment_selector.h>
#include <arch/x86/pic.h>

static idtr IDTR;
__attribute((aligned(0x10)))
static idt_entry IDT[256];
static int_handler irq_handlers[256];

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void idt_init()
{
    pic_remap();

    IDTR.size = sizeof(idt_entry) * 256 - 1;
    IDTR.base = (u64) &IDT;

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

    idt_set_gate(32, (u64) irq0, 0x08, 0, 0x8E);
    idt_set_gate(33, (u64) irq1, 0x08, 0, 0x8E);
    idt_set_gate(34, (u64) irq2, 0x08, 0, 0x8E);
    idt_set_gate(35, (u64) irq3, 0x08, 0, 0x8E);
    idt_set_gate(36, (u64) irq4, 0x08, 0, 0x8E);
    idt_set_gate(37, (u64) irq5, 0x08, 0, 0x8E);
    idt_set_gate(38, (u64) irq6, 0x08, 0, 0x8E);
    idt_set_gate(39, (u64) irq7, 0x08, 0, 0x8E);
    idt_set_gate(40, (u64) irq8, 0x08, 0, 0x8E);
    idt_set_gate(41, (u64) irq9, 0x08, 0, 0x8E);
    idt_set_gate(42, (u64) irq10, 0x08, 0, 0x8E);
    idt_set_gate(43, (u64) irq11, 0x08, 0, 0x8E);
    idt_set_gate(44, (u64) irq12, 0x08, 0, 0x8E);
    idt_set_gate(45, (u64) irq13, 0x08, 0, 0x8E);
    idt_set_gate(46, (u64) irq14, 0x08, 0, 0x8E);
    idt_set_gate(47, (u64) irq15, 0x08, 0, 0x8E);

    asm (
        "lidt %0"
        : : "m"(IDTR)
    );
}

void idt_set_gate(u8 idx, u64 base, u16 sel, u8 ist, u8 attrs)
{
    IDT[idx].base_low      = base & 0xFFFF;
    IDT[idx].base_mid      = (base >> 16) & 0xFFFF;
    IDT[idx].base_high     = (base >> 32) & 0xFFFFFFFF;

    IDT[idx].selector.full = sel;
    IDT[idx].ist           = ist & 0b111;
    IDT[idx].attrs.full    = attrs;
};

void irq_set_handler(u8 irq, int_handler handler)
{
    irq_handlers[irq] = handler;
}

struct regs *isr_handler(struct regs *r)
{
    if (irq_handlers[r->int_no]) {
        int_handler handler = irq_handlers[r->int_no];
        handler(r);
    }
    
    return r;
}

struct regs *irq_handler(struct regs *r)
{
    if (irq_handlers[r->int_no]) {
        int_handler handler = irq_handlers[r->int_no];
        handler(r);
    }

    irq_ack(r->int_no - 32);

    return r;
}
