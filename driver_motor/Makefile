BINDIR := ../bin
SOURCEDIR := ../src

obj-m = motordriver.o
motordriver-objs := motordriver_module.o $(SOURCEDIR)/gpio.o

KERNEL_DIR ?= ~/Downloads/buildroot/buildroot-2021.02.6/output/build/linux-custom
ARCH ?= arm
CROSS_COMPILE ?= arm-buildroot-linux-gnueabihf-
export ARCH CROSS_COMPILE

all: motordriver clean

motordriver:
	$(MAKE) -C $(KERNEL_DIR) M=$$PWD

clean:
	rm *.o *.mod *.mod.c *.symvers *.order
	rm $(SOURCEDIR)/gpio.o
	mv motordriver.ko $(BINDIR)