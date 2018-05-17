/***************************************************************************//**
 * @file main-blink-demo.c
 * @brief libmonarco - Simple Application Example
 *
 * This is a complex demo application for Monarco HAT and C library libmonarco.
 *
 * It blinks with LEDs, periodically toggles first digital output, and print
 * all input signals with 0.5 s period.
 *
 * For advanced possibilities please see the complex demo application in file
 * main-complex-demo.c.
 *
 * See also https://www.monarco.io/
 *
 *******************************************************************************
 * @section License
 * Copyright REX Controls s.r.o. https://www.rexcontrols.com
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

/* Enable all debug print flags for monarco_platform.h */
int monarco_platform_dprint_flags = MONARCO_DPF_ERROR | MONARCO_DPF_WARNING | MONARCO_DPF_INFO | MONARCO_DPF_VERB;

/* Context data for libmonarco. */
static monarco_cxt_t cxt;

/* Helper macros for digital inputs, outputs and LEDs */
#define GET_DIN(n) ((cxt.rx_data.din & (1 << n)) ? 1 : 0)
#define GET_DOUT(n) ((cxt.tx_data.dout & (1 << n)) ? 1 : 0)
#define SET_DOUT(n, value) cxt.tx_data.dout = (cxt.tx_data.dout & ~(1 << n)) | ((value) ? (1 << n) : 0)
#define GET_LED(n) ((cxt.tx_data.led_value & (1 << n)) ? 1 : 0)
#define SET_LED(n, value) cxt.tx_data.led_value = (cxt.tx_data.led_value & ~(1 << n)) | ((value) ? (1 << n) : 0)

/*
 * Application Initialization
 */
void application_init()
{
    // Nothing needs to be done here
}

/*
 * Application Cyclic Loop
 *   `tick` is incremented by 1 with each invocation.
 */
void application_loop(int tick)
{
    /* ---
     * 1) Calculate new Process Data Channel (PDC) outputs to the Monarco HAT - to cxt.tx_data
     * --- */

    // enable user control of LED1 and LED3 - set mask bits for these LEDs
    cxt.tx_data.control_byte.status_led_mask = 1;
    cxt.tx_data.led_mask = (1 << 0) | (1 << 2);

    // Each 25 ticks (0.5 s) - flash LED0
    if ((tick % 25) < 5) {
        cxt.tx_data.control_byte.status_led_value = 1;
    }
    else {
        cxt.tx_data.control_byte.status_led_value = 0;
    }

    // Each 10 ticks (0.2 s) - toggle LED1 / LED2
    if ((tick % 10) == 0) {
        if ((tick % 20) == 0) {
            SET_LED(0, 1);
            SET_LED(2, 0);
        }
        else {
            SET_LED(0, 0);
            SET_LED(2, 1);
        }
    }

    //  Each 50 ticks (1 s) - toggle DOUT1
    if ((tick % 50) == 0) {
        if (GET_DOUT(0)) {
            SET_DOUT(0, 0);
        }
        else {
            SET_DOUT(0, 1);
        }
    }

    /* ---
     * 2) Call main function of libmonarco - do SPI transaction, dispatch SDC items
     * --- */

    monarco_main(&cxt);

    /* ---
     * 3) Handle new Process Data Channel (PDC) inputs from the Monarco HAT - from cxt.rx_data
     * --- */

    // Each 25 tick (0.5 s), print status of inputs
    if ((tick % 25) == 0) {
        printf("DI1: %u DI2: %u DI3: %u DI4: %u | AIN1: %02.03f | AIN2: %02.03f\n",
            GET_DIN(0),
            GET_DIN(1),
            GET_DIN(2),
            GET_DIN(3),
            monarco_util_ain_10v_to_real(cxt.rx_data.ain1),
            monarco_util_ain_10v_to_real(cxt.rx_data.ain2)
        );
    }
}

/*
 * Application Main
 */
int main(int argc, char *argv[])
{
    struct sched_param rt_param;
    int interval_ns = 20 * 1000 *1000; // 20 ms period
    int rt_prio = 60; // realtime scheduling priority for Linux kernel
    struct timespec ts;
    int tick = 0;

    printf("\n### Monarco HAT C library (libmonarco) 'blink' example v1.3\n\n");

    // Enable realtime fifo scheduling
    rt_param.sched_priority = rt_prio;
    if (sched_setscheduler(0, SCHED_FIFO, &rt_param) == -1) {
       perror("sched_setscheduler failed");
       return -1;
    }

    // Lock memory allocations for realtime performance
    if (mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        perror("mlockall failed");
        return -2;
    }

    printf("Cycle interval: %f ms\n\n", interval_ns / 1.0e6);

    monarco_init(&cxt, "/dev/spidev0.0", 4000000, "monarco-demo: ");

    /* Application init */
    
    application_init();

    /* Application cyclic loop */
    
    // get current time
    clock_gettime(0, &ts);

    while (1) {
        tick++;

        application_loop(tick);

        // calculate next time shot = current time + interval
        ts.tv_nsec += interval_ns;

        // normalize ts structure, tv_nsec should be < 1e9
        while (ts.tv_nsec >= 1e9) {
            ts.tv_nsec -= 1e9;
            ts.tv_sec++;
         }

        // wait untill next time shot
        clock_nanosleep(0, TIMER_ABSTIME, &ts, NULL);
    }

    /* Cleanup on exit (but demo example never goes here) */

    monarco_exit(&cxt);

    return 0;
}
