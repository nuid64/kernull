.global long_mode_start
.extern kmain

        .section .text
        .code64

long_mode_start:
        movw       $0, %ax
        movw       %ax, %ss
        movw       %ax, %ds
        movw       %ax, %es
        movw       %ax, %fs
        movw       %ax, %gs

        movl       $0xF036, %eax
        movl       %eax, (0xB8000)
        movl       $0xF034, %eax
        movl       %eax, (0xB8002)

        cli
        hlt
