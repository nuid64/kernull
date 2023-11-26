#pragma once

#include <kernel/types.h>
#include "pml.h"

/* Bitmap page allocator */
static volatile u32* frames;
static u64 nframes;

#define PAGE_SIZE 0x1000
#define PAGE_SHIFT 12

#define INDEX_FROM_BIT(b) ((b) >> 5)
#define OFFSET_FROM_BIT(b) ((b) & 0x1F)

void mmu_frame_set(u64 frame_addr);
void mmu_frame_clear(u64 frame_addr);
u32 mmu_frame_test(u64 frame_addr);
u64 mmu_first_frame();
void mmu_frame_allocate(pml_entry* page, u64 flags);
void mmu_frame_map_address(pml_entry* page, u64 flags, u64 phys_addr);
