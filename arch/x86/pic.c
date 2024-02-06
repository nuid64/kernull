#include <arch/x86/pic.h>
#include <arch/x86/asm.h>

void pic_remap()
{
    // start init sequence
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    outb(PIC1_DATA, 0x20); // master PIC vector offset
    outb(PIC2_DATA, 0x28); // slave PIC vector offset

    outb(PIC1_DATA, 0x04); // tell master there is a slave PIC at IRQ2
    outb(PIC2_DATA, 0x02); // tell slave it's cascade identity

    // use 8086 mode
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // masks
    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);
}
