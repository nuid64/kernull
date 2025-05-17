#include <arch/x86/pic.h>

#include <arch/x86/asm.h>

void pic_remap()
{
	/* Start init sequence */
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

	outb(PIC1_DATA, 0x20); // Master PIC vector offset
	outb(PIC2_DATA, 0x28); // Slave PIC vector offset

	outb(PIC1_DATA, 0x04); // Tell master there is a slave PIC at IRQ2
	outb(PIC2_DATA, 0x02); // Tell slave it's cascade identity

	/* Use 8086 mode */
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);

	/* Masks */
	outb(PIC1_DATA, 0x00);
	outb(PIC2_DATA, 0x00);
}

void irq_ack(uint8_t irq_no)
{
	if (irq_no >= 8) {
		outb(PIC2_COMMAND, PIC_EOI);
	}
	outb(PIC1_COMMAND, PIC_EOI);
}
