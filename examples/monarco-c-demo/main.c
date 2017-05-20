/***************************************************************************//**
 * @file main.c
 * @brief LibMonarco - Example "monarco-c-demo" main file
 *******************************************************************************
 * @section License
 * Copyright REX Controls s.r.o. http://www.rexcontrols.com
 * Author: Vlastimil Setka
 * This file is covered by the BSD 3-Clause License
 *   see LICENSE.txt in the root directory of this project
 *   or <https://opensource.org/licenses/BSD-3-Clause>
 *******************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <math.h>

#include "src/monarco.h"
#include "src/monarco_util.h"
#include "src/monarco_sdc.h"
#include "monarco_platform.h"

#define SDC_FIXED__SIZE 14
#define SDC_FIXED_STATUSWORD 0
#define SDC_FIXED_FWVERL 1
#define SDC_FIXED_FWVERH 2
#define SDC_FIXED_HWVERL 3
#define SDC_FIXED_HWVERH 4
#define SDC_FIXED_CPUID1 5
#define SDC_FIXED_CPUID2 6
#define SDC_FIXED_CPUID3 7
#define SDC_FIXED_CPUID4 8
#define SDC_FIXED_CONFIG1 9
#define SDC_FIXED_RS485BAUD 10
#define SDC_FIXED_RS485MODE 11
#define SDC_FIXED_CNT1MODE 12
#define SDC_FIXED_CNT2MODE 13


int monarco_platform_dprint_flags = MONARCO_DPF_ERROR | MONARCO_DPF_WARNING | MONARCO_DPF_INFO | MONARCO_DPF_VERB;

static monarco_cxt_t cxt;
static int sdc_init_status = 0;

void timespec_normalize(struct timespec *ts)
{
   while (ts->tv_nsec >= 1000000000) {
      ts->tv_nsec -= 1000000000;
      ts->tv_sec++;
   }
}

void sdc_init()
{
    cxt.sdc_items[SDC_FIXED_STATUSWORD] = (monarco_sdc_item_t){ .address = 0x000, .factor = 1 };

    cxt.sdc_items[SDC_FIXED_FWVERL] = (monarco_sdc_item_t){ .address = 0x001, .request = 1 };
    cxt.sdc_items[SDC_FIXED_FWVERH] = (monarco_sdc_item_t){ .address = 0x002, .request = 1 };
    cxt.sdc_items[SDC_FIXED_HWVERL] = (monarco_sdc_item_t){ .address = 0x003, .request = 1 };
    cxt.sdc_items[SDC_FIXED_HWVERH] = (monarco_sdc_item_t){ .address = 0x004, .request = 1 };

    cxt.sdc_items[SDC_FIXED_CPUID1] = (monarco_sdc_item_t){ .address = 0x005, .request = 1 };
    cxt.sdc_items[SDC_FIXED_CPUID2] = (monarco_sdc_item_t){ .address = 0x006, .request = 1 };
    cxt.sdc_items[SDC_FIXED_CPUID3] = (monarco_sdc_item_t){ .address = 0x007, .request = 1 };
    cxt.sdc_items[SDC_FIXED_CPUID4] = (monarco_sdc_item_t){ .address = 0x008, .request = 1 };

    cxt.sdc_items[SDC_FIXED_CONFIG1] = (monarco_sdc_item_t){ .address = 0x00A, .value = MONARCO_SDC_CONFIG1_RS485TERM, .write = 1, .request = 1 };

    cxt.sdc_items[SDC_FIXED_RS485BAUD] = (monarco_sdc_item_t){ .address = 0x010, .value = 384, .write = 1, .request = 1 };
    cxt.sdc_items[SDC_FIXED_RS485MODE] = (monarco_sdc_item_t){ .address = 0x011, .value = MONARCO_SDC_RS485_DEFAULT_MODE, .write = 1, .request = 1 };

    cxt.sdc_items[SDC_FIXED_CNT1MODE] = (monarco_sdc_item_t){ .address = 0x024, .value = MONARCO_SDC_COUNTER_MODE_OFF, .write = 1, .request = 1 };
    cxt.sdc_items[SDC_FIXED_CNT2MODE] = (monarco_sdc_item_t){ .address = 0x025, .value = MONARCO_SDC_COUNTER_MODE_QUAD, .write = 1, .request = 1 };

    cxt.sdc_size = SDC_FIXED__SIZE;
}

void sdc_check_init()
{
    int i;

    if (sdc_init_status == 0) {
        int done = 1;
        for (i = 0; i < SDC_FIXED__SIZE; i++) {
            if (cxt.sdc_items[i].done == 0) {
                done = 0;
                break;
            }
        }
        if (done) {
            sdc_init_status = 1;
            printf("MONARCO SDC INIT DONE, FW=%08X, HW=%08X, CPUID=%08X%08X\n",
                    (cxt.sdc_items[SDC_FIXED_FWVERH].value<<16) + cxt.sdc_items[SDC_FIXED_FWVERL].value,
                    (cxt.sdc_items[SDC_FIXED_HWVERH].value<<16) + cxt.sdc_items[SDC_FIXED_HWVERL].value,
                    (cxt.sdc_items[SDC_FIXED_CPUID4].value<<16) + cxt.sdc_items[SDC_FIXED_CPUID3].value,
                    (cxt.sdc_items[SDC_FIXED_CPUID2].value<<16) + cxt.sdc_items[SDC_FIXED_CPUID1].value);
        }
    }
}

int main(int argc, char *argv[])
{
    struct sched_param param;
    int interval_ns = 20 * 1000 *1000; // 20 ms period
    int rt_prio = 60;
    struct timespec ts;
    int tick = 0;

    printf("\n### Monarco HAT C library (libmonarco) 'demo' example v1.0\n\n");

    // enable realtime fifo scheduling
    param.sched_priority = rt_prio;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
       perror("sched_setscheduler failed");
       return -1;
    }

    // lock memory allocations for realtime performance
    if (mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        perror("mlockall failed");
        return -2;
    }

    printf("Cycle interval: %f ms\n\n", interval_ns / 1.0e6);

    monarco_init(&cxt, "/dev/spidev0.0", 4000000, "monarco-c-demo: ");

    sdc_init();

    /* --- Initial output values --- */

    // enable user control on LED1-LED8
    cxt.tx_data.led_en = 0xFF;

    /* --- Cyclic loop --- */

    clock_gettime(0, &ts);
    while (1) {
        tick++;

        /* --- Calculate new Process Data Channel (PDC) outputs to the Monarco HAT --- */

        // LED1-LED8 - loop
        if ((tick % 16) == 0) {
            if (cxt.tx_data.led_on == 0) {
                cxt.tx_data.led_on = 0x01;
            } else {
                cxt.tx_data.led_on = cxt.tx_data.led_on << 1;
            }
        }

        // AOUT1 = 2.0 V
        cxt.tx_data.aout1 = monarco_util_aout_volts_to_u16(2.0);

        // AOUT2 = 5.0 V + superimposed sinus with amplitude 4.0 V
        cxt.tx_data.aout2 = monarco_util_aout_volts_to_u16(5.0 + 4.0 * sin((tick % 2048) / 2048.0 * 2 * 3.141592));

        // DOUT3, DOUT4 -- quadrature encoder emulation
        if ((tick % 32) == 0) {
            switch (cxt.tx_data.dout & ((1 << 3) + (1 << 2))) {
                case 0x0: cxt.tx_data.dout = (cxt.tx_data.dout & ((1 << 1) + (1 << 0))) | 0x8; break;
                case 0x4: cxt.tx_data.dout = (cxt.tx_data.dout & ((1 << 1) + (1 << 0))) | 0x0; break;
                case 0xC: cxt.tx_data.dout = (cxt.tx_data.dout & ((1 << 1) + (1 << 0))) | 0x4; break;
                case 0x8: cxt.tx_data.dout = (cxt.tx_data.dout & ((1 << 1) + (1 << 0))) | 0xC; break;
            }
        }

        /* --- SPI Transaction --- */

        monarco_main(&cxt);

        /* --- Handle new Process Data Channel (PDC) inputs from the Monarco HAT --- */

        if ((tick % 32) == 0) {
            printf("DI1..4: %u%u%u%u | CNT1: %05u | CNT2: %05u | AIN1: %02.03f | AIN2: %02.03f\n",
                (cxt.rx_data.din & (1 << 0)) > 0, (cxt.rx_data.din & (1 << 1)) > 0, (cxt.rx_data.din & (1 << 2)) > 0, (cxt.rx_data.din & (1 << 3)) > 0,
                cxt.rx_data.cnt1, cxt.rx_data.cnt2,
                monarco_util_ain_10v_to_real(cxt.rx_data.ain1),
                monarco_util_ain_10v_to_real(cxt.rx_data.ain2)
            );
        }

        sdc_check_init();


        /* --- Loop again after the interval_ns --- */

        /* calculate next time shot */
        ts.tv_nsec += interval_ns;
        timespec_normalize(&ts);

        /* wait untill next shot */
        clock_nanosleep(0, TIMER_ABSTIME, &ts, NULL);
    }

    monarco_exit(&cxt);

    return 0;
}
