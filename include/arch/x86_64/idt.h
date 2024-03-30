#pragma once

#include <kernel/types.h>
#include <arch/x86_64/regs.h>
#include <arch/x86_64/segment_selector.h>

void idt_init();
void idt_set_gate(u8 idx, u64 base, u16 sel, u8 ist, u8 attrs);

typedef u32 (*int_handler) (struct regs *);
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
    u32 _res;
} idt_entry;

typedef struct __attribute((packed)) {
    u16 size; /* IDT size - 1 */
    u64 base; /* IDT address */
} idtr;
