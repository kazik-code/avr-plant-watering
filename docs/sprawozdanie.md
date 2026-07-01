# Sprawozdanie — Autonomiczna podlewaczka roślin

## Opis projektu

Projekt realizuje autonomiczny system podlewania roślin oparty na mikrokontrolerze ATmega328PB.
Urządzenie cyklicznie mierzy wilgotność gleby i w razie potrzeby uruchamia pompę wodną.
Przez większość czasu procesor pozostaje w trybie głębokiego snu (`SLEEP_MODE_PWR_DOWN`),
co minimalizuje pobór prądu.

## Działanie systemu

Co ~8 sekund watchdog timer wybudza procesor. Układ sprawdza zegar RTC (DS1302) —
jeśli minęło 15 minut od ostatniego pomiaru, odczytuje wilgotność gleby przez przetwornik ADC
(czujnik rezystancyjny na PC0). Wynik jest klasyfikowany na trzy stany: mokro, średnio, sucho.
Stan sygnalizowany jest diodą LED (zielona / żółta / czerwona na PD5–PD7). Przy stanie „sucho"
uruchamiana jest pompa (~16 s, sterowana tranzystorem przez PB3).
Dodatkowo przycisk podłączony do INT0 (PD2) pozwala wymusić natychmiastowy pomiar poza harmonogramem.

## Wykorzystane mechanizmy

- **Przerwanie WDT** (`WDT_vect`) — cykliczne wybudzanie procesora ze snu
- **Przerwanie zewnętrzne INT0** (`INT0_vect`) — obsługa przycisku, wyzwalanie poziomem niskim
- **ADC** — odczyt napięcia z czujnika wilgotności (kanał ADC0, prescaler 128 → 125 kHz)
- **Zegar RTC DS1302** — dokładny pomiar czasu, komunikacja bit-bang po 3 liniach (PB0–PB2)
- **GPIO** — sterowanie pompą, diodami LED, czujnikiem wilgotności
- **Tryb power-down** — mikroprocesor śpi między pomiarami, piny wyjściowe utrzymują stan

## Peryferia i piny

| Pin | Funkcja |
|-----|---------|
| PB0 / PB1 / PB2 | RTC DS1302 (CLK / DAT / RST) |
| PB5 | Bramka tranzystora pompy |
| PC0 | Czujnik wilgotności (ADC0) |
| PC1 | Zasilanie czujnika (wyłączane między pomiarami) |
| PD2 | Przycisk (INT0) |
| PD5 / PD6 / PD7 | Diody LED: zielona / żółta / czerwona |
