#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <simplecrc/reference.h>
#include <simplecrc.h>

#include "utility/algo_test_utils.h"

/* Generated by crcany */
uint64_t crc40gsm_bit(uint64_t crc, void const *mem, size_t len)
{
	unsigned char const *data = mem;
	if (data == NULL)
		return 0xffffffffff;
	crc = ~crc;
	for (size_t i = 0; i < len; i++) {
		crc ^= (uint64_t)data[i] << 32;
		for (unsigned k = 0; k < 8; k++) {
			crc = crc & 0x8000000000 ? (crc << 1) ^ 0x4820009 : crc << 1;
		}
	}
	crc = ~crc;
	crc &= 0xffffffffff;
	return crc;
}

EXHAUSTIVE_CRC_CHECK(CRC_GSM_40, crc40gsm_bit)
