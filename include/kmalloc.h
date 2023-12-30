#pragma once

#include <kernull/types.h>

u64 kmalloc(u64 size);
u64 kmalloc_a(u64 size); /* Page aligned */
u64 kmalloc_p(u64 size, u64* phys); /* Return physical address */
u64 kmalloc_ap(u64 size, u64* phys); /* Page aligned. Return physical address */

