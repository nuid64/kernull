#pragma once

#include <abort.h>

#define assert(expr) if (!(expr)) abort(); 
