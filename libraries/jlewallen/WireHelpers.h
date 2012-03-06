#ifndef WIRE_HELPERS_H
#define WIRE_HELPERS_H

#include <Wire.h>

#define DEFAULT_TWI_TIMEOUT 10

extern uint8_t wireWaitForAvailableBytes(uint8_t bytes, uint16_t time);
extern uint8_t wireReadBlock(void *p, size_t sz, uint16_t time);

#endif
