        section .text
        bits 32

extern long_mode_start
extern protected_vga_print
extern protected_vga_set_color

global _start
_start:
        mov        esp, stack_top

        ; <- init all what I need here :' ;just concentrated silly face

        ; some checks
        call       multiboot_check
        call       cpuid_check
        call       long_supported_check

        ; enable paging
        call       setup_page_tables
        call       enable_paging

        ; call kmain from there
        lgdt       [gdt64.pointer]
        jmp        gdt64.code:long_mode_start

        cli
.hang:  hlt
        jmp .hang


setup_page_tables:
        ; map first P4 entry to P3 table
        mov        eax, p3_table
        or         eax, 0b11                                   ; present + writeable
        mov        [p4_table], eax

        ; map first P3 entry to P2 table
        mov        eax, p2_table
        or         eax, 0b11                                   ; present + writeable
        mov        [p3_table], eax

        ; map each P2 entry to a huge 2MiB page
        mov        ecx, 0
.map_p2_table:
        mov        eax, 0x200000                               ; 2MiB
        mul        ecx
        or         eax, 0b10000011                             ; present + writeable + huge
        mov        [p2_table + ecx * 8], eax                   ; map entry

        inc        ecx
        cmp        ecx, 512
        jne        .map_p2_table

        ret


enable_paging:
        ; load P4 to cr3 register
        mov        eax, p4_table
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


        section .rodata
gdt64:
        dq 0
.code: equ $ - gdt64
        dq (1<<43) | (1<<44) | (1<<47) | (1<<53)               ; code segment
.pointer:
        dw $ - gdt64 - 1
        dq gdt64

err_prefix db "[ERR]: ", 0x00
no_multiboot_err db "needs to be load by Multiboot compiant loader", 0x00
no_cpuid_err db "CPUID is not supported by the processor", 0x00
no_long_supported_err db "long mode is not supported by the processor", 0x00


        section .bss
align 4096
p4_table:
        resb 4096
p3_table:
        resb 4096
p2_table:
        resb 4096

align 16
stack_bottom:
        resb 16384 ; 16KiB
stack_top:
