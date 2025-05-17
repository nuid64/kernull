#include <arch/x86/gdt.h>

#include <kernel/string.h>
#include <stdint.h>

void gdt_init(void);

struct gdt_entry GDT[6];
struct gdt_ptr GDTR;
struct tss_entry TSS;

extern void gdt_load(uint32_t gdtr);
extern void gdt_flush(uint32_t);
extern void tss_flush();

void gdt_set_gate(int i, uint32_t base, uint32_t limit, uint8_t access,
				  uint8_t gran)
{
	GDT[i].limit_low = limit & 0xFFFF;
	GDT[i].base_low = base & 0xFFFF;
	GDT[i].base_middle = (base >> 16) & 0xFF;
	GDT[i].access.full = access;
	GDT[i].flags.full = ((limit >> 16) & 0x0F) | (gran & 0xF0);
	GDT[i].base_high = (base >> 24) & 0xFF;
}

void write_tss(int index, uint16_t kernel_ss, uint32_t kernel_esp)
{
	uint32_t base = (uint32_t)&TSS;
	uint32_t limit = sizeof(struct tss_entry);

	gdt_set_gate(index, base, limit, 0x89, 0x00);
	memset(&TSS, 0, sizeof(TSS));
	TSS.ss0 = kernel_ss;
	TSS.esp0 = kernel_esp;
}

void gdt_init(void)
{
	GDTR.limit = sizeof(GDT) - 1;
	GDTR.base = (uint32_t)&GDT;

	gdt_set_gate(0, 0, 0, 0, 0); // NULL
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Kernel code
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Kernel data
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User code
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User data
	write_tss(5, 0x10, 0); // TSS setup

	gdt_flush((uint32_t)&GDTR);
	tss_flush();
}
