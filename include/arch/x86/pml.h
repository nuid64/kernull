#pragma once

#include <stdint.h>

#define PML_FLAG_WRITABLE     0x02
#define PML_FLAG_USER         0x04
#define PML_FLAG_WRITETHROUGH 0x08
#define PML_FLAG_NOCACHE      0x10
#define PML_FLAG_HUGE         0x80
#define PML_FLAG_GLOBAL       0x100
#define PML_FLAGS_MASK        0x0000019E

// TODO: Adapt for huge pages
typedef union {
    struct {
        uint32_t present     : 1;  /* Page currently in memory (it can be swapped) */
        uint32_t writable    : 1;  /* Page writable */
        uint32_t user        : 1;  /* Page is user accessible */
        uint32_t writetrough : 1;  /* Write directly to memory, passing cache */
        uint32_t nocache     : 1;  /* Disable caching */
        uint32_t accessed    : 1;  /* Set, when PDE or PTE was read during address translation. Not cleared by the CPU */
        uint32_t dirty       : 1;  /* Determines whether page has been written to */
        uint32_t huge        : 1;  /* Must be 0 in P1 and P4. Creates 1GiB page in P3, 2MiB in P2 */
        uint32_t global      : 1;  /* Don't flush page from caches on addr space switch. PGE bit of CR4 must be set */
        uint32_t _available0 : 3;  /* Available */
        uint32_t address     : 20; /* The actual page aligned address of the frame/next table */
    } bits;
    uint32_t full;
} pml_entry; /* Page Map Level entry */
