#include <avr/interrupt.h>
#include "pump.h"
#include "ds1302.h"

int main(void)
{
    pump_init();
    ds1302_init();
    ds1302_time_t t = {
        .seconds = 0,
        .minutes = 0,
        .hours   = 12,
        .date    = 1,
        .month   = 1,
        .day     = 3, // wtorek
        .year    = 24, // 2024
    };
    ds1302_set_time(&t);
    sei();
    while (1) {}
}
