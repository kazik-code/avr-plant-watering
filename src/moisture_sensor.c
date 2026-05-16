#include <stdint.h>
#include <avr/io.h>

void moisture_sensor_init(void)
{
    DDRC  &= ~(1 << PC0); // PC0 jako wejście
    PORTC |=  (1 << PC0); // włącz pull-up
}

void adc_init(void) {
    // Referencja = AVCC, kanał = ADC0 (PC0)
    ADMUX = (1 << REFS0);

    // Preskaler 128 → przy 8MHz daje 62.5kHz (ADC potrzebuje 50-200kHz)
    ADCSRA = (1 << ADEN)  |   // włącz ADC
             (1 << ADPS2) |   // preskaler 128
             (1 << ADPS1) |
             (1 << ADPS0);
}

uint16_t sensor_read(void)
{
    ADMUX = (1 << REFS0); // Referencja = AVCC, kanał = ADC0 (PC0)
    ADCSRA |= (1 << ADSC); // Start konwersji
    while (ADCSRA & (1 << ADSC)); // czekaj na zakończenie konwersji
    return ADC; // zwróć wynik (10-bitowy)
}