#pragma once
#include <stdint.h>
#include <stdbool.h>

bool oled_init(void);
bool oled_display_test(void);
bool oled_clear_display(void);
bool oled_write_centered_text(const char *text, uint8_t page);


typedef enum {
    OLED_ENABLE_DISPLAY         = 0xAF,
    OLED_DISABLE_DISPLAY        = 0xAE,
    OLED_CMD_NORMAL_DISPLAY     = 0xA6,
    OLED_CMD_INVERSE_DISPLAY    = 0xA7,
    OLED_COMMAND_SET_CONTRAST   = 0x81,
    OLED_ENTIRE_DISPLAY_ON      = 0xA5,
    OLED_DISPLAY_CONTENT        = 0xA4,
} oled_fundamental_command_t;

typedef enum {
    OLED_SET_ADDRESSING_MODE    = 0x20,
    OLED_SET_COLUMN_ADDRESS     = 0x21,
    OLED_SET_PAGE_ADDRESS       = 0x22,
} oled_addressing_commad_t;

typedef enum {
    OLED_ADDRESSING_HORIZONTAL = 0x00,
    OLED_ADDRESSING_VERTICAL   = 0x01,
    OLED_ADDRESSING_PAGE       = 0x02,
} oled_addressing_mode_t;
