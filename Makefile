MCU     = atmega328p
F_CPU   = 16000000UL
CC      = avr-gcc
INCDIRS = $(sort $(dir $(shell find lib -name '*.h' 2>/dev/null)))
CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=c99 -Iinc $(addprefix -I,$(INCDIRS))
AVRDUDE = avrdude
PORT    ?= $(shell ls /dev/cu.usbmodem* /dev/cu.usbserial* 2>/dev/null | head -n 1)

# Arduino Nano (bootloader)
#PROGRAMMER = arduino
#BAUD    = 57600

# Arduino Uno (bootloader)
PROGRAMMER = arduino
BAUD    = 115200

TARGET  = plant-watering
SRCS    = $(shell find src lib -name '*.c' 2>/dev/null)

$(TARGET).hex: $(TARGET).elf

$(TARGET).elf: $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

%.hex: %.elf
	avr-objcopy -O ihex $< $@

flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$<:i

clean:
	rm -f *.elf *.hex

.PHONY: flash clean
