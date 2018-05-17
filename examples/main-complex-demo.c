/***************************************************************************//**
 * @file main-complex-demo.c
 * @brief libmonarco - Complex Application Example
 *
 * This is a complex demo application for Monarco HAT and C library libmonarco.
 *
 * Initial values of important service registers (SDC, Service Data Channel)
 * are defined to be written - baudrate and mode of RS-485, mode of counters.
 * Also identification registers like hardware and firmware version and CPU ID
 * are read out during startup phase. You can use service registers handling
 * approach provided by this example as a basis for your projects.
 *
 * To fully explore this example, connect together AIN1<>AOUT1, AIN2<>AOUT2,
 * DIN3<>DOUT3, DIN4<>DOUT4.
 *
 * Digital output DOUT1 is toggled each 1 second, DOUT3 and DOUT4 simulate
 * quadrature encoder signal. COUNTER1 is configured for pulse counting mode
 * with both active edges, COUNTER2 is configured for quadrature encoder.
 *
 * Analog output AOUT1 is driven by constant 2.0 Volts, AOUT2 by sinusoidal
 * signal between 1.0 Volts and 9.0 Volts.
 *
 * All input signals are dumped out with 500 ms period. 
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

/* Service Data Channel (SDC) registers data Items */
monarco_sdc_item_t *sdc_item_status;
monarco_sdc_item_t *sdc_item_fw_ver_lo, *sdc_item_fw_ver_hi;
monarco_sdc_item_t *sdc_item_hw_ver_lo, *sdc_item_hw_ver_hi;
monarco_sdc_item_t *sdc_item_mcu_id_1, *sdc_item_mcu_id_2, *sdc_item_mcu_id_3, *sdc_item_mcu_id_4;
monarco_sdc_item_t *sdc_item_config1;
monarco_sdc_item_t *sdc_item_rs485_baud, *sdc_item_rs485_mode;
monarco_sdc_item_t *sdc_item_cnt1_mode, *sdc_item_cnt2_mode;

/*
 * Application Initialization
 *   We define here our set of SDC (Service Data Channel) Registers and initialize corresponding Item structures
 */
void application_init()
{
    int i = 0;

    /* Status Code */
    sdc_item_status = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_STATUS,
        .factor = 1
    };
    /* Firmware version */
    sdc_item_fw_ver_lo = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_FWVERL,
        .request = 1
    };
    sdc_item_fw_ver_hi = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_FWVERH,
        .request = 1
    };
    /* Hardware version */
    sdc_item_hw_ver_lo = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_HWVERL,
        .request = 1
    };
    sdc_item_hw_ver_hi = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_HWVERH,
        .request = 1
    };
    /* MCU ID */
    sdc_item_mcu_id_1 = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_MCUID1,
        .request = 1
    };
    sdc_item_mcu_id_2 = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_MCUID2,
        .request = 1
    };
    sdc_item_mcu_id_3 = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_MCUID3,
        .request = 1
    };
    sdc_item_mcu_id_4 = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_MCUID4,
        .request = 1
    };
    /* Hardware Configurarion Register 1 - enable RS-485 termination, both analog inputs in voltage mode */
    sdc_item_config1 = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_HWCONFIG1,
        .value = MONARCO_SDC_CONFIG1_RS485TERM | MONARCO_SDC_CONFIG1_AI1V | MONARCO_SDC_CONFIG1_AI2V,
        .write = 1,
        .request = 1
    };
    /* RS-485 Configuration - 38400 Baud, 8 data bits, 1 stop bit */
    sdc_item_rs485_baud = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_RS485BAUD,
        .value = 384,
        .write = 1,
        .request = 1
    };
    sdc_item_rs485_mode = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_RS485MODE,
        .value = MONARCO_SDC_RS485_MODE_PARITY_NONE | MONARCO_SDC_RS485_MODE_DATABITS_8 | MONARCO_SDC_RS485_MODE_STOPBITS_1_0,
        .write = 1,
        .request = 1
    };
    /* Counter 1, 2 Configuration */
    sdc_item_cnt1_mode = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_CNT1CFG,
        .value = MONARCO_SDC_COUNTER_MODE_PCNT | MONARCO_SDC_COUNTER_EDGE_BOTH,
        .write = 1,
        .request = 1
    };
    sdc_item_cnt2_mode = &cxt.sdc_items[i]; cxt.sdc_items[i++] = (monarco_sdc_item_t){
        .address = MONARCO_SDC_REG_CNT2CFG,
        .value = MONARCO_SDC_COUNTER_MODE_QUAD,
        .write = 1,
        .request = 1
    };

    cxt.sdc_size = i;
}

