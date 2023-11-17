#include <kernel/types.h>
#include "kheap.h"
#include "mmu.h"

u64 placement_address = KERNEL_HEAP_START;

u64 kmalloc(u64 size)
{

    u64 tmp = placement_address;
    placement_address += size;

    return tmp;
}

u64 kmalloc_a(u64 size)
{
    if (placement_address & 0xFFFFFFFFFFFFF000) {
    // if (placement_address & ~0xFFF) {
        placement_address &= 0xFFFFFFFFFFFFF000;
        // placement_address &= ~0xFFF;
        placement_address += 0x1000;
    }

    u64 tmp = placement_address;
    placement_address += size;

    return tmp;
}

u64 kmalloc_ap(u64 size, u64* phys)
{
    if (placement_address & 0xFFFFFFFFFFFFF000) {
    // if (placement_address & ~0xFFF) {
        placement_address &= 0xFFFFFFFFFFFFF000;
        // placement_address &= ~0xFFF;
        placement_address += 0x1000;
    }

    *phys = placement_address;

    u64 tmp = placement_address;
    placement_address += size;

    return tmp;
}
