#ifndef SIMPLECRC_H_
#define SIMPLECRC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define DECLARE_CRC(NAME, POLY, INIT, REF_IN, REF_OUT, XOR_OUT, RESIDUE,       \
					CHECK, WIDTH)                                              \
	struct crc_def CRC_##NAME = (struct crc_def)                               \
	{                                                                          \
		.poly = POLY, .init = INIT, .ref_in = REF_IN, .ref_out = REF_OUT,      \
		.xor_out = XOR_OUT, .residue = RESIDUE, .check = CHECK, .width = WIDTH \
	}

struct crc_def {
	uint64_t poly;
	uint64_t init;
	uint64_t xor_out;
	uint64_t residue;
	uint64_t check;
	uint64_t width;
	bool ref_in;
	bool ref_out;
};

uint64_t compute_crc(struct crc_def params, const unsigned char *buf,
					 size_t len);
bool precompute_crc_table(struct crc_def params, uint64_t **table, size_t len);
uint64_t compute_crc_fast(struct crc_def params, const unsigned char *buf,
						  size_t len, const uint64_t *table);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLECRC_H_ */
