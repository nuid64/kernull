#include <kernel/types.h>
#include "mmu.h"
#include "pml.h"
#include "allocator.h"
#include "idt.h"

#define KERNEL_PML_ACCESS 0x03
#define USER_PML_ACCESS   0x07
#define LARGE_PAGE_BIT    0x80

#define CANONICAL_MASK 0x0000FFFFFFFFFFFF
#define PHYS_MASK      0x0000007FFFFFFFFF

#define LARGE_PAGE_SIZE 0x200000
#define PAGE_SIZE         0x1000
#define PAGE_SHIFT 12
#define PAGE_SIZE_MASK 0xFFFFFFFFFFFFF000
#define PAGE_LOW_MASK  0x0000000000000FFF

#define PML3_MASK  0x3FFFFFFF
#define PML2_MASK    0x1FFFFF
#define PT_MASK  PAGE_LOW_MASK
#define ENTRY_MASK      0x1FF

static size_t total_memory = 0;
static size_t unavailable_memory = 0;

pml_entry initial_page_tables[3][512] __attribute((aligned(PAGE_SIZE))) = {0};
// kernel
pml_entry high_base_pml4[512] __attribute((aligned(PAGE_SIZE))) = {0};
pml_entry high_base_pml3[512] __attribute((aligned(PAGE_SIZE))) = {0};
pml_entry high_base_pml2s[64][512] __attribute((aligned(PAGE_SIZE))) = {0};
pml_entry low_base_pml4s[2][512] __attribute((aligned(PAGE_SIZE))) = {0};
pml_entry low_base_pml2s[32][512] __attribute((aligned(PAGE_SIZE))) = {0};
// heap
pml_entry heap_base_pml3[512] __attribute((aligned(PAGE_SIZE))) = {0};
pml_entry heap_base_pml2[512] __attribute((aligned(PAGE_SIZE))) = {0};
pml_entry heap_base_pml1[512*3] __attribute((aligned(PAGE_SIZE))) = {0};

pml_entry* current_pml4;

void* mmu_to_virt(u64 phys_addr)
{
    return (void*) (phys_addr | HIGH_MAP_REGION);
}

// get the physical address
// if page is not mapped, a negative value from -1 to -4 returned, which indicates which level
// of the page directory is unmapped (-1 = no PML4, -4 = no page in PML1)
u64 mmu_to_phys(pml_entry* root, u64 virt_addr)
{
    u64 real_bits = virt_addr & CANONICAL_MASK;
    u64 page_addr = real_bits >> PAGE_SHIFT;

    u32 pml4_entry_idx = (page_addr >> 27) & ENTRY_MASK;
    u32 pml3_entry_idx = (page_addr >> 18) & ENTRY_MASK;
    u32 pml2_entry_idx = (page_addr >> 9)  & ENTRY_MASK;
    u32 pml1_entry_idx = page_addr         & ENTRY_MASK;

    if (!root[pml4_entry_idx].bits.present) return -1;

    u64 next_addr = root[pml4_entry_idx].bits.address << PAGE_SHIFT;

    pml_entry* pml3 = mmu_to_virt(next_addr);
    if (!pml3[pml3_entry_idx].bits.present) return -2;
    next_addr = pml3[pml3_entry_idx].bits.address << PAGE_SHIFT;

    if (pml3[pml3_entry_idx].bits.huge) return (next_addr | (virt_addr & PML3_MASK));

    pml_entry* pml2 = mmu_to_virt(next_addr);
    if (!pml2[pml2_entry_idx].bits.present) return -3;
    next_addr = pml2[pml2_entry_idx].bits.address << PAGE_SHIFT;

    if (pml2[pml2_entry_idx].bits.huge) return (next_addr | (virt_addr & PML2_MASK));

    pml_entry* pml1 = mmu_to_virt(next_addr);
    if (!pml1[pml1_entry_idx].bits.present) return -4;
    next_addr = pml1[pml1_entry_idx].bits.address << PAGE_SHIFT;

    return (next_addr | (virt_addr & PT_MASK));
}

