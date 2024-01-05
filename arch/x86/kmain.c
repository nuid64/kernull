#include "multiboot2.h"
#include "multiboot2_parser.h"
#include <kernel/vga_print.h>

extern u64 kernel_end; /* End of kernel code */

extern void gdt_init();
extern void idt_init();
extern void mmu_init(size_t memsize, u64 kernel_end);
extern void pit_init();

void kmain(u64 mb_info_addr, u32 mb_magic)
{
    vga_terminal_initialize();

    if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        vga_print("Fucked up magic number\n");

        return;
    }
    struct multiboot_tag* tags = (struct multiboot_tag*) (mb_info_addr + 8);

     /* Highest physicall address available */
    u64 memory_end = multiboot_get_memory_end(tags);
    u64 memsize = memory_end + 1 - 1024*1024; // first mebibyte is shitful...

    gdt_init();
    idt_init();
    mmu_init(memory_end, kernel_end);
    pit_init();

    vga_print("Kernel end: ");
    vga_print_num((u64) &kernel_end);
    vga_print("\n");
    vga_print("Highest address: ");
    vga_print_num(memory_end);
    vga_print("\n");
    vga_print("Memory size: ");
    vga_print_num(memsize);
    vga_print("\nConvert it to MiB/KiB by yourself.\n");

    vga_print("kmain dispatcher is here. So far so good. "
              "Executing protocol \"nuidpocalypse\"\n");
}
