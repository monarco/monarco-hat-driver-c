# libmonarco - C language driver library for the Monarco HAT

VERSION: 1.3, FIRMWARE: 2.006

Libmonarco provides C language functions to control the Monarco HAT board over SPI interface.

## Other Resources

* [Monarco Homepage - https://www.monarco.io/](https://www.monarco.io/)
* [Repository - Documentation for the Monarco HAT](https://github.com/monarco/monarco-hat-documentation)
* [Monarco HAT SPI Protocol Reference Manual](https://github.com/monarco/monarco-hat-documentation/blob/master/Monarco_HAT_SPI_Protocol.md)

## Getting started with application example

The most comprehensive way how to get into using the C language driver library for the Monarco HAT is to examine **application examples in directory `examples/`**.

### Prepare environment

Install git and build-dependencies on your Raspberry Pi runinng Raspbian:

<pre>
sudo apt update
sudo apt install git build-essential 
</pre>

Clone the repository:

<pre>
cd ~
git clone https://github.com/monarco/monarco-hat-driver-c.git
</pre>

### Build and run the 'blink' example project

This is a simple demo application for Monarco HAT and C library libmonarco which blinks with LEDs, periodically toggles first digital output, and print all input signals with 0.5 s period.

Compile:

<pre>
cd ~/monarco-hat-driver-c/examples
make
</pre>

There should be no build errors.

Run the example:

<pre>
sudo ./monarco-blink-demo

### Monarco HAT C library (libmonarco) 'blink' example v1.3

Cycle interval: 20.000000 ms

monarco-demo: [VERB] monarco_init: OK
DI1: 1 DI2: 1 DI3: 1 DI4: 1 | AIN1: 0.000 | AIN2: 0.000
DI1: 1 DI2: 1 DI3: 1 DI4: 1 | AIN1: 0.000 | AIN2: 0.002
DI1: 0 DI2: 1 DI3: 1 DI4: 1 | AIN1: 0.002 | AIN2: 0.000
DI1: 0 DI2: 1 DI3: 1 DI4: 1 | AIN1: 0.002 | AIN2: 0.000
DI1: 1 DI2: 1 DI3: 1 DI4: 1 | AIN1: 0.000 | AIN2: 0.002
DI1: 1 DI2: 1 DI3: 1 DI4: 1 | AIN1: 0.005 | AIN2: 0.002
DI1: 0 DI2: 1 DI3: 1 DI4: 1 | AIN1: 0.000 | AIN2: 0.002
DI1: 0 DI2: 1 DI3: 1 DI4: 1 | AIN1: 0.002 | AIN2: 0.002
</pre>

Note: `DIN1` was connected to `DOUT1` while running the example above, so we can see periodic changes on `DIN1`.

### Build and run the 'complex' example project

This is a complex demo application for Monarco HAT and C library libmonarco which illustrates all aspects of libmonarco API.

Compile:

<pre>
cd ~/monarco-hat-driver-c/examples
make
</pre>

There should be no build errors.

Run the example:

<pre>
sudo ./monarco-complex-demo

### Monarco HAT C library (libmonarco) 'complex' example v1.3

Cycle interval: 20.000000 ms

monarco-demo: [VERB] monarco_init: OK
DI1..4: 1111 | CNT1: 00000 | CNT2: 00035 | AIN1: 1.985 | AIN2: 5.255
MONARCO SDC INIT DONE, FW=00002006, HW=00000105, CPUID=247DBC06581720B1
DI1..4: 1110 | CNT1: 00000 | CNT2: 00000 | AIN1: 1.985 | AIN2: 5.570
DI1..4: 0100 | CNT1: 00001 | CNT2: 00001 | AIN1: 1.980 | AIN2: 5.871
DI1..4: 0101 | CNT1: 00001 | CNT2: 00002 | AIN1: 1.988 | AIN2: 6.171
DI1..4: 1111 | CNT1: 00002 | CNT2: 00003 | AIN1: 1.985 | AIN2: 6.474
DI1..4: 1110 | CNT1: 00002 | CNT2: 00004 | AIN1: 1.985 | AIN2: 6.762
DI1..4: 0100 | CNT1: 00003 | CNT2: 00005 | AIN1: 1.983 | AIN2: 7.031
DI1..4: 0101 | CNT1: 00003 | CNT2: 00006 | AIN1: 1.980 | AIN2: 7.284
DI1..4: 1111 | CNT1: 00004 | CNT2: 00007 | AIN1: 1.980 | AIN2: 7.534
</pre>

Initial values of important service registers (SDC, Service Data Channel)
are defined to be written - baudrate and mode of RS-485, mode of counters.
Also identification registers like hardware and firmware version and CPU ID
are read out during startup phase. You can use service registers handling
approach provided by this example as a basis for your projects.

To fully explore this example, connect together `AIN1`<>`AOUT1`, `AIN2<>AOUT2`,
`DIN1`<>`DOUT1`, `DIN3`<>`DOUT3`, `DIN4`<>`DOUT4`.

Digital output `DOUT1` is toggled each 1 second, `DOUT3` and `DOUT4` simulate
quadrature encoder signal. `COUNTER1` is configured for pulse counting mode
with both active edges, `COUNTER2` is configured for quadrature encoder.

Analog output `AOUT1` is driven by constant 2.0 Volts, `AOUT2` by sinusoidal
signal between 1.0 Volts and 9.0 Volts.

All input signals are dumped out with 500 ms period. 

## Recommendations for libmonarco users

In your C code:

* Include headers `src/monarco.h`, `monarco_platform.h`
* Define `int monarco_platform_dprint_flags` with value composed by `MONARCO_DPF_*` flags according to required debug print level. Alternatively you can provide your own `monarco_platform.h` implementation which is intended for integration into logging system in existing applications.
* Allocate context data `monarco_cxt_t cxt`.
* At init, call `monarco_init(&cxt, "/dev/spidev0.0", 4000000, "some-debug-print-prefix: ")`.
* Cyclically:
  * set output process data (to the Monarco HAT) into `cxt.tx_data` structure,
  * call `monarco_main(&cxt)`,
  * read input process data (from the Monarco HAT) from `cxt.rx_data` structure.
* At exit, call `monarco_exit(&cxt)`.

Notes:

* Period of cyclic loop calling `monarco_main(&cxt)` should be lower than process data watdog time (100 ms by default). For fluent communication of service registers (SDC), cycle time should not be extended too much. We recommend 5 - 50 ms. 
* Do not call `monarco_main(&cxt)` from multiple threads or multiple times without delay. Firmware of the Monarco HAT needs some time (200us is safe) to prepare for next SPI transaction.   
* For multi-theaded applications, do not forget to use mutex lock to protect access to context structure `monarco_cxt_t cxt` from multiple concurrent threads. Every direct access of the context structure or a function call with the context structure as an argument should be mutually exclusive between different threads.   

## Version history and compatibility notes

### Version 1.2

* Initial version.

### Version 1.3

* **API Changes:**
  * in `monarco_struct_tx_t`, e.g. `cxt.tx_data`, we changed `led_en` to `led_mask` and `led_on` to `led_value`
  * in `monarco_struct_control_byte_t`, e.g. `cxt.control_byte`, we changed `status_led_en` to `status_led_mask` and `status_led_on` to `status_led_value`
* Improved documentation comments in the whole libmonarco.
* Refactored example applications - there is a simple 'blink' demo, and advanced 'complex' demo.

## How do I ...?

* Convert required PWM frequency to `tx_data` format
  * use `monarco_util_pwm_freq_to_u16(double freq_hz)`
* Convert required PWM duty cycle to `tx_data` format
  * use `monarco_util_pwm_dc_to_u16(double dc)`
* Convert required Analog Output voltage to `tx_data` format
  * use `monarco_util_aout_volts_to_u16(double volts)`.
* Convert measured Analog Input voltage / current from `rx_data` format to real values
  * use `monarco_util_ain_10v_to_real(uint16_t ain)` / `monarco_util_ain_20ma_to_real(uint16_t ain)`.

## License

Libmonarco and examples provided in this repository are covered by the BSD 3-Clause License. See LICENSE.txt or https://opensource.org/licenses/BSD-3-Clause

Copyright REX Controls s.r.o. http://www.rexcontrols.com, Author: Vlastimil Setka, <setka@rexcontrols.com>