// get pml entry for a virtual address
pml_entry* mmu_get_page(u64 virt_addr)
{
    u64 real_bits = virt_addr & CANONICAL_MASK;
    u64 page_addr = real_bits >> PAGE_SHIFT;

    u32 pml4_entry_idx = (page_addr >> 27) & ENTRY_MASK;
    u32 pml3_entry_idx = (page_addr >> 18) & ENTRY_MASK;
    u32 pml2_entry_idx = (page_addr >> 9)  & ENTRY_MASK;
    u32 pml1_entry_idx = page_addr         & ENTRY_MASK;

    if (!current_pml4[pml4_entry_idx].bits.present) return NULL;

    pml_entry* pml3 = mmu_to_virt(current_pml4[pml4_entry_idx].bits.address << PAGE_SHIFT);
    if (!pml3[pml3_entry_idx].bits.present) return NULL;

    pml_entry* pml2 = mmu_to_virt(pml3[pml3_entry_idx].bits.address << PAGE_SHIFT);
    if (!pml2[pml2_entry_idx].bits.present) return NULL;

    pml_entry* pml1 = mmu_to_virt(pml2[pml2_entry_idx].bits.address << PAGE_SHIFT);
    return &pml1[pml1_entry_idx];
}

// get amount of usable memory in KiB
size_t mmu_total_memory()
{
    return total_memory;
}

// get amount of used memory in KiB
size_t mmu_used_memory()
{
    size_t used = 0;
    for (u64 i = 0; i < INDEX_FROM_BIT(nframes); ++i)
        for (u64 j = 0; j < 32; ++j) {
            u32 bit = 1 << j;
            if (frames[i] & bit)
                ++used;
        }
    return used * 4 - unavailable_memory;
}

pml_entry* mmu_get_current_dir()
{
    return current_pml4;
}

pml_entry* mmu_get_kernel_dir()
{
    return mmu_to_virt((u64) &high_base_pml4);
}

void mmu_set_directory(pml_entry* new)
{
    if (!new) new = mmu_to_virt((u64) &high_base_pml4);
    current_pml4 = new;
    asm volatile ("movq %0, %%cr3" : : "r" ((u64) new & PHYS_MASK));
}

void mmu_invalidate(u64 addr)
{
    asm volatile ("invlpg (%0)" : : "r"(addr));
}

u8 mmu_get_page_deep(u64 virt_addr, pml_entry** pml4_out, pml_entry** pml3_out, pml_entry** pml2_out, pml_entry** pml1_out)
{
    u64 real_bits = virt_addr & CANONICAL_MASK;
    u64 page_addr = real_bits >> PAGE_SHIFT;

    u32 pml4_entry_idx = (page_addr >> 27) & ENTRY_MASK;
    u32 pml3_entry_idx  = (page_addr >> 18) & ENTRY_MASK;
    u32 pml2_entry_idx   = (page_addr >> 9) & ENTRY_MASK;
    u32 pml1_entry_idx   = page_addr & ENTRY_MASK;

    // clean outputs
    // omitting pml4 because it will be assigned a value
    *pml3_out = NULL;
    *pml2_out = NULL;
    *pml1_out = NULL;

    *pml4_out = &current_pml4[pml4_entry_idx];
    if (!(**pml4_out).bits.present) return 1;

    pml_entry* pml3 = mmu_to_virt(current_pml4[pml4_entry_idx].bits.address << PAGE_SHIFT);
    *pml3_out = &pml3[pml3_entry_idx];
    if (!(**pml3_out).bits.present) return 1;

    pml_entry* pml2 = mmu_to_virt(current_pml4[pml4_entry_idx].bits.address << PAGE_SHIFT);
    *pml2_out = &pml2[pml2_entry_idx];
    if (!(**pml2_out).bits.present) return 1;

    pml_entry* pml1 = mmu_to_virt(current_pml4[pml4_entry_idx].bits.address << PAGE_SHIFT);
    *pml1_out = &pml1[pml1_entry_idx];
    if (!(**pml1_out).bits.present) return 1; // WARN maybe obsolette

    return 0;
}

extern u8* end;

