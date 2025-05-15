#include <arch/x86/memory.h>

static uint32_t page_directory[PAGE_ENTRIES]
	__attribute__((aligned(PAGE_SIZE)));
static uint32_t first_page_table[PAGE_ENTRIES]
	__attribute__((aligned(PAGE_SIZE)));

void paging_init(void)
{
	/* Identity map first 4MB */
	for (int i = 0; i < PAGE_ENTRIES; i++) {
		first_page_table[i] = (i * PAGE_SIZE) | 0b11; // Present, RW
	}

	page_directory[0] = ((uint32_t)first_page_table) | 0b11;

	/* Load page directory */
	__asm__ volatile("mov %0, %%cr3" ::"r"(page_directory));
	uint32_t cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000; // Enable paging
	__asm__ volatile("mov %0, %%cr0" ::"r"(cr0));
}
