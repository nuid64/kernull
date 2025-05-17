#pragma once

#include <stdint.h>

union gdt_access {
	struct {
		uint8_t accessed : 1; /* Set to 1 on access */
		uint8_t rw : 1; /* Readable bit for code, writable bit for data */
		uint8_t dc : 1; /* Direction bit for data, conforming bit for code */
		uint8_t exec : 1; /* Executable bit. 0 = data, 1 = code */
		uint8_t type : 1; /* 0 = system segment, 1 = code/data */
		uint8_t
			dpl : 2; /* Descriptor Privilege Level field. 0 = highest, 3 = lowest */
		uint8_t present : 1; /* Must be 1 for any valid segment */
	} bits;
	uint8_t full;
};

union gdt_flags {
	struct {
		uint8_t limit_high : 4;
		uint8_t _available : 1; /* Available */
		uint8_t long_mode : 1; /* Defines 64-bit code segment. When set, DB should
                               always be clear */
		uint8_t db : 1; /* Size flag. 0 for 16-bit protected mode segment, 1 for
                       32-bit protected mode */
		uint8_t granularity : 1; /* 0 for byte granularity, 1 for page granularity
                                (4KiB) */
	} bits;
	uint8_t full;
};

struct gdt_entry {
	uint16_t limit_low; /* Maximum addressable unit */
	uint16_t base_low; /* Segment's beginning address */
	uint8_t base_middle;
	union gdt_access access; /* Access byte */
	union gdt_flags
		flags; /* WARN: Limit's bits 16-19 stored in flags' least 4 bits */
	uint8_t base_high;
};

struct gdt_ptr {
	uint16_t limit; /* GDT size - 1 */
	uint32_t base; /* GDT address */
} __attribute__((packed));

struct tss_entry {
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t unused[23];
} __attribute__((packed));

void gdt_init(void);
