#pragma once

#include <stdint.h>

union segment_selector {
	struct {
		uint8_t
			rpl : 2; /* Requested Privilege Level field. Used for permission check */
		uint8_t ti : 1; /* Table to use. 0 = GDT, 1 = LDT */
		uint16_t idx : 13; /* Index */
	} bits;
	uint16_t full;
};
