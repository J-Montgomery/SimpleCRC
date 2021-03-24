#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

uint16_t table[256] = {0};

struct crc16 {
	uint16_t poly;
	uint16_t init;
	bool ref_in;
	bool ref_out;
	uint16_t xor_out;
	uint16_t residue;
    uint16_t check;
};

#define xstr(a) str(a)
#define str(a) #a
#define BITMASK(X) (1L << (X))
#define CRC16_BITS (16)
#define DECLARE_CRC16(NAME, POLY, INIT, REF_IN, REF_OUT, XOR_OUT, RESIDUE, CHECK)   \
    struct crc16 CRC16_##NAME = (struct crc16){.poly = POLY, .init = INIT, \
                                   .ref_in = REF_IN, .ref_out = REF_OUT,     \
                                   .xor_out = XOR_OUT, .residue = RESIDUE, .check = CHECK }

DECLARE_CRC16(IBM_3740, 0x1021, 0xFFFF, false, false, 0x0, 0x0, 0x29b1);
DECLARE_CRC16(KERMIT, 0x1021, 0x0000, true, true, 0x0, 0x0, 0x2189);
DECLARE_CRC16(ISO_14443_3_A, 0x1021, 0xc6c6, true, true, 0x0, 0x0, 0xbf05);
DECLARE_CRC16(MAXIM_DOW, 0x8005, 0x0000, true, true, 0xffff, 0x0, 0x44c2);
DECLARE_CRC16(USB, 0x8005, 0xffff, true, true, 0xffff, 0x0, 0xb4c8);

#define DECLARE_TEST(NAME, BUF, VAR) \
    VAR = compute_crc(BUF, 9, CRC16_##NAME); \
    printf("%s %s: 0x%.4x\n", xstr(CRC16_##NAME),\
        (result == CRC16_##NAME.check)?"PASSED":"FAILED", VAR)

uint64_t reflect(uint64_t val, unsigned count)
{
    uint64_t ret = val;
    for (int i = 0; i < count; i++) {
        uint64_t srcbit = ((uint64_t)1 << i);
        uint64_t dstbit = ((uint64_t)1 << (count - i - 1));
        if((val & srcbit) != 0)
            ret |= dstbit;
        else
            ret = ret & (~dstbit);
    }
    return ret;
}

void precompute_table(struct crc16 params) {
    uint16_t i;
	uint16_t j;
	uint16_t crc;
	uint16_t c;

	for (i=0; i<255; i++) {

		crc = 0;
		c   = i << 8;

		for (j=0; j<8; j++) {

			if ( (crc ^ c) & 0x8000 ) crc = ( crc << 1 ) ^ params.poly;
			else                      crc =   crc << 1;

			c = c << 1;
		}

		table[i] = crc;
	}

}
uint16_t compute_crc(const char* buf, size_t len, struct crc16 params) {
	uint16_t crc;
	const char *ptr;
	size_t a;

    precompute_table(params);

	crc = params.init;
	ptr = buf;

	if ( ptr != NULL ) {
        for (a=0; a<len; a++) {
            uint16_t u_char = (uint16_t) *ptr++;
            if(params.ref_in)
                u_char = reflect(u_char, 8);
		    crc = (crc << 8) ^ table[ ((crc >> 8) ^ u_char) & 0x00FF ];
	    }
    }

    if(params.ref_out)
        return reflect(crc,16) ^ params.xor_out;
    else
	    return crc ^ params.xor_out;

}

int main(int argc, char**argv) {
    uint16_t result = 0;
    char buf[] = "123456789";

   DECLARE_TEST(IBM_3740, buf, result);
   DECLARE_TEST(KERMIT, buf, result);

    DECLARE_TEST(ISO_14443_3_A, buf, result);
    DECLARE_TEST(MAXIM_DOW, buf, result);
    DECLARE_TEST(USB, buf, result);

    return 0;
}