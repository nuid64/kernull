#include <kernel/string.h>
#include <stdint.h>

void gdt_init(void);

union gdt_access {
	struct {
		uint8_t accessed : 1; /* Set to 1 on access */
		uint8_t rw : 1; /* Readable bit for code, writable bit for data */
		uint8_t dc : 1; /* Direction bit for data, conforming bit for code */
		uint8_t exec : 1; /* Executable bit. 0 = data, 1 = code */
		uint8_t type : 1; /* 0 = system segment, 1 = code/data */
		uint8_t
			dpl : 2; /* Descriptor Privilege Level field. 0 = highest, 3 = lowest */
		uint8_t present : 1; /* Must be 1 for any valid segment */
	} bits;
	uint8_t full;
};

union gdt_flags {
	struct {
		uint8_t limit_high : 4;
		uint8_t _available : 1; /* Available */
		uint8_t long_mode : 1; /* Defines 64-bit code segment. When set, DB should
                               always be clear */
		uint8_t db : 1; /* Size flag. 0 for 16-bit protected mode segment, 1 for
                       32-bit protected mode */
		uint8_t granularity : 1; /* 0 for byte granularity, 1 for page granularity
                                (4KiB) */
	} bits;
	uint8_t full;
};

struct gdt_entry {
	uint16_t limit_low; /* Maximum addressable unit */
	uint16_t base_low; /* Segment's beginning address */
	uint8_t base_middle;
	union gdt_access access; /* Access byte */
	union gdt_flags
		flags; /* WARN: Limit's bits 16-19 stored in flags' least 4 bits */
	uint8_t base_high;
};

struct gdt_ptr {
	uint16_t limit; /* GDT size - 1 */
	uint32_t base; /* GDT address */
} __attribute__((packed));

struct tss_entry {
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t unused[23];
} __attribute__((packed));

static struct gdt_entry GDT[6];
static struct gdt_ptr GDTR;
static struct tss_entry TSS;

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
