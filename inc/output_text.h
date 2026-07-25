#pragma once

typedef struct {
    const char *items[7];
} DaysOfWeek;

typedef struct {
    char watering[sizeof("Podlewanie...")];
    char currentHumidity[sizeof("Akt. wilgotnosc:")];
    char menu[sizeof("MENU")];
    char wateredToday[sizeof("Dzis podlano:")];
    char brightness[sizeof("Jasnosc")];
    char contrast[sizeof("Kontrast")];
    char waterNow[sizeof("Podlej teraz")];
    char checkHumidity[sizeof("Sprawdz wilg: >")];
    char settings[sizeof("Ustawienia")];
} Keywords;

extern const DaysOfWeek days;
extern const Keywords keywords;
