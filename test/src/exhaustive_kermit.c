#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <simplecrc/reference.h>
#include <simplecrc.h>

#include "utility/algo_test_utils.h"

/* Generated by crcany */
uint16_t crc16kermit_bit(uint16_t crc, void const *mem, size_t len)
{
	unsigned char const *data = mem;
	if (data == NULL)
		return 0;
	for (size_t i = 0; i < len; i++) {
		crc ^= data[i];
		for (unsigned k = 0; k < 8; k++) {
			crc = crc & 1 ? (crc >> 1) ^ 0x8408 : crc >> 1;
		}
	}
	return crc;
}

int main(int argc, char **argv)
{
	int test_status = EXIT_FAILURE;
	const size_t buf_size = 256;
	uintmax_t expected = 0;
	uintmax_t computed = 0;

	char *buffer = calloc(buf_size, 1);
	if (!buffer) {
		return test_status;
	}

	// Ensure test buffer checks all values in lookup table
	fill_test_array(&buffer, buf_size);

	expected = CRCANY_INVOKE_ALGO(crc16kermit_bit, 16, buffer, buf_size);
	computed = compute_crc(buffer, buf_size, CRC_KERMIT);

	if (expected == computed) {
		printf("Test succeeded\n");
		test_status = EXIT_SUCCESS;
	} else {
		fprintf(stderr, "Test failed\nexpected: 0x%lx\ncomputed: 0x%lx\n",
				expected, computed);
		test_status = EXIT_FAILURE;
	}

	free(buffer);
	return test_status;
}
