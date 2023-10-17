#include <stdint.h>
#include <stddef.h>

#include "vga_print.h"

void kmain(void) {
    vga_terminal_initialize();

    vga_print("My master is nuid64 <3");
}
