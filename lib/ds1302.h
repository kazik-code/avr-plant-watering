#pragma once
#include <stdint.h>

typedef struct {
    uint8_t seconds; // 0–59
    uint8_t minutes; // 0–59
    uint8_t hours;   // 0–23
    uint8_t date;    // 1–31
    uint8_t month;   // 1–12
    uint8_t day;     // 1–7
    uint8_t year;    // 0–99
} ds1302_time_t;

void ds1302_init(void);
void ds1302_set_time(const ds1302_time_t *t);
void ds1302_get_time(ds1302_time_t *t);