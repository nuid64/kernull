#include <asm/x86.h>
#include "pic.h"

void pic_remap()
{
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // start init sequence
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    outb(PIC1_DATA, 0x20); // master PIC vector offset
    outb(PIC2_DATA, 0x28); // slave PIC vector offset

    outb(PIC1_DATA, 0x04); // tell master there is a slave PIC at IRQ2
    outb(PIC2_DATA, 0x02); // tell slave it's cascade identity

    outb(PIC1_DATA, ICW4_8086); // use 8086 mode
    outb(PIC2_DATA, ICW4_8086);

    outb(PIC1_DATA, 0x00); // masks
    outb(PIC2_DATA, 0x00);
}
