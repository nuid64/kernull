        bits 32
        section .bootstrap

extern long_mode_start
extern protected_vga_print
extern protected_vga_set_color

global _start
_start:
        mov        esp, stack_top

        ; some checks
        call       multiboot_check
        call       cpuid_check
        call       long_supported_check

        ; enable paging
        call       setup_page_tables
        call       enable_paging

        lgdt       [gdt.pointer]
        jmp        gdt.code:long_mode_start                    ; call kmain from there


setup_page_tables:
extern initial_page_tables
        mov        edi, initial_page_tables                    ; P4

        ; map first P4 entry to P3 table
        mov        eax, 0x1000                                 ; P3 table offset...
        add        eax, edi                                    ; ... from P4
        or         eax, 0b11                                   ; PRESENT | WRITABLE
        mov        [edi], eax                                  ; P4[0] = &P3[0] | PRESENT | WRITABLE

        add        edi, 0x1000                                 ; P3
        ; map first P3 entry to P2 table
        mov        eax, 0x1000                                 ; P2 table offset...
        add        eax, edi                                    ; ... from P3
        or         eax, 0b11                                   ; PRESENT | WRITABLE
        mov        [edi], eax                                  ; P3[0] = &P2[0] | PRESENT | WRITABLE

        ; map 32 2MiB pages to 64MiB of low memory temporarily, until MMU initialization
        add        edi, 0x1000                                 ; P2
        mov        ebx, 0b10000011                             ; PRESENT | WRITABLE | HUGE
        mov        ecx, 32
.set_entry:
        mov        [edi], ebx
        add        edx, 0x200000                               ; next 2MiB
        add        edi, 8
        loop       .set_entry

        ret


enable_paging:
        ; load P4 to cr3 register
        mov        eax, initial_page_tables
        mov        cr3, eax

        ; enable PAE-flag in cr4
        mov        eax, cr4
        or         eax, 1 << 5
        mov        cr4, eax

        ; set the long mode bit in the EFER MSR
        mov        ecx, 0xC0000080
        rdmsr
        or         eax, 1 << 8
        wrmsr

        ; enable paging in the cr0 register
        mov        eax, cr0
        or         eax, 1 << 31
        mov        cr0, eax

        ret


multiboot_check:
        cmp        eax, 0x36d76289
        jne        .no_multiboot
        ret
.no_multiboot:
        mov        esi, no_multiboot_err
        call       error


; check if CPUID is supported by attempting to flip the ID bit (bit 21) in
; the FLAGS register. If we can flip it, CPUID is available.
cpuid_check:
        ; copy FLAGS in to EAX via stack
        pushfd
        pop        eax
 
        ; copy to ECX as well for comparing later on
        mov        ecx, eax
 
        ; flip the ID bit
        xor        eax, 1 << 21
 
        ; copy EAX to FLAGS via the stack
        push       eax
        popfd
 
        ; copy FLAGS back to EAX (with the flipped bit if CPUID is supported)
        pushfd
        pop        eax
 
        ; restore FLAGS from the old version stored in ECX (i.e. flipping the ID bit
        ; back if it was ever flipped).
        push       ecx
        popfd
 
        ; compare EAX and ECX. If they are equal then that means the bit wasn't
        ; flipped, and CPUID isn't supported.
        xor        eax, ecx
        jz         .no_cpuid
        ret
.no_cpuid:
        mov        esi, no_cpuid_err
        call       error


long_supported_check:
        mov        eax, 0x80000000
        cpuid
        cmp        eax, 0x80000001
        jb         .no_long_supported

        mov        eax, 0x80000001
        cpuid
        test       edx, 1 << 29                                ; LM-bit
        jz         .no_long_supported
        ret
.no_long_supported:
        mov        esi, no_long_supported_err
        call       error


; print error message and hangs
; IN  = ESI: error message addr
error:
        ; prefix with red [ERR]
        mov        dl, 0x0
        mov        dh, 0x4
        call       protected_vga_set_color

        push       esi
        mov        esi, err_prefix
        call       protected_vga_print

        ; print the message
        mov        dl, 0x0
        mov        dh, 0xF
        call       protected_vga_set_color
        pop        esi
        call       protected_vga_print
        hlt


gdt:
        dq 0
.code: equ $ - gdt
        dw 0
        dw 0
        db 0
        db 0x9a
        db 0x20
        db 0
.data:
        dw 0xFFFF
        dw 0
        db 0
        db 0x92
        db 0
        db 0
.pointer:
        dw $ - gdt - 1
        dq gdt


        section .rodata

err_prefix db "[ERR]: ", 0x00
no_multiboot_err db "needs to be load by Multiboot compiant loader", 0x00
no_cpuid_err db "CPUID is not supported by the processor", 0x00
no_long_supported_err db "long mode is not supported by the processor", 0x00


        section .bss

align 16
stack_bottom:
        resb 16384 ; 16KiB
stack_top:
