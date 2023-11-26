#include "gdt.h"

static gdt_entry GDT[5];
static gdtr GDTR;

extern void gdt_load(u64 gdtr);

static void gdt_set_entry(u64 idx, u32 base, u32 limit, u8 access, u8 flags);

void init_gdt()
{
    GDTR.size = (sizeof(gdt_entry) * 5) - 1;
    GDTR.base  = (u64) &GDT;

    gdt_set_entry(0, 0, 0, 0, 0);                /* NULL */
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0x0A); /* Kernel code */
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0x0C); /* Kernel data */
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0x0A); /* User code */
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0x0C); /* User data */

    gdt_load((u64) &GDTR);
}

static void gdt_set_entry(u64 idx, u32 base, u32 limit, u8 access, u8 flags)
{
    GDT[idx].base_low    = base & 0xFFFF;
    GDT[idx].base_middle = (base >> 16) & 0xFF;
    GDT[idx].base_high   = (base >> 24) & 0xFF;

    GDT[idx].limit_low   = limit & 0xFFFF;
    GDT[idx].flags.bits.limit_high  = (limit >> 16) & 0x0F;

    GDT[idx].flags.full |= (flags << 4) & 0xF0;
    GDT[idx].access.full = access;
}
