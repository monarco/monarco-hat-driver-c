/***************************************************************************//**
 * @file monarco_util.c
 * @brief libmonarco - Misc Functions
 *******************************************************************************
 * @section License
 * Copyright REX Controls s.r.o. http://www.rexcontrols.com
 * Author: Vlastimil Setka
 * This file is covered by the BSD 3-Clause License
 *   see LICENSE.txt in the root directory of this project
 *   or <https://opensource.org/licenses/BSD-3-Clause>
 *******************************************************************************/

#include "monarco_util.h"

#include <stdio.h>
#include <math.h>

// 12bit full ADC range
#define MONARCO_ADC_RANGE 4095
// full ADC range is 10.0 V in current Monarco HAT firmware voltage mode
#define MONARCO_ADC_10V_RANGE 10.0
// full ADC range is 52.475 in current Monarco HAT firmware current loop mode
#define MONARCO_ADC_20MA_RANGE 52.475

uint16_t monarco_util_pwm_freq_to_u16(double freq_hz)
{
    if (freq_hz < 1) {
        return 0;
    } else if (freq_hz < 10) {
        return 3 + ((uint16_t)round(32000000 / 512 / freq_hz) & 0xFFFC);
    } else if (freq_hz < 100) {
        return 2 + ((uint16_t)round(32000000 / 64 / freq_hz) & 0xFFFC);
    } else if (freq_hz < 1000) {
        return 1 + ((uint16_t)round(32000000 / 8 / freq_hz) & 0xFFFC);
    }  else if (freq_hz < 100000) {
        return 0 + ((uint16_t)round(32000000 / 1 / freq_hz) & 0xFFFC);
    } else {
        return 0;
    }
}

uint16_t monarco_util_pwm_dc_to_u16(double dc)
{
    if (dc <= 0) {
        return 0;
    } else if (dc >= 1) {
        return 65535;
    } else {
        return (uint16_t)round(65535 * dc);
    }
}

uint16_t monarco_util_aout_volts_to_u16(double volts)
{
    if (volts < 0) {
        return 0;
    } if (volts > 10) {
        return MONARCO_ADC_RANGE;
    } else {
        return round(volts / MONARCO_ADC_10V_RANGE * MONARCO_ADC_RANGE);
    }
}

double monarco_util_ain_10v_to_real(uint16_t ain)
{
    return (double)(ain) * MONARCO_ADC_10V_RANGE / MONARCO_ADC_RANGE;
}

double monarco_util_ain_20ma_to_real(uint16_t ain)
{
    return (double)(ain) * MONARCO_ADC_20MA_RANGE / MONARCO_ADC_RANGE;
}

void monarco_util_dump_tx(monarco_struct_tx_t *tx)
{
    printf("TX: SDC[V:0x%04X A:0x%03X W:%u E:%u] CTRL:0x%02X LED_MASK:0x%02X LED_VALUE:0x%02X DO:0x%1X PWM1DIV:0x%02X PWM1A:0x%02X PWM1B:0x%02X PWM1C:0x%02X PWM2DIV:0x%02X PWM2A:0x%02X AO1:0x%02X AO2:0x%02X CRC:0x%04X\n",
        tx->sdc_req.value, tx->sdc_req.address, tx->sdc_req.write, tx->sdc_req.error,
        tx->control_byte.u8, tx->led_mask, tx->led_value, tx->dout, tx->pwm1_div, tx->pwm1a_dc, tx->pwm1b_dc, tx->pwm1c_dc, tx->pwm2_div, tx->pwm2a_dc, tx->aout1, tx->aout2, tx->crc
    );
}

void monarco_util_dump_rx(monarco_struct_rx_t *rx)
{
    printf("RX: SDC[V:0x%04X A:0x%03X W:%u E:%u] STAT:0x%02X DI:0x%1X CNT1:0x%04X CNT2:0x%04X CNT3:0x%04X AI1:0x%04X AI2:0x%04X CRC:0x%04X\n",
        rx->sdc_resp.value, rx->sdc_resp.address, rx->sdc_resp.write, rx->sdc_resp.error,
        rx->status_byte.u8, rx->din, rx->cnt1, rx->cnt2, rx->cnt3, rx->ain1, rx->ain2, rx->crc
    );
}
