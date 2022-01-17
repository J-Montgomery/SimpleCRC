#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#include "utility/algo_test_utils.h"

DECLARE_CRC(KERMIT, 0x1021, 0x0000, true, true, 0x0, 0x0, 0x2189, 16);

BASIC_CRC_CHECK(CRC_KERMIT)
