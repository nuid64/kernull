#include <kernel/page_alloc.h>

#include <kernel/types.h>
#include <kernel/printk.h>
#include <kernel/string.h>
#include <kernel/mm.h>
#include <arch/x86_64/pml.h>

#define BITMAP_INDEX(b) ((b) >> 3)
#define BITMAP_OFFSET(b) ((b) & 0x07)

/* Bitmap page allocator */
static volatile u8 *frames;
static u32 nframes;

static size_t total_memory = 0;
static size_t unavailable_memory = 0;

void frame_set(size_t frame_addr);

/* Initialize page allocator */
void page_allocator_init(void *first_free_page, size_t memsize) {
    nframes = (memsize >> PAGE_SHIFT);
    size_t metadata_bytes = nframes / (sizeof(*frames) * 8);

    /* Mark memory for page allocator */
    frames = (u8 *) KERNEL_HEAP_START;
    memset((void *) frames, 0x00, metadata_bytes);
    // TODO: Mark unavailable memory areas when start using all available memory regions

    size_t unavail = 0;
    for (; unavail < (size_t) first_free_page + metadata_bytes; unavail += PAGE_SIZE)
        frame_set(unavail);

    size_t avail = nframes - unavail;

    total_memory = avail * PAGE_SIZE;
    unavailable_memory = unavail * PAGE_SIZE;
}

/* Mark a physical frame as in use */
void frame_set(size_t frame_addr)
{
    if (frame_addr < nframes * PAGE_SIZE) {
        size_t frame = frame_addr >> PAGE_SHIFT;
        size_t idx = BITMAP_INDEX(frame);
        u8 off = BITMAP_OFFSET(frame);
        frames[idx] |= ((u8) 1 << off);
    }
}

/* Mark a physical frame as available */
void frame_clear(size_t frame_addr)
{
    if (frame_addr < nframes * PAGE_SIZE) {
        size_t frame = frame_addr >> PAGE_SHIFT;
        size_t idx = BITMAP_INDEX(frame);
        size_t off = BITMAP_OFFSET(frame);
        frames[idx] &= ~((u8) 1 << off);
    }
}

/* Determine if a physical frame available */
u8 frame_test(size_t frame_addr)
{
    if (!(frame_addr < nframes * PAGE_SIZE)) return 1;

    size_t frame = frame_addr >> PAGE_SHIFT;
    size_t idx = BITMAP_INDEX(frame);
    size_t off = BITMAP_OFFSET(frame);
    return frames[idx] & ((u8) 1 << off);
}

/* Find the first available frame */
size_t first_free_frame()
{
    for (size_t i = 0; i < BITMAP_INDEX(nframes); ++i) {
        if (frames[i] == (u8) ~0) continue; // all frames are reserved
        for (size_t j = 0; j < 8; ++j) {
            size_t bit = 1 << j;
            if ( !(frames[i] & bit) )
                return (i << 3) + j;
        }
    }
    // FIXME: wtf should I return? Zero, cause it always is occupied by allocator's metadata?
}

void page_alloc(pml_entry *page, u64 flags)
{
    if (page->bits.address != 0) return; // already allocated

    size_t idx = first_free_frame();
    u64 addr = idx << PAGE_SHIFT;
    frame_set(addr);
    page->bits.address = idx;
    page->bits.present = 1;
    flags = (flags & PML_FLAGS_MASK) & ~(PML_FLAG_HUGE);
    page->full |= flags;

    ++unavailable_memory;
}

void page_free(pml_entry *page)
{
    u64 addr = page->bits.address;

    if (!frame_test(addr)) return;

    frame_clear(page->bits.address);
    page->bits.address = 0; // prevent use after free

    --unavailable_memory;
}

/* Get amount of usable memory in KiB */
size_t get_total_memory()
{
    return total_memory;
}

/* Get amount of used memory in KiB */
size_t get_used_memory()
{
    return unavailable_memory;
}

/* Get the amount of pages required for the metadata for this memory size */
size_t metadata_size(size_t memsize)
{
    size_t nframes = (memsize >> PAGE_SHIFT);
    size_t metadata_bytes = nframes >> sizeof(*frames);

    return metadata_bytes;
}
