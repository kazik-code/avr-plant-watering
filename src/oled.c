#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include "i2c_line.h"
#include "oled.h"

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


bool oled_init(void)
{
    if (!i2c_ping_device(0x3C))
        return false;

    send_command_byte(false, false);
    send_byte(OLED_ENABLE_DISPLAY);
    send_byte(OLED_CMD_NORMAL_DISPLAY);
    send_byte(OLED_COMMAND_SET_CONTRAST);
    send_byte(0x7F); // kontrast 0–255
    send_byte(OLED_SET_ADDRESSING_MODE);
    send_byte(OLED_ADDRESSING_HORIZONTAL);
    return true;
}

