#include "simplecrc.h"
#include "internal/crc_utility.h"
#include <stdio.h>

uint64_t compute_crc_lhs(struct crc_def params, const unsigned char *buf,
						  size_t len)
{
	uint64_t crc;
	size_t a;

    uint64_t topbit = (uint64_t)1 << (params.width - 1);
	crc = params.init;

	for (a = 0; a < len; a++) {
	    uint8_t val = buf[a];
	    val = (params.ref_in) ? reflect(val, 8) : val;
		crc ^= (uint64_t)val << (params.width - 8);
        for (int bit = 8; bit > 0; --bit) {
	    	if (crc & topbit)
		    	crc = (crc << 1) ^ params.poly;
		    else
			    crc <<= 1;
	    }
	}

	if (params.ref_out)
		crc = reflect(crc, params.width) ^ params.xor_out;
	else
		crc = crc ^ params.xor_out;

	return crc & gen_mask(params.width);
}

uint64_t compute_crc_rhs(struct crc_def params, const unsigned char *buf,
						  size_t len)
{
	uint64_t crc;
	size_t a;
	uint64_t poly = reflect(params.poly, params.width);

	crc = reflect(params.init, params.width);

	for (a = 0; a < len; a++) {
	    uint8_t val = buf[a];
		crc ^= (uint64_t)val;
        for (int bit = 8; bit > 0; --bit) {
	    	if (crc & 1)
		    	crc = (crc >> 1) ^ poly;
		    else
			    crc >>= 1;
	    }
	}

	crc = crc ^ params.xor_out;
	return crc & gen_mask(params.width);
}

uint64_t compute_crc(struct crc_def params, const unsigned char *buf,
						  size_t len)
{
	uint64_t crc;
	size_t a;

	if(params.ref_in && params.ref_out)
		return compute_crc_rhs(params, buf, len);
	else
		return compute_crc_lhs(params, buf, len);
}
