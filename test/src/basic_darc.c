#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#include "utility/algo_test_utils.h"

DECLARE_CRC(DARC, 0x39, 0x00, true, true, 0x00, 0x0, 0x15, 8);

BASIC_CRC_CHECK(CRC_DARC)
