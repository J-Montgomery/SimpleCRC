#include <stdio.h>
#include "simplecrc.h"
#include "internal/crc_utility.h"

uint64_t table[256] = { 0 };
#define ONES(n) (((uint8_t)0 - 1) >> (8 - (n)))

void print_mem(uint64_t *buf, unsigned len)
{
	for (unsigned i = 0; i < len; i++) {
		printf("%02lx ", buf[i]);
		if ((i + 1) % 16 == 0)
			printf("\n");
	}
}

void precompute_table_bit(struct crc_def params)
{
	const uint64_t topbit = (uint64_t)1 << (params.width - 1);
	const uint64_t mask = gen_mask(params.width);
	const unsigned bits = 8;

	uint64_t crc = 0;
	for (int i = 0; i < 256; i++) {
		crc = i;
		for (int j = bits; j > 0; --j) {
				if (crc & 1)
					crc = (crc >> 1) ^ params.poly;
				else
					crc = (crc >> 1);
		}
		table[i] = crc & mask;
	}
}

unsigned char crc_calc_test(unsigned char *buf, int size, uint64_t *table,
							unsigned init, unsigned width)
{
	unsigned char crc = init;
	unsigned mask = (unsigned)gen_mask(width);
	for (int i = 0; i < size; i++) {
		crc = table[buf[i] ^ (crc & mask)];
	}
	return crc;
}

uint64_t compute_crc_bit(const char *buf, size_t len, struct crc_def params)
{
	uint64_t crc;
	const uint64_t mask = gen_mask(params.width);
	const char *ptr;
	size_t a;

	precompute_table_bit(params);
	print_mem(table, 256);

	crc = params.init;
	ptr = buf;

	for (a = 0; a < len; a++) {
		uint8_t u_char = (uint8_t)*ptr++;
		if (params.ref_in)
			u_char = reflect(u_char, 8);
		crc = table[u_char ^ (crc & mask)];
	}

	if (params.ref_out)
		crc = reflect(crc, params.width) ^ params.xor_out;
	else
		crc = crc ^ params.xor_out;

	return crc & mask;
}

#ifdef ENABLE_MAIN

#endif /* ENABLE_MAIN */
