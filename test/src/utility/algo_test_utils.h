#ifndef ALGO_TEST_UTILS_H_
#define ALGO_TEST_UTILS_H_

#include <stddef.h>
#include <stdint.h>

/* This macro would look nicer as a braced-expression, but that isn't allowed in
 * ISO C. See the git history
 */
#define CRCANY_INVOKE_ALGO(func, bitwidth, buf, buf_size)                     \
	func((func(0, NULL, 0) | ~((((uintmax_t)1 << (bitwidth - 1)) << 1) - 1)), \
		 buf, buf_size)

#define EXHAUSTIVE_CRC_CHECK(CRC_NAME, REF_ALGO_NAME)                          \
	int main(int argc, char **argv)                                            \
	{                                                                          \
		int test_status = EXIT_FAILURE;                                        \
		const size_t buf_size = 256;                                           \
		uintmax_t expected = 0, computed = 0;                                  \
		unsigned char *buffer = calloc(buf_size, 1);                           \
		if (!buffer)                                                           \
			return test_status;                                                \
		fill_test_array(&buffer, buf_size);                                    \
		expected = CRCANY_INVOKE_ALGO(REF_ALGO_NAME, CRC_NAME.width, buffer,   \
									  buf_size);                               \
		computed = compute_crc(CRC_NAME, buffer, buf_size);                    \
		if (expected == computed) {                                            \
			printf("Test succeeded\n");                                        \
			test_status = EXIT_SUCCESS;                                        \
		} else {                                                               \
			fprintf(stderr, "Test failed\nexpected: 0x%lx\ncomputed: 0x%lx\n", \
					expected, computed);                                       \
			test_status = EXIT_FAILURE;                                        \
		}                                                                      \
		free(buffer);                                                          \
		return test_status;                                                    \
	}

void fill_test_array(unsigned char **buf, size_t buf_size)
{
	for (size_t i = 0; i < buf_size; i++) {
		(*buf)[i] = (unsigned char)i % 256;
	}
}

#endif /* ALGO_TEST_UTILS_H_ */
