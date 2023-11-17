#include "pml.h"
#include "vga_print.h"
#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "mmu.h"

extern u8* end;

void kmain()
{
    vga_terminal_initialize();

    init_gdt();
    init_idt();
    mmu_init(64*1024, (u64) &end);
    pit_init();

    vga_print("kmain dispatcher is here. Everything is good. Executing protocol \"nuidpocalypse\"\n");
}
