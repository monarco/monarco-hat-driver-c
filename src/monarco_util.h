/***************************************************************************//**
 * @file monarco_util.h
 * @brief libmonarco - Misc Functions
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

/* Convert required PWM frequency in Hz `freq_hz` to 16-bit value for process data. */
uint16_t monarco_util_pwm_freq_to_u16(double freq_hz);

/* Convert required PWM duty cycle in range 0.0 to 1.0 `dc` to 16-bit value for process data. */
uint16_t monarco_util_pwm_dc_to_u16(double dc);

/* Convert required voltage on analog output in V `volts` to 16-bit value for process data. */
uint16_t monarco_util_aout_volts_to_u16(double volts);

/* Convert 16-bit value `ain` from process data to voltage in V on analog input (0-10V mode). */
double monarco_util_ain_10v_to_real(uint16_t ain);

/* Convert 16-bit value `ain` from process data to current in mA on analog input (0-25mA mode). */
double monarco_util_ain_20ma_to_real(uint16_t ain);

/* Debug print of SPI TX data structure. */
void monarco_util_dump_tx(monarco_struct_tx_t *tx);

/* Debug print of SPI RX data structure. */
void monarco_util_dump_rx(monarco_struct_rx_t *rx);

#ifdef __cplusplus
}
#endif

#endif
