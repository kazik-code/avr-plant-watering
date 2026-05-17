#include "pump.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define GATE_PIN PB3
#define BTN_PIN  PB7
#define BTN2_PIN PD2

void pump_init(void)
{
    DDRB  |=  (1 << GATE_PIN);
    PORTB |=  (1 << GATE_PIN);

    DDRB  &= ~(1 << BTN_PIN);
    PORTB |=  (1 << BTN_PIN);

    DDRD  &= ~(1 << BTN2_PIN);
    PORTD |=  (1 << BTN2_PIN);

    PCICR  |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT7);

    EICRA |= (1 << ISC01); // INT0 na opadające zbocze
    EIMSK |= (1 << INT0);
}

ISR(PCINT0_vect) // Push i Pop wewnątrz makro
{
    if (!(PINB & (1 << BTN_PIN))) // Odczyt stanu portu B, z maską na PB7
        PORTB ^= (1 << GATE_PIN);
}

ISR(INT0_vect)
{
    PORTB ^= (1 << GATE_PIN);
}