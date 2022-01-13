# documentation
DOXYGEN = /usr/local/bin/doxygen
DOXYFILE = Doxyfile
DOCDIR = doc

CFLAGS = -g -lm -pthread -lsndfile -lpython3.9 -lcrypt -ldl -lutil -lrt
SOURCEDIR := src
INCDIR := inc
BINDIR := bin
MDRIVERDIR := driver_motor
DDRIVERDIR := driver_dht

MAIN_DEPS := $(INCDIR)/motor.h \
	$(INCDIR)/dht.h \
	$(INCDIR)/livestream.h \
	$(INCDIR)/utils.h \
	$(INCDIR)/database.h

DAEMON_DEPS := $(INCDIR)/microphone.h

OBJS_MAIN := $(SOURCEDIR)/motor.c \
	$(SOURCEDIR)/dht.c \
	$(SOURCEDIR)/livestream.c \
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

all: main daemon drivers documentation

main: $(OBJS_MAIN)
	$(CC) $(SOURCEDIR)/main.c -o $@.elf $(OBJS_MAIN) $(CFLAGS) 
	mv main.elf $(BINDIR)

daemon: $(OBJS_DAEMON)
	$(CC) $(SOURCEDIR)/daemon.c -o $@.elf $(OBJS_DAEMON) $(CFLAGS)
	mv daemon.elf $(BINDIR)

drivers:
	$(MAKE) -C $(KERNEL_DIR) M=$$PWD
	rm *.o *.mod *.mod.c *.symvers *.order
	rm $(SOURCEDIR)/utils.o
	rm $(MDRIVERDIR)/*.o
	mv motordriver.ko $(BINDIR)
	rm $(DDRIVERDIR)/*.o
	mv dht11.ko $(BINDIR)

documentation:
	$(DOXYGEN) $(DOXYFILE)

%.o: %.c $(MAIN_DEPS) $(DAEMON_DEPS)
	$(CROSS_COMPILE) -c -o $@ $< $(CFLAGS)

# remove all built files
clean:
	rm -f -r $(BINDIR)/* $(DOCDIR)/*