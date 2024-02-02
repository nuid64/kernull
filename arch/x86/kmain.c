#include <kernel/page_alloc.h>
#include <multiboot2.h>
#include <multiboot2_parser.h>
#include <kernel/vga_print.h>
#include <arch/x86/pml.h>
#include <kernel/page_alloc.h>
#include <kernel/mm.h>

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

    gdt_init();
    idt_init();
    mmu_init(memory_end, kernel_end);
    pit_init();

    pml_entry page = {0};
    page_alloc(&page, PML_FLAG_WRITABLE);

    u64 paddr = page.bits.address << PAGE_SHIFT;
    u64 vaddr = 0xFFFFFFFFFFF00000;
    map_addr((void*) vaddr, (void*) paddr, 0);

    char* joke = (char*) vaddr;
    __builtin_memcpy(joke, "The great thing about this message is that it's nuid bytes long\n", 64);
    vga_print(joke);

    vga_print("kmain dispatcher is here. So far so good. "
              "Executing protocol \"nuidpocalypse\"\n");
}
