        bits 64
        section .code

extern isr_handler
extern irq_handler

%macro ISR_NOERR 1
[global isr%1]
isr%1:
        cli
        push       byte 0                                      ; dummy error code
        push       byte %1                                     ; interrupt number
        jmp        isr_common
%endmacro

%macro ISR_ERR 1
[global isr%1]
isr%1:
        cli
        ; error code is pushed already
        push       byte %1                                     ; interrupt number
        jmp        isr_common
%endmacro

%macro IRQ 2
[global irq%1]
irq%1:
        cli
        push       byte 0                                      ; dummy error code
        push       byte %2                                     ; interrupt number
        jmp        irq_common
%endmacro

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31
IRQ    0, 32
IRQ    1, 33
IRQ    2, 34
IRQ    3, 35
IRQ    4, 36
IRQ    5, 37
IRQ    6, 38
IRQ    7, 39
IRQ    8, 40
IRQ    9, 41
IRQ   10, 42
IRQ   11, 43
IRQ   12, 44
IRQ   13, 45
IRQ   14, 46
IRQ   15, 47

isr_common:
        ; save all registers
        swapgs
        push       rax
        push       rbx
        push       rcx
        push       rdx
        push       rsi
        push       rdi
        push       rbp
        push       r8
        push       r9
        push       r10
        push       r11
        push       r12
        push       r13
        push       r14
        push       r15

        cld

        ; call interrupt handler
        mov        rdi, rsp
        call       isr_handler
        mov        rsp, rax

        ; restore all registers
        pop        r15
        pop        r14
        pop        r13
        pop        r12
        pop        r11
        pop        r10
        pop        r9
        pop        r8
        pop        rbp
        pop        rdi
        pop        rsi
        pop        rdx
        pop        rcx
        pop        rbx
        pop        rax
        swapgs

        add        rsp, 16                                     ; forget error code and interrupt
        sti
        iretq


irq_common:
        ; save all registers
        swapgs
        push       rax
        push       rbx
        push       rcx
        push       rdx
        push       rsi
        push       rdi
        push       rbp
        push       r8
        push       r9
        push       r10
        push       r11
        push       r12
        push       r13
        push       r14
        push       r15

        cld

        ; call interrupt handler
        mov        rdi, rsp
        call       irq_handler
        mov        rsp, rax

        ; restore all registers
        pop        r15
        pop        r14
        pop        r13
        pop        r12
        pop        r11
        pop        r10
        pop        r9
        pop        r8
        pop        rbp
        pop        rdi
        pop        rsi
        pop        rdx
        pop        rcx
        pop        rbx
        pop        rax
        swapgs

        add        rsp, 16                                     ; forget error code and interrupt
        sti
        iretq
