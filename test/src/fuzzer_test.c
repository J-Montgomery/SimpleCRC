#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <simplecrc/reference.h>
#include <simplecrc.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t data_size)
{
	uint64_t computed = compute_crc(CRC_KERMIT, data, data_size);
	return 0;
}
