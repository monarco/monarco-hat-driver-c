/***************************************************************************//**
 * @file monarco.h
 * @brief LibMonarco - Main API
 *******************************************************************************
 * @section License
 * Copyright REX Controls s.r.o. http://www.rexcontrols.com
 * Author: Vlastimil Setka
 * This file is covered by the BSD 3-Clause License
 *   see LICENSE.txt in the root directory of this project
 *   or <https://opensource.org/licenses/BSD-3-Clause>
 *******************************************************************************/

#ifndef LIBMONARCO_H_
#define LIBMONARCO_H_

#include <stdint.h>
#include "monarco_struct.h"

#ifndef MONARCO_SDC_ITEMS_SIZE
#define MONARCO_SDC_ITEMS_SIZE 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t address;
    uint16_t value;
    int factor;
    int counter;
    int busy;
    unsigned int write : 1;
    unsigned int request : 1;
    unsigned int done : 1;
    unsigned int error : 1;
} monarco_sdc_item_t;

typedef struct {
    void *platform;
    monarco_struct_tx_t tx_data;
    monarco_struct_rx_t rx_data;
    int spi_fd;
    int sdc_size;
    int sdc_idx;
    monarco_sdc_item_t sdc_items[MONARCO_SDC_ITEMS_SIZE];
    int err_throttle_crc;
} monarco_cxt_t ;

int monarco_init(monarco_cxt_t *cxt, const char *spi_device, uint32_t spi_clkfreq, void *platform);

int monarco_main(monarco_cxt_t *cxt);

int monarco_exit(monarco_cxt_t *cxt);

#ifdef __cplusplus
}
#endif

#endif
