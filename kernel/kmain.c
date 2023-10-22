#include "vga_print.h"
#include "gdt.h"
#include "idt.h"

void kmain()
{
    vga_terminal_initialize();

    init_gdt();
    init_idt();

    vga_print("My master is nuid64 <3\n");
    asm volatile ("int $0x03");            // test interrupt
}
