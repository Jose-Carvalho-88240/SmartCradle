CFLAGS = -g -lm -pthread -lsndfile -lpython3.9 -lcrypt -ldl -lutil -lrt
SOURCEDIR := src
INCDIR := inc
BINDIR := bin
MDRIVERDIR := driver_motor
DDRIVERDIR := driver_dht

MAIN_DEPS := $(INCDIR)/motor.h \
	$(INCDIR)/dht.h \
	$(INCDIR)/livestream.h \
	$(INCDIR)/microphone.h \
	$(INCDIR)/utils.h \
	$(INCDIR)/database.h

DAEMON_DEPS := $(INCDIR)/microphone.h

OBJS_MAIN := $(SOURCEDIR)/motor.c \
	$(SOURCEDIR)/dht.c \
	$(SOURCEDIR)/livestream.c \
	$(SOURCEDIR)/microphone.c \
	$(SOURCEDIR)/utils.c \
	$(SOURCEDIR)/database.c

OBJS_DAEMON := $(SOURCEDIR)/microphone.c

obj-m = motordriver.o dht11.o
motordriver-objs := $(MDRIVERDIR)/motordriver_module.o $(SOURCEDIR)/utils.o
dht11-objs := $(DDRIVERDIR)/dht11_module.o $(SOURCEDIR)/utils.o

KERNEL_DIR ?= ~/Downloads/buildroot/buildroot-2021.02.6/output/build/linux-custom
ARCH ?= arm
CROSS_COMPILE ?= arm-buildroot-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc
export ARCH CROSS_COMPILE

all: main daemon drivers

main: $(OBJS_MAIN)
	$(CC) $(SOURCEDIR)/main.c -o $@.elf $(OBJS_MAIN) $(CFLAGS) 
	mv main.elf $(BINDIR)

daemon: $(OBJS_DAEMON)
	$(CC) $(SOURCEDIR)/daemon.c -o $@.elf $(OBJS_DAEMON) $(CFLAGS)
	mv daemon.elf $(BINDIR)

drivers:
	$(MAKE) -C $(KERNEL_DIR) M=$$PWD
	rm *.o *.mod *.mod.c *.symvers *.order
	rm src/utils.o
	rm driver_motor/*.o
	mv motordriver.ko bin/
	rm driver_dht/*.o
	mv dht11.ko bin/

%.o: %.c $(MAIN_DEPS) $(DAEMON_DEPS)
	$(CROSS_COMPILE) -c -o $@ $< $(CFLAGS)

# remove all built files
clean:
	rm -f -r bin/*