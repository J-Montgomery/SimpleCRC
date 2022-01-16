#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#include "utility/algo_test_utils.h"

DECLARE_CRC(GO_ISO, 0x000000000000001b, 0xffffffffffffffff, true, true,
			0xffffffffffffffff, 0x0, 0xb90956c775a41001, 64);

BASIC_CRC_CHECK(CRC_GO_ISO)
