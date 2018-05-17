/***************************************************************************//**
 * @file monarco.h
 * @brief libmonarco - Main API
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

/* Service Data Channel Item Structure
 *   You can define a set of items which can be read/write periodically or one-shot.
 */
typedef struct {
    uint16_t address; /* Register Address, see MONARCO_SDC_REG_* */
    uint16_t value; /* Register Value, or Error Code if `.error = 1` */
    int factor; /* If `factor>0`, this Item is communicated periodically each `factor-th` scan over SDC Items Array */
    int counter; /* Private, counter for factor cycles */
    int busy; /* Private, counter of busy cycles after request before response */
    unsigned int write : 1; /* 0 = Read Item, 1 = Write Item */
    unsigned int request : 1; /* 1 = Trigger one-shot action, cleared automatically when request for this Item is sent */
    unsigned int done : 1; /* 1 = Indication of completion, should be cleared by user with (or before) next trigger */
    unsigned int error : 1; /* 0 = Success result, 1 = Error result (`value` contains Error Code) */
} monarco_sdc_item_t;

/* Monarco Context Structure
 *   Normally only members `tx_data`, `rx_data`, `sdc_items` and `sdc_size` should be accessed outside monarco.c.
 */
typedef struct {
    void *platform;
    monarco_struct_tx_t tx_data; /* Output Process Data (from Host to Monarco HAT) */
    monarco_struct_rx_t rx_data; /* Input Process Data (to Host from Monarco HAT) */
    int spi_fd; /* Private */
    int sdc_size; /* Number of valid SDC Items in `sdc_items` array */
    int sdc_idx; /* Private, index of current SDC Item */
    monarco_sdc_item_t sdc_items[MONARCO_SDC_ITEMS_SIZE]; /* SDC Items Array, see description of monarco_sdc_item_t */
    int err_throttle_crc; /* Private */
} monarco_cxt_t ;

/* Monarco Initialization
 *   Connect to `*spi_device` with `spi_clkfreq` clock frequency (Hz)
 *   and provide some `*platform` data - for generic linux platform it is a debug print prefix.
 */
int monarco_init(monarco_cxt_t *cxt, const char *spi_device, uint32_t spi_clkfreq, void *platform);

/* Monarco Main
 *   Performs one SPI transaction with Monarco HAT - exchange of complete input and output process data
 *   and single new service data reqeust and response to previous request.
 *   Have to be called periodically, at least faster than process data watchod timeout (default 100 ms)!
 */
int monarco_main(monarco_cxt_t *cxt);

/* Monarco Cleanup
 *   Free all resources allocated by `monarco_init()`.
 */
int monarco_exit(monarco_cxt_t *cxt);

#ifdef __cplusplus
}
#endif

#endif
