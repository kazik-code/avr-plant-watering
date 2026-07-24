#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "pump.h"
#include "ds1302.h"
#include "moisture_sensor.h"
#include "led.h"
#include "i2c_line.h"
#include "oled.h"
#include "font_5x7.h"
#include "output_text.h"

#define CHECK_INTERVAL_MIN 15
#define PUMP_TICKS          32
#define BUTTON_PIN          PD2

#define WDT_PERIOD_500_MS ((1 << WDP2) | (1 << WDP0))
#define WDT_PERIOD_8_S    ((1 << WDP3) | (1 << WDP0))

static volatile uint8_t wdt_flag    = 0;
static volatile uint8_t button_flag = 0;

ISR(WDT_vect)
{
    WDTCSR |= (1 << WDIE); // Przerwanie czyści flagę WDIE
    wdt_flag = 1;
}

static void wdt_set_prescaler(uint8_t prescaler)
{
    uint8_t previous_sreg = SREG;
    cli();
    WDTCSR = (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | prescaler;
    // zamiast sei() -> nie chcemy przypadkowo włączyć przerwania
    SREG = previous_sreg;
}

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

static void format_time(char output[6], uint8_t hours, uint8_t minutes)
{
    output[0] = '0' + hours / 10;
    output[1] = '0' + hours % 10;
    output[2] = ':';
    output[3] = '0' + minutes / 10;
    output[4] = '0' + minutes % 10;
    output[5] = '\0';
}

static void display_default_page(ds1302_time_t now)
{
    oled_write_centered_text("PODLEWACZKA", 0);
    oled_write_centered_text("DLA MLEMIKA <3", 1);
    char temp_time[6];
    format_time(temp_time, now.hours, now.minutes);
    oled_write_centered_text(temp_time, 3);

    if (now.day >= 1 && now.day <= 7) {
        const char *day_name = days.items[now.day - 1];

        oled_write_centered_text(day_name, 5);
    }
}

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
static void check_and_water(void)
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
    //     .minutes = 0,
    //     .hours   = 12,
    //     .date    = 1,
    //     .month   = 1,
    //     .day     = 3,
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
