TARGET_BLINK = monarco-blink-demo
TARGET_COMPLEX = monarco-complex-demo
LIBS = -lm
CC = /usr/bin/arm-linux-gnueabihf-gcc
CFLAGS = -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=hard -g -Wall -Wno-write-strings -fmessage-length=0 -Wno-uninitialized -Werror=uninitialized -Wno-sign-compare -Werror=strict-aliasing -fvisibility=hidden -Wno-maybe-uninitialized -Wno-strict-aliasing

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
