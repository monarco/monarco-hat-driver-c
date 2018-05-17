/***************************************************************************//**
 * @file monarco_crc.h
 * @brief libmonarco - CRC16
 *******************************************************************************
 * @section License
 * Copyright REX Controls s.r.o. http://www.rexcontrols.com
 * Author: Vlastimil Setka
 * This file is covered by the BSD 3-Clause License
 *   see LICENSE.txt in the root directory of this project
 *   or <https://opensource.org/licenses/BSD-3-Clause>
 *******************************************************************************/

#ifndef LIBMONARCO_CRC_H_
#define LIBMONARCO_CRC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Return CRC16 checksum (Modbus RTU CRC-16 / CRC-16-IBM) of `*data` with length `length`. */
uint16_t monarco_crc16(const char *data, int length);

#ifdef __cplusplus
}
#endif

#endif
