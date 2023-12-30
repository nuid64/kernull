OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS {
    . = 1M;
    phys = .;

    .code : ALIGN(4K)
    {
        *(.multiboot)
        *(.bootstrap)
        code = .;
        *(.code)
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

    end = .;
}
