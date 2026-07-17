#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "pump.h"
#include "ds1302.h"
#include "moisture_sensor.h"
#include "led.h"
#include "i2c_line.h"

// WDT budzi co ~8 s tylko po to, by odpytać RTC — o interwale decyduje zegar
#define CHECK_INTERVAL_MIN 15
#define PUMP_TICKS          2
#define BUTTON_PIN          PD2

static volatile uint8_t wdt_flag    = 0;
static volatile uint8_t button_flag = 0;

ISR(WDT_vect)
{
    WDTCSR |= (1 << WDIE); // Przerwanie czyści flagę WDIE
    wdt_flag = 1;
}

static void wdt_init(void)
{
    WDTCSR = (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0); // ~8s, tryb przerwania
}

ISR(INT0_vect)
{
    button_flag = 1;
}

static void button_init(void)
{
    DDRD  &= ~(1 << BUTTON_PIN);
    PORTD |=  (1 << BUTTON_PIN);
    EICRA &= ~((1 << ISC01) | (1 << ISC00));
    EIMSK |=  (1 << INT0);
}

static void pump_run(void)
{
    pump_on();
    wdt_reset(); // synchronizuj licznik z momentem startu
    for (uint8_t i = 0; i < PUMP_TICKS; i++) {
        wdt_flag = 0;
        while (!wdt_flag)
            sleep_mode();
    }
    pump_off();
}

// Pełny cykl: pomiar → sygnalizacja → podlanie tylko gdy sucho
static void check_and_water(void)
{
    uint8_t raw = read_moisture();

    moisture_state_t state = moisture_classify(raw);
    led_set_state(state); // sygnalizacja stanu na diodach

    if (state == MOISTURE_DRY)
        pump_run();
}

static void init_peripherals(void)
{
    pump_init();
    ds1302_init();
    moisture_sensor_init();
    led_init();
    button_init();
    wdt_init();
    i2c_init();
}

int main(void)
{
    init_peripherals();
    ds1302_time_t t = {
        .seconds = 0,
        .minutes = 0,
        .hours   = 12,
        .date    = 1,
        .month   = 1,
        .day     = 3,
        .year    = 26,
    };
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sei();

    ds1302_time_t now;
    ds1302_get_time(&now);
    uint16_t last_check = (uint16_t)now.hours * 60 + now.minutes; // minuta doby 0–1439

    while (1) {
        
        if (i2c_ping_device(0x3C)) {
            PORTD |= (1 << LED_ACK_PING);
        } else {
            PORTD &= ~(1 << LED_ACK_PING);
        }

        sleep_mode();

        uint8_t do_check = 0;

        // Przycisk — pomiar poza harmonogramem 
        if (button_flag) {
            button_flag = 0;
            do_check = 1;
        }

        if (wdt_flag) {
            wdt_flag = 0;
            ds1302_get_time(&now);
            uint16_t mod = (uint16_t)now.hours * 60 + now.minutes;

            int16_t diff = (int16_t)mod - (int16_t)last_check;
            if (diff < 0)
                diff += 1440; // przejście przez północ

            if (diff >= CHECK_INTERVAL_MIN) {
                last_check = mod;
                do_check   = 1;
            }
        }

        if (do_check)
            check_and_water();
    }
}
