# Directories
INCDIR := ../inc
BINDIR := ../bin

# Main Dependencies
MAIN_DEPS := $(INCDIR)/motor.h \
	$(INCDIR)/dht.h \
	$(INCDIR)/livestream.h \
	$(INCDIR)/gpio.h \
	$(INCDIR)/database.h

SRC_MAIN := motor.c \
	dht.c \
	livestream.c \
	gpio.c \
	database.c

# Daemon Dependencies
SRC_DAEMON := microphone.c

DAEMON_DEPS := $(INCDIR)/microphone.h

# Flags
CFLAGS = -g -lm -pthread -lsndfile -lpython3.9 -lcrypt -ldl -lutil -lrt

# Cross-Compiling
CC = arm-buildroot-linux-gnueabihf-gcc

#######################################################################

all: main daemon

main: $(SRC_MAIN)
	$(CC) main.c -o $@.elf $(SRC_MAIN) $(CFLAGS) 
	mv main.elf $(BINDIR)

daemon: $(OBJS_DAEMON)
	$(CC) daemon.c -o $@.elf $(SRC_DAEMON) $(CFLAGS)
	mv daemon.elf $(BINDIR)