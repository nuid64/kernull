#pragma once

#include <kernull/types.h>
#include "segment_selector.h"
#include "regs.h"
 void init_idt(); void idt_set_gate(u8 idx, u64 base, u16 sel, u8 ist, u8 attrs);

typedef u32 (*int_handler) (struct regs*);
void irq_set_handler(u8 irq, int_handler handler);

union idt_attributes {
    struct {
        u8 gate_type : 4; /* 0b1110 = Interrupt Gate, 0b1111 = Trap Gate */
        u8 zero      : 1; /* Must be 0 */
        u8 dpa       : 2; /* Descriptor Privilege Level field. 0 = highest, 3 = lowest */
        u8 present   : 1; /* Must be 1 for any valid descriptor */
    } bits;
    u8 full;
} ;

typedef struct {
    u16 base_low;
    union segment_selector selector; /* 16-bit selector */
    u8 ist;                          /* First 3 bits are the actual IST value, rest are reserved */
    union idt_attributes attrs;      /* Attributes byte */
    u16 base_mid;
    u32 base_high;
    u32 _res;                        /* Available */
} idt_entry;

typedef struct __attribute((packed)) {
    u16 size; /* IDT size - 1 */
    u64 base; /* IDT address */
} idtr;

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
