#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#include "utility/algo_test_utils.h"

DECLARE_CRC(ISO_14443, 0x1021, 0xc6c6, true, true, 0x0, 0x0, 0xbf05, 16);

BASIC_CRC_CHECK(CRC_ISO_14443)
