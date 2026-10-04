#include "pump.h"
#include <avr/io.h>

#define GATE_PIN PB4

void pump_init(void)
{
    // The assembled pump switch is active-high: LOW = off, HIGH = on.
    // Set the latch before enabling the output to avoid a startup pulse.
    PORTB &= ~(1 << GATE_PIN);
    DDRB  |=  (1 << GATE_PIN);
}

void pump_off(void)
{
    PORTB &= ~(1 << GATE_PIN);
}

void pump_on(void)
{
    PORTB |=  (1 << GATE_PIN);
}
