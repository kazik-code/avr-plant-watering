#pragma once

#include <stdbool.h>
#include <stdint.h>

void i2c_init(void);
bool send_byte(uint8_t frame);
uint8_t get_byte(bool is_last_byte);
bool i2c_ping_device(uint8_t address);
void start_condition(void);
void stop_condition(void);
uint8_t create_initial_frame(uint8_t address, bool read);
