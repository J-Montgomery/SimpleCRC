#include "simplecrc.h"
#include "internal/crc_utility.h"

uint64_t big_table[256] = { 0 };

uint64_t crc_one_byte(int init, int width, uint64_t poly)
{
	uint64_t topbit = (uint64_t)1 << (width - 1);
	uint64_t crc = init;

	for (int bit = width; bit > 0; --bit) {
		if (crc & topbit)
			crc = (crc << 1) ^ poly;
		else
			crc <<= 1;
	}

	return crc & gen_mask(width);
}

void precompute_table_bitshift(struct crc_def params)
{
	for (int byte = 0; byte < 256; ++byte) {
		uint64_t crc = crc_one_byte(byte, params.width, params.poly);
		big_table[byte] = crc;
	}
}

uint64_t compute_crc(const unsigned char *buf, size_t len,
					 struct crc_def params)
{
	uint64_t crc;
	const unsigned char *ptr;
	size_t a;

	precompute_table_bitshift(params);

	crc = params.init;
	ptr = buf;

	for (a = 0; a < len; a++) {
		uint8_t u_char = (uint8_t)*ptr++;
		if (params.ref_in)
			u_char = reflect(u_char, 8);
		if (params.width >= 8)
			crc = big_table[((crc >> (params.width - 8)) ^ (uint64_t)u_char) &
							0xFFull] ^
				  (crc << 8);
	}

	if (params.ref_out)
		crc = reflect(crc, params.width) ^ params.xor_out;
	else
		crc = crc ^ params.xor_out;

	return crc & gen_mask(params.width);
}