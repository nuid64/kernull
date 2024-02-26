#include <kernel/mm.h>

#include <kernel/types.h>
#include <kernel/page_alloc.h>
#include <kernel/printk.h>
#include <kernel/kmalloc.h>
#include <arch/x86/asm.h>
#include <arch/x86/idt.h>
#include <arch/x86/regs.h>
#include <arch/x86/pml.h>
#include <assert.h>

#define KERNEL_PML_ACCESS 0x03
#define USER_PML_ACCESS   0x07
#define LARGE_PAGE_BIT    0x80

#define CANONICAL_MASK 0x0000FFFFFFFFFFFF
#define PHYS_MASK      0x0000007FFFFFFFFF

#define PML3_MASK  0x3FFFFFFF
#define PML2_MASK    0x1FFFFF
#define PML1_MASK       0xFFF
#define PML_ENTRY_MASK  0x1FF

#define TABLE_ATTRIBUTE __attribute((aligned(PAGE_SIZE)))
pml_entry initial_page_tables[3][512] TABLE_ATTRIBUTE = {0};
/* Kernel */
pml_entry high_base_pml4[512] TABLE_ATTRIBUTE = {0};
pml_entry high_base_pml3[512] TABLE_ATTRIBUTE = {0};
pml_entry high_base_pml2s[64][512] TABLE_ATTRIBUTE = {0};
pml_entry low_base_pml4s[2][512] TABLE_ATTRIBUTE = {0};
pml_entry low_base_pml2s[32][512] TABLE_ATTRIBUTE = {0};
/* Heap */
pml_entry heap_base_pml3[512] TABLE_ATTRIBUTE = {0};
pml_entry heap_base_pml2[512] TABLE_ATTRIBUTE = {0};
pml_entry heap_base_pml1[512*3] TABLE_ATTRIBUTE = {0};

pml_entry *current_pml4;

void page_fault_handler(struct regs *r);

void *mmu_to_virt(u64 phys_addr)
{
    return (void *) (phys_addr | HIGH_MAP_REGION);
}

/* Map 4KiB virtual page */
void map_addr(void *virt_addr, void *phys_addr, u64 flags)
{
    assert(((u64) phys_addr & PAGE_LOW_MASK) == 0);
    assert(((u64) virt_addr & PAGE_LOW_MASK) == 0);

    u64 page_addr = ((u64) virt_addr) >> PAGE_SHIFT;
    pml_entry *table = current_pml4;
    for (int i = 0; i < 3; ++i) {
        size_t idx = (page_addr >> (27 - i*9)) & PML_ENTRY_MASK;

        if (!table[idx].bits.present) {
            pml_entry table_page = {0};
            page_alloc(&table_page, PML_FLAG_WRITABLE);
            table[idx] = table_page;
        }

        void *next_table_addr = (void *) (table[idx].bits.address << PAGE_SHIFT);
        table = (pml_entry *) mmu_to_virt((u64) next_table_addr);
    }

    size_t idx = page_addr & PML_ENTRY_MASK;
    table[idx].bits.address = (u64) phys_addr;
    table[idx].bits.present = 1;
     /* Ignore stupid attempts to broke sth */
    flags = (flags & PML_FLAGS_MASK) & ~(PML_FLAG_HUGE);
    table[idx].full |= flags;
}

/* Get the physical address
   If page is not mapped, a negative value from -1 to -4 returned, which indicates which level
   of the page directory is unmapped (-1 = no PML4, -4 = no page in PML1) */
u64 mmu_translate(pml_entry *root, u64 virt_addr)
{
    u64 real_bits = virt_addr & CANONICAL_MASK;
    u64 page_addr = real_bits >> PAGE_SHIFT;

    size_t pml4_entry_idx = (page_addr >> 27) & PML_ENTRY_MASK;
    size_t pml3_entry_idx = (page_addr >> 18) & PML_ENTRY_MASK;
    size_t pml2_entry_idx = (page_addr >> 9)  & PML_ENTRY_MASK;
    size_t pml1_entry_idx = page_addr         & PML_ENTRY_MASK;

    if (!root[pml4_entry_idx].bits.present) return -1;

    u64 next_addr = root[pml4_entry_idx].bits.address << PAGE_SHIFT;

    pml_entry *pml3 = mmu_to_virt(next_addr);
    if (!pml3[pml3_entry_idx].bits.present) return -2;
    next_addr = pml3[pml3_entry_idx].bits.address << PAGE_SHIFT;

    if (pml3[pml3_entry_idx].bits.huge) return (next_addr | (virt_addr & PML3_MASK));

    pml_entry *pml2 = mmu_to_virt(next_addr);
    if (!pml2[pml2_entry_idx].bits.present) return -3;
    next_addr = pml2[pml2_entry_idx].bits.address << PAGE_SHIFT;

    if (pml2[pml2_entry_idx].bits.huge) return (next_addr | (virt_addr & PML2_MASK));

    pml_entry *pml1 = mmu_to_virt(next_addr);
    if (!pml1[pml1_entry_idx].bits.present) return -4;
    next_addr = pml1[pml1_entry_idx].bits.address << PAGE_SHIFT;

    return (next_addr | (virt_addr & PML1_MASK));
}

