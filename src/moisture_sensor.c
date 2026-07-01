#include <stdint.h>
#include <avr/io.h>
#include "moisture_sensor.h"

#define DISABLE_PIN PC1
#define SENSOR_PIN PC0

static void ADC_init(void)
{
    ADMUX = (1 << REFS0); // AVcc reference
    ADMUX |= (1 << ADLAR); // kanał ADC0 (PC0 = SENSOR_PIN), wynik wyrównany w lewo → ADCH
    ADCSRA = (1 << ADEN) | (7 << ADPS0); // Prescaler 128

    // Według datasheeta, pierwszy odczyt jest śmieciowy, więc wykonujemy go przy init
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
}

void moisture_sensor_init(void)
{
    DDRC  &= ~(1 << SENSOR_PIN);
    DDRC  |=  (1 << DISABLE_PIN);
}

void moisture_sensor_enable(void)
{
    PORTC &= ~(1 << DISABLE_PIN);
    ADC_init();
}

void moisture_sensor_disable(void)
{
    PORTC |= (1 << DISABLE_PIN);
}

uint8_t read_moisture(void)
{
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADCH;
}

moisture_state_t moisture_classify(uint8_t raw)
{
    if (raw < MOISTURE_THRESHOLD_WET)   return MOISTURE_WET;
    if (raw < MOISTURE_THRESHOLD_MOIST) return MOISTURE_MOIST;
    return MOISTURE_DRY;
}
