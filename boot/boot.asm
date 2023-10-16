        section .text
        bits 32

global _start
_start:
        ; <- init all what I need here :' ;just concentrated silly face

        mov        eax, 0xF036
        mov        [0xB8000], eax
        mov        eax, 0x0F34
        mov        [0xB8002], eax

        cli
.hang:  hlt
        jmp .hang
