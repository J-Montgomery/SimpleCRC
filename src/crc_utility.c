#include "simplecrc.h"
#include "internal/crc_utility.h"

uint64_t reflect(uint64_t val, uint8_t count)
{
	uint64_t ret = val;
	for (uint8_t i = 0; i < count; i++) {
		uint64_t srcbit = ((uint64_t)1 << i);
		uint64_t dstbit = ((uint64_t)1 << (count - i - 1));
		if ((val & srcbit) != 0)
			ret |= dstbit;
		else
			ret = ret & (~dstbit);
	}
	return ret;
}

uint64_t gen_mask(uint8_t width)
{
	if (width >= 64)
		return 0xffffffffffffffff;
	else
		return ((uint64_t)1 << width) - 1;
}
