# Change these parameters for your device

F_CPU = 16000000
DEVICE = atmega328p
DUDECPUTYPE=m328p

LOADCMD=avrdude
LOADARG=-p $(DUDECPUTYPE) -c stk500v2 -e -P /dev/ttyUSB0 -U flash:w:

# Tools:
CC = avr-gcc

LIB_PXL         = light_ws2812
WRAPPER         = wrapper
COMMAND         = command
AURORA          = aurora
DEP             = ws2812b/ws2812_config.h ws2812b/light_ws2812.h wrapper/wrapper.h lib/command.h
TCP_OBJ         = eth_lcd_tcp-2.10/enc28j60.o eth_lcd_tcp-2.10/ip_arp_udp_tcp.o eth_lcd_tcp-2.10/lcd.o

CFLAGS = -g2 -I. -Iws2812b -Iwrapper -Icommand -mmcu=$(DEVICE) -DF_CPU=$(F_CPU)
CFLAGS+= -Os -ffunction-sections -fdata-sections -fpack-struct -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions
CFLAGS+= -Wall -Wno-pointer-to-int-cast

LDFLAGS = -Wl,--relax,--section-start=.text=0,-Map=main.map

all:
			cd eth_lcd_tcp-2.10 && $(MAKE)
			$(MAKE) $(AURORA)

light_ws2812.o: $(DEP)
				@echo Building Library $@
				@$(CC) $(CFLAGS) -o objects/light_ws2812.o -c ws2812b/light_ws2812.c

wrapper.o: $(DEP)
				@echo Building Library $@
				@$(CC) $(CFLAGS) -o wrapper/wrapper.o -c wrapper/wrapper.c

command.o: $(DEP)
				@echo Building Library $@
				@$(CC) $(CFLAGS) -o lib/command.o -c lib/command.c

#-------------------
$(AURORA): light_ws2812.o wrapper.o
				@echo Building $@
				@$(CC) $(CFLAGS) -o objects/light_ws2812.o wrapper/wrapper.o lib/command.o $(TCP_OBJ) $@.c ws2812b/light_ws2812.c
				@avr-size objects/light_ws2812.o
				@avr-objcopy -j .text  -j .data -O ihex objects/light_ws2812.o aurora.hex
				@avr-objdump -d -S objects/light_ws2812.o >objects/light_ws2812.lss


#------------------
flash:	aurora.hex
				$(LOADCMD) $(LOADARG)aurora.hex
#-------------------
.PHONY: clean

clean:
				rm -f *.hex wrapper/*.o lib/*.o objects/*.o objects/*.lss

allclean:
				rm -f *.hex wrapper/*.o lib/*.o objects/*.o objects/*.lss
				cd eth_lcd_tcp-2.10 && $(MAKE) clean

