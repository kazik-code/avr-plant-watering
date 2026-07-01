#include "pump.h"
#include <avr/io.h>

#define GATE_PIN PB5

void pump_init(void)
{
    DDRB  |=  (1 << GATE_PIN);
    PORTB &= ~(1 << GATE_PIN);
}

void pump_on(void)
{
    PORTB |=  (1 << GATE_PIN);
}

void pump_off(void)
{
    PORTB &= ~(1 << GATE_PIN);
}
