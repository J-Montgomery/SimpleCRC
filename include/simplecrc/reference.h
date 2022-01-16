#ifndef SIMPLECRC_REFERENCE_H_
#define SIMPLECRC_REFERENCE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <simplecrc.h>

#ifndef INCLUDE_CRCS_8_15
#define INCLUDE_CRCS_8_15 (true)
#endif

#ifndef INCLUDE_CRCS_16_31
#define INCLUDE_CRCS_16_31 (true)
#endif

#ifndef INCLUDE_CRCS_32_63
#define INCLUDE_CRCS_32_63 (true)
#endif

#ifndef INCLUDE_CRCS_MISC
#define INCLUDE_CRCS_MISC (true)
#endif

#if INCLUDE_CRCS_8_15
DECLARE_CRC(CDMA2000_8, 0x9b, 0xff, false, false, 0x00, 0x0, 0xda, 8);
#endif /* INCLUDE_CRCS_8_15 */

#if INCLUDE_CRCS_16_31
DECLARE_CRC(IBM_3740, 0x1021, 0xFFFF, false, false, 0x0, 0x0, 0x29b1, 16);
DECLARE_CRC(KERMIT, 0x1021, 0x0000, true, true, 0x0, 0x0, 0x2189, 16);
DECLARE_CRC(LTE_A, 0x864cfb, 0x00, false, false, 0x000000, 0x000000, 0xcde703,
			24); /* A.K.A CRC-24Q */
#endif /* INCLUDE_CRCS_16_31 */

#if INCLUDE_CRCS_32_63
DECLARE_CRC(AUTOSAR_32, 0xf4acfb13, 0xffffffff, true, true, 0xffffffff, 0x0,
			0x1697d06a, 32);
DECLARE_CRC(GSM_40, 0x0004820009, 0x0, false, false, 0xffffffffff, 0x0,
			0xd4164fc646, 40);
#endif /* INCLUDE_CRCS_32_63 */

#if INCLUDE_CRCS_MISC
DECLARE_CRC(GO_ISO, 0x000000000000001b, 0xffffffffffffffff, true, true,
			0xffffffffffffffff, 0x0, 0xb90956c775a41001, 64);
#endif /* INCLUDE_CRCS_MISC */

#ifdef __cplusplus
}
#endif

#endif /* SIMPLECRC_REFERENCE_H_ */
