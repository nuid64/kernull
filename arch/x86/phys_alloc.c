#include <stdint.h>
#include <kernel/string.h>
#include <arch/x86/phys_alloc.h>
#include <arch/x86/memory.h>

#define BITMAP_INDEX(b)	 ((b) >> 3)
#define BITMAP_OFFSET(b) ((b) & 0x07)

/* Bitmap page allocator */
static volatile uint8_t *frames;
static uint32_t nframes;

static size_t total_memory = 0;
static size_t unavailable_memory = 0;

void frame_set(size_t frame_addr);

/* Initialize page allocator */
void page_allocator_init(void *first_free_page, size_t memsize)
{
	nframes = (memsize >> PAGE_SHIFT);
	size_t metadata_bytes = nframes / (sizeof(*frames) * 8);

	/* Mark memory for page allocator */
	frames = (uint8_t *)KERNEL_HEAP_START;
	memset((void *)frames, 0x00, metadata_bytes);
	// TODO: Mark unavailable memory areas when start using all available memory regions

	size_t unavail = 0;
	for (; unavail < (size_t)first_free_page + metadata_bytes;
		 unavail += PAGE_SIZE)
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
		uint8_t off = BITMAP_OFFSET(frame);
		frames[idx] |= ((uint8_t)1 << off);
	}
}

/* Mark a physical frame as available */
void frame_clear(size_t frame_addr)
{
	if (frame_addr < nframes * PAGE_SIZE) {
		size_t frame = frame_addr >> PAGE_SHIFT;
		size_t idx = BITMAP_INDEX(frame);
		uint8_t off = BITMAP_OFFSET(frame);
		frames[idx] &= ~((uint8_t)1 << off);
	}
}

/* Determine if a physical frame available */
uint8_t frame_test(size_t frame_addr)
{
	if (!(frame_addr < nframes * PAGE_SIZE))
		return 1;

	size_t frame = frame_addr >> PAGE_SHIFT;
	size_t idx = BITMAP_INDEX(frame);
	uint8_t off = BITMAP_OFFSET(frame);
	return frames[idx] & ((uint8_t)1 << off);
}

/* Find the first available frame */
size_t first_free_frame(void)
{
	for (size_t i = 0; i < BITMAP_INDEX(nframes); ++i) {
		if (frames[i] == (uint8_t)~0)
			continue; // All frames are reserved
		for (size_t j = 0; j < 8; ++j) {
			uint8_t bit = 1 << j;
			if (!(frames[i] & bit))
				return (i << 3) + j;
		}
	}
	// FIXME: WTF should I return? Zero, cause it always is occupied by allocator's metadata?
	return 0;
}

void page_alloc(pml_entry *page, uint32_t flags)
{
	if (page->bits.address != 0)
		return; // Already allocated

	size_t idx = first_free_frame();
	uint32_t addr = idx << PAGE_SHIFT;
	frame_set(addr);
	page->bits.address = idx;
	page->bits.present = 1;
	flags = (flags & PML_FLAGS_MASK) & ~(PML_FLAG_HUGE);
	page->full |= flags;

	++unavailable_memory;
}

void page_free(pml_entry *page)
{
	uint32_t addr = page->bits.address;

	if (!frame_test(addr))
		return;

	frame_clear(page->bits.address);
	page->bits.address = 0; // Prevent use after free

	--unavailable_memory;
}

/* Get amount of usable memory in KiB */
size_t get_total_memory(void)
{
	return total_memory;
}

/* Get amount of used memory in KiB */
size_t get_used_memory(void)
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
