#include "simplecrc.h"
#include "internal/crc_utility.h"
#include <stdio.h>

#define TABLE_LEN (256)

static uint64_t internal_table[TABLE_LEN] = { 0 };

static uint64_t crc_one_byte(int init, int width, uint64_t poly)
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

static void precompute_table_bitshift(struct crc_def params, uint64_t **buf)
{
	uint64_t *tbl = *buf;

	for (int byte = 0; byte < TABLE_LEN; ++byte) {
		uint64_t crc = crc_one_byte(byte, params.width, params.poly);
		tbl[byte] = crc;
	}
}

bool precompute_crc_table(struct crc_def params, uint64_t **table, size_t len)
{
	if (len != (TABLE_LEN * sizeof(uint64_t)) || !table || !(*table))
		return false;

	precompute_table_bitshift(params, table);
	return true;
}

uint64_t compute_crc_fast(struct crc_def params, const unsigned char *buf,
						  size_t len, const uint64_t *table)
{
	uint64_t crc;
	const unsigned char *ptr;
	size_t a;

	crc = params.init;
	ptr = buf;

	for (a = 0; a < len; a++) {
		uint8_t u_char = (uint8_t)*ptr++;
		if (params.ref_in)
			u_char = reflect(u_char, 8);
		if (params.width >= 8)
			crc = table[((crc >> (params.width - 8)) ^ (uint64_t)u_char) &
						0xFFull] ^
				  (crc << 8);
	}

	if (params.ref_out)
		crc = reflect(crc, params.width) ^ params.xor_out;
	else
		crc = crc ^ params.xor_out;

	return crc & gen_mask(params.width);
}

uint64_t compute_crc(struct crc_def params, const unsigned char *buf,
					 size_t len)
{
	uint64_t *tbl_ptr = internal_table;

	precompute_crc_table(params, &tbl_ptr, TABLE_LEN * sizeof(uint64_t));
	return compute_crc_fast(params, buf, len, internal_table);
}