void* memset(void* dest, int c, size_t n)
{
    asm volatile (
        "cld; rep stosb"
        : "=c"((int){0})
        : "rdi"(dest), "a"(c), "c"(n)
        : "flags", "memory", "rdi"
    );
    return dest;
}

void mmu_init(size_t memsize, u64 first_free_page)
{
    current_pml4 = high_base_pml4;

    // map high base PML3
    high_base_pml4[511].full = (u64) &high_base_pml3 | KERNEL_PML_ACCESS;
    high_base_pml4[510].full = (u64) &heap_base_pml3 | KERNEL_PML_ACCESS;

    // identity map from 0xFFFFFFE000000000
    for (u64 i = 0; i < 64; ++i) {
        high_base_pml3[i].full = (u64) &high_base_pml2s[i] | KERNEL_PML_ACCESS;
        for (u64 j = 0; j < 512; ++j) {
            high_base_pml2s[i][j].full = (((i << 30) + (j << 21)) | LARGE_PAGE_BIT | KERNEL_PML_ACCESS);
       }
    }

    // map low base
    low_base_pml4s[0][0].full = (u64) &low_base_pml4s[1] | USER_PML_ACCESS;

    // map kernel space
    u64 end_pml1r = ((u64) &end + PAGE_LOW_MASK) & PAGE_SIZE_MASK; // N bytes
    u64 low_pages = end_pml1r >> PAGE_SHIFT;                       // N pages
    u64 pml2_count = (low_pages + ENTRY_MASK) >> 9;                // N 512-page blocks
    for (size_t j = 0; j < pml2_count; ++j) {
        low_base_pml4s[1][j].full = (u64) &low_base_pml2s[j] | KERNEL_PML_ACCESS;
        for (int i = 0; i < 512; ++i)
            low_base_pml2s[j][i].full = (u64) (LARGE_PAGE_SIZE * j + PAGE_SIZE * i) | KERNEL_PML_ACCESS;
    }
    // unmap null
    low_base_pml2s[0][0].full = 0;

    // map new low base
    high_base_pml4[0].full = (u64) &low_base_pml4s[0] | USER_PML_ACCESS;

    // setup bitmap allocator
    nframes = (memsize >> PAGE_SHIFT);
    u64 bytes_of_frames = INDEX_FROM_BIT(nframes * 8);
    bytes_of_frames = (bytes_of_frames + PAGE_LOW_MASK) & PAGE_SIZE_MASK;
    first_free_page = (first_free_page + PAGE_LOW_MASK) & PAGE_SIZE_MASK;
    u64 pages_of_frames = bytes_of_frames >> PAGE_SHIFT;

    // setup heap
    heap_base_pml3[0].full = (u64) &heap_base_pml2      | KERNEL_PML_ACCESS;
    heap_base_pml2[0].full = (u64) &heap_base_pml1[0]    | KERNEL_PML_ACCESS;
    heap_base_pml2[1].full = (u64) &heap_base_pml1[512]  | KERNEL_PML_ACCESS;
    heap_base_pml2[2].full = (u64) &heap_base_pml1[1024] | KERNEL_PML_ACCESS;

    for (size_t i = 0; i < pages_of_frames; ++i)
        heap_base_pml1[i].full = (first_free_page + (i << 12)) | KERNEL_PML_ACCESS;

    // IMPORTANT
    current_pml4 = mmu_to_virt((u64) current_pml4);

    frames = (void*) (u64) KERNEL_HEAP_START;
    memset((void*) frames, 0xFF, bytes_of_frames);

    size_t unavail = 0, avail = 0;
    for (size_t i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
        for (size_t j = 0; j < 32; ++j) {
            uint32_t testFrame = (uint32_t)0x1 << j;
            if (frames[i] & testFrame) {
                unavail++;
            } else {
                avail++;
            }
        }
    }

    total_memory = avail * 4;
    unavailable_memory = unavail * 4;

    // mark everything as in use
    for (size_t i = 0; i < first_free_page + bytes_of_frames; i += PAGE_SIZE)
        mmu_frame_set(i);

    mmu_set_directory(current_pml4);
}
