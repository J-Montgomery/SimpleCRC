#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

uint64_t table[256] = {0};
uint64_t table2[256] = {0};

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

#define xstr(a) str(a)
#define str(a) #a
#define BITMASK(X) (1L << (X))
#define CRC16_BITS (16)
#define DECLARE_CRC(NAME, POLY, INIT, REF_IN, REF_OUT, XOR_OUT, RESIDUE, CHECK, WIDTH)   \
    struct crc16 CRC_##NAME = (struct crc16){.poly = POLY, .init = INIT, \
                                   .ref_in = REF_IN, .ref_out = REF_OUT,     \
                                   .xor_out = XOR_OUT, .residue = RESIDUE, \
                                   .check = CHECK, .width = WIDTH}

DECLARE_CRC(IBM_3740, 0x1021, 0xFFFF, false, false, 0x0, 0x0, 0x29b1, 16);
DECLARE_CRC(KERMIT, 0x1021, 0x0000, true, true, 0x0, 0x0, 0x2189, 16);
DECLARE_CRC(ISO_14443_3_A, 0x1021, 0xc6c6, true, true, 0x0, 0x0, 0xbf05, 16);
DECLARE_CRC(MAXIM_DOW, 0x8005, 0x0000, true, true, 0xffff, 0x0, 0x44c2, 16);
DECLARE_CRC(USB, 0x8005, 0xffff, true, true, 0xffff, 0x0, 0xb4c8, 16);
DECLARE_CRC(AUTOSAR_32, 0xf4acfb13, 0xffffffff, true, true, 0xffffffff, 0x0, 0x1697d06a, 32);
DECLARE_CRC(GO_ISO, 0x000000000000001b, 0xffffffffffffffff, true, true, 0xffffffffffffffff, 0x0, 0xb90956c775a41001, 64);
DECLARE_CRC(CDMA2000, 0x2f, 0x1f, false, false, 0xff, 0x0, 0xdf, 8);

#define DECLARE_TEST(NAME, BUF, VAR) \
    VAR = compute_crc(BUF, 9, CRC_##NAME); \
    printf("%s %s: 0x%.8lx\n", xstr(CRC_##NAME),\
        (result == CRC_##NAME.check)?"PASSED":"FAILED", VAR)

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

void precompute_table(struct crc16 params)
{
    uint64_t mask = (uint64_t)1 << (params.width - 1);
    for (int byte = 0; byte < 255; ++byte)
    {
        uint64_t crc = byte;

        for (int bit = params.width; bit > 0; --bit)
        {
            if (crc & mask)
                crc = (crc << 1) ^ params.poly;
            else
                crc <<= 1;
        }

        table[byte] = crc;
    }
}

void precompute_table2(struct crc16 params)
{
  for (int byte = 0; byte < 256; ++byte)
  {
    uint64_t crc = byte;

    for (int bit = 32; bit > 0; --bit)
    {
      if (crc & 0x80000000)
      {
        crc = (crc << 1) ^ params.poly;
      }
      else
      {
        crc <<= 1;
      }
    }

    table2[byte] = crc;
  }
}

uint64_t compute_crc(const char* buf, size_t len, struct crc16 params) {
	uint64_t crc;
	const char *ptr;
	size_t a;

    precompute_table(params);

	crc = params.init; //reflect(params.init, 16);
	ptr = buf;


    for (a=0; a<len; a++) {
        uint8_t u_char = (uint8_t) *ptr++;
        if(params.ref_in)
            u_char = reflect(u_char, 8);
        //crc = (crc << 8) ^ table[ ((crc >> 24) ^ u_char) & 0x00FF ];
        crc = table[((crc>>(params.width - 8)) ^ u_char) & 0xFFL] ^ (crc << 8);
    }

    if(params.ref_out)
        crc = reflect(crc,params.width) ^ params.xor_out;
    else
	    crc = crc ^ params.xor_out;

    return crc & (((uint64_t)1 << params.width) - 1);
}

int main(int argc, char**argv) {
    uint64_t result = 0;
    char buf[] = "123456789";

   DECLARE_TEST(IBM_3740, buf, result);
   DECLARE_TEST(KERMIT, buf, result);

    DECLARE_TEST(ISO_14443_3_A, buf, result);
    DECLARE_TEST(MAXIM_DOW, buf, result);
    DECLARE_TEST(USB, buf, result);

    DECLARE_TEST(AUTOSAR_32, buf, result);
    DECLARE_TEST(GO_ISO, buf, result);
    //precompute_table2(CRC_AUTOSAR_32);
    //printf("0x%.8lx 0x%.8lx 0x%.8lx 0x%.8lx\n", table[0], table[1], table[2], table[3]);
    //printf("0x%.8lx 0x%.8lx 0x%.8lx 0x%.8lx\n", table2[0], table2[1], table2[2], table2[3]);
    DECLARE_TEST(CDMA2000, buf, result);

    return 0;
}