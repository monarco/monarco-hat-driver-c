TARGET = monarco-c-demo
LIBS = -lm
CC = /usr/bin/arm-linux-gnueabihf-gcc
CFLAGS = -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=hard -g -Wall -Wno-write-strings -fmessage-length=0 -Wno-uninitialized -Werror=uninitialized -Wno-sign-compare -Werror=strict-aliasing -fvisibility=hidden -Wno-maybe-uninitialized -Wno-strict-aliasing

.PHONY: default all clean

default: $(TARGET)
all: default

SRCPATH = ../../src
INCLUDEPATH = -I../../ -I../../platform/linux
OBJECTS = main.o $(patsubst %.c, %.o, $(wildcard $(SRCPATH)/*.c))
HEADERS = $(wildcard $(SRCPATH)/*.h) ../../platform/linux/monarco_platform.h

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDEPATH) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(SRCPATH)/*.o
	-rm -f $(TARGET)
