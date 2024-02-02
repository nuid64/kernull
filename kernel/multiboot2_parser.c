#include "multiboot2_parser.h"
#include "multiboot2.h"

struct multiboot_tag* multiboot_find_tag(struct multiboot_tag* tags, u32 tag_type)
{
    while (tags->type != MULTIBOOT_TAG_TYPE_END) {
        if (tags->type == tag_type)
            return tags;
        else 
            // HINT: Pointer arithmetics, fuckuuuuzzzzz
            tags = (struct multiboot_tag*) ((u8*) tags + ((tags->size + 7) & ~7));
    }

    return NULL;
}

u64 multiboot_get_memory_end(struct multiboot_tag* tags)
{
    struct multiboot_tag_mmap* mmap;
    mmap = (struct multiboot_tag_mmap*) multiboot_find_tag(tags, MULTIBOOT_TAG_TYPE_MMAP);

    struct multiboot_mmap_entry* mmap_entry = mmap->entries;

    u64 highest_valid_address = 0;

    /* WARN: Assuming machine has one contiguous memory block right after first MiB.
     * This approach will work with <4GiB RAM machines only. The reason is
     * FEC00000-FFFFFFFF hardware memory mappings. A fucky ones.
     */
    for (; (u8*) mmap_entry < (u8*) mmap + mmap->size; mmap_entry += mmap->entry_size / 8) {
        u64 mmap_highest_addr = mmap_entry->addr + mmap_entry->len - 1;
        if (mmap_entry->type == 1 && highest_valid_address < mmap_highest_addr)
            highest_valid_address = mmap_highest_addr;
    }

    return highest_valid_address;
}
