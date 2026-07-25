#pragma once

#include <avr/io.h>
#include <stdint.h>

#define WDT_PERIOD_500_MS ((1 << WDP2) | (1 << WDP0))
#define WDT_PERIOD_8_S    ((1 << WDP3) | (1 << WDP0))

extern volatile uint8_t wdt_flag;

void wdt_set_prescaler(uint8_t prescaler);
