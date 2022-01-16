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

void fill_test_array(unsigned char **buf, size_t buf_size)
{
	for (size_t i = 0; i < buf_size; i++) {
		(*buf)[i] = (unsigned char)i % 256;
	}
}

#endif /* ALGO_TEST_UTILS_H_ */
