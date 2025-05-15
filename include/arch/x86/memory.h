#pragma once

#include <stdint.h>

#define KERNEL_HEAP_START  0xFFFFFF0000000000
#define KERNEL_STACK_SIZE  256 * 1024
#define KERNEL_STACK_END   0xFFFF800000001000
#define KERNEL_STACK_START KERNEL_STACK_END + KERNEL_STACK_SIZE
#define HIGH_MAP_REGION    0xFFFFFF8000000000

#define PAGE_SIZE      4096
#define PAGE_SIZE_MASK 0xFFFFF000
#define PAGE_LOW_MASK  0x00000FFF
#define PAGE_SHIFT     12

#define PAGE_ENTRIES 1024

typedef uint32_t page_dir_entry_t;
typedef uint32_t page_table_entry_t;

void paging_init(void);
