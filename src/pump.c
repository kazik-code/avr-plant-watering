#include "pump.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define GATE_BIT PB3
#define BTN_BIT  PB7
#define BTN2_PIN PD2

void pump_init(void)
{
    DDRB  |=  (1 << GATE_BIT);
    PORTB |=  (1 << GATE_BIT);

    DDRB  &= ~(1 << BTN_BIT);
    PORTB |=  (1 << BTN_BIT);

    DDRD  &= ~(1 << BTN2_PIN);
    PORTD |=  (1 << BTN2_PIN);

    PCICR  |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT7);

    EICRA |= (1 << ISC01); // INT0 na opadające zbocze
    EIMSK |= (1 << INT0);
}

ISR(PCINT0_vect) // Push i Pop wewnątrz makro
{
    if (!(PINB & (1 << BTN_BIT))) // Odczyt stanu portu B, z maską na PB7
        PORTB ^= (1 << GATE_BIT);
}

ISR(INT0_vect)
{
    PORTB ^= (1 << GATE_BIT);
}