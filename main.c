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

#define BITMASK(X) (1L << (X))
#define CRC16_BITS (16)
#define DECLARE_CRC16(NAME, POLY, INIT, REF_IN, REF_OUT, XOR_OUT, RESIDUE, CHECK)   \
    struct crc16 CRC16_##NAME = (struct crc16){.poly = POLY, .init = INIT, \
                                   .ref_in = REF_IN, .ref_out = REF_OUT,     \
                                   .xor_out = XOR_OUT, .residue = RESIDUE, .check = CHECK }

//DECLARE_CRC16(KERMIT, 0x1021, 0x0, true, false, 0x0, 0x0);
DECLARE_CRC16(KERMIT, 0x1021, 0xFFFF, true, true, 0x0, 0x0, 0x29b1);

/*
void precompute_table(struct crc16 params) {
    for(int i=0; i<0xFF; i++) {
        uint16_t crc = i;
        for(int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (-(uint16_t)(crc & 1) & params.poly);
        }
        table[i] = crc;
    }
}

uint16_t compute_crc(char* buf, size_t len, struct crc16 params)
{
    uint16_t crc = params.init;
    char *current = buf;
    while(len--) {
        crc = (crc >> 8) ^ table[(crc ^ *current++) & 0xFF];
    }
    return crc;
} */

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

#ifdef EVIL
uint16_t compute_crc(const char* buf, size_t len, struct crc16 params) {
	uint16_t crc;
	const char *ptr;
	size_t a;

	crc = params.init;
	ptr = buf;

	if ( ptr != NULL ) for (a=0; a<len; a++) {

		crc = (crc << 8) ^ table[ ((crc >> 8) ^ (uint16_t) *ptr++) & 0x00FF ];
	}

	return crc;

}

void precompute_table(struct crc16 params) {

	uint16_t i;
	uint16_t j;
	uint16_t crc;
	uint16_t c;

	for (i=0; i<256; i++) {

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
#else
uint16_t compute_crc(const char* buf, size_t len, struct crc16 params) {
	uint16_t crc;
	const unsigned char *ptr;
	size_t a;

	crc = params.init;
	ptr = buf;

	if ( ptr != NULL ) for (a=0; a<len; a++) {

		crc = (crc << 8) ^ table[ ((crc >> 8) ^ (uint16_t) *ptr++) & 0x00FF ];
	}

	return crc;

}
void precompute_table(struct crc16 params) {
    uint16_t i;
	uint16_t j;
	uint16_t crc;
	uint16_t c;

	for (i=0; i<256; i++) {

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
#endif
/*
uint16_t compute_crc(char* buf, size_t len, struct crc16 params) {
	uint16_t crc;
	const char *ptr;
	size_t a;

	crc = params.init;
	ptr = buf;

	if ( ptr != NULL ) for (a=0; a<len; a++) {

		crc = (crc >> 8) ^ table[ (crc ^ (uint16_t) *ptr++) & 0x00FF ];
	}

	return crc;

}

void precompute_table(struct crc16 params) {

	uint16_t i;
	uint16_t j;
	uint16_t crc;
	uint16_t c;

	for (i=0; i<256; i++) {

		crc = 0;
		c   = i;

		for (j=0; j<8; j++) {

			if ( (crc ^ c) & 0x0001 ) crc = ( crc >> 1 ) ^ params.poly;
			else                      crc =   crc >> 1;
			c = c >> 1;
		}
		table[i] = crc;
	}
}*/

int main(int argc, char**argv) {
    uint16_t result = 0;
    char buf[] = "123456789";

    printf("hello, world!\n");
    result = compute_crc(buf, 9, CRC16_KERMIT);
    printf("CRC Kermit %s: 0x%.4x\n", (0x29b1 == CRC16_KERMIT.check)?"PASSED":"FAILED", result);
    return 0;
}