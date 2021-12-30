#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#define xstr(a) str(a)
#define str(a) #a

DECLARE_CRC(ROHC_3, 0x3, 0x7, true, true, 0x0, 0x0, 0x6, 3);
DECLARE_CRC(ROHC_7, 0x79, 0x7f, false, false, 0x0, 0x0, 0x53, 7);
DECLARE_CRC(GSM, 0x2f, 0x00, false, false, 0x3f, 0x3a, 0x13, 6);
DECLARE_CRC(MMC, 0x09, 0x00, false, false, 0x00, 0x00, 0x75, 7);
DECLARE_CRC(INTERLAKEN, 0x3, 0xf, false, false, 0xf, 0x2, 0xb, 4);
DECLARE_CRC(UMTS, 0x45, 0x00, false, false, 0x0, 0x0, 0x61, 7);


#define ONES(n) (((uint8_t)0 - 1) >> (8 - (n)))
#define WIDTH  (8 * sizeof(uint8_t))
#define TOPBIT (1 << (WIDTH - 1))

static uint8_t const table_byte[256] = {
    0x90, 0xa0, 0xf0, 0xc0, 0x50, 0x60, 0x30, 0x00, 0x20, 0x10, 0x40, 0x70, 0xe0,
    0xd0, 0x80, 0xb0, 0xc0, 0xf0, 0xa0, 0x90, 0x00, 0x30, 0x60, 0x50, 0x70, 0x40,
    0x10, 0x20, 0xb0, 0x80, 0xd0, 0xe0, 0x30, 0x00, 0x50, 0x60, 0xf0, 0xc0, 0x90,
    0xa0, 0x80, 0xb0, 0xe0, 0xd0, 0x40, 0x70, 0x20, 0x10, 0x60, 0x50, 0x00, 0x30,
    0xa0, 0x90, 0xc0, 0xf0, 0xd0, 0xe0, 0xb0, 0x80, 0x10, 0x20, 0x70, 0x40, 0xe0,
    0xd0, 0x80, 0xb0, 0x20, 0x10, 0x40, 0x70, 0x50, 0x60, 0x30, 0x00, 0x90, 0xa0,
    0xf0, 0xc0, 0xb0, 0x80, 0xd0, 0xe0, 0x70, 0x40, 0x10, 0x20, 0x00, 0x30, 0x60,
    0x50, 0xc0, 0xf0, 0xa0, 0x90, 0x40, 0x70, 0x20, 0x10, 0x80, 0xb0, 0xe0, 0xd0,
    0xf0, 0xc0, 0x90, 0xa0, 0x30, 0x00, 0x50, 0x60, 0x10, 0x20, 0x70, 0x40, 0xd0,
    0xe0, 0xb0, 0x80, 0xa0, 0x90, 0xc0, 0xf0, 0x60, 0x50, 0x00, 0x30, 0x70, 0x40,
    0x10, 0x20, 0xb0, 0x80, 0xd0, 0xe0, 0xc0, 0xf0, 0xa0, 0x90, 0x00, 0x30, 0x60,
    0x50, 0x20, 0x10, 0x40, 0x70, 0xe0, 0xd0, 0x80, 0xb0, 0x90, 0xa0, 0xf0, 0xc0,
    0x50, 0x60, 0x30, 0x00, 0xd0, 0xe0, 0xb0, 0x80, 0x10, 0x20, 0x70, 0x40, 0x60,
    0x50, 0x00, 0x30, 0xa0, 0x90, 0xc0, 0xf0, 0x80, 0xb0, 0xe0, 0xd0, 0x40, 0x70,
    0x20, 0x10, 0x30, 0x00, 0x50, 0x60, 0xf0, 0xc0, 0x90, 0xa0, 0x00, 0x30, 0x60,
    0x50, 0xc0, 0xf0, 0xa0, 0x90, 0xb0, 0x80, 0xd0, 0xe0, 0x70, 0x40, 0x10, 0x20,
    0x50, 0x60, 0x30, 0x00, 0x90, 0xa0, 0xf0, 0xc0, 0xe0, 0xd0, 0x80, 0xb0, 0x20,
    0x10, 0x40, 0x70, 0xa0, 0x90, 0xc0, 0xf0, 0x60, 0x50, 0x00, 0x30, 0x10, 0x20,
    0x70, 0x40, 0xd0, 0xe0, 0xb0, 0x80, 0xf0, 0xc0, 0x90, 0xa0, 0x30, 0x00, 0x50,
    0x60, 0x40, 0x70, 0x20, 0x10, 0x80, 0xb0, 0xe0, 0xd0};

uint8_t crc4interlaken_byte(uint8_t crc, void const *mem, size_t len) {
    uint8_t const *data = mem;
    crc &= 0xf;
    crc <<= 4;
    while (len--)
        crc = table_byte[crc ^ *data++];
    crc >>= 4;
    return crc;
}

uint64_t bit_table[256] = { 0 };

