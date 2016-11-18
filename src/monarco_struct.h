/***************************************************************************//**
 * @file monarco_struct.h
 * @brief LibMonarco - SPI Protocol Structures
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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __attribute__((packed)) {
    uint16_t value;
    unsigned int address : 12;
    unsigned int write : 1;
    unsigned int error : 1;
    unsigned int reserved : 2;
} monarco_struct_sdc_t;

#define MONARCO_SERVICE_STATUS_OK 0xABCD
#define MONARCO_SERVICE_ERROR_UNKNOWN_REG 0xFFFF
#define MONARCO_SERVICE_ERROR_CRC 0xCCCC

typedef struct __attribute__((packed)) {
    unsigned int status_led_en : 1;
    unsigned int status_led_on : 1;
    unsigned int ow_shutdown : 1;
    unsigned int reserved1 : 1;
    unsigned int cnt1_reset : 1;
    unsigned int cnt2_reset : 1;
    unsigned int sign_of_life : 2;
} monarco_struct_control_byte_t;

typedef struct __attribute__((packed)) {
    unsigned int reserved1 : 4;
    unsigned int cnt1_reset_done : 1;
    unsigned int cnt2_reset_done : 1;
    unsigned int sign_of_life : 2;
} monarco_struct_status_byte_t;

typedef struct __attribute__((packed)) {
    monarco_struct_sdc_t sdc_req;
    monarco_struct_control_byte_t control_byte;
    uint8_t led_en;
    uint8_t led_on;
    uint8_t dout;
    uint16_t pwm1_div;
    uint16_t pwm1a_dc;
    uint16_t pwm1b_dc;
    uint16_t pwm1c_dc;
    uint16_t pwm2_div;
    uint16_t pwm2a_dc;
    uint16_t aout1;
    uint16_t aout2;
    uint16_t crc;
} monarco_struct_tx_t;

typedef struct __attribute__((packed)) {
    monarco_struct_sdc_t sdc_resp;
    monarco_struct_status_byte_t status_byte;
    uint16_t reserved1;
    uint8_t din;
    uint32_t cnt1;
    uint32_t cnt2;
    uint32_t cnt3;
    uint16_t ain1;
    uint16_t ain2;
    uint16_t crc;
} monarco_struct_rx_t;

#define MONARCO_STRUCT_SIZE 26 // FIXME

#ifdef __cplusplus
}
#endif

#endif
