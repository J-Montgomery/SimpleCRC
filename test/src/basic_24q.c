#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#include "utility/algo_test_utils.h"

DECLARE_CRC(24Q, 0x864cfb, 0x00, false, false, 0x000000, 0x000000, 0xcde703,
			24);

BASIC_CRC_CHECK(CRC_24Q)
