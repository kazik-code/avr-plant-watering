#include "i2c_line.h"
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include "led.h"


#define SCL_I2C PC5
#define SDA_I2C PC4
#define OLED_ADDRESS 0x3C

inline static void set_low(uint8_t pin)
{
    DDRC |= (1 << pin);
    PORTC &= ~(1 << pin);
}

inline static void release_high(uint8_t pin)
{
    DDRC &= ~(1 << pin);
    PORTC &= ~(1 << pin);
}

void i2c_init(void)
{
    release_high(SCL_I2C);
    release_high(SDA_I2C);
}

inline static void start_condition(void)
{
    set_low(SDA_I2C);
    _delay_us(1);
    set_low(SCL_I2C);
}

inline static void stop_condition(void)
{
    set_low(SDA_I2C);
    _delay_us(1);
    release_high(SCL_I2C);
    _delay_us(1);
    release_high(SDA_I2C);
}

inline static uint8_t create_initial_frame(uint8_t address, bool read)
{
    uint8_t frame = (address << 1) | read;
    return frame;
}

static bool get_ack(void)
{
    release_high(SDA_I2C);
    release_high(SCL_I2C);
    _delay_us(1);
    bool ack = !(PINC & (1 << SDA_I2C)); // ack == true -> jest ack
    set_low(SCL_I2C);
    return ack;
}

static void send_ack(bool ack)
{
    if (ack) {
        set_low(SDA_I2C);
    } else {
        release_high(SDA_I2C);
    }

    release_high(SCL_I2C);
    _delay_us(1);
    set_low(SCL_I2C);
}

bool send_byte(uint8_t frame)
{
    for (int i = 7; i >= 0; i--) {
        bool temp_bit = frame & (1 << i);

        if (temp_bit) {
            release_high(SDA_I2C);
        } else {
            set_low(SDA_I2C);
        }

        _delay_us(1);
        release_high(SCL_I2C);
        _delay_us(1);
        set_low(SCL_I2C);
    }

    return get_ack();
}
    
uint8_t get_byte(bool is_last_byte)
{
    release_high(SDA_I2C);
    uint8_t received_byte = 0x00;
    for (int i = 7; i >= 0; i--) {
        release_high(SCL_I2C);
        _delay_us(1);

        bool temp_bit = PINC & (1 << SDA_I2C);
        received_byte |= (temp_bit << i);
            
        set_low(SCL_I2C);
        _delay_us(1);
    }

    // jesli to ostatni bajt, to wysyłamy NACK, w przeciwnym wypadku ACK
    send_ack(!is_last_byte);
    return received_byte;
}
   