/* Get pml entry for a virtual address */
pml_entry *mmu_get_page(u64 virt_addr)
{
    u64 real_bits = virt_addr & CANONICAL_MASK;
    u64 page_addr = real_bits >> PAGE_SHIFT;

    u32 pml4_entry_idx = (page_addr >> 27) & PML_ENTRY_MASK;
    u32 pml3_entry_idx = (page_addr >> 18) & PML_ENTRY_MASK;
    u32 pml2_entry_idx = (page_addr >> 9)  & PML_ENTRY_MASK;
    u32 pml1_entry_idx = page_addr         & PML_ENTRY_MASK;

    if (!current_pml4[pml4_entry_idx].bits.present) return NULL;

    pml_entry *pml3 = mmu_to_virt(current_pml4[pml4_entry_idx].bits.address << PAGE_SHIFT);
    if (!pml3[pml3_entry_idx].bits.present) return NULL;

    pml_entry *pml2 = mmu_to_virt(pml3[pml3_entry_idx].bits.address << PAGE_SHIFT);
    if (!pml2[pml2_entry_idx].bits.present) return NULL;

    pml_entry *pml1 = mmu_to_virt(pml2[pml2_entry_idx].bits.address << PAGE_SHIFT);
    return &pml1[pml1_entry_idx];
}

/* Get amount of usable memory in KiB */
size_t mmu_total_memory()
{
    return get_total_memory();
}

/* Get amount of used memory in KiB */
size_t mmu_used_memory()
{
    return get_used_memory();
}

pml_entry *mmu_get_current_dir()
{
    return current_pml4;
}

pml_entry *mmu_get_kernel_dir()
{
    return mmu_to_virt((u64) &high_base_pml4);
}

void mmu_set_directory(pml_entry *new)
{
    if (!new) new = mmu_to_virt((u64) &high_base_pml4);
    current_pml4 = new;
    set_cr3((u64) new & PHYS_MASK);
}

/* Invalidate page */
void mmu_invalidate(u64 addr)
{
    invlpg(addr);
}

u8 mmu_get_page_deep(u64 virt_addr, pml_entry **pml4_out, pml_entry **pml3_out, pml_entry **pml2_out, pml_entry **pml1_out)
{
    u64 real_bits = virt_addr & CANONICAL_MASK;
    u64 page_addr = real_bits >> PAGE_SHIFT;

    u32 pml4_entry_idx  = (page_addr >> 27) & PML_ENTRY_MASK;
    u32 pml3_entry_idx  = (page_addr >> 18) & PML_ENTRY_MASK;
    u32 pml2_entry_idx  = (page_addr >> 9) & PML_ENTRY_MASK;
    u32 pml1_entry_idx  = page_addr & PML_ENTRY_MASK;

    // clean outputs
    // omitting pml4 because it will be assigned a value
    *pml3_out = NULL;
    *pml2_out = NULL;
    *pml1_out = NULL;

    *pml4_out = &current_pml4[pml4_entry_idx];
    if (!(**pml4_out).bits.present) return 1;

    pml_entry *pml3 = mmu_to_virt(current_pml4[pml4_entry_idx].bits.address << PAGE_SHIFT);
    *pml3_out = &pml3[pml3_entry_idx];
    if (!(**pml3_out).bits.present) return 1;

    pml_entry *pml2 = mmu_to_virt(pml3[pml3_entry_idx].bits.address << PAGE_SHIFT);
    *pml2_out = &pml2[pml2_entry_idx];
    if (!(**pml2_out).bits.present) return 1;

    pml_entry *pml1 = mmu_to_virt(pml2[pml2_entry_idx].bits.address << PAGE_SHIFT);
    *pml1_out = &pml1[pml1_entry_idx];
    if (!(**pml1_out).bits.present) return 1; // WARN: Maybe it's unnecessary

    return 0;
}

