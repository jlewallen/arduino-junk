#include <Arduino.h>
#include "WireHelpers.h"

uint8_t wireWaitForAvailableBytes(uint8_t bytes, uint16_t time) {
  uint32_t started = millis();
	while (Wire.available() < bytes) {
    if (millis() - started > time) {
      return false;
    }
  } 
  return true;
}

uint8_t wireReadBlock(void *p, size_t sz, uint16_t time) {
  uint8_t *ptr = (uint8_t *)p;
  uint32_t started = millis();
  while ((size_t)(ptr - (uint8_t *)p) < sz) {
    if (Wire.available()) {
      *ptr++ = Wire.read();
    }
    if (millis() - started > time) {
      return false;
    }
  }
  return true;
}


