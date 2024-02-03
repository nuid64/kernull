#pragma once

/* INFO: Yes. Including arch-dependent code in (wannabe)arch-independent header.
 * Only x86 is supported at the time. Fuck off.
 */
#include <arch/x86/pml.h>

#define KERNEL_HEAP_START  0xFFFFFF0000000000
#define KERNEL_STACK_SIZE  256 * 1024
#define KERNEL_STACK_END   0xFFFF800000001000
#define KERNEL_STACK_START KERNEL_STACK_END + KERNEL_STACK_SIZE
#define HIGH_MAP_REGION    0xFFFFFF8000000000

#define PAGE_SIZE         0x1000
#define LARGE_PAGE_SIZE 0x200000
#define PAGE_SIZE_MASK 0xFFFFFFFFFFFFF000
#define PAGE_LOW_MASK  0x0000000000000FFF
#define PAGE_SHIFT 12

void       mmu_init(size_t memsize, u64 first_free_page);

void       map_addr(void* virt_addr, void* phys_addr, u64 flags);
pml_entry* mmu_get_current_dir();
pml_entry* mmu_get_kernel_dir();
u64        mmu_translate(pml_entry* root, u64 virt_addr);
pml_entry* mmu_get_page(u64 virt_addr);
void       mmu_set_directory(pml_entry* new);
void       mmu_invalidate(u64 addr);
u8         mmu_get_page_deep(u64 virt_addr, pml_entry** pml4_out, pml_entry** pml3_out,
                             pml_entry** pml2_out, pml_entry** pml1_out);
void*      memset(void* dest, int c, size_t n); // TODO: Make string.h for such things
size_t     mmu_total_memory();
size_t     mmu_used_memory();
