#include "vga_print.h"

extern u8* end; /* End of kernel code */

extern void init_gdt();
extern void init_idt();
extern void mmu_init(size_t memsize, u64 first_free_page);
extern void pit_init();

void kmain(void* mb_info, u32 mb_magic)
{
    vga_terminal_initialize();

    init_gdt();
    init_idt();
    mmu_init(64*1024, (u64) &end);
    pit_init();

    vga_print("main dispatcher is here. Everything is good. Executing protocol \"nuidpocalypse\"\n");

    vga_print("The answer to all questions is: ");
    vga_print_num((u64) mb_magic);
    vga_print("\n");
}