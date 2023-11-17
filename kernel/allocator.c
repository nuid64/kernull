#include "allocator.h"

// mark a physical frame as in use
void mmu_frame_set(u64 frame_addr)
{
    if (frame_addr < nframes * PAGE_SIZE) {
        u64 frame = frame_addr >> PAGE_SHIFT;
        u64 idx = INDEX_FROM_BIT(frame);
        u32 off = OFFSET_FROM_BIT(frame);
        frames[idx] |= ((u32) 1 << off);
    }
}

// mark a physical frame as available
void mmu_frame_clear(u64 frame_addr)
{
    if (frame_addr < nframes * PAGE_SIZE) {
        u64 frame = frame_addr >> PAGE_SHIFT;
        u64 idx = INDEX_FROM_BIT(frame);
        u64 off = OFFSET_FROM_BIT(frame);
        frames[idx] &= ~((u32) 1 << off);
    }
}

// determine if a physical frame available
u32 mmu_frame_test(u64 frame_addr)
{
    if (!(frame_addr < nframes * PAGE_SIZE)) return 1;

    u64 frame = frame_addr >> PAGE_SHIFT;
    u64 idx = INDEX_FROM_BIT(frame);
    u64 off = OFFSET_FROM_BIT(frame);
    return frames[idx] & ((u32) 1 << off); // WARN perhaps double negation is needed here
}

// find the first available frame
u64 mmu_first_frame()
{
    for (u64 i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
        if (frames[i] == (u32) ~0) continue; // all frames are reserved
        for (u64 j = 0; j < 32; ++j) {
            u64 bit = 1 << j;
            if ( !(frames[i] & bit) )
                return (i << 5) + j;
        }
    }
    // FIXME wtf should I return? Error handling here
}

// pretty self-explanatory :/
void mmu_frame_allocate(pml_entry* page, u64 flags)
{
    if (page->bits.address != 0) return; // already allocated

    u64 idx = mmu_first_frame();
    u64 addr = idx << PAGE_SHIFT;
    // FIXME handle error here
    mmu_frame_set(addr);
    page->bits.address = addr;
    page->bits.present = 1;
    page->full |= flags;
}
// FIXME which to use? ,':/
// allocate physical page and return it's index
u64 mmu_allocate_frame()
{
    u64 idx = mmu_first_frame();
    mmu_frame_set(idx << PAGE_SHIFT);
    return idx;
}

void mmu_frame_map_address(pml_entry* page, u64 flags, u64 phys_addr)
{
    mmu_frame_set(phys_addr);
    page->bits.address = phys_addr >> PAGE_SHIFT;
    mmu_frame_allocate(page, flags);
}
