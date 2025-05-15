#include <stdint.h>
#include <stddef.h>

#include <arch/x86/vga_print.h>

void kmain(void)
{
	vga_terminal_initialize();

	vga_set_color(VGA_COLOR_GREEN);
	vga_print("\
    __                         ____\n\
   / /_____  _________  __  __/ / /\n\
  / //_/ _ \\/ ___/ __ \\/ / / / / / \n\
 / ,< /  __/ /  / / / / /_/ / / /  \n\
/_/|_|\\___/_/  /_/ /_/\\__,_/_/_/   \n\
");
}
