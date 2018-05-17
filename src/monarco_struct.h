/***************************************************************************//**
 * @file monarco_struct.h
 * @brief libmonarco - SPI Protocol Structures
 *******************************************************************************
 * @section License
 * Copyright REX Controls s.r.o. http://www.rexcontrols.com
 * Author: Vlastimil Setka
 * This file is covered by the BSD 3-Clause License
 *   see LICENSE.txt in the root directory of this project
 *   or <https://opensource.org/licenses/BSD-3-Clause>
 *******************************************************************************/

#ifndef LIBMONARCO_STRUCT_H_
#define LIBMONARCO_STRUCT_H_

#include <stdint.h>

/* Check SPI protocol documentation for more details:
 *   https://github.com/monarco/monarco-hat-documentation/blob/master/Monarco_HAT_SPI_Protocol.md
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Service Data Channel (SDC) Request / Response bit-structure */
typedef struct __attribute__((packed)) {
    uint16_t value; /* Register value to write / readed, or error code */
    unsigned int address : 12; /* Register Address */
    unsigned int write : 1; /* 0 = read, 1 = write */
    unsigned int error : 1; /* Request: allways 0; Response: 0 = success, 1 = error */
    unsigned int reserved : 2; /* Reserved */
} monarco_struct_sdc_t;

/* Control Byte bit-structure */
typedef union {
    uint8_t u8;
    struct __attribute__((packed)) {
        unsigned int status_led_mask : 1; /* Status LED (LED0) user control enable (mask) */
        unsigned int status_led_value : 1; /* Status LED (LED0) user control value */
        unsigned int ow_shutdown : 1; /* 1-Wire power shutdown when 1 */
        unsigned int reserved1 : 1; /* Reserved */
        unsigned int cnt1_reset : 1; /* COUNTER1 value reset (edge sensitive) */
        unsigned int cnt2_reset : 1; /* COUNTER2 value reset (edge sensitive) */
        unsigned int sign_of_life : 2; /* Not implemented */
    };
} monarco_struct_control_byte_t;

/* Main SPI Structure - Master TX to Monarco HAT Slave RX */
typedef struct __attribute__((packed)) {
    monarco_struct_sdc_t sdc_req; /* Service Data Channel (SDC) Request struct */
    monarco_struct_control_byte_t control_byte; /* Control Byte bit-structure */
    uint8_t led_mask; /* User LEDs (LED1..LED8) user control mask - set bit for each LED under user control to 1 */
    uint8_t led_value; /* User LEDs (LED1..LED8) user control value - set bit for each LED to 1 for light on */
    uint8_t dout; /* Digital outputs (bits 0..3 = DOUT1..DOUT4, bits 4..7 = RESERVED) */
    uint16_t pwm1_div; /* PWM1 frequency, use monarco_util_pwm_freq_to_u16() for conversion */
    uint16_t pwm1a_dc; /* PWM1 Channel A (DOUT1) duty cycle, use monarco_util_pwm_dc_to_u16() for conversion */
    uint16_t pwm1b_dc; /* PWM1 Channel B (DOUT2) duty cycle, use monarco_util_pwm_dc_to_u16() for conversion */
    uint16_t pwm1c_dc; /* PWM1 Channel C (DOUT3) duty cycle, use monarco_util_pwm_dc_to_u16() for conversion */
    uint16_t pwm2_div; /* PWM2 frequency, use monarco_util_pwm_freq_to_u16() for conversion */
    uint16_t pwm2a_dc; /* PWM2 Channel A (DOUT4) duty cycle, use monarco_util_pwm_dc_to_u16() for conversion */
    uint16_t aout1; /* Analog output 1 (0..4095 = 0..10 V, > 4095 = RESERVED) */
    uint16_t aout2; /* Analog output 2 (0..4095 = 0..10 V, > 4095 = RESERVED) */
    uint16_t crc; /* Checksum */
} monarco_struct_tx_t;

/* Status Byte bit-structure */
typedef union {
    uint8_t u8;
    struct __attribute__((packed)) {
        unsigned int reserved1 : 4; /* Reserved */
        unsigned int cnt1_reset_done : 1; /* COUNTER1 value reset done */
        unsigned int cnt2_reset_done : 1; /* COUNTER2 value reset done */
        unsigned int sign_of_life : 2; /* Incremented with each SPI transfer */
    };
} monarco_struct_status_byte_t;

/* Main SPI Structure - Master RX from Monarco HAT Slave TX */
typedef struct __attribute__((packed)) {
    monarco_struct_sdc_t sdc_resp; /* Service Data Channel (SDC) Response struct */
    monarco_struct_status_byte_t status_byte; /* Status Byte bit-structure */
    uint16_t reserved1; /* Reserved */
    uint8_t din; /* Digital inputs (bits 0..3 = DIN1..DIN4, bits 4..7 = RESERVED) */
    uint32_t cnt1; /* COUNTER1 Value, now only 2 bytes are used */
    uint32_t cnt2; /* COUNTER2 Value, now only 2 bytes are used */
    uint32_t cnt3; /* COUNTER3 Value, now not implemented */
    uint16_t ain1; /* Analog input 1, use monarco_util_ain_10v_to_real() or monarco_util_ain_20ma_to_real for conversion */
    uint16_t ain2; /* Analog input 2, use monarco_util_ain_10v_to_real() or monarco_util_ain_20ma_to_real for conversion */
    uint16_t crc; /* Checksum */
} monarco_struct_rx_t;

#define MONARCO_STRUCT_SIZE 26 // FIXME

#ifdef __cplusplus
}
#endif

#endif
