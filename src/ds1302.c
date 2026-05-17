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
#define DAT_IN()   (DDRB  &= ~(1 << DAT_BIT))

/* adresy rejestrów RTC */
#define WRITE_SECONDS 0x80
#define WRITE_MINUTES 0x82
#define WRITE_HOURS   0x84
#define WRITE_DATE    0x86
#define WRITE_MONTH   0x88
#define WRITE_DAY     0x8A
#define WRITE_YEAR    0x8C
#define WRITE_CONTROL 0x8E

#define READ_SECONDS  0x81
#define READ_MINUTES  0x83
#define READ_HOURS    0x85
#define READ_DATE     0x87
#define READ_MONTH    0x89
#define READ_DAY      0x8B
#define READ_YEAR     0x8D

static void write_byte(uint8_t byte)
{
    DAT_OUT();
    for (uint8_t i = 0; i < 8; i++) {
        if (byte & (1 << i)) DAT_HIGH(); else DAT_LOW();
        CLK_HIGH(); // DS1302 próbkuje dane na rosnącym zboczu
        CLK_LOW();
    }
}

static uint8_t read_byte(void)
{
    uint8_t byte = 0;
    DAT_IN();
    DAT_HIGH();
    for (uint8_t i = 0; i < 8; i++) {
        if (PINB & (1 << DAT_BIT)) byte |= (1 << i);
        CLK_HIGH();
        CLK_LOW();
    }
    return byte;
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

static void read_register(uint8_t cmd, uint8_t *data)
{
    RST_LOW();
    CLK_LOW();
    RST_HIGH();
    _delay_us(1);
    write_byte(cmd);
    *data = read_byte();
    RST_LOW();
}

static uint8_t to_bcd(uint8_t val)
{
    return ((val / 10) << 4) | (val % 10);
}

static uint8_t from_bcd(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void ds1302_init(void)
{
    DDRB |= (1 << CLK_BIT) | (1 << RST_BIT);
    RST_LOW();
    CLK_LOW();

    write_register(WRITE_CONTROL, 0x00); // wyczyść WP — bez tego zapis jest zablokowany

    uint8_t dummy;
    read_register(READ_SECONDS, &dummy);
    if (dummy & (1 << 7)) // bit7 = CH (clock halt) — jeśli ustawiony, zegar stoi
        write_register(WRITE_SECONDS, dummy & ~(1 << 7)); // wyczyść CH, uruchom oscylator
}

void ds1302_set_time(const ds1302_time_t *t)
{
    // bit7 rejestru sekund to CH (clock halt) — 0 uruchamia oscylator
    write_register(WRITE_SECONDS, to_bcd(t->seconds));
    write_register(WRITE_MINUTES, to_bcd(t->minutes));
    write_register(WRITE_HOURS,   to_bcd(t->hours));  // bit7=0 → tryb 24h
    write_register(WRITE_DATE,    to_bcd(t->date));
    write_register(WRITE_MONTH,   to_bcd(t->month));
    write_register(WRITE_DAY,     to_bcd(t->day));    // 1–7, wartość bezpośrednia
    write_register(WRITE_YEAR,    to_bcd(t->year));
}

void ds1302_get_time(ds1302_time_t *t)
{
    uint8_t dummy;
    read_register(READ_SECONDS, &dummy);
    t->seconds = from_bcd(dummy & 0x7F); // bit7 to CH, ignorujemy
    read_register(READ_MINUTES, &dummy);
    t->minutes = from_bcd(dummy);
    read_register(READ_HOURS,   &dummy);
    t->hours = from_bcd(dummy);
    read_register(READ_DATE,    &dummy);
    t->date = from_bcd(dummy);
    read_register(READ_MONTH,   &dummy);
    t->month = from_bcd(dummy);
    read_register(READ_DAY,     &dummy);
    t->day = from_bcd(dummy);
    read_register(READ_YEAR,    &dummy);
    t->year = from_bcd(dummy);
}