#pragma once

#include "types.h"
#include "segment_selector.h"

void init_idt();
void idt_set_gate(u8 idx, u64 base, u16 sel, u8 ist, u8 attrs);

union idt_attrs {
    struct {
        u8 gate_type : 4; // 0b1110 = Interrupt Gate, 0b1111 = Trap Gate
        u8 zero      : 1; // must be 0
        u8 dpa       : 2; // Descriptor Privilege Level field. 0 = highest, 3 = lowest
        u8 present   : 1; // must be 1 for any valid descriptor
    } bits;
    u8 full;
};

struct idt_entry {
    u16 base_low;
    union segment_selector selector; // 16-bit selector
    u8 ist;                          // first 3 bits are the actual IST value, rest are reserved
    union idt_attrs attrs;           // attributes byte
    u16 base_mid;
    u32 base_high;
    u32 _res;                        // reserved
};

struct idtr {
    u16 size; // IDT size - 1
    u64 base; // IDT address
} __attribute((packed));

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
