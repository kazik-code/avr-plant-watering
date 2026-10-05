# AVR Plant Watering Controller

A bare-metal C project for an ATmega168 that measures soil moisture and waters a plant when the soil is dry. A DS1302 real-time clock provides the schedule, an SSD1306 OLED shows the date and time, and the microcontroller sleeps between checks. The firmware uses AVR registers and interrupts directly; it does not depend on the Arduino framework.

The assembled prototype has been tested with the RTC, moisture sensor, OLED, manual-check button, status LEDs, and automatic pump control.

## Features

- **Scheduled watering:** check soil moisture every 15 minutes and run the pump only when the reading is classified as dry.
- **Manual check:** use the button to take an additional measurement without waiting for the next scheduled check.
- **Visible feedback:** show date, time, and day of the week on the OLED; use three LEDs to indicate wet, moist, or dry soil; display a message during watering.
- **Sleep between checks:** use watchdog interrupts to wake the ATmega168 from power-down sleep, then consult the RTC to decide whether a measurement is due.

## How a watering cycle works

1. A watchdog interrupt or button press wakes the microcontroller.
2. The firmware reads the DS1302 and updates the OLED. On scheduled wake-ups, it compares the RTC time with the previous scheduled check, including the transition through midnight.
3. When a check is due, it enables the moisture sensor, takes 16 ADC readings, averages them, and classifies the result as wet, moist, or dry.
4. It updates the status LEDs. If the soil is dry, it switches on the pump for approximately 16 seconds, then switches it off.
5. The microcontroller returns to power-down sleep.

## Implementation highlights

- **DS1302 driver:** the RTC uses its own three-wire interface, implemented by driving ATmega168 GPIO pins directly. The driver handles register access, BCD conversion, and the clock-halt bit.
- **Software I2C and OLED rendering:** the display uses a GPIO-based I2C implementation. The SSD1306 driver writes text directly to display memory using a 5 × 7 font kept in AVR program memory.
- **Two time scales:** watchdog interrupts wake the CPU roughly every 8 seconds, while the RTC determines the 15-minute measurement schedule. During watering, the watchdog is reconfigured for roughly 500 ms ticks.
- **ADC measurement:** the sensor is enabled for a reading, allowed to settle, and sampled 16 times before the result is classified. Thresholds are kept in one header for calibration.
- **Pump startup state:** the output latch is set LOW before the pump-control pin becomes an output, avoiding an unintended startup pulse.

The firmware separates hardware drivers in `src/` from the main scheduling and watering logic in `src/main.c` and `src/watering.c`.

## Hardware and connections

The prototype uses an ATmega168, DS1302 RTC, analog soil-moisture sensor, I2C OLED, DC pump with a MOSFET switch, three status LEDs, and a manual-check button.

### Pinout

Pin numbers refer to the 32-pin ATmega168-20M package used in the project. The signal assignments come from the firmware.

| Peripheral | Signal | ATmega168 port | Pin |
| --- | --- | --- | ---: |
| DS1302 RTC | CLK | PB0 | 12 |
| DS1302 RTC | DAT | PB1 | 13 |
| DS1302 RTC | CE / RST | PB2 | 14 |
| Pump switch | Control, HIGH = on | PB4 | 16 |
| Moisture sensor | Analog output (ADC0) | PC0 | 23 |
| Moisture sensor | Enable, LOW = on | PC1 | 24 |
| OLED | SDA | PC4 | 27 |
| OLED | SCL | PC5 | 28 |
| Manual check button | INT0, active LOW | PD2 | 32 |
| Wet indicator | LED output | PD5 | 9 |
| Moist indicator | LED output | PD6 | 10 |
| Dry indicator | LED output | PD7 | 11 |

PD4 (pin 2) is configured as an additional LED output, but is not currently used by the application.

## Build and flash

Install `make`, `avr-gcc`, `avr-objcopy`, and `avrdude`, then build the firmware:

```sh
make
```

This produces `plant-watering.elf` and `plant-watering.hex`. The Makefile targets an **ATmega168 at 16 MHz**. Its flash command is configured for an Arduino-compatible serial bootloader at **19200 baud**:

```sh
make flash PORT=/dev/cu.usbserial-XXXX
```

Replace the port with the one used by your programmer. On macOS, the Makefile also tries to find a matching `/dev/cu.usbmodem*` or `/dev/cu.usbserial*` port automatically. Use `make clean` to remove build outputs.

The DS1302 time can be set through `ds1302_set_time()` in `src/ds1302.c`. There is currently no time-setting menu, so initial setup requires a temporary call from the firmware.

## Configuration

| Setting | Current value | Defined in |
| --- | --- | --- |
| Interval between scheduled checks | 15 minutes | `CHECK_INTERVAL_MIN` in `src/main.c` |
| ADC thresholds | Wet: below 138; moist: 138–157; dry: 158 and above | `inc/moisture_sensor.h` |
| Pump duration | 32 watchdog ticks of roughly 500 ms (about 16 seconds) | `PUMP_TICKS` in `src/watering.c` |
| Microcontroller clock | 16 MHz | `F_CPU` in `Makefile` |

The moisture thresholds depend on the particular sensor, soil, and supply voltage. Calibrate them for the assembled device before relying on automatic watering.

## Project status

The measurement, RTC scheduling, OLED display, button, LED indicators, and automatic watering have been exercised on the assembled prototype. The next engineering step is to add protection against a faulty or disconnected sensor and an independent limit on pump operation. The current firmware also has no button debounce or water-level feedback.
