        bits 32
        section .code

extern gdt

global enable_paging_with

; IN  = RDI: pml4 address
enable_paging_with:
        ; load P4 to cr3 register
        mov        eax, edi
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
