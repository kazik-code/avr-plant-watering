#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "pump.h"
#include "ds1302.h"
#include "moisture_sensor.h"
#include "led.h"
#include "i2c_line.h"
#include "oled.h"
#include "ui.h"
#include "watchdog.h"
#include "watering.h"

#define CHECK_INTERVAL_MIN 15
#define BUTTON_PIN          PD2

static volatile uint8_t button_flag = 0;

ISR(INT0_vect)
{
    //EIMSK &= ~(1 << INT0);
    button_flag = 1;
}

static void button_init(void)
{
    DDRD  &= ~(1 << BUTTON_PIN);
    PORTD |=  (1 << BUTTON_PIN);
    EICRA &= ~((1 << ISC01) | (1 << ISC00));
    EIMSK |=  (1 << INT0);
}

static void init_peripherals(void)
{
    pump_init();
    ds1302_init();
    moisture_sensor_init();
    moisture_sensor_disable();
    led_init();
    button_init();
    wdt_set_prescaler(WDT_PERIOD_8_S);
    i2c_init();
    oled_init();

    // ds1302_time_t init_time = {
    //     .seconds = 0,
    //     .minutes = 1,
    //     .hours   = 17,
    //     .date    = 24,
    //     .month   = 7,
    //     .day     = 5,
    //     .year    = 26,
    // };

    // ds1302_set_time(&init_time);
}

int main(void)
{
    init_peripherals();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sei();

    ds1302_time_t now;
    ds1302_get_time(&now);
    uint16_t last_check = (uint16_t)now.hours * 60 + now.minutes; // minuta doby 0–1439



    while (1) {
        ds1302_get_time(&now);
        display_default_page(now);

        uint8_t do_check = 0;

        // Przycisk — pomiar poza harmonogramem 
        if (button_flag) {
            button_flag = 0;
            do_check = 1;
        }

        if (wdt_flag) {
            wdt_flag = 0;

            uint16_t mod = (uint16_t)now.hours * 60 + now.minutes;

            int16_t diff = (int16_t)mod - (int16_t)last_check;
            if (diff < 0)
                diff += 1440; // przejście przez północ

            if (diff >= CHECK_INTERVAL_MIN) {
                last_check = mod;
                do_check   = 1;
            }
        }

        if (do_check){
            check_and_water();
            ds1302_get_time(&now);
            display_default_page(now);
        }

        sleep_mode();
    }
}
