        bits 64
        section .code

global gdt_load

; IN  = RDI: GDTR
gdt_load:
        lgdt       [rdi]
        call       reload_segments
        ret

        ; reload segment registers
reload_segments:
        push       0x08
        lea        rax, [rel .reload_cs]
        push       rax
        retfq
.reload_cs:
        mov        ax, 0x10
        mov        ds, ax
        mov        es, ax
        mov        fs, ax
        mov        gs, ax
        mov        ss, ax

        ret
