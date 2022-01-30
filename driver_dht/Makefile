# Directories
BINDIR := ../bin
SOURCEDIR := ../src
KERNEL_DIR ?= ~/Downloads/buildroot/buildroot-2021.02.6/output/build/linux-custom

# Objects
obj-m = dht11.o
dht11-objs := dht11_module.o $(SOURCEDIR)/gpio.o

# Cross-Compiling
ARCH ?= arm
CROSS_COMPILE ?= arm-buildroot-linux-gnueabihf-
export ARCH CROSS_COMPILE

###############################################

all: dht11driver clean

dht11driver:
	$(MAKE) -C $(KERNEL_DIR) M=$$PWD
	mv dht11.ko $(BINDIR)

# Remove all built files
clean:
	rm *.o *.mod *.mod.c *.symvers *.order
	rm $(SOURCEDIR)/gpio.o