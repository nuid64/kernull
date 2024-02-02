#pragma once

#include <stdnoreturn.h>

noreturn inline void abort()
{
    asm ("cli; hlt");
    while(1) ; // stub for compiler
}
