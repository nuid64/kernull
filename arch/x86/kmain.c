#include <stddef.h>
#include <stdint.h>

#include <arch/x86/memory.h>
#include <arch/x86/vga_print.h>

extern void gdt_init(void);
extern void idt_init(void);

void kmain(void)
{
	vga_terminal_initialize();
	gdt_init();
	idt_init();
	paging_init();

	vga_set_color(VGA_COLOR_GREEN);
	vga_print("\
    __                         ____\n\
   / /_____  _________  __  __/ / /\n\
  / //_/ _ \\/ ___/ __ \\/ / / / / / \n\
 / ,< /  __/ /  / / / / /_/ / / /  \n\
/_/|_|\\___/_/  /_/ /_/\\__,_/_/_/   \n\
");
}
