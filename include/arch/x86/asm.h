#pragma once

#include <stdint.h>

inline void invlpg(uint32_t addr)
{
    __asm__ volatile (
        "invlpg (%0)"
        : : "r" (addr)
    );
}

inline void idt_load(uint32_t addr)
{
    __asm__ (
        "lidt %0"
        : : "m"(addr)
    );
}

inline uint32_t get_cr2(void)
{
    uint32_t cr2;
    __asm__ volatile (
        "mov %%cr2, %0"
        : "=r" (cr2)
    );
    return cr2;
}

inline void set_cr3(uint32_t value)
{
    __asm__ volatile (
        "movl %0, %%cr3"
        : : "r" (value)
    );
}

inline uint8_t inb(uint32_t port)
{
    uint8_t rv;
    __asm__ volatile (
        "inb %1, %0"
        : "=a" (rv)
        : "dN" (port)
    );
    return rv;
}

inline uint16_t inw(uint32_t port)
{
    uint16_t rv;
    __asm__ volatile (
        "inw %1, %0"
        : "=a" (rv)
        : "dN" (port)
    );
    return rv;
}

inline uint32_t inl(uint32_t port)
{
    uint32_t rv;
    __asm__ volatile (
        "inl %%dx, %%eax"
        : "=a" (rv)
        : "dN" (port)
    );
    return rv;
}

inline void outb(uint32_t port, uint8_t data)
{
    __asm__ volatile (
        "outb %1, %0"
        : : "dN" (port), "a" (data)
    );
}

inline void outw(uint32_t port, uint16_t data)
{
    __asm__ volatile (
        "outw %1, %0"
        : : "dN" (port), "a" (data)
    );
}

inline void outl(uint32_t port, uint32_t data)
{
    __asm__ volatile (
        "outl %%eax, %%dx"
        : : "dN" (port), "a" (data)
    );
}

inline uint32_t popcntl(uint32_t value)
{
    // TODO: use popcnt instruction, if available
    uint32_t odd = value & 0x55555555;
    uint32_t evn = value & 0xAAAAAAAA;
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

