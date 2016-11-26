################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include ws2812b/subdir.mk
-include wrapper/subdir.mk
-include eth_lcd_tcp-2.10/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
aurora.lss \

FLASH_IMAGE += \
aurora.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: aurora.elf secondary-outputs

# Tool invocations
aurora.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,aurora.map -mmcu=atmega328p -o "aurora.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

aurora.lss: aurora.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S aurora.elf  >"aurora.lss"
	@echo 'Finished building: $@'
	@echo ' '

aurora.hex: aurora.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex aurora.elf  "aurora.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: aurora.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega328p aurora.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) aurora.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
