typedef struct {
    const char *items[7];
} DaysOfWeek;

const DaysOfWeek days = {
    .items = {
        "Poniedzialek",
        "Wtorek",
        "Sroda",
        "Czwartek",
        "Piatek",
        "Sobota",
        "Niedziela"
    }
};

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

const Keywords keywords = {
    .watering        = "Podlewanie...",
    .currentHumidity = "Akt. wilgotnosc:",
    .menu             = "MENU",
    .wateredToday     = "Dzis podlano:",
    .brightness       = "Jasnosc",
    .contrast         = "Kontrast",
    .waterNow         = "Podlej teraz",
    .checkHumidity    = "Sprawdz wilg: >",
    .settings         = "Ustawienia"
};