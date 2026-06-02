#ifndef MOISTURE_SENSOR_H
#define MOISTURE_SENSOR_H

#include <stdint.h>

#define MOISTURE_THRESHOLD_WET   138
#define MOISTURE_THRESHOLD_MOIST 158

typedef enum {
    MOISTURE_WET,
    MOISTURE_MOIST,
    MOISTURE_DRY
} moisture_state_t;

void moisture_sensor_init(void);
void moisture_sensor_enable(void);
void moisture_sensor_disable(void);
uint8_t read_moisture(void);
moisture_state_t moisture_classify(uint8_t raw);

#endif