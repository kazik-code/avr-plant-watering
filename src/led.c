#include "led.h"
#include <avr/io.h>
#define LED_WET_PIN   PD5 
#define LED_MOIST_PIN PD6 
#define LED_DRY_PIN   PD7 
#define LED_ALWAYS_PIN PD4

#define LED_MASK ((1 << LED_WET_PIN) | (1 << LED_MOIST_PIN) | (1 << LED_DRY_PIN))

void led_init(void)
{
    DDRD  |=  LED_MASK;
    PORTD &= ~LED_MASK;

    DDRD  |=  (1 << LED_ALWAYS_PIN);
    PORTD |=  (1 << LED_ALWAYS_PIN);
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
