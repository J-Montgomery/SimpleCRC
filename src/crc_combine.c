#include "simplecrc.h"
#include "internal/crc_utility.h"
#include <stdio.h>

uint64_t combine_crcs(struct crc_def params, uint64_t crc_msg1, uint64_t crc_msg2, size_t msg2_len)
{
    /* CRC(msg1 + msg2) = CRC(m1 + ZEROS(m2_len)) ^ CRC(m2) */
    return crc_msg1 ^ crc_msg2;
}