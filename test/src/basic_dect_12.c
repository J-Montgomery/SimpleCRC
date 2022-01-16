#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#include "utility/algo_test_utils.h"

DECLARE_CRC(DECT_12, 0x80f, 0x000, false, false, 0x000, 0x0, 0xf5b, 12);

BASIC_CRC_CHECK(CRC_DECT_12)
