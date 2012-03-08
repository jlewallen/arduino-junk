#ifndef CRC8_H
#define CRC8_H

#include <Arduino.h>

uint8_t crc8_update_non_avr(uint8_t crc, uint8_t data);
uint8_t crc8_update(uint8_t crc, uint8_t data);
uint8_t crc8_block(uint8_t crc, uint8_t *data, uint8_t sz);

#endif
