OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS {
    . = 1M;
    phys = .;

    .code BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.bootstrap)
        code = .;
        *(.code)
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        data = .;
        *(.data)
        *(.symbols)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        bss = .;
        *(COMMON)
        *(.bss)
        *(.stack)
    }

    end = .;
}
