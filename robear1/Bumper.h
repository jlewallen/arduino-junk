#ifndef BUMPER_H
#define BUMPER_H

#define BUMPER_PIN_RIGHT  12
#define BUMPER_PIN_LEFT   11

#include "ObstructionSensor.h"

class BumperSensor : public ObstructionSensor {
private:
public:
  BumperSensor() {
  }

  void begin() {
    pinMode(BUMPER_PIN_LEFT, INPUT);
    pinMode(BUMPER_PIN_RIGHT, INPUT);
  }

  uint8_t isCenterOrBothBlocked() {
    return !digitalRead(BUMPER_PIN_LEFT) && !digitalRead(BUMPER_PIN_RIGHT);
  }

  uint8_t isLeftBlocked() {
    return !digitalRead(BUMPER_PIN_LEFT);
  }

  uint8_t isRightBlocked() {
    return !digitalRead(BUMPER_PIN_RIGHT);
  }

  void service() {
  }
};

#endif
