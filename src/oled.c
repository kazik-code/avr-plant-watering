#include <avr/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/delay.h>
#include "i2c_line.h"
#include "oled.h"
#include "font_5x7.h"

#define OLED_I2C_ADDRESS       0x3C
#define OLED_WIDTH             128
#define OLED_PAGE_COUNT        8
#define FONT_5X7_CELL_WIDTH    (FONT_5X7_WIDTH + 1)
#define OLED_MAX_TEXT_LENGTH   (OLED_WIDTH / FONT_5X7_CELL_WIDTH)

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

bool oled_write_centered_text(const char *text, uint8_t page)
{
    if (text == NULL || page >= OLED_PAGE_COUNT) {
        return false;
    }

    uint8_t text_length = 0;

    while (text[text_length] != '\0') {
        if (text_length == OLED_MAX_TEXT_LENGTH) {
            return false;
        }
        ++text_length;
    }

    /*
     * Ostatni znak nie potrzebuje odstępu do obliczenia wizualnej
     * szerokości tekstu. oled_write_char() nadal wysyła tę pustą kolumnę,
     * dzięki czemu reszta strony może zostać wyczyszczona jednym przebiegiem.
     */
    uint8_t text_width = 0;
    if (text_length > 0) {
        text_width = text_length * FONT_5X7_CELL_WIDTH - 1U;
    }

    uint8_t left_padding = (OLED_WIDTH - text_width) / 2U;

    start_condition();
    bool success =
        send_byte(create_initial_frame(OLED_I2C_ADDRESS, false)) &&
        send_command_byte(false, false) &&
        send_byte(OLED_SET_COLUMN_ADDRESS) &&
        send_byte(0) &&
        send_byte(OLED_WIDTH - 1U) &&
        send_byte(OLED_SET_PAGE_ADDRESS) &&
        send_byte(page) &&
        send_byte(page);
    stop_condition();

    if (!success) {
        return false;
    }

    start_condition();
    success =
        send_byte(create_initial_frame(OLED_I2C_ADDRESS, false)) &&
        send_command_byte(false, true);

    uint8_t bytes_written = 0;

    while (success && bytes_written < left_padding) {
        success = send_byte(0x00);
        ++bytes_written;
    }

    for (uint8_t i = 0; success && i < text_length; ++i) {
        success = oled_write_char(text[i]);
        bytes_written += FONT_5X7_CELL_WIDTH;
    }

    while (success && bytes_written < OLED_WIDTH) {
        success = send_byte(0x00);
        ++bytes_written;
    }

    stop_condition();
    return success;
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
