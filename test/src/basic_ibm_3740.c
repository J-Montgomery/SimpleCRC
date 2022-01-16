#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#include "utility/algo_test_utils.h"

DECLARE_CRC(IBM_3740, 0x1021, 0xFFFF, false, false, 0x0, 0x0, 0x29b1, 16);

BASIC_CRC_CHECK(CRC_IBM_3740)
