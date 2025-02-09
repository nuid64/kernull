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

        callq      kmain

        cli
        hlt
