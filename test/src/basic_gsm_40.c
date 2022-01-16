#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#include "utility/algo_test_utils.h"

DECLARE_CRC(GSM_40, 0x0004820009, 0x0, false, false, 0xffffffffff, 0x0,
			0xd4164fc646, 40);

BASIC_CRC_CHECK(CRC_GSM_40)
