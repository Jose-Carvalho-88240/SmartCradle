obj-m := dht11.o
dht11-objs := dht11_module.o utils.o

KERNEL_DIR ?= /home/carvalho/Downloads/buildroot/buildroot-2021.02.6/output/build/linux-custom

ARCH ?= arm

CROSS_COMPILE ?= arm-buildroot-linux-gnueabihf-

export ARCH CROSS_COMPILE

default: ../inc/dht.h
	$(MAKE) -C $(KERNEL_DIR) M=$$PWD
	
install:
	$(MAKE) -C $(KERNEL_DIR) M=$$PWD modules_install
	
clean:
	$(MAKE) -C $(KERNEL_DIR) M=$$PWD clean
