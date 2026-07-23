#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include "i2c_line.h"
#include "oled.h"
#include "font_5x7.h"

/*
 * SSD1306 I2C control byte:
 *
 * Co | D/C# | Byte | Meaning
 * ---|------|------|-----------------------------------------------
 *  0 |   0  | 0x00 | Following bytes are commands
 *  0 |   1  | 0x40 | Following bytes are GDDRAM data
 *  1 |   0  | 0x80 | Next byte is a command, then another control byte
 *  1 |   1  | 0xC0 | Next byte is data, then another control byte
 */

static bool send_command_byte(bool continuation, bool is_data)
{
    uint8_t command_frame = 0x00 | (continuation << 7) | (is_data << 6);
    return send_byte(command_frame);
}

bool oled_clear_display(void)
{
    start_condition();
    send_byte(create_initial_frame(0x3C, false));
    send_command_byte(false, false);
    send_byte(OLED_SET_COLUMN_ADDRESS);
    send_byte(0);
    send_byte(127);
    send_byte(OLED_SET_PAGE_ADDRESS);
    send_byte(0);
    send_byte(7);
    stop_condition();

    start_condition();
    send_byte(create_initial_frame(0x3C, false));
    send_command_byte(false, true);
    for (uint16_t i = 0; i < 1024; ++i) {
        send_byte(0x00);
    }
    stop_condition();

    return true;
}

bool oled_init(void)
{
    start_condition();
    send_byte(create_initial_frame(0x3C, false));

    send_command_byte(false, false);
    send_byte(OLED_DISABLE_DISPLAY);
    send_byte(0x8D);
    send_byte(0x14); // włączenie charge pump
    send_byte(0xA1); // odwrócenie kolejności segmentów
    send_byte(0xC8); // odwrócenie kierunku skanowania COM

    
    send_byte(OLED_ENABLE_DISPLAY);
    send_byte(OLED_CMD_NORMAL_DISPLAY);
    send_byte(OLED_COMMAND_SET_CONTRAST);
    send_byte(0xAA); // kontrast 0–255
    send_byte(OLED_SET_ADDRESSING_MODE);
    send_byte(OLED_ADDRESSING_HORIZONTAL);
    stop_condition();

    oled_clear_display();

    return true;
}

bool oled_write_char(char character)
{
    for (uint8_t column = 0; column < FONT_5X7_WIDTH; ++column) {
        uint8_t pixels =
            font_5x7_get_column(character, column);

        if (!send_byte(pixels)) {
            return false;
        }
    }

    // Pusta kolumna jako odstęp poziomy.
    return send_byte(0x00);
}

bool oled_display_test(){
    //if (!i2c_ping_device(0x3C))
        //return false;

    // start_condition();
    // send_byte(create_initial_frame(0x3C, false));
    // send_command_byte(false, false);
    // send_byte(OLED_SET_COLUMN_ADDRESS);
    // send_byte(0);
    // send_byte(127);
    // send_byte(OLED_SET_PAGE_ADDRESS);
    // send_byte(0);
    // send_byte(7);
    // stop_condition();

    start_condition();
    send_byte(create_initial_frame(0x3C, false));
    send_command_byte(false, true);
    oled_write_char('H');
    oled_write_char('e');
    oled_write_char('l');
    oled_write_char('l');
    oled_write_char('o');
    oled_write_char(' ');
    oled_write_char('W');
    oled_write_char('o');
    oled_write_char('r');
    oled_write_char('l');
    oled_write_char('d');
    oled_write_char('!');
    stop_condition();
    return true;
}