#ifndef LED_H
#define LED_H

#define LED_ACK_PING PD4

#include "moisture_sensor.h"

void led_init(void);
void led_set_state(moisture_state_t state);

#endif