/*
 * Application Cyclic Loop
 *   `tick` is incremented by 1 with each invocation.
 */
void application_loop(int tick)
{
    static int sdc_done = 0;
    int i;

    /* ---
     * 1) Calculate new Process Data Channel (PDC) outputs to the Monarco HAT - to cxt.tx_data
     * --- */

    // enable user control on LED1 - LED8, set mask bits to 1 for all LEDs (0xFF)
    cxt.tx_data.led_mask = 0xFF;

    // Each 16 ticks (0.32 s) - loop over LED1 - LED8
    if ((tick % 16) == 0) {
        if (cxt.tx_data.led_value == 0) {
            cxt.tx_data.led_value = 1; // LED 1
        } else {
            cxt.tx_data.led_value = cxt.tx_data.led_value << 1; // Shift left by 1
        }
    }

    // AOUT1 = 2.0 V
    cxt.tx_data.aout1 = monarco_util_aout_volts_to_u16(2.0);

    // AOUT2 = 5.0 V + superimposed sinus with amplitude 4.0 V, period 2000 ticks * 20 ms = 40 s
    cxt.tx_data.aout2 = monarco_util_aout_volts_to_u16(5.0 + 4.0 * sin((tick % 2000) / 2000.0 * 2 * 3.141592));

    //  Each 50 ticks (1 s) - toggle DOUT1
    if ((tick % 50) == 0) {
        if (GET_DOUT(0)) {
            SET_DOUT(0, 0);
        }
        else {
            SET_DOUT(0, 1);
        }
    }

    //  Each 25 ticks (0.5 s) - DOUT3, DOUT4 -- quadrature encoder emulation
    if ((tick % 25) == 0) {
        if ((GET_DOUT(2) == 0) && (GET_DOUT(3) == 0)) { SET_DOUT(2, 0); SET_DOUT(3, 1); }
        else if ((GET_DOUT(2) == 0) && (GET_DOUT(3) == 1)) { SET_DOUT(2, 1); SET_DOUT(3, 1); }
        else if ((GET_DOUT(2) == 1) && (GET_DOUT(3) == 1)) { SET_DOUT(2, 1); SET_DOUT(3, 0); }
        else if ((GET_DOUT(2) == 1) && (GET_DOUT(3) == 0)) { SET_DOUT(2, 0); SET_DOUT(3, 0); }
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
        printf("DI1..4: %u%u%u%u | CNT1: %05u | CNT2: %05u | AIN1: %02.03f | AIN2: %02.03f\n",
            (cxt.rx_data.din & (1 << 0)) > 0,
            (cxt.rx_data.din & (1 << 1)) > 0,
            (cxt.rx_data.din & (1 << 2)) > 0,
            (cxt.rx_data.din & (1 << 3)) > 0,
            cxt.rx_data.cnt1, cxt.rx_data.cnt2,
            monarco_util_ain_10v_to_real(cxt.rx_data.ain1),
            monarco_util_ain_10v_to_real(cxt.rx_data.ain2)
        );
    }

    /* ---
     * 4) Check if all SDC Items read/write successfully completed
     * --- */

    if (sdc_done == 0) {
        int done = 1;
        for (i = 0; i < cxt.sdc_size; i++) {
            if (cxt.sdc_items[i].done == 0) {
                done = 0;
                break;
            }
        }
        if (done) {
            sdc_done = 1;
            printf("MONARCO SDC INIT DONE, FW=%04X%04X, HW=%04X%04X, CPUID=%04X%04X%04X%04X\n",
                    sdc_item_fw_ver_hi->value, sdc_item_fw_ver_lo->value,
                    sdc_item_hw_ver_hi->value, sdc_item_hw_ver_lo->value,
                    sdc_item_mcu_id_4->value, sdc_item_mcu_id_3->value,
                    sdc_item_mcu_id_2->value, sdc_item_mcu_id_1->value);
        }
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

    printf("\n### Monarco HAT C library (libmonarco) 'complex' example v1.3\n\n");

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
