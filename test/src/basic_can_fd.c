#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#include "utility/algo_test_utils.h"

DECLARE_CRC(CAN_FD, 0x1685b, 0x00, false, false, 0x00, 0x0, 0x04f03, 17);

BASIC_CRC_CHECK(CRC_CAN_FD)
