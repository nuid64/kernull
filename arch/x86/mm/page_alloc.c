#include "page_alloc.h"

/* Mark a physical frame as in use */
void mmu_frame_set(u64 frame_addr)
{
    if (frame_addr < nframes * PAGE_SIZE) {
        u64 frame = frame_addr >> PAGE_SHIFT;
        u64 idx = INDEX_FROM_BIT(frame);
        u32 off = OFFSET_FROM_BIT(frame);
        frames[idx] |= ((u32) 1 << off);
    }
}

/* Mark a physical frame as available */
void mmu_frame_clear(u64 frame_addr)
{
    if (frame_addr < nframes * PAGE_SIZE) {
        u64 frame = frame_addr >> PAGE_SHIFT;
        u64 idx = INDEX_FROM_BIT(frame);
        u64 off = OFFSET_FROM_BIT(frame);
        frames[idx] &= ~((u32) 1 << off);
    }
}

/* Determine if a physical frame available */
u32 mmu_frame_test(u64 frame_addr)
{
    if (!(frame_addr < nframes * PAGE_SIZE)) return 1;

    u64 frame = frame_addr >> PAGE_SHIFT;
    u64 idx = INDEX_FROM_BIT(frame);
    u64 off = OFFSET_FROM_BIT(frame);
    return frames[idx] & ((u32) 1 << off); // WARN perhaps double negation is needed here
}

/* Find the first available frame */
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

void mmu_frame_allocate(u64 flags)
{
    // TODO: Allocate :]
}
