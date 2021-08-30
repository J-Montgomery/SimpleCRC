#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

uint64_t table[256] = { 0 };
struct crc16 {
	uint64_t poly;
	uint64_t init;
	uint64_t xor_out;
	uint64_t residue;
	uint64_t check;
	uint64_t width;
	bool ref_in;
	bool ref_out;
};

uint64_t gen_mask_bit(uint64_t topbit)
{
	uint64_t mask = ((topbit - 1) << 1) | 0x01;
	return mask;
}

uint64_t reflect_bit(uint64_t val, unsigned count)
{
	uint64_t ret = val;
	for (int i = 0; i < count; i++) {
		uint64_t srcbit = ((uint64_t)1 << i);
		uint64_t dstbit = ((uint64_t)1 << (count - i - 1));
		if ((val & srcbit) != 0)
			ret |= dstbit;
		else
			ret = ret & (~dstbit);
	}
	return ret;
}

void print_mem(uint64_t *buf, unsigned len)
{
	for (unsigned i = 0; i < len; i++) {
		printf("%02lx ", buf[i]);
		if ((i + 1) % 16 == 0)
			printf("\n");
	}
}

void precompute_table_bit(struct crc16 params)
{
	const uint64_t topbit = (uint64_t)1 << (params.width - 1);
	const uint64_t mask = gen_mask_bit(topbit);
	const unsigned bits = (params.width > 8) ? params.width : 8;

	uint64_t crc = 0;
	for (int i = 0; i < 256; i++) {
		crc = i;
		for (int j = bits; j > 0; --j) {
			if (params.width < 8) {
				if (crc & 1)
					crc = (crc >> 1) ^ params.poly;
				else
					crc = (crc >> 1);
			} else {
				if (crc & topbit)
					crc = (crc << 1) ^ params.poly;
				else
					crc <<= 1;
			}
		}
		table[i] = crc & mask;
	}
}

unsigned char crc_calc_test(unsigned char *buf, int size, uint64_t *table,
							unsigned init, unsigned width)
{
	unsigned char crc = init;
	unsigned mask = (unsigned)gen_mask_bit((uint64_t)1 << (width - 1));
	for (int i = 0; i < size; i++) {
		crc = table[buf[i] ^ (crc & mask)];
	}
	return crc;
}

uint64_t compute_crc_bit(const char *buf, size_t len, struct crc16 params)
{
	uint64_t crc;
	const uint64_t mask = gen_mask_bit((uint64_t)1 << (params.width - 1));
	const char *ptr;
	size_t a;

	precompute_table_bit(params);

	crc = params.init;
	ptr = buf;

	for (a = 0; a < len; a++) {
		uint8_t u_char = (uint8_t)*ptr++;
		if (params.ref_in)
			u_char = reflect_bit(u_char, 8);
		if (params.width >= 8)
			crc = table[((crc >> (params.width - 8)) ^ (uint64_t)u_char) &
						0xFFull] ^
				  (crc << 8);
		else {
			crc = table[u_char ^ (crc & mask)];
		}
	}

	if (params.ref_out)
		crc = reflect_bit(crc, params.width) ^ params.xor_out;
	else
		crc = crc ^ params.xor_out;

	return crc & mask;
}

#ifdef ENABLE_MAIN

#endif /* ENABLE_MAIN */
