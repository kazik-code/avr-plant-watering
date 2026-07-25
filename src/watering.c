#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <util/delay.h>
#include "led.h"
#include "moisture_sensor.h"
#include "oled.h"
#include "pump.h"
#include "watchdog.h"
#include "watering.h"

#define PUMP_TICKS 32

static void pump_run(void)
{
    wdt_set_prescaler(WDT_PERIOD_500_MS);
    oled_clear_display();
    oled_write_centered_text("TRWA", 2);
    oled_write_centered_text("PODLEWANIE", 3);
    pump_on();
    wdt_reset(); // synchronizuj licznik z momentem startu
    for (uint8_t i = 0; i < PUMP_TICKS; i++) {
        char watering_text[4];
        uint8_t dots = i % 4; // kolejno 0, 1, 2, 3 kropki

        for (uint8_t j = 0; j < dots; j++) {

            watering_text[j] = '.';
        }

        watering_text[dots] = '\0';

        oled_write_centered_text(watering_text, 4);
        wdt_flag = 0;
        while (!wdt_flag)
            sleep_mode();
    }
    pump_off();
    wdt_set_prescaler(WDT_PERIOD_8_S);
    oled_clear_display();
}

// Pełny cykl: pomiar → sygnalizacja → podlanie tylko gdy sucho
void check_and_water(void)
{
    moisture_sensor_enable();
    _delay_ms(500);

    uint16_t sum = 0;

    for (uint8_t i = 0; i < 16; i++) {
        sum += read_moisture();
        _delay_ms(10);
    }

    uint8_t raw = sum / 16;
    moisture_sensor_disable();

    moisture_state_t state = moisture_classify(raw);
    led_set_state(state); // sygnalizacja stanu na diodach

    if (state == MOISTURE_DRY)
        pump_run();
}
