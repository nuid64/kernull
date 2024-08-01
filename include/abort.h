#pragma once

#include <stdnoreturn.h>

noreturn inline void abort()
{
    asm ("cli; hlt");
    __builtin_unreachable();
}
