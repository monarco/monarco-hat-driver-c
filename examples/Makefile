TARGET_BLINK = monarco-blink-demo
TARGET_COMPLEX = monarco-complex-demo
LIBS = -lm
# ARM comparison: https://en.wikipedia.org/wiki/Comparison_of_ARMv7-A_processors
# RPi ARMs: https://en.wikipedia.org/wiki/Raspberry_Pi
# 64-bit CC settings
ifeq ($(shell uname -m), aarch64)
	CC = /usr/bin/aarch64-linux-gnu-gcc
	# The AArch64 compiler does not have a -mfpu option of any format, it's all part of the architecture (-march) option.
	# The -mfloat-abi= option is invalid when compiling for AArch64 targets, for which the compiler will always 
	# generate FPU instructions and will always pass floating-point arguments in FPU registers.
	CFLAGS = -march=armv8-a -g -Wall -Wno-write-strings -fmessage-length=0 -Wno-uninitialized -Werror=uninitialized -Wno-sign-compare -Werror=strict-aliasing -fvisibility=hidden -Wno-maybe-uninitialized -Wno-strict-aliasing
else
	# 32 bit CC settings
	CC = /usr/bin/arm-linux-gnueabihf-gcc
	# armv7-a
	ifeq ($(shell uname -m), armv7l) 
		CFLAGS = -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=hard -g -Wall -Wno-write-strings -fmessage-length=0 -Wno-uninitialized -Werror=uninitialized -Wno-sign-compare -Werror=strict-aliasing -fvisibility=hidden -Wno-maybe-uninitialized -Wno-strict-aliasing
	else
$(error Unrecognised CPU architecture)
	endif
endif

.PHONY: default all clean

default: $(TARGET_COMPLEX) $(TARGET_BLINK)
all: default

SRCPATH = ../src
INCLUDEPATH = -I../ -I../platform/linux
LIBOBJECTS = $(patsubst %.c, %.o, $(wildcard $(SRCPATH)/*.c))
HEADERS = $(wildcard $(SRCPATH)/*.h) ../platform/linux/monarco_platform.h

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c $< -o $@

.PRECIOUS: $(LIBOBJECTS) $(TARGET_COMPLEX) main-blink-app.o $(TARGET_BLINK) main-complex-app.o

$(TARGET_BLINK): main-blink-demo.o $(LIBOBJECTS)
	$(CC) main-blink-demo.o $(LIBOBJECTS) -Wall $(LIBS) -o $@

$(TARGET_COMPLEX): main-complex-demo.o $(LIBOBJECTS)
	$(CC) main-complex-demo.o $(LIBOBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(SRCPATH)/*.o
	-rm -f $(TARGET_COMPLEX) $(TARGET_BLINK)
