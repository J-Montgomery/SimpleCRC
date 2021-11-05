#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#define xstr(a) str(a)
#define str(a) #a

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

#define DECLARE_BIT_TEST(NAME, BUF, VAR)                           \
	VAR = compute_crc_bit(BUF, 9, CRC_##NAME);                 \
	printf("%s %-16s 0x%.8lx\n",                               \
	       (result == CRC_##NAME.check) ? "PASSED" : "FAILED", \
	       xstr(CRC_##NAME), VAR);   \
    if(result != CRC_##NAME.check) return 1

int main(int argc, char **argv)
{
	uint64_t result = 0;
	char buf[] = "123456789";

	DECLARE_BIT_TEST(IBM_3740, buf, result);
	DECLARE_BIT_TEST(KERMIT, buf, result);

	DECLARE_BIT_TEST(ISO_14443, buf, result);
	DECLARE_BIT_TEST(MAXIM_DOW, buf, result);
	DECLARE_BIT_TEST(USB_16, buf, result);

	DECLARE_BIT_TEST(DECT_12, buf, result);
	DECLARE_BIT_TEST(AUTOSAR_32, buf, result);
	DECLARE_BIT_TEST(GO_ISO, buf, result);

	DECLARE_BIT_TEST(GSM_40, buf, result);
	DECLARE_BIT_TEST(DARC, buf, result);
	DECLARE_BIT_TEST(CDMA2000, buf, result);
	DECLARE_BIT_TEST(CAN_FD, buf, result);
	DECLARE_BIT_TEST(GO_ISO, buf, result);
	DECLARE_BIT_TEST(CDMA2000, buf, result);
	DECLARE_BIT_TEST(BZIP2, buf, result);
	DECLARE_BIT_TEST(ROHC_3, buf, result);
	DECLARE_BIT_TEST(ROHC_7, buf, result);
	DECLARE_BIT_TEST(GSM, buf, result);
	return 0;
}