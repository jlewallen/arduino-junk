#include "crc8.h"

// CRC-8-CCIT x^8 + x^2 + x + 1

#define POLYNOMIAL    (0x1070U << 3) 

uint8_t crc8_update_non_avr(uint8_t crc, uint8_t data) {
  int16_t updated = (crc ^ data) << 8;
  
  for (uint8_t i = 0; i < 8; ++i) {
    if ((updated & 0x8000 ) != 0) {
      updated = updated ^ POLYNOMIAL;
    }
    updated = updated << 1;
  }
  return (uint8_t)(updated >> 8);
}

uint8_t crc8_update(uint8_t crc, uint8_t data) {
  uint8_t updated = crc ^ data;
  for (uint8_t i = 0; i < 8; ++i) {
    if ((updated & 0x80 ) != 0) {
      updated <<= 1;
      updated ^= 0x07;
    }
    else {
      updated <<= 1;
    }
  }
  return updated;
}

uint8_t crc8_block(uint8_t crc, uint8_t *data, uint8_t sz) {
  while (sz > 0) {
    crc = crc8_update(crc, *data++);
    sz--;
  }
  return crc;
}
