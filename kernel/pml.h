#pragma once

#include <kernel/types.h>

typedef union {
    struct {
        u64 present     : 1;  /* Page currently in memory (it can be swapped) */
        u64 writable    : 1;  /* Page writable */
        u64 user        : 1;  /* Page is user accessible */
        u64 writetrough : 1;  /* Write directly to memory, passing cache */
        u64 nocache     : 1;  /* Disable caching */
        u64 accessed    : 1;  /* Set, when PDE or PTE was read during address translation. Not cleared by the CPU */
        u64 dirty       : 1;  /* Determines whether page has been written to */
        u64 huge        : 1;  /* Must be 0 in P1 and P4. Creates 1GiB page in P3, 2MiB in P2 */
        u64 global      : 1;  /* Don't flush page from caches on addr space switch. PGE bit of CR4 must be set */
        u64 _available0 : 3;  /* Available */
        u64 address     : 40; /* The actual page aligned address of the frame/next table */
        u64 _available1 : 11; /* Available */
        u64 noexec      : 1;  /* Forbid executing code on this page. NXE bit in EFER must be set */
    } bits;
    u32 full;
} pml_entry; /* Page Map Level entry */
