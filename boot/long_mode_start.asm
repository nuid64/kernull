 	section .code
        bits 64

global long_mode_start
extern kmain

long_mode_start:
        cli

        mov        ax, 0x10
        mov        ss, ax
        mov        ds, ax
        mov        es, ax
        mov        fs, ax
        mov        gs, ax

        cli

        call       kmain

halt:
        cli
        hlt
        jmp        halt
