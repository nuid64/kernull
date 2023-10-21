#ifndef GDT_H
#define GDT_H

#include "types.h"

void init_gdt();

union gdt_access {
    struct {
        u8 accessed : 1; // set to 1 on access
        u8 rw       : 1; // readable bit for code, writeable bit for data
        u8 dc       : 1; // direction bit for data, conforming bit for code
        u8 exec     : 1; // executable bit. 0 = data, 1 = code
        u8 type     : 1; // 0 = system segment, 1 = code/data
        u8 dpl      : 2; // Descriptor Privilege Level field. 0 = highest, 3 = lowest
        u8 present  : 1; // must be 1 for any valid segment
    } bits;
    u8 full;
} __attribute__((packed));

union gdt_flags {
    struct {
        u8 limit_high  : 4;
        u8 _res        : 1; // reserved
        u8 long_mode   : 1; // long mode. Defines 64-bit code segment. When set, DB should always be clear
        u8 db          : 1; // size flag. 0 for 16-bit protected mode segment, 1 for 32-bit protected mode
        u8 granularity : 1; // 0 for byte granularity, 1 for page granularity (4KiB)
    } bits;
    u8 full;
} __attribute__((packed));

struct gdt_entry {
    u16 limit_low;
    u16 base_low;
    u8  base_middle;
    union gdt_access access;
    union gdt_flags flags;   // WARN: limit_high stored in flags' least 4 bits
    u8 base_high;
} __attribute__((packed));

struct gdtr {
    u16 limit;
    u64 base;
} __attribute__((packed));

#endif
