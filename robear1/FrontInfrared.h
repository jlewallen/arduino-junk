#ifndef FRONT_INFRARED_H
#define FRONT_INFRARED_H

#define PIN_LEFT   13
#define PIN_CENTER 12
#define PIN_RIGHT  11

class Obstructions : public Servicable {
public:
  Obstructions() {
  }

  void begin() {
    pinMode(PIN_LEFT, INPUT);
    pinMode(PIN_CENTER, INPUT);
    pinMode(PIN_RIGHT, INPUT);
  }

  uint8_t isCenterOrBothBlocked() {
    return !digitalRead(PIN_CENTER) || (!digitalRead(PIN_LEFT) && !digitalRead(PIN_RIGHT));
  }

  uint8_t isLeftBlocked() {
    return !digitalRead(PIN_LEFT);
  }

  uint8_t isRightBlocked() {
    return !digitalRead(PIN_RIGHT);
  }

  uint8_t any() {
    return !digitalRead(PIN_CENTER) || !digitalRead(PIN_LEFT) || !digitalRead(PIN_RIGHT);
  }

  void service() {
  }
};

#endif
