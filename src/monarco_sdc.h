/***************************************************************************//**
 * @file monarco_sdc.h
 * @brief libmonarco - Service Data Channel (SDC) definitions
 *******************************************************************************
 * @section License
 * Copyright REX Controls s.r.o. http://www.rexcontrols.com
 * Author: Vlastimil Setka
 * This file is covered by the BSD 3-Clause License
 *   see LICENSE.txt in the root directory of this project
 *   or <https://opensource.org/licenses/BSD-3-Clause>
 *******************************************************************************/

#ifndef LIBMONARCO_SDC_H_
#define LIBMONARCO_SDC_H_

/* Check SPI protocol documentation for SDC registers details:
 *   https://github.com/monarco/monarco-hat-documentation/blob/master/Monarco_HAT_SPI_Protocol.md
 */

/* SDC Registers Address List */

#define MONARCO_SDC_REG_STATUS 0x000 /* Status Code */

#define MONARCO_SDC_REG_FWVERL 0x001 /* Firmware Version - Low Word [R] */
#define MONARCO_SDC_REG_FWVERH 0x002 /* Firmware Version - High Word [R] */
#define MONARCO_SDC_REG_HWVERL 0x003 /* Hardware Version - Low Word [R] */
#define MONARCO_SDC_REG_HWVERH 0x004 /* Hardware Version - High Word [R] */
#define MONARCO_SDC_REG_MCUID1 0x005 /* MCU Unique ID word 1 [R] */
#define MONARCO_SDC_REG_MCUID2 0x006 /* MCU Unique ID word 2 [R] */
#define MONARCO_SDC_REG_MCUID3 0x007 /* MCU Unique ID word 3 [R] */
#define MONARCO_SDC_REG_MCUID4 0x008 /* MCU Unique ID word 4 [R] */

#define MONARCO_SDC_REG_HWCONFIG1 0x00A /* HW Config 1 [RW], see MONARCO_SDC_CONFIG1_* */
#define MONARCO_SDC_REG_WDTIMEOUT 0x00F /* Process Data Watchdog Timeout [RW], unit 1ms, 0 = disabled */

#define MONARCO_SDC_REG_RS485BAUD 0x010 /* RS-485 Baudrate [RW], unit 100Bd, min 3, max 1000 */
#define MONARCO_SDC_REG_RS485MODE 0x011 /* RS-485 Mode [RW], see MONARCO_SDC_RS485_MODE_* */
#define MONARCO_SDC_REG_HOSTBAUD 0x012 /* Host UART Baudrate [RW], unit 100Bd, min 3, max 1000, 0 = disabled - MCU pins in Hi-Z */
#define MONARCO_SDC_REG_RS485RXCNT 0x014 /* RS-485 Bytes Received Diagnostics Counter [R] */
#define MONARCO_SDC_REG_RS485TXCNT 0x015 /* RS-485 Bytes Transmitted Diagnostics Counter [R] */
#define MONARCO_SDC_REG_RS485FECNT 0x018 /* RS-485 RX Framing Error Diagnostics Counter [R] */
#define MONARCO_SDC_REG_RS485PECNT 0x019 /* RS-485 RX Parity Error Diagnostics Counter [R] */

#define MONARCO_SDC_REG_CNT1CFG 0x024 /* COUNTER1 Configuration [W], see MONARCO_SDC_COUNTER_MODE_* */
#define MONARCO_SDC_REG_CNT2CFG 0x025 /* COUNTER2 Configuration [W], see MONARCO_SDC_COUNTER_MODE_* */

/* Status Codes */

#define MONARCO_SDC_STATUS_OK 0xABCD
#define MONARCO_SDC_STATUS_ERROR_CRC 0xCCCC

/* Error Codes */

#define MONARCO_SDC_ERROR_UNKNOWN_REG 0xFFFF

/* COUNTER Configuration Flags */

#define MONARCO_SDC_COUNTER_MODE__SHIFT 0
#define MONARCO_SDC_COUNTER_MODE__MASK (0x7 << MONARCO_SDC_COUNTER_MODE__SHIFT)
#define MONARCO_SDC_COUNTER_MODE__MAX 2
#define MONARCO_SDC_COUNTER_MODE_OFF (0 << MONARCO_SDC_COUNTER_MODE__SHIFT)
#define MONARCO_SDC_COUNTER_MODE_PCNT (1 << MONARCO_SDC_COUNTER_MODE__SHIFT)
#define MONARCO_SDC_COUNTER_MODE_QUAD (2 << MONARCO_SDC_COUNTER_MODE__SHIFT)

#define MONARCO_SDC_COUNTER_CTRL__SHIFT 3
#define MONARCO_SDC_COUNTER_CTRL__MASK (0x7 << MONARCO_SDC_COUNTER_CTRL__SHIFT)
#define MONARCO_SDC_COUNTER_CTRL__MAX 1
#define MONARCO_SDC_COUNTER_CTRL_UP (0 << MONARCO_SDC_COUNTER_CTRL__SHIFT)
#define MONARCO_SDC_COUNTER_CTRL_EXT (1 << MONARCO_SDC_COUNTER_CTRL__SHIFT) /* direction selectable by B input, high = down-counting */

