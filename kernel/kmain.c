#include "pml.h"
#include "vga_print.h"
#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "mmu.h"
#include "kmalloc.h"

extern u8* end;

void kmain(void* mb_info, u32 mb_magic)
{
    vga_terminal_initialize();

    init_gdt();
    init_idt();
    mmu_init(64*1024, (u64) &end);
    pit_init();

    vga_print("kmain dispatcher is here. Everything is good. Executing protocol \"nuidpocalypse\"\n");

    vga_print("The answer to all questions is: ");
    vga_print_num((u64) mb_magic);
    vga_print("\n");
}
