        .section .text
        .code32

.global _start
_start: 
        /* Print me */
        movl       $0xF036, %eax
        movl       %eax, 0xB8000
        movl       $0x0F34, %eax
        movl       %eax, 0xB8002

        cli
        hlt