#define MONARCO_SDC_COUNTER_EDGE__SHIFT 6
#define MONARCO_SDC_COUNTER_EDGE__MASK (0x3 << MONARCO_SDC_COUNTER_EDGE__SHIFT)
#define MONARCO_SDC_COUNTER_EDGE__MAX 2
#define MONARCO_SDC_COUNTER_EDGE_RISE (0 << MONARCO_SDC_COUNTER_EDGE__SHIFT)
#define MONARCO_SDC_COUNTER_EDGE_FALL (1 << MONARCO_SDC_COUNTER_EDGE__SHIFT)
#define MONARCO_SDC_COUNTER_EDGE_BOTH (2 << MONARCO_SDC_COUNTER_EDGE__SHIFT)

#define MONARCO_SDC_COUNTER_CAPTURE__SHIFT 8
#define MONARCO_SDC_COUNTER_CAPTURE__MASK (0x3 << MONARCO_SDC_COUNTER_CAPTURE__SHIFT)
#define MONARCO_SDC_COUNTER_CAPTURE__MAX 3
#define MONARCO_SDC_COUNTER_CAPTURE_OFF (0 << MONARCO_SDC_COUNTER_CAPTURE__SHIFT)
#define MONARCO_SDC_COUNTER_CAPTURE_RISE (1 << MONARCO_SDC_COUNTER_CAPTURE__SHIFT)
#define MONARCO_SDC_COUNTER_CAPTURE_FALL (2 << MONARCO_SDC_COUNTER_CAPTURE__SHIFT)
#define MONARCO_SDC_COUNTER_CAPTURE_BOTH (3 << MONARCO_SDC_COUNTER_CAPTURE__SHIFT)

/* CONFIG1 Register Flags*/

#define MONARCO_SDC_CONFIG1_RS485TERM (1 << 0) /* Enable RS-485 termination */
#define MONARCO_SDC_CONFIG1_AI1V 0 /* Analog input AIN1 - Voltage mode */
#define MONARCO_SDC_CONFIG1_AI1CL (1 << 1) /* Analog input AIN1 - Current loop mode */
#define MONARCO_SDC_CONFIG1_AI2V 0 /* Analog input AIN2 - Voltage mode */
#define MONARCO_SDC_CONFIG1_AI2CL (1 << 2) /* Analog input AIN2 - Current loop mode */

/* RS485 Mode Register */

#define MONARCO_SDC_RS485_MODE_PARITY__SHIFT 0
#define MONARCO_SDC_RS485_MODE_PARITY_NONE (0 << MONARCO_SDC_RS485_MODE_PARITY__SHIFT)
#define MONARCO_SDC_RS485_MODE_PARITY_EVEN (1 << MONARCO_SDC_RS485_MODE_PARITY__SHIFT)
#define MONARCO_SDC_RS485_MODE_PARITY_ODD (2 << MONARCO_SDC_RS485_MODE_PARITY__SHIFT)

#define MONARCO_SDC_RS485_MODE_DATABITS__SHIFT 3
#define MONARCO_SDC_RS485_MODE_DATABITS_5 (0 << MONARCO_SDC_RS485_MODE_DATABITS__SHIFT)
#define MONARCO_SDC_RS485_MODE_DATABITS_6 (1 << MONARCO_SDC_RS485_MODE_DATABITS__SHIFT)
#define MONARCO_SDC_RS485_MODE_DATABITS_7 (2 << MONARCO_SDC_RS485_MODE_DATABITS__SHIFT)
#define MONARCO_SDC_RS485_MODE_DATABITS_8 (3 << MONARCO_SDC_RS485_MODE_DATABITS__SHIFT)

#define MONARCO_SDC_RS485_MODE_STOPBITS__SHIFT 5
#define MONARCO_SDC_RS485_MODE_STOPBITS_0_5 (0 << MONARCO_SDC_RS485_MODE_STOPBITS__SHIFT)
#define MONARCO_SDC_RS485_MODE_STOPBITS_1_0 (1 << MONARCO_SDC_RS485_MODE_STOPBITS__SHIFT)
#define MONARCO_SDC_RS485_MODE_STOPBITS_1_5 (2 << MONARCO_SDC_RS485_MODE_STOPBITS__SHIFT)
#define MONARCO_SDC_RS485_MODE_STOPBITS_2_0 (3 << MONARCO_SDC_RS485_MODE_STOPBITS__SHIFT)

#define MONARCO_SDC_RS485_DEFAULT_BAUDRATE 96
#define MONARCO_SDC_RS485_DEFAULT_MODE (MONARCO_SDC_RS485_MODE_PARITY_NONE | MONARCO_SDC_RS485_MODE_DATABITS_8 | MONARCO_SDC_RS485_MODE_STOPBITS_1_0)

#endif
