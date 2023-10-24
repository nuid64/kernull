#include "vga_print.h"
#include "gdt.h"
#include "idt.h"
#include "pit.h"

void kmain()
{
    vga_terminal_initialize();

    init_gdt();
    init_idt();
    pit_init();

    vga_print("My ma... oh, no, I'M TICKING AWAY! HlEPP!P!!!!\n");

    asm volatile ("sti"); // enable interrupts :P
}
