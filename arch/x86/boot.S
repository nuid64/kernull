        .section .text
        .code32

.global _start
_start:
        cli

        call       multiboot_check

        movl       %cr0, %eax
        orb        %al, 1
        movl       %eax, %cr0

        call       kmain
        hlt


multiboot_check: 
        cmpl       $0x36d76289, %eax
        jne        multiboot_check.no_multiboot
        ret
multiboot_check.no_multiboot: 
        call       error


/* Oopsie */
error:
        movw       $0x0F46, (0xB8000)
        movw       $0x0F4B, (0xB8002)
        hlt


.align 16
stack_bottom:
        .skip 64
stack_top:
