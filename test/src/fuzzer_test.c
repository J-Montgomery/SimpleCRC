#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <simplecrc/reference.h>
#include <simplecrc.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t data_size)
{
	uint64_t computed = compute_crc(data, data_size, CRC_KERMIT);
	return 0;
}
