#ifndef ALGO_TEST_UTILS_H_
#define ALGO_TEST_UTILS_H_

#include <stddef.h>
#include <stdint.h>

#define CRCANY_INVOKE_ALGO(func, bitwidth, buf, buf_size)                \
	({                                                                   \
		uintmax_t crc_null = func(0, NULL, 0);                           \
		uintmax_t init = crc_null |                                      \
						 ~((((uintmax_t)1 << (bitwidth - 1)) << 1) - 1); \
		func(init, buf, buf_size);                                       \
	})

void fill_test_array(char **buf, size_t buf_size)
{
	for (size_t i = 0; i < buf_size; i++) {
		(*buf)[i] = (char)i % 256;
	}
}

#endif /* ALGO_TEST_UTILS_H_ */
