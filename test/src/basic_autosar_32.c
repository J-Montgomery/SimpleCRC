#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#include "utility/algo_test_utils.h"

DECLARE_CRC(AUTOSAR_32, 0xf4acfb13, 0xffffffff, true, true, 0xffffffff, 0x0,
			0x1697d06a, 32);

BASIC_CRC_CHECK(CRC_AUTOSAR_32)
