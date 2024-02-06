#pragma once

#include <stddef.h>

void kheap_allocator_init(void *start);
void* kmalloc(size_t size);
void kfree(void *ptr);
