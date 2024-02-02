#pragma once

#include <kernel/types.h>
#include <multiboot2.h>

struct multiboot_tag* multiboot_find_tag(struct multiboot_tag* tags, u32 tag_type);
u64 multiboot_get_memory_end(struct multiboot_tag* tags);
