        .section .text
        .code32

.global _start
_start:
        /* Do checks. */
        call       cpuid_check
        call       long_mode_support_check

        /* Enable paging. */
        call       setup_page_tables
        call       enable_paging

        /* Print me. */
        movl       $0xF036, %eax
        movl       %eax, 0xB8000
        movl       $0x0F34, %eax
        movl       %eax, 0xB8002

        cli
        hlt

setup_page_tables:
        /* Map first P4 entry to P3 table. */
        movl       $p3_table, %eax
        orl        $0b11, %eax                                 #  Writable | Present
        movl       %eax, (p4_table)

        /* Map first P3 entry to P2 table. */
        movl       $p2_table, %eax
        orl        $0b11, %eax                                 #  Writable | Present
        movl       %eax, (p3_table)

        /* Map each P2 entry to 2MiB page. */
        movl       $0, %ecx
.map_p2_table:
        movl       $0x200000, %eax                             # 2 MiB
        mull       %ecx
        orl        $0b10000011, %eax                           # Huge | Writable | Present
        movl       %eax, p2_table(,%ecx, 8)

        incl       %ecx
        cmpl       $512, %ecx
        jne        .map_p2_table

        ret


enable_paging:
        /* Load P4 to cr3 register. */
        movl       $p4_table, %eax
        movl       %eax, %cr3

        /* Set PAE-flag in cr4. */
        movl       %cr4, %eax
        orl        $(1 << 5), %eax
        movl       %eax, %cr4

        /* Set long mode bit in EFER MSR. */
        movl       $0xC0000080, %ecx
        rdmsr
        orl        $(1 << 8), %eax
        wrmsr

        /* Set paging in cr0 register. */
        movl       %cr0, %eax
        orl        $(1 << 31), %eax
        movl       %eax, %cr0

        ret


multiboot_check: 
        cmpl       $0x36d76289, %eax
        jne        multiboot_check.no_multiboot
        ret
multiboot_check.no_multiboot: 
        call       error


/* Check if CPUID is supported by attempting to flip ID bit (bit 21) in
   FLAGS register. If we can flip it, CPUID is available. */
cpuid_check:
        /* Copy FLAGS in to EAX via stack */
        pushfl
        popl       %eax

        /* Copy to ECX as well for comparing later on */
        movl       %eax, %ecx

        /* Flip ID bit */
        xorl       $(1 << 21), %eax

        /* Copy EAX to FLAGS via stack */
        pushl      %eax
        popfl

        /* Copy FLAGS back to EAX (with flipped bit if CPUID is supported) */
        pushfl
        popl       %eax

        /* Restore FLAGS from old version stored in ECX (i.e. flipping
           ID bit back if it was ever flipped). */
        pushl      %ecx
        popfl

        /* Compare EAX and ECX. If they are equal then that means bit
           wasn't flipped, and CPUID isn't supported. */
        xorl       %ecx, %eax
        je         cpuid_check.no_cpuid

        ret

cpuid_check.no_cpuid:
        call       error


long_mode_support_check:
        movl       $0x80000000, %eax
        cpuid
        cmpl       $0x80000001, %eax
        jb         long_mode_support_check.no_long_mode_support

        movl       $0x80000001, %eax
        cpuid
        testl      $(1 << 29), %edx
        jz         long_mode_support_check.no_long_mode_support

        ret

long_mode_support_check.no_long_mode_support:
        call       error


/* Oopsie */
error:
        movw       $0x0F46, (0xB8000)
        movw       $0x0F4B, (0xB8002)
        hlt


        .section .rodata
gdt64:
        .quad 0
.code:
        .short . - gdt64
        .quad ((1<<53) | (1<<47) | (1<<44) | (1<<43))  # Code segment
.pointer:
        .short . - gdt64 - 1
        .quad gdt64


        .section .bss
.align 4096
p4_table:
        .skip 4096
p3_table:
        .skip 4096
p2_table:
        .skip 4096

.align 16
stack_bottom:
        .skip 64 # Just like me...
stack_top:
