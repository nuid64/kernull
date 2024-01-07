#include <kernel/types.h>
#include <kernel/kmalloc.h>
#include <kernel/mm.h>

u64 placement_address;

u64 kmalloc(u64 size)
{

    u64 tmp = placement_address;
    placement_address += size;

    return tmp;
}

u64 kmalloc_a(u64 size)
{
    if (placement_address & 0xFFF) {
        placement_address &= ~0xFFF;
        placement_address += 0x1000;
    }

    u64 tmp = placement_address;
    placement_address += size;

    return tmp;
}

u64 kmalloc_ap(u64 size, u64* phys)
{
    if (placement_address & 0xFFF) {
        placement_address &= ~0xFFF;
        placement_address += 0x1000;
    }

    *phys = placement_address;

    u64 tmp = placement_address;
    placement_address += size;

    return tmp;
}
