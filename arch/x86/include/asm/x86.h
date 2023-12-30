#pragma once

#include <kernull/types.h>

inline u8 inb(u64 port)
{
    u8 rv;
    asm volatile (
        "inb %1, %0"
        : "=a" (rv)
        : "dN" (port)
    );
    return rv;
}

inline u16 inw(u64 port)
{
    u16 rv;
    asm volatile (
        "inw %1, %0"
        : "=a" (rv)
        : "dN" (port)
    );
    return rv;
}

inline u32 inl(u64 port)
{
    u32 rv;
    asm volatile (
        "inl %%dx, %%eax"
        : "=a" (rv)
        : "dN" (port)
    );
    return rv;
}

inline void outb(u64 port, u8 data)
{
    asm volatile (
        "outb %1, %0"
        :
        : "dN" (port), "a" (data)
    );
}

inline void outw(u64 port, u16 data)
{
    asm volatile (
        "outw %1, %0"
        :
        : "dN" (port), "a" (data)
    );
}

inline void outl(u64 port, u32 data)
{
    asm volatile (
        "outl %%eax, %%dx"
        :
        : "dN" (port), "a" (data)
    );
}
