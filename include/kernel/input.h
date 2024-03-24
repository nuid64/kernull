#pragma once

#include "input_codes.h"

#include <kernel/types.h>

#define INPUT_FLAG_KEY_RELEASED 0x1

struct input_event {
    u16 flags;
    u16 code;
};
