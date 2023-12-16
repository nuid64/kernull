#pragma once

#include "pml.h"

#define KERNEL_HEAP_START  0xFFFFFF0000000000
#define KERNEL_STACK_SIZE  256 * 1024
#define KERNEL_STACK_END   0xFFFF800000001000
#define KERNEL_STACK_START KERNEL_STACK_END + KERNEL_STACK_SIZE
#define HIGH_MAP_REGION    0xFFFFFF8000000000

#define MMU_FLAG_WRITABLE     0x02
#define MMU_FLAG_USER         0x04
#define MMU_FLAG_WRITETHROUGH 0x08
#define MMU_FLAG_NOCACHE      0x10
#define MMU_FLAG_HUGE         0x80
#define MMU_FLAG_GLOBAL       0x100
#define MMU_FLAG_NOEXECUTE    0x8000000000000000

#define MMU_PTR_NULL  1
#define MMU_PTR_WRITE 2

void       mmu_init(size_t memsize, u64 first_free_page);

pml_entry* mmu_get_current_dir();
pml_entry* mmu_get_kernel_dir();
void*      mmu_get_mapped(u64 frame_addr);
u64        mmu_translate(pml_entry* root, u64 virt_addr);
pml_entry* mmu_get_page(u64 virt_addr);
void       mmu_set_directory(pml_entry* new);
void       mmu_invalidate(u64 addr);
u8         mmu_get_page_deep(u64 virt_addr, pml_entry** pml4_out, pml_entry** pml3_out,
                             pml_entry** pml2_out, pml_entry** pml1_out);
void*      memset(void* dest, int c, size_t n); // FIXME move it out?
size_t     mmu_total_memory();
size_t     mmu_used_memory();

/* Page fault error code structure */
struct page_fault_err {
    // using u64 here because error code is qword
    u64 present           : 1; /* Present */
    u64 write             : 1; /* Read/Write */
    u64 user              : 1; /* Supervisor/User */
    u64 reserved_set      : 1; /* Reserved bit was set */
    u64 instruction_fetch : 1; /* Data access/Instruction fetch */
    u64 prot_key_viol     : 1; /* Protection-key violation */
    u64 shadow_stack_acc  : 1; /* Shadow-stack access */
    u64 _padding          : 8;
    u64 sgx_viol          : 1; /* SGX violation */
};
