#ifndef SIMPLECRC_UTILITY_H_
#define SIMPLECRC_UTILITY_H_

#define DECLARE_CRC(NAME, POLY, INIT, REF_IN, REF_OUT, XOR_OUT, RESIDUE,    \
		    CHECK, WIDTH)                                           \
	struct crc_def CRC_##NAME = (struct crc_def)                        \
	{                                                                   \
		.poly = POLY, .init = INIT, .ref_in = REF_IN,               \
		.ref_out = REF_OUT, .xor_out = XOR_OUT, .residue = RESIDUE, \
		.check = CHECK, .width = WIDTH                              \
	}

uint64_t reflect(uint64_t val, unsigned count);
uint64_t gen_mask(uint64_t width);
#endif /* SIMPLECRC_UTILITY_H_ */