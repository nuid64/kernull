#include <kernel/kmalloc.h>

#include <kernel/types.h>
#include <kernel/printk.h>
#include <kernel/page_alloc.h>
#include <kernel/mm.h>
#include <assert.h>

#ifdef ARCH_X86_64
#include <arch/x86_64/pml.h>
#endif

/* Approach
 * Blocks are stored in-place and contains their size and, if free, pointer to the next free block
 * Block's 'next free' pointer is a part of the block's memory and NULLified on the block's reservation
 * Blocks in the list are ordered by their addresses
 * On allocation: take the First Fit block or create one if no block are big enough
 * On free: insert freed block back into the list keeping order and coalesce it with adjacent blocks
 */

struct block {
    size_t size;
    struct block* next;
} typedef block_t;

block_t *split_block(block_t *block, size_t size);
int are_adjacent(block_t *block, block_t *next);
void coalesce_blocks(block_t *block, block_t *next);

static block_t *free_block_list;
static void *heap_start;
static void *heap_end;

// TODO: Implement heap size decreasing
void *sbrk(u64 incr)
{
    if (incr == 0) return heap_end;
    incr = (incr + PAGE_LOW_MASK) & ~PAGE_LOW_MASK;
    void *heap_end_old = heap_end;

    // Allocate and map some pages
    u64 pages_to_alloc = incr / PAGE_SIZE;
    for (u64 i = 0; i < pages_to_alloc; ++i) {
        pml_entry page = {0};
        page_alloc(&page, PML_FLAG_WRITABLE);

        void *paddr = (void *) (page.bits.address << PAGE_SHIFT);
        map_addr(heap_end, paddr, PML_FLAG_WRITABLE);

        heap_end += PAGE_SIZE;
    }

    return heap_end_old;
}

void kheap_allocator_init(void *start)
{
    assert((u64) start >= KERNEL_HEAP_START);   // be sane
    assert(((u64) start & PAGE_LOW_MASK) == 0); // be page-aligned

    heap_start = start;
    heap_end = start;

    free_block_list = NULL;
}

/* WARN: Returns page-unaligned address */
__attribute((malloc))
void *kmalloc(size_t size)
{
    /* Sanity check */
    if (size > mmu_total_memory()) {
        printk("kmalloc: You're asking for too much (%dKiB). Go jerk off and get some sleep.", size / 1024);
        return NULL;
    }

    /* No blocks */
    if (free_block_list == NULL) {
        free_block_list = sbrk(size + sizeof(size_t));
        free_block_list->size = (u64) heap_end - (u64) free_block_list;
        free_block_list->next = NULL;
    }

    /* First Fit allocation */

    /* First block check */
    if (free_block_list->size >= size) {
        // Split block
        block_t *block_old = free_block_list;
        free_block_list = split_block(free_block_list, size);

        /* Skip the size of the block before it */
        return &block_old->next;
    }

    /* Next blocks check */
    block_t *prev = free_block_list;
    block_t *block = prev->next;

    while (block != NULL) {
        if (block->size < size) {
            prev = block;
            block = block->next;
        } else {
            // Split block
            prev->next = split_block(block, size);

            /* Skip the size of the block before it */
            return &block->next;
        }
    }

    /* Unsatisfied. I'll start my own block, with size and bytes */
    block_t *new = sbrk(size + sizeof(size_t));
    new->size = size;
    new->next = NULL;

    return &new->next;
}

void kfree(void *ptr)
{
    block_t *freed = (block_t *) (((u8 *) ptr) - sizeof(size_t)); // Grab size
    freed->next = NULL;

    /* No blocks */
    if (free_block_list == NULL) {
        free_block_list = freed;
        return;
    }

    /* One block */
    if (free_block_list->next == NULL) {
        block_t *left, *right;
        if (free_block_list < freed) {
            left = free_block_list;
            right = freed;
        } else {
            left = freed;
            right = free_block_list;
        }

        left->next = right;
        free_block_list = left;
        if (are_adjacent(left, right))
            coalesce_blocks(left, right);

        return;
    }

    /* Many blocks */
    block_t *prev = free_block_list;
    block_t *block = prev->next;

    while (block < freed && block != NULL) {
        prev = block;
        block = block->next;
    }

    prev->next = freed;
    freed->next = block;

    if (block != NULL && are_adjacent(freed, block)) {
        coalesce_blocks(freed, block);
    }
    if (are_adjacent(prev, freed)) {
        coalesce_blocks(prev, freed);
    }
} __attribute((malloc));

/* Split block and return next free block */
block_t *split_block(block_t *block, size_t size)
{
    size_t rem = block->size - size;

    if (rem > sizeof(block_t)) {
        block->size = size;

        block_t *rem_block = (block_t *) (((size_t) block) + sizeof(size_t) + size);
        rem_block->size = rem - sizeof(size_t);
        rem_block->next = block->next;
        block->next = NULL;

        return rem_block;
    }

    return block->next;
}

int are_adjacent(block_t *block, block_t *next)
{
    return (((size_t) block) + (block->size + sizeof(size_t)) == (size_t) next);
}

void coalesce_blocks(block_t *block, block_t *next)
{
    block->size += next->size + sizeof(size_t);
    block->next = next->next;

    next->size = 0;
    next->next = NULL;
}
