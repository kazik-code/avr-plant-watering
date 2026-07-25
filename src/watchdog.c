#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include "watchdog.h"

volatile uint8_t wdt_flag = 0;

ISR(WDT_vect)
{
    WDTCSR |= (1 << WDIE); // Przerwanie czyści flagę WDIE
    wdt_flag = 1;
}

void wdt_set_prescaler(uint8_t prescaler)
{
    uint8_t previous_sreg = SREG;
    cli();
    WDTCSR = (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | prescaler;
    // zamiast sei() -> nie chcemy przypadkowo włączyć przerwania
    SREG = previous_sreg;
}
