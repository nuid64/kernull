#include "vga_print.h"
#include "gdt.h"

void kmain()
{
    init_gdt();
    vga_terminal_initialize();

    vga_print("My master is nuid64 <3");
}
