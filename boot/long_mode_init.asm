global long_mode_start
extern kmain

 	section .text
        bits 64

long_mode_start:
        mov        ax, 0
        mov        ss, ax
        mov        ds, ax
        mov        es, ax
        mov        fs, ax
        mov        gs, ax

        mov        eax, 0xF036
        mov        [0xB8000], eax
        mov        eax, 0x0F34
        mov        [0xB8002], eax

        cli
        hlt
