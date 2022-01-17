#include "simplecrc.h"
#include "internal/crc_utility.h"

/* This function reverses the bits of the input x as if it were done an n-bit
 * register. Example: reflect(0x01234567, 25) -> 0x01cd4589
 * It assumes that the input x is an n bit number (no higher bits set) and
 * that n lies in the range [1, 64] inclusive.
 * This code is equivalent to the following more obvious, but dramatically
 * slower implementation:
 *  uint64_t reflect(uint64_t x, uint8_t width) {
 *      uint64_t dst = 0;
 *      for (uint8_t i = 0; i < width; ++i)
 *          dst |= ((x >> i) & 1) << (width - i - 1);
 *      return dst;
 *  }
 */
uint64_t reflect(uint64_t x, uint8_t width)
{
	x = (x & 0x5555555555555555) << 1 | (x & 0xAAAAAAAAAAAAAAAA) >> 1;
	x = (x & 0x3333333333333333) << 2 | (x & 0xCCCCCCCCCCCCCCCC) >> 2;
	x = (x & 0x0F0F0F0F0F0F0F0F) << 4 | (x & 0xF0F0F0F0F0F0F0F0) >> 4;
	x = (x & 0x00FF00FF00FF00FF) << 8 | (x & 0xFF00FF00FF00FF00) >> 8;
	x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
	x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;

	x >>= (64 - width);
	return x;
}

uint64_t gen_mask(uint8_t width)
{
	if (width >= 64)
		return 0xffffffffffffffff;
	else
		return ((uint64_t)1 << width) - 1;
}
