/***************************************************************************//**
 * @file monarco.c
 * @brief LibMonarco - Main API
 *******************************************************************************
 * @section License
 * Copyright REX Controls s.r.o. http://www.rexcontrols.com
 * Author: Vlastimil Setka
 * This file is covered by the BSD 3-Clause License
 *   see LICENSE.txt in the root directory of this project
 *   or <https://opensource.org/licenses/BSD-3-Clause>
 *******************************************************************************/

#include "monarco.h"

#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "monarco_crc.h"
#include "monarco_util.h"
#include "monarco_platform.h"


int monarco_init(monarco_cxt_t *cxt, const char *spi_device, uint32_t spi_clkfreq, void *platform)
{
    cxt->platform = platform;

    memset(&cxt->sdc_items, 0, sizeof(cxt->sdc_items));
    memset(&cxt->tx_data, 0, sizeof(monarco_struct_tx_t));
    memset(&cxt->rx_data, 0, sizeof(monarco_struct_rx_t));

    cxt->sdc_size = 0;
    cxt->sdc_idx = 0;
    cxt->err_throttle_crc = 0;

    if ((cxt->spi_fd = open(spi_device, O_RDWR)) < 0)
    {
        MONARCO_DPRINT(MONARCO_DPF_ERROR, "monarco_init: Failed to open the device %s: %i: %s\n", spi_device, errno, strerror(errno));
        return -1;
    }

    // possible modes: mode |= SPI_LOOP; mode |= SPI_CPHA; mode |= SPI_CPOL; mode |= SPI_LSB_FIRST; mode |= SPI_CS_HIGH; mode |= SPI_3WIRE; mode |= SPI_NO_CS; mode |= SPI_READY;
    uint32_t spi_mode = 0;
    if (ioctl(cxt->spi_fd, SPI_IOC_WR_MODE32, &spi_mode) < 0) {
        MONARCO_DPRINT(MONARCO_DPF_ERROR, "monarco_init: Failed to set SPI mode: %i: %s\n", errno, strerror(errno));
        return -2;
    }

    uint8_t spi_bits = 8;
    if (ioctl(cxt->spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bits) < 0) {
        MONARCO_DPRINT(MONARCO_DPF_ERROR, "monarco_init: Failed to set SPI bits per word: %i: %s\n", errno, strerror(errno));
        return -3;
    }

    if (ioctl(cxt->spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_clkfreq) < 0) {
        MONARCO_DPRINT(MONARCO_DPF_ERROR, "monarco_init: Failed to set SPI speed: %i: %s\n", errno, strerror(errno));
        return -4;
    }

    MONARCO_DPRINT(MONARCO_DPF_VERB, "monarco_init: OK\n");

    return 0;
}

static void monarco_sdc_tx(monarco_cxt_t *cxt)
{
    int idx_last = cxt->sdc_idx;

    if (cxt->sdc_idx >= cxt->sdc_size) {
        return;
    }

    monarco_sdc_item_t *item;

    item = &cxt->sdc_items[cxt->sdc_idx];

    // FIXME do not send each request two times (oportunistic tx strategy) - can be problem especially as duplicate writes!
    if (item->busy > 0) {
        if (item->busy < INT_MAX) {
            item->busy++;
        }
        if (item->busy == 10) {
            MONARCO_DPRINT(MONARCO_DPF_ERROR, "monarco_sdc_tx: SDC item %i %c ADDR=0x%03X timeout\n",
                    cxt->sdc_idx, item->write ? 'W' : 'R', item->address);
        }
        return;
    }

    while (1) {
        if (cxt->sdc_items[cxt->sdc_idx].factor > 0) {
            break;
        }

        if(cxt->sdc_items[cxt->sdc_idx].request != 0) {
            break;
        }

        cxt->sdc_idx++;
        if (cxt->sdc_idx >= cxt->sdc_size) {
            cxt->sdc_idx = 0;
        }
        if (cxt->sdc_idx == idx_last) {
            MONARCO_DPRINT(MONARCO_DPF_VERB, "monarco_sdc_tx: No SDC request in this cycle\n");
            return;
        }
    }

    item = &cxt->sdc_items[cxt->sdc_idx];

    cxt->tx_data.sdc_req.value = item->value;
    cxt->tx_data.sdc_req.address = item->address;
    cxt->tx_data.sdc_req.write = item->write;
    cxt->tx_data.sdc_req.error = 0;
    cxt->tx_data.sdc_req.reserved = 0;

    item->busy = 1;
    item->request = 0;

    // printf("SDC_TX[%2i]: 0x%03X = W%X E%X 0x%04X\n", cxt->sdc_idx, item->address, item->write, item->error, item->value);
}

