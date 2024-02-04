#pragma once

#include <kernel/types.h>
#include <arch/x86/pml.h>

void page_allocator_init(void *first_free_page, size_t memsize);
void page_alloc(pml_entry *page, u64 flags);
void page_free(pml_entry *page);
size_t get_total_memory();
size_t get_used_memory();
size_t metadata_size(size_t memsize);
