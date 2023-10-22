#ifndef SEGMENT_SELECTOR_H
#define SEGMENT_SELECTOR_H

#include "types.h"

union segment_selector {
    struct {
        u8  rpl : 2;  // Requested Privilege Level field. Used for permission check
        u8  ti  : 1;  // Table to use. 0 = GDT, 1 = LDT
        u16 idx : 13; // index
    } bits;
    u16 full;
};

#endif
