# libmonarco - C language driver library for the Monarco HAT

VERSION: 1.2, FIRMWARE: 2.006

## Getting started

The most comprehensive way how to get into using the C language driver library for the Monarco HAT is to examine **application example in directory `examples/monarco-c-demo/`.**

Install required build-dependencies on your Raspbian running on Raspberry Pi:

<pre>
sudo apt update
sudo apt install build-essential 
</pre>

Install Git:

<pre>
sudo apt install git
</pre>

Clone the repository:

<pre>
git clone git@github.com:monarco/monarco-hat-driver-c.git
</pre>

Build the example:

<pre>
cd monarco-hat-driver-c/examples/monarco-c-demo
make
</pre>

There should be no build errors.

Run as root:

<pre>
sudo ./monarco-c-demo

### Monarco HAT C library (libmonarco) demo example v1.0

Cycle interval: 20.000000 ms

monarco-c-demo: [VERB] monarco_init: OK
MONARCO SDC INIT DONE, FW=00002004, HW=00000104, CPUID=247DBC0257516B45
DI1..4: 1111 | CNT1: 00000 | CNT2: 00000 | AIN1: 1.988 | AIN2: 5.333
DI1..4: 1110 | CNT1: 00000 | CNT2: 00001 | AIN1: 1.988 | AIN2: 5.717
DI1..4: 1100 | CNT1: 00000 | CNT2: 00002 | AIN1: 1.983 | AIN2: 6.100
DI1..4: 1101 | CNT1: 00000 | CNT2: 00003 | AIN1: 1.983 | AIN2: 6.466
DI1..4: 1111 | CNT1: 00000 | CNT2: 00004 | AIN1: 1.990 | AIN2: 6.823
DI1..4: 1110 | CNT1: 00000 | CNT2: 00005 | AIN1: 1.985 | AIN2: 7.158
DI1..4: 1100 | CNT1: 00000 | CNT2: 00006 | AIN1: 1.985 | AIN2: 7.485
DI1..4: 1101 | CNT1: 00000 | CNT2: 00007 | AIN1: 1.980 | AIN2: 7.785
DI1..4: 1111 | CNT1: 00000 | CNT2: 00008 | AIN1: 1.985 | AIN2: 8.044
DI1..4: 1110 | CNT1: 00000 | CNT2: 00009 | AIN1: 1.988 | AIN2: 8.276
DI1..4: 1100 | CNT1: 00000 | CNT2: 00010 | AIN1: 1.985 | AIN2: 8.479

</pre>

This example code runs in 20 ms cyclic loop. Initial values of important service registers (SDC) are defined to be written, and identification registers like hardware and firmware version and CPU ID are read out during startup phase. You can use service registers handling approach provided by this example as a basis for your projects. Completion of SDC startup phase is signalised by message starting with `MONARCO SDC INIT DONE`.

For demonstration, `AOUT1` is driven by constant 2.0 Volts, `AOUT2` is driven by sinusoidal signal between 1.0 Volts and 9.0 Volts. Digital outputs `DOUT3` and `DOUT4` simulate quadrature encoder signal.

Input signals are dumped out with 640 ms period. For the test, it is expected you connected together AIN1<>AOUT1, AIN2<>AOUT2, DIN3<>DOUT3, DIN4<>DOUT4. COUNTER2 module is switched to quadrature decoder mode.

## Basic notes for libmonarco users

In your C code:

- Include headers `src/monarco.h`, `monarco_platform.h`
- Define `int monarco_platform_dprint_flags` with value composed by `MONARCO_DPF_*` flags according to required debug print level. Alternatively you can provide your own `monarco_platform.h` implementation.
- Allocate context data `monarco_cxt_t cxt`.
- At init, call `monarco_init(&cxt, "/dev/spidev0.0", 4000000, "some-debug-print-prefix: ")`.
- Cyclically, with period under under 100 ms:
 - set output process data (to the Monarco HAT) into `cxt.tx_data` structure,
 - call `monarco_main(&cxt)`,
 - read input process data (from the Monarco HAT) from `cxt.rx_data` structure.
- At exit, call `monarco_exit(&cxt)`.
- For multi-theaded applications, do not forget to use mutex lock to protect access to context structure `monarco_cxt_t cxt`. Every direct access of the context structure or a function call with the context structure as an argument should be mutually exclusive between different threads.   

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
