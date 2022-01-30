INCDIR := ../inc
BINDIR := ../bin

MAIN_DEPS := $(INCDIR)/motor.h \
	$(INCDIR)/dht.h \
	$(INCDIR)/livestream.h \
	$(INCDIR)/gpio.h \
	$(INCDIR)/database.h

DAEMON_DEPS := $(INCDIR)/microphone.h

OBJS_MAIN := motor.c \
	dht.c \
	livestream.c \
	gpio.c \
	database.c

OBJS_DAEMON := microphone.c

CFLAGS = -g -lm -pthread -lsndfile -lpython3.9 -lcrypt -ldl -lutil -lrt

all: main daemon

main: $(OBJS_MAIN)
	$(CC) main.c -o $@.elf $(OBJS_MAIN) $(CFLAGS) 
	mv main.elf $(BINDIR)

daemon: $(OBJS_DAEMON)
	$(CC) daemon.c -o $@.elf $(OBJS_DAEMON) $(CFLAGS)
	mv daemon.elf $(BINDIR)