#pragma once

#include <kernel/types.h>

void init_gdt();

union gdt_access {
    struct {
        u8 accessed : 1; /* Set to 1 on access */
        u8 rw       : 1; /* Readable bit for code, writable bit for data */
        u8 dc       : 1; /* Direction bit for data, conforming bit for code */
        u8 exec     : 1; /* Executable bit. 0 = data, 1 = code */
        u8 type     : 1; /* 0 = system segment, 1 = code/data */
        u8 dpl      : 2; /* Descriptor Privilege Level field. 0 = highest, 3 = lowest */
        u8 present  : 1; /* Must be 1 for any valid segment */
    } bits;
    u8 full;
};

union gdt_flags {
    struct {
        u8 limit_high  : 4;
        u8 _available  : 1; /* Available */
        u8 long_mode   : 1; /* Defines 64-bit code segment. When set, DB should always be clear */
        u8 db          : 1; /* Size flag. 0 for 16-bit protected mode segment, 1 for 32-bit protected mode */
        u8 granularity : 1; /* 0 for byte granularity, 1 for page granularity (4KiB) */
    } bits;
    u8 full;
};

typedef struct {
    u16 limit_low;           /* Maximum addressable unit */
    u16 base_low;            /* Segment's beginning address */
    u8  base_middle;
    union gdt_access access; /* Access byte */
    union gdt_flags flags;   /* WARN: Limit's bits 16-20 stored in flags' least 4 bits */
    u8 base_high;
} gdt_entry;

typedef struct __attribute((packed)) {
    u16 size; /* GDT size - 1 */
    u64 base; /* GDT address */
} gdtr;
