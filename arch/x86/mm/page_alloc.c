#include <kernel/page_alloc.h>
#include <kernel/types.h>
#include <kernel/mm.h>
#include "arch/x86/asm.h"
#include "arch/x86/pml.h"

#define INDEX_FROM_BIT(b) ((b) >> 5)
#define OFFSET_FROM_BIT(b) ((b) & 0x1F)

/* Bitmap page allocator */
static volatile u32* frames;
static u32 nframes;

static size_t total_memory = 0;
static size_t unavailable_memory = 0;

/* Initialize page allocator. Returns number of pages it requires */
void page_allocator_init(u64 first_free_page, u64 memsize) {
    nframes = (memsize >> PAGE_SHIFT);
    u64 metadata_bytes = nframes >> sizeof(*frames);

    /* Mark memory for page allocator */
    frames = (void*) (u64) KERNEL_HEAP_START;
    memset((void*) frames, 0x00, metadata_bytes);
    // TODO: Mark unavailable memory areas when start using all available memory regions

    for (size_t i = 0; i < first_free_page + metadata_bytes; i += PAGE_SIZE)
        frame_set(i);

    size_t unavail = 0;
    for (size_t i = 0; i < INDEX_FROM_BIT(nframes); ++i)
        unavail += popcntl(frames[i]);
    size_t avail = nframes - unavail;

    total_memory = avail * PAGE_SIZE;
    unavailable_memory = unavail * PAGE_SIZE;
}

/* Mark a physical frame as in use */
void frame_set(u64 frame_addr)
{
    if (frame_addr < nframes * PAGE_SIZE) {
        u64 frame = frame_addr >> PAGE_SHIFT;
        u64 idx = INDEX_FROM_BIT(frame);
        u32 off = OFFSET_FROM_BIT(frame);
        frames[idx] |= ((u32) 1 << off);
    }
}

/* Mark a physical frame as available */
void frame_clear(u64 frame_addr)
{
    if (frame_addr < nframes * PAGE_SIZE) {
        u64 frame = frame_addr >> PAGE_SHIFT;
        u64 idx = INDEX_FROM_BIT(frame);
        u64 off = OFFSET_FROM_BIT(frame);
        frames[idx] &= ~((u32) 1 << off);
    }
}

/* Determine if a physical frame available */
u32 frame_test(u64 frame_addr)
{
    if (!(frame_addr < nframes * PAGE_SIZE)) return 1;

    u64 frame = frame_addr >> PAGE_SHIFT;
    u64 idx = INDEX_FROM_BIT(frame);
    u64 off = OFFSET_FROM_BIT(frame);
    return frames[idx] & ((u32) 1 << off); // WARN: perhaps double negation is needed here
}

/* Find the first available frame */
u64 first_free_frame()
{
    for (u64 i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
        if (frames[i] == (u32) ~0) continue; // all frames are reserved
        for (u64 j = 0; j < 32; ++j) {
            u64 bit = 1 << j;
            if ( !(frames[i] & bit) )
                return (i << 5) + j;
        }
    }
    // FIXME: wtf should I return? Error handling here
}

void page_alloc(pml_entry* page, u64 flags)
{
    if (page->bits.address != 0) return; // already allocated

    u64 idx = first_free_frame();
    u64 addr = idx << PAGE_SHIFT;
    frame_set(addr);
    page->bits.address = addr;
    page->bits.present = 1;
    page->full |= flags;
}

void page_free(pml_entry* page)
{
    page->bits.address = 0; // prevent use after free
    frame_clear(page->bits.address);
}

/* Get amount of usable memory in KiB */
size_t get_total_memory()
{
    return total_memory;
}

/* Get amount of used memory in KiB */
size_t get_used_memory()
{
    size_t used = 0;
    for (u64 i = 0; i < INDEX_FROM_BIT(nframes); ++i)
        for (u64 j = 0; j < 32; ++j) {
            u32 bit = 1 << j;
            if (frames[i] & bit)
                ++used;
        }
    return used * 4 - unavailable_memory;
}

/* Get the amount of pages required for the metadata for this memory size */
size_t metadata_size(u64 memsize)
{
    u64 nframes = (memsize >> PAGE_SHIFT);
    u64 metadata_bytes = nframes >> sizeof(*frames);

    return metadata_bytes;
}
