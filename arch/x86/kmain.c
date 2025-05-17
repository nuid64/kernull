#include <stddef.h>
#include <stdint.h>

#include <arch/x86/memory.h>
#include <arch/x86/vga_print.h>
#include <kernel/printk.h>

extern void gdt_init(void);
extern void idt_init(void);
extern void pit_init(void);

void kmain(void)
{
	vga_terminal_initialize();
	gdt_init();
	idt_init();
	pit_init();
	paging_init();

	vga_set_color(VGA_COLOR_GREEN);
	printk("\
    __                         ____\n\
   / /_____  _________  __  __/ / /\n\
  / //_/ _ \\/ ___/ __ \\/ / / / / /\n\
 / ,< /  __/ /  / / / / /_/ / / /  \n\
/_/|_|\\___/_/  /_/ /_/\\__,_/_/_/\n\
");
	for (;;)
		__asm__ __volatile__("hlt");
}
