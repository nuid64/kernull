#include <kernel/types.h>
#include <kernel/printk.h>
#include <kernel/ps2_keyboard.h>
#include <multiboot2.h>
#include <multiboot2_parser.h>

extern u64 kernel_end; /* End of kernel code */

extern void vga_terminal_initialize();
extern void idt_init();
extern void mmu_init(size_t memsize, u64 kernel_end);
extern void pit_init();
extern void ps2_init();

void kmain(u64 mb_info_addr, u32 mb_magic)
{
    vga_terminal_initialize();

    if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        printk("Fucked up magic number\n");

        return;
    }
    struct multiboot_tag *tags = (struct multiboot_tag *) (mb_info_addr + 8);

    /* Highest physicall address available */
    u64 memory_end = multiboot_get_memory_end(tags);

    idt_init();
    mmu_init(memory_end, kernel_end);
    pit_init();
    ps2_init();

    asm ("sti");
    ps2_kbd_command(PS2_KBD_COM_ECHO);
    ps2_kbd_command_arg(PS2_KBD_COM_SET_SCAN_SET, 2);

    printk("Executin%c %s %d.NUIDPOCALYPSE\n", 'g', "protocol", 666);
    printk("[ERROR]: Password required\nEnter THE password to proceed: ");

    while(1) ;
}
