#include <arch/x86/vga_print.h>
#include <kernel/panic.h>

void panic(const char *msg)
{
	vga_print("KERNEL PANIC: ");
	vga_print(msg);
	for (;;)
		__asm__ volatile("hlt");
}
