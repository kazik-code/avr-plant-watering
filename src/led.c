#include "led.h"
#include <avr/io.h>

// Diody sygnalizacyjne stanu wilgotności (PORTD — wolne piny,
// PB0–PB3 zajmuje RTC i pompa, PC0/PC1 czujnik)
#define LED_WET_PIN   PD5 // zielona  — wilgotno
#define LED_MOIST_PIN PD6 // żółta    — średnio
#define LED_DRY_PIN   PD7 // czerwona — sucho (pompa pracuje)

#define LED_MASK ((1 << LED_WET_PIN) | (1 << LED_MOIST_PIN) | (1 << LED_DRY_PIN))

void led_init(void)
{
    DDRD  |=  LED_MASK;
    PORTD &= ~LED_MASK;
}

void led_set_state(moisture_state_t state)
{
    PORTD &= ~LED_MASK; // zgaś wszystkie, świeci tylko aktualny stan
    switch (state) {
        case MOISTURE_WET:   PORTD |= (1 << LED_WET_PIN);   break;
        case MOISTURE_MOIST: PORTD |= (1 << LED_MOIST_PIN); break;
        case MOISTURE_DRY:   PORTD |= (1 << LED_DRY_PIN);   break;
    }
}