void mmu_init(size_t memsize, u64 kernel_end)
{
    current_pml4 = high_base_pml4;

    /* Map high base PML3 */
    high_base_pml4[511].full = (u64) &high_base_pml3 | KERNEL_PML_ACCESS;
    high_base_pml4[510].full = (u64) &heap_base_pml3 | KERNEL_PML_ACCESS;

    /* Identity map 128GiB from 0xFFFFFF8000000000 */
    for (u64 i = 0; i < 64; ++i) {
        high_base_pml3[i].full = (u64) &high_base_pml2s[i] | KERNEL_PML_ACCESS;
        for (u64 j = 0; j < 512; ++j) {
            high_base_pml2s[i][j].full = (((i << 30) + (j << 21)) | LARGE_PAGE_BIT | KERNEL_PML_ACCESS);
       }
    }

    /* Map low base */
    low_base_pml4s[0][0].full = (u64) &low_base_pml4s[1] | USER_PML_ACCESS;

    /* Map kernel space */
    u64 end_pml1r = ((u64) &kernel_end + PAGE_LOW_MASK) & PAGE_SIZE_MASK; // N bytes
    u64 low_pages = end_pml1r >> PAGE_SHIFT;                              // N pages
    low_pages = (low_pages + PAGE_LOW_MASK) & ~PAGE_LOW_MASK;             // round up a page
    u64 pml2_count = (low_pages + PML_ENTRY_MASK) >> 9;                   // N 512-page blocks
    for (size_t j = 0; j < pml2_count; ++j) {
        low_base_pml4s[1][j].full = (u64) &low_base_pml2s[j] | KERNEL_PML_ACCESS;
        for (int i = 0; i < 512; ++i)
            low_base_pml2s[j][i].full = (u64) (LARGE_PAGE_SIZE * j + PAGE_SIZE * i) | KERNEL_PML_ACCESS;
    }
    /* Unmap null */
    low_base_pml2s[0][0].full = 0;

    /* Map new low base */
    high_base_pml4[0].full = (u64) &low_base_pml4s[0] | USER_PML_ACCESS;

    /* Transition into illusory realm T.T */
    mmu_set_directory(current_pml4);
    current_pml4 = mmu_to_virt((u64) current_pml4);

    /* Setup heap for page allocator */
    heap_base_pml3[0].full = (u64) &heap_base_pml2       | KERNEL_PML_ACCESS;
    heap_base_pml2[0].full = (u64) &heap_base_pml1[0]    | KERNEL_PML_ACCESS;
    heap_base_pml2[1].full = (u64) &heap_base_pml1[512]  | KERNEL_PML_ACCESS;
    heap_base_pml2[2].full = (u64) &heap_base_pml1[1024] | KERNEL_PML_ACCESS;

    void *first_free_page = (void *) ((kernel_end + PAGE_LOW_MASK) & PAGE_SIZE_MASK);
    size_t metadata_bytes = metadata_size(memsize);
    size_t metadata_pages = ((metadata_bytes + PAGE_LOW_MASK) & PAGE_SIZE_MASK) >> PAGE_SHIFT;
    for (size_t i = 0; i < metadata_pages; ++i)
        heap_base_pml1[i].full = (((u64) first_free_page + (i << 12))) | KERNEL_PML_ACCESS;

    /* Setup page allocator */
    page_allocator_init(first_free_page, memsize);

    /* Setup heap allocator */
    void *heap_start = (void *) KERNEL_HEAP_START + (metadata_pages * PAGE_SIZE);
    kheap_allocator_init(heap_start);

    /* Set PAGE_FAULT handler */
    irq_set_handler(0x0E, (int_handler) page_fault_handler);
}

/* Page fault error code structure */
struct page_fault_err {
    // using u64 here because error code is quadword

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

void page_fault_handler(struct regs *r) {
    u64 fault_addr = get_cr2();

    struct page_fault_err *err = (struct page_fault_err *) &r->err_code;

    // fuck
    printk("Page fault at address 0x%x\n", fault_addr);

    // why
    printk("Cause: ");
    if (!err->present) {
        printk("page is not present ");
    }
    if (err->reserved_set) {
        printk("reserved bit set ");
    }
    if (err->prot_key_viol) {
        printk("protection-key violation ");
    }
    if (err->shadow_stack_acc) {
        printk("shadow-stack access ");
    }
    if (err->sgx_viol) {
        printk("SGX violation");
    }
    printk("\n");

    // when
    printk("On: ");
    if (err->instruction_fetch) {
        printk("instruction fetch");
    } else {
        printk("Data ");
        if (err->write) {
            printk("write");
        } else {
            printk("read");
        }
    }
    printk("\n");

    // blame
    printk("Blame: ");
    if (err->user) {
        printk("user"); // well, actually we can't blame any user at this point. Bummer
    } else {
        printk("kernel");
    }

    abort();
}
