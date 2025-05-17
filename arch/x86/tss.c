#include <arch/x86/tss.h>

#include <arch/x86/gdt.h>

extern struct tss_entry TSS;

void tss_set_kernel_stack(uint32_t stack)
{
	TSS.esp0 = stack;
}
