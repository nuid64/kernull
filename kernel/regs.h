#ifndef REGS_H
#define REGS_H

#include "types.h"

struct regs {
    // pushed by isr_common
    u64 r15, r14, r13, r12;
    u64 r11, r10, r9, r8;
    u64 rbp, rdi, rsi, rdx, rcx, rbx, rax;

    // pushed by wrapper
    u64 int_no, err_code;

    // pushed by interrupt
    u64 rip, cs, rflags, rsp, ss;
};

#endif
