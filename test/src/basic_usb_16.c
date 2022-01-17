#include <stdio.h>
#include <simplecrc.h>
#include <internal/crc_utility.h>

#include "utility/algo_test_utils.h"

DECLARE_CRC(USB_16, 0x8005, 0xffff, true, true, 0xffff, 0x0, 0xb4c8, 16);

BASIC_CRC_CHECK(CRC_USB_16)