uint64_t crc_bit_one_byte(int init, int width, uint64_t poly)
{
	uint64_t topbit = (uint64_t)1 << (width - 1);
	uint64_t crc = init;

	for (int bit = width; bit > 0; --bit) {
		if (crc & 1)
			crc = (crc >> 1) ^ poly;
		else
			crc = crc << 1;
	}

	return crc & gen_mask(width);
}

void precompute_table_bit_bitshift(struct crc_def params)
{
	for (int byte = 0; byte < 256; ++byte) {
		uint64_t crc = crc_bit_one_byte(byte, params.width, params.poly);
		bit_table[byte] = crc;
	}
}

uint64_t compute_crc_test(const unsigned char *buf, size_t len,
					 struct crc_def params)
{
	uint64_t crc;
	const unsigned char *ptr;
	size_t a;

	precompute_table_bit_bitshift(params);

	crc = params.init;
	ptr = buf;

	for (a = 0; a < len; a++) {
		uint8_t u_char = (uint8_t)*ptr++;
		if (params.ref_in)
			u_char = reflect(u_char, 8);
		if (params.width >= 8)
			crc = bit_table[((crc >> (params.width - 8)) ^ (uint64_t)u_char) &
							0xFFull] ^
				  (crc << 8);
	}

	if (params.ref_out)
		crc = reflect(crc, params.width) ^ params.xor_out;
	else
		crc = crc ^ params.xor_out;
	return crc & gen_mask(params.width);
}

uint8_t crc3rohc_bit(uint8_t crc, void const *mem, size_t len) {
    uint8_t const *data = mem;
    if (data == NULL)
        return 0x7;
    crc &= 0x7;

	printf("crc rohc %x\n", crc);
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (unsigned k = 0; k < 8; k++) {
            crc = crc & 1 ? (crc >> 1) ^ 0x6 : crc >> 1;
        }
    }
    return crc;
}
uint64_t local_reflect(uint64_t val, unsigned count)
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

uint8_t crc_bitwise(struct crc_def params, void const *dat, size_t len) {
    unsigned char const *buf = dat;
    uint8_t poly = params.poly;
	uint8_t crc = params.init;
	crc &= (1 << params.width) - 1;

	if(params.ref_in)
	{
		poly = local_reflect(poly << (8 - params.width), 8);
	}

    // If requested, return the initial CRC.
    if (buf == NULL)
        return crc;

	//crc &= 0x7;
    // Pre-process the CRC.
    // crc ^= params.xor_out;

	printf("crc bitwise %x %x\n", crc, poly);
    // Process the input data a bit at a time.
	/*if (params.width > 8) {
        crc &= ONES(params.width);
        while (len--) {
            crc ^= *buf++;
            for (int k = 0; k < 8; k++)
                crc = crc & 1 ? (crc >> 1) ^ poly : crc >> 1;
        }
    }
	else */ if (params.width <= 8) {
        //unsigned shift = 8 - params.width;  // 0..7
        //poly <<= shift;
        //crc <<= shift;
        for (size_t i = 0; i < len; i++) {
            crc ^= buf[i];
            for (int k = 0; k < 8; k++)
                crc = crc & 0x1 ? (crc >> 1) ^ poly : crc >> 1;
        }
        //crc >>= shift;
        //crc &= ONES(params.width);
    }

	//if (params.ref_out)
    //    crc = reflect(crc, params.width);
    return crc ^ params.xor_out;
	//return crc;
}

#define DECLARE_BIT_TEST(NAME, BUF, VAR)                       \
	VAR = compute_crc_bit(BUF, 9, CRC_##NAME);                 \
	printf("%s %-16s 0x%.8lx\n",                               \
		   (result == CRC_##NAME.check) ? "PASSED" : "FAILED", \
		   xstr(CRC_##NAME), VAR);                             \
	if (result != CRC_##NAME.check)                            \
	return 1

int main(int argc, char **argv)
{
	uint64_t result = 0;
	char buf[] = "123456789";
	char buf2[1] = {1};
	uint8_t res = 0;

	//DECLARE_BIT_TEST(ROHC_3, buf, result);
	//DECLARE_BIT_TEST(ROHC_7, buf, result);

	res = crc4interlaken_byte(0x0, buf, 9);
	printf("result 0x%x\n", res);
	res = crc_bitwise(CRC_INTERLAKEN, buf, 9);
	printf("result2 0x%x\n", res);

	uintmax_t init = crc3rohc_bit(0, NULL, 0);
    uintmax_t blot = init | ~((((uintmax_t)1 << (3 - 1)) << 1) - 1);
	res = crc_bitwise(CRC_ROHC_3, buf, 9);
	printf("result3 0x%x\n", res);
	res = crc3rohc_bit(blot, buf, 9);
	printf("result rohc 0x%x 0x%lx\n", res, blot);

	res = compute_crc_test(buf, 9, CRC_ROHC_3);
	printf("result4 0x%x\n", res);
	//DECLARE_BIT_TEST(GSM, buf, result);
	//DECLARE_BIT_TEST(MMC, buf, result);
	DECLARE_BIT_TEST(INTERLAKEN, buf, result);

	return 0;
}