ENTRY(_start)

SECTIONS {
    . = 1M;
    phys = .;

    .text : ALIGN(4K)
    {
        *(.multiboot_header)
       .text = .;
        *(.text)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata)
    }

    .data : ALIGN(4K)
    {
        data = .;
        *(.data)
        *(.symbols)
    }

    .bss : ALIGN(4K)
    {
        bss = .;
        *(COMMON)
        *(.bss)
        *(.stack)
    }

    kernel_end = .;
}
