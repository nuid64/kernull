#pragma once

#ifdef ARCH_X86_64
#include "arch/x86_64/pml.h"
#endif

typedef u32 pid_t;

enum proc_state {
    EMBRYO = 0,
    READY,
    RUNNING,
    BLOCKED,
    ZOMBIE,
};

struct context {
    u64 r15, r14, r13, r12;
    u64 r11, r10, r9, r8;
    u64 rbp, rdi, rsi, rdx, rcx, rbx, rax;
    u64 rip, rsp, rflags; //, cs, ss;
};

struct thread {
    pid_t pid;          /* Unique identifier */
};

struct task {
    pid_t pid;             /* Unique identifier */
    enum proc_state state;

// NOTE: I really should abstract concept of address space out of page tables
#ifdef ARCH_X86_64
    pml_entry *
#endif
    address_space;

    struct context ctx; /* Executing state */

    // TODO: Sex mode (task' children)
};

void  init_tasking();
pid_t create_task(struct task *);
int   free_task(struct task *proc);
