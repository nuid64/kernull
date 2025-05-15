#pragma once

#include <arch/x86/pml.h>

#include <stddef.h>
#include <stdint.h>

void page_allocator_init(void *first_free_page, size_t memsize);
void page_alloc(pml_entry *page, uint32_t flags);
void page_free(pml_entry *page);
size_t get_memory_total(void);
size_t get_memory_used(void);
size_t get_memory_free(void);
size_t metadata_size(size_t memsize);
