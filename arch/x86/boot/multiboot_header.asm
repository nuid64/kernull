        section .multiboot
header_start:
        dd 0xE85250D6                                          ; magic
        dd 0                                                   ; arch (i386)
        dd header_end - header_start                           ; header length
        ; checksum
        dd 0x100000000 - (0xE85250D6 + 0 + (header_end - header_start))

        dw 0                                                   ; type
        dw 0                                                   ; flags
        dd 8                                                   ; size
header_end:
