#pragma once

#include <kernel/types.h>

u64 kmalloc(u64 size);
u64 kmalloc_a(u64 size); // page aligned
u64 kmalloc_p(u64 size, u64* phys); // return physical address
u64 kmalloc_ap(u64 size, u64* phys); // both

