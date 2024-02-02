#pragma once

#include <kernel/types.h>
#include <arch/x86/pml.h>

void page_allocator_init(u64 first_free_page, u64 memsize);
void page_alloc(pml_entry* page, u64 flags);
void page_free(pml_entry* page);
size_t get_total_memory();
size_t get_used_memory();
size_t metadata_size(u64 memsize);
