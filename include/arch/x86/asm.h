#pragma once

#include <kernel/types.h>

inline void invlpg(u64 addr)
{
    asm (
        "invlpg (%0)"
        : : "r" (addr)
    );
}

inline u64 get_cr2()
{
    u64 cr2;
    asm (
        "mov %%cr2, %0"
        : "=r" (cr2)
    );
    return cr2;
}

inline void set_cr3(u64 value)
{
    asm (
        "movq %0, %%cr3"
        : : "r" (value)
    );
}

inline u8 inb(u64 port)
{
    u8 rv;
    asm (
        "inb %1, %0"
        : "=a" (rv)
        : "dN" (port)
    );
    return rv;
}

inline u16 inw(u64 port)
{
    u16 rv;
    asm (
        "inw %1, %0"
        : "=a" (rv)
        : "dN" (port)
    );
    return rv;
}

inline u32 inl(u64 port)
{
    u32 rv;
    asm (
        "inl %%dx, %%eax"
        : "=a" (rv)
        : "dN" (port)
    );
    return rv;
}

inline void outb(u64 port, u8 data)
{
    asm (
        "outb %1, %0"
        : : "dN" (port), "a" (data)
    );
}

inline void outw(u64 port, u16 data)
{
    asm (
        "outw %1, %0"
        : : "dN" (port), "a" (data)
    );
}

inline void outl(u64 port, u32 data)
{
    asm (
        "outl %%eax, %%dx"
        : : "dN" (port), "a" (data)
    );
}

inline u32 popcntl(u32 value)
{
    // TODO: use popcnt instruction, if available
    u32 odd = value & 0x55555555;
    u32 evn = value & 0xAAAAAAAA;
    value = odd + (evn >> 1);

    odd = value & 0x33333333;
    evn = value & 0xCCCCCCCC;
    value = odd + (evn >> 2);

    odd = value & 0x0F0F0F0F;
    evn = value & 0xF0F0F0F0;
    value = odd + (evn >> 4);

    odd = value & 0x00FF00FF;
    evn = value & 0xFF00FF00;
    value = odd + (evn >> 8);

    odd = value & 0x0000FFFF;
    evn = value & 0xFFFF0000;
    value = odd + (evn >> 16);

    return value;
}
