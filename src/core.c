#include "simplecrc.h"
#include "internal/crc_utility.h"
#include <stdio.h>

uint64_t compute_crc(struct crc_def params, const unsigned char *buf,
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
