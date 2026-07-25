#include <stdint.h>
#include "oled.h"
#include "output_text.h"
#include "ui.h"

static void format_time(char output[6], uint8_t hours, uint8_t minutes)
{
    output[0] = '0' + hours / 10;
    output[1] = '0' + hours % 10;
    output[2] = ':';
    output[3] = '0' + minutes / 10;
    output[4] = '0' + minutes % 10;
    output[5] = '\0';
}

static void format_date(char output[11], uint8_t date, uint8_t month, uint8_t year)
{
    output[0]  = '0' + date / 10;
    output[1]  = '0' + date % 10;
    output[2]  = '.';
    output[3]  = '0' + month / 10;
    output[4]  = '0' + month % 10;
    output[5]  = '.';
    output[6]  = '2';
    output[7]  = '0';
    output[8]  = '0' + year / 10;
    output[9]  = '0' + year % 10;
    output[10] = '\0';
}

void display_default_page(ds1302_time_t now)
{
    oled_write_centered_text("PODLEWACZKA", 0);
    oled_write_centered_text("DLA MLEMIKA <3", 1);
    char temp_time[6];
    format_time(temp_time, now.hours, now.minutes);
    oled_write_centered_text(temp_time, 4);

    char temp_date[11];
    format_date(temp_date, now.date, now.month, now.year);
    oled_write_centered_text(temp_date, 3);

    if (now.day >= 1 && now.day <= 7) {
        const char *day_name = days.items[now.day - 1];

        oled_write_centered_text(day_name, 2);
    }
}
