MCU     = atmega328pb
F_CPU   = 16000000UL
CC      = avr-gcc
INCDIRS = $(sort $(dir $(shell find lib -name '*.h' 2>/dev/null)))
CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=c99 -Iinc $(addprefix -I,$(INCDIRS))

TARGET  = plant-watering
SRCS    = $(shell find src lib -name '*.c')

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
