#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#include "utility/algo_test_utils.h"

DECLARE_CRC(CDMA2000, 0x9b, 0xff, false, false, 0x00, 0x0, 0xda, 8);

BASIC_CRC_CHECK(CRC_CDMA2000)
