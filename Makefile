# makefile
MCU=atmega328p
DUDECPUTYPE=m328p

F_CPU = 16000000
DEVICE = atmega328p

LOADCMD=avrdude
LOADARG=-p $(DUDECPUTYPE) -c stk500v2 -e -P /dev/ttyUSB0 -U flash:w:

PATH_TCP= eth_lcd_tcp-2.10
PATH_WRAPPER = wrapper

LIB			= aurora
AURORA  = aurora
DEP		  = $(PATH_TCP)/ip_arp_udp_tcp.h $(PATH_TCP)/avr_compat.h $(PATH_TCP)/enc28j60.h $(PATH_TCP)/timeout.h $(PATH_TCP)/net.h

#
CC=avr-gcc
OBJCOPY=avr-objcopy
# optimize for size:
#CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues

CFLAGS = -g2 -I. -Iaurora -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) 
CFLAGS+= -Os -ffunction-sections -fdata-sections -fpack-struct -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions  
CFLAGS+= -Wall -Wno-pointer-to-int-cast

#
#-------------------
help: 
	@echo "Usage: make all"
	@echo "Usage: make flash -- flash aurora.hex after build process"
	@echo "or"
	@echo "Usage: make clean"
#-------------------
#main.o : main.c ip_arp_udp_tcp.h avr_compat.h enc28j60.h timeout.h net.h
#	$(CC) $(CFLAGS) -Os -c main.c
#------------------
all: $(AURORA)

$(LIB): $(DEP)
	@echo Building Library 
	@$(CC) $(CFLAGS) -o Objects/$@.o -c $@.c 

$(AURORA): $(LIB)
	@echo Building $@
	@$(CC) $(CFLAGS) -o Objects/$@.o -c $@.c 
	@avr-size Objects/$@.o
	@@avr-objcopy -j .text  -j .data -O ihex Objects/$@.o $@.hex
	@avr-objdump -d -S Objects/$@.o >Objects/$@.lss

#------------------
load: aurora.hex
	$(LOADCMD) $(LOADARG)aurora.hex
#-------------------
.PHONY:	clean

clean:
	rm -f Objects/*.o *.map *.out $@.hex
#-------------------
