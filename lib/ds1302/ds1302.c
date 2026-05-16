#include "ds1302.h"
#include <avr/io.h>
#include <util/delay.h>

#define CLK_BIT PB0
#define DAT_BIT PB1
#define RST_BIT PB2

#define CLK_HIGH() (PORTB |=  (1 << CLK_BIT))
#define CLK_LOW()  (PORTB &= ~(1 << CLK_BIT))
#define DAT_HIGH() (PORTB |=  (1 << DAT_BIT))
#define DAT_LOW()  (PORTB &= ~(1 << DAT_BIT))
#define RST_HIGH() (PORTB |=  (1 << RST_BIT))
#define RST_LOW()  (PORTB &= ~(1 << RST_BIT))
#define DAT_OUT()  (DDRB  |=  (1 << DAT_BIT))

/* adresy zapisu rejestrów RTC (bit0=0 → write) */
#define REG_SECONDS 0x80
#define REG_MINUTES 0x82
#define REG_HOURS   0x84
#define REG_DATE    0x86
#define REG_MONTH   0x88
#define REG_DAY     0x8A
#define REG_YEAR    0x8C
#define REG_CONTROL 0x8E


void ds1302_init(void)
{
    DDRB |= (1 << CLK_BIT) | (1 << DAT_BIT) | (1 << RST_BIT);
    RST_LOW();
    CLK_LOW();

    write_register(REG_CONTROL, 0x00); // wyczyść WP — bez tego zapis jest zablokowany
}

static uint8_t to_bcd(uint8_t val)
{
    return ((val / 10) << 4) | (val % 10);
}

static void write_byte(uint8_t byte)
{
    DAT_OUT();
    for (uint8_t i = 0; i < 8; i++) {
        if (byte & (1 << i)) DAT_HIGH(); else DAT_LOW();
        CLK_HIGH(); // DS1302 próbkuje dane na rosnącym zboczu
        CLK_LOW();
    }
}

static void write_register(uint8_t cmd, uint8_t data)
{
    RST_LOW();
    CLK_LOW();
    RST_HIGH();
    _delay_us(1); 
    write_byte(cmd);
    write_byte(data);
    RST_LOW();
}

void ds1302_set_time(const ds1302_time_t *t)
{
    // bit7 rejestru sekund to CH (clock halt) — 0 uruchamia oscylator
    write_register(REG_SECONDS, to_bcd(t->seconds) & 0x7F);
    write_register(REG_MINUTES, to_bcd(t->minutes));
    write_register(REG_HOURS,   to_bcd(t->hours));  // bit7=0 → tryb 24h
    write_register(REG_DATE,    to_bcd(t->date));
    write_register(REG_MONTH,   to_bcd(t->month));
    write_register(REG_DAY,     t->day);             // 1–7, wartość bezpośrednia
    write_register(REG_YEAR,    to_bcd(t->year));
}
