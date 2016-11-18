/***************************************************************************//**
 * @file monarco_util.h
 * @brief LibMonarco - Misc Functions
 *******************************************************************************
 * @section License
 * Copyright REX Controls s.r.o. http://www.rexcontrols.com
 * Author: Vlastimil Setka
 * This file is covered by the BSD 3-Clause License
 *   see LICENSE.txt in the root directory of this project
 *   or <https://opensource.org/licenses/BSD-3-Clause>
 *******************************************************************************/

#ifndef LIBMONARCO_UTIL_H_
#define LIBMONARCO_UTIL_H_

#include <stdint.h>
#include "monarco_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

void monarco_util_dump_tx(monarco_struct_tx_t *tx);
void monarco_util_dump_rx(monarco_struct_rx_t *rx);

uint16_t monarco_util_pwm_freq_to_u16(double freq_hz);
uint16_t monarco_util_pwm_dc_to_u16(double dc);

uint16_t monarco_util_aout_volts_to_u16(double volts);

double monarco_util_ain_10v_to_real(uint16_t ain);
double monarco_util_ain_20ma_to_real(uint16_t ain);

#ifdef __cplusplus
}
#endif

#endif
