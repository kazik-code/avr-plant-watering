MCU     = atmega328pb
F_CPU   = 16000000UL
CC      = avr-gcc
CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=c99 -Iinc

TARGET  = plant-watering
SRCS    = $(wildcard src/*.c)

$(TARGET).hex: $(TARGET).elf

$(TARGET).elf: $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

%.hex: %.elf
	avr-objcopy -O ihex $< $@

flash: $(TARGET).hex
	avrdude -c xplainedmini -p m328pb -U flash:w:$<

clean:
	rm -f *.elf *.hex

.PHONY: flash clean
