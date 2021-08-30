#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

uint64_t table[256] = { 0 };
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
#define DECLARE_CRC(NAME, POLY, INIT, REF_IN, REF_OUT, XOR_OUT, RESIDUE,    \
		    CHECK, WIDTH)                                           \
	struct crc16 CRC_##NAME = (struct crc16)                            \
	{                                                                   \
		.poly = POLY, .init = INIT, .ref_in = REF_IN,               \
		.ref_out = REF_OUT, .xor_out = XOR_OUT, .residue = RESIDUE, \
		.check = CHECK, .width = WIDTH                              \
	}

DECLARE_CRC(IBM_3740, 0x1021, 0xFFFF, false, false, 0x0, 0x0, 0x29b1, 16);
DECLARE_CRC(KERMIT, 0x1021, 0x0000, true, true, 0x0, 0x0, 0x2189, 16);
DECLARE_CRC(ISO_14443, 0x1021, 0xc6c6, true, true, 0x0, 0x0, 0xbf05, 16);
DECLARE_CRC(MAXIM_DOW, 0x8005, 0x0000, true, true, 0xffff, 0x0, 0x44c2, 16);
DECLARE_CRC(USB_16, 0x8005, 0xffff, true, true, 0xffff, 0x0, 0xb4c8, 16);
DECLARE_CRC(DECT_12, 0x80f, 0x000, false, false, 0x000, 0x0, 0xf5b, 12);
DECLARE_CRC(AUTOSAR_32, 0xf4acfb13, 0xffffffff, true, true, 0xffffffff, 0x0,
	    0x1697d06a, 32);
DECLARE_CRC(GO_ISO, 0x000000000000001b, 0xffffffffffffffff, true, true,
	    0xffffffffffffffff, 0x0, 0xb90956c775a41001, 64);
DECLARE_CRC(GSM_40, 0x0004820009, 0x0, false, false, 0xffffffffff, 0x0,
	    0xd4164fc646, 40);
DECLARE_CRC(CDMA2000, 0x9b, 0xff, false, false, 0x00, 0x0, 0xda, 8);
DECLARE_CRC(DARC, 0x39, 0x00, true, true, 0x00, 0x0, 0x15, 8);
DECLARE_CRC(CAN_FD, 0x1685b, 0x00, false, false, 0x00, 0x0, 0x04f03, 17);

DECLARE_CRC(BZIP2, 0x04c11db7, 0xffffffff, false, false, 0xffffffff, 0x0,
	    0xfc891918, 32);
DECLARE_CRC(ROHC_3, 0x6, 0x7, false, false, 0x0, 0x0, 0x6, 3);
DECLARE_CRC(ROHC_7, 0x79, 0x7f, false, false, 0x0, 0x0, 0x53, 7);
DECLARE_CRC(GSM, 0x2f, 0x00, false, false, 0x3f, 0x0, 0x13, 6);

#define DECLARE_TEST(NAME, BUF, VAR)                               \
	VAR = compute_crc_bit(BUF, 9, CRC_##NAME);                 \
	printf("%s %s:\t\t0x%.8lx\n",                              \
	       (result == CRC_##NAME.check) ? "PASSED" : "FAILED", \
	       xstr(CRC_##NAME), VAR)

uint64_t gen_mask_bit(uint64_t topbit)
{
	uint64_t mask = ((topbit - 1) << 1) | 0x01;
	return mask;
}

uint64_t reflect_bit(uint64_t val, unsigned count)
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

void print_mem(uint64_t *buf, unsigned len)
{
	for (unsigned i = 0; i < len; i++) {
		printf("%02lx ", buf[i]);
		if ((i + 1) % 16 == 0)
			printf("\n");
	}
}

void precompute_table_bit(struct crc16 params)
{
	const uint64_t topbit = (uint64_t)1 << (params.width - 1);
	const uint64_t mask = gen_mask_bit(topbit);
	const unsigned bits = (params.width > 8) ? params.width : 8;

	uint64_t crc = 0;
	for (int i = 0; i < 256; i++) {
		crc = i;
		for (int j = bits; j > 0; --j) {
			if (params.width < 8) {
				if (crc & 1)
					crc = (crc >> 1) ^ params.poly;
				else
					crc = (crc >> 1);
			} else {
				if (crc & topbit)
					crc = (crc << 1) ^ params.poly;
				else
					crc <<= 1;
			}
		}
		table[i] = crc & mask;
	}
}

unsigned char crc_calc_test(unsigned char *buf, int size, uint64_t *table,
			    unsigned init, unsigned width)
{
	unsigned char crc = init;
	unsigned mask = (unsigned)gen_mask_bit((uint64_t)1 << (width - 1));
	for (int i = 0; i < size; i++) {
		crc = table[buf[i] ^ (crc & mask)];
	}
	return crc;
}

uint64_t compute_crc_bit(const char *buf, size_t len, struct crc16 params)
{
	uint64_t crc;
	const uint64_t mask = gen_mask_bit((uint64_t)1 << (params.width - 1));
	const char *ptr;
	size_t a;

	precompute_table_bit(params);

	crc = params.init;
	ptr = buf;

	for (a = 0; a < len; a++) {
		uint8_t u_char = (uint8_t)*ptr++;
		if (params.ref_in)
			u_char = reflect_bit(u_char, 8);
		if (params.width >= 8)
			crc = table[((crc >> (params.width - 8)) ^
				     (uint64_t)u_char) &
				    0xFFull] ^
			      (crc << 8);
		else {
			crc = table[u_char ^ (crc & mask)];
		}
	}

	if (params.ref_out)
		crc = reflect_bit(crc, params.width) ^ params.xor_out;
	else
		crc = crc ^ params.xor_out;

	return crc & mask;
}

#ifdef ENABLE_MAIN

int main(int argc, char **argv)
{
	uint64_t result = 0;
	char buf[] = "123456789";

	DECLARE_TEST(IBM_3740, buf, result);
	DECLARE_TEST(KERMIT, buf, result);

	DECLARE_TEST(ISO_14443, buf, result);
	DECLARE_TEST(MAXIM_DOW, buf, result);
	DECLARE_TEST(USB_16, buf, result);

	DECLARE_TEST(DECT_12, buf, result);
	DECLARE_TEST(AUTOSAR_32, buf, result);
	DECLARE_TEST(GO_ISO, buf, result);

	DECLARE_TEST(GSM_40, buf, result);
	DECLARE_TEST(DARC, buf, result);
	DECLARE_TEST(CDMA2000, buf, result);
	DECLARE_TEST(CAN_FD, buf, result);
	DECLARE_TEST(GO_ISO, buf, result);
	DECLARE_TEST(CDMA2000, buf, result);
	DECLARE_TEST(BZIP2, buf, result);
	DECLARE_TEST(ROHC_3, buf, result);
	DECLARE_TEST(ROHC_7, buf, result);
	DECLARE_TEST(GSM, buf, result);
	return 0;
}

#endif /* ENABLE_MAIN */
