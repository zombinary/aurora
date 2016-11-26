################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../eth_lcd_tcp-2.10/enc28j60.o \
../eth_lcd_tcp-2.10/ip_arp_udp_tcp.o \
../eth_lcd_tcp-2.10/lcd.o \
../eth_lcd_tcp-2.10/main.o \
../eth_lcd_tcp-2.10/test0.o \
../eth_lcd_tcp-2.10/test1.o \
../eth_lcd_tcp-2.10/test2.o \
../eth_lcd_tcp-2.10/test_readSiliconRev.o \
../eth_lcd_tcp-2.10/timeout.o 

C_SRCS += \
../eth_lcd_tcp-2.10/enc28j60.c \
../eth_lcd_tcp-2.10/ip_arp_udp_tcp.c \
../eth_lcd_tcp-2.10/lcd.c \
../eth_lcd_tcp-2.10/timeout.c 

OBJS += \
./eth_lcd_tcp-2.10/enc28j60.o \
./eth_lcd_tcp-2.10/ip_arp_udp_tcp.o \
./eth_lcd_tcp-2.10/lcd.o \
./eth_lcd_tcp-2.10/timeout.o 

C_DEPS += \
./eth_lcd_tcp-2.10/enc28j60.d \
./eth_lcd_tcp-2.10/ip_arp_udp_tcp.d \
./eth_lcd_tcp-2.10/lcd.d \
./eth_lcd_tcp-2.10/timeout.d 


# Each subdirectory must supply rules for building sources it contributes
eth_lcd_tcp-2.10/%.o: ../eth_lcd_tcp-2.10/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