static void monarco_sdc_rx(monarco_cxt_t *cxt)
{
    if (cxt->sdc_idx >= cxt->sdc_size) {
        return;
    }

    monarco_sdc_item_t *item = &cxt->sdc_items[cxt->sdc_idx];

    if ((cxt->rx_data.sdc_resp.address != item->address) || (cxt->rx_data.sdc_resp.write != item->write)) {
        return;
    }

    if ((cxt->rx_data.sdc_resp.write == 1) && (cxt->rx_data.sdc_resp.error == 0) && (cxt->rx_data.sdc_resp.value != item->value)) {
        return;
    }

    if (cxt->rx_data.sdc_resp.error && (!item->error || (item->value != cxt->rx_data.sdc_resp.value))) {
        MONARCO_DPRINT(MONARCO_DPF_ERROR, "monarco_sdc_rx: SDC item %i %c ADDR=0x%03X ERROR=0x%04X\n",
                cxt->sdc_idx, item->write ? 'W' : 'R', item->address, cxt->rx_data.sdc_resp.value);
    }

    item->busy = 0;
    item->done = 1;

    item->value = cxt->rx_data.sdc_resp.value;
    item->error = cxt->rx_data.sdc_resp.error;

    // printf("SDC_RX[%2i]: 0x%03X = W%X E%X 0x%04X\n", cxt->sdc_idx, item->address, item->write, item->error, item->value);

    cxt->sdc_idx++; if (cxt->sdc_idx == cxt->sdc_size) { cxt->sdc_idx = 0; }
}

int monarco_main(monarco_cxt_t *cxt)
{
    monarco_struct_rx_t rx_data;

    if (cxt->spi_fd <= 0) {
        MONARCO_DPRINT(MONARCO_DPF_ERROR, "monarco_main: SPI not open, exiting\n");
        return -1;
    }

    monarco_sdc_tx(cxt);

    cxt->tx_data.crc = monarco_crc16((const char *)&(cxt->tx_data), MONARCO_STRUCT_SIZE - 2);

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)&(cxt->tx_data),
        .rx_buf = (unsigned long)&(rx_data),
        .len = MONARCO_STRUCT_SIZE,
        .delay_usecs = 0,
        .speed_hz = 0,
        .bits_per_word = 8,
    };

    int rc = ioctl(cxt->spi_fd, SPI_IOC_MESSAGE(1), &transfer);

    if (rc < 1) {
        MONARCO_DPRINT(MONARCO_DPF_ERROR, "monarco_main: Failed to send SPI message: %i: %s\n", errno, strerror(errno));
        return -2;
    }

    // monarco_util_dump_tx(&cxt->tx_data);
    // monarco_util_dump_rx(&cxt->rx_data);

    if (rx_data.crc != monarco_crc16((const char *)&(rx_data), MONARCO_STRUCT_SIZE - 2)) {
        if (cxt->err_throttle_crc == 0) {
            MONARCO_DPRINT(MONARCO_DPF_ERROR, "monarco_main: Invalid RX CRC\n");
        }
        if (cxt->err_throttle_crc < INT_MAX) {
            cxt->err_throttle_crc++;
        }
        return -3;
    }
    else if (cxt->err_throttle_crc) {
        MONARCO_DPRINT(MONARCO_DPF_ERROR, "monarco_main: Invalid RX CRC (%i times)\n", cxt->err_throttle_crc);
        cxt->err_throttle_crc = 0;
    }

    // copy data only if CRC OK
    cxt->rx_data = rx_data;

    monarco_sdc_rx(cxt);

    return 0;
}

int monarco_exit(monarco_cxt_t *cxt)
{
    if (cxt->spi_fd > 0) {
        close(cxt->spi_fd);
    }

    MONARCO_DPRINT(MONARCO_DPF_VERB, "monarco_exit: OK\n");

    return 0;
}
