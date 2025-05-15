#pragma once

#include <stdint.h>
#include <arch/x86/regs.h>
#include <arch/x86/segment_selector.h>

void idt_init(void);
void idt_set_gate(uint8_t idx, uint32_t base, uint16_t sel, uint8_t attrs);

typedef uint32_t (*int_handler)(struct regs *);
void irq_set_handler(uint8_t irq, int_handler handler);

union idt_attributes {
    struct {
        uint8_t gate_type : 4; /* 0b1110 = Interrupt Gate, 0b1111 = Trap Gate */
        uint8_t zero      : 1; /* Must be 0 */
        uint8_t dpa       : 2; /* Descriptor Privilege Level field. 0 = highest, 3 = lowest */
        uint8_t present   : 1; /* Must be 1 for any valid descriptor */
    } bits;
    uint8_t full;
};

struct idt_entry {
    uint16_t base_low;
    union segment_selector selector; /* 16-bit selector */
    uint8_t _res;
    union idt_attributes attrs; /* Attributes byte */
    uint16_t base_high;
};

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));
