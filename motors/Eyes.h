#ifndef EYES_H
#define EYES_H

#include <Arduino.h>
#include <Servo.h>
#include "Servicable.h"

class Eyes : public Servicable {
private:
  byte detectorPin;
  byte servoPin;
  Servo servo;
  uint32_t previous;
  uint8_t index;

public:
  Eyes(byte detectorPin, byte servoPin);
  void begin();
  void service();
  boolean hasDetectedObstacle();
};

#endif
