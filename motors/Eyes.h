#ifndef EYES_H
#define EYES_H

#include <Arduino.h>
#include <Servo.h>
#include "Servicable.h"

class Navigator;

class Eyes : public Servicable {
private:
  typedef enum {
    Starting,
    Scanning,
    Found,
    Lost,
    Alert
  } state_t;

  Navigator *navigator;
  byte detectorPin;
  byte servoPin;
  byte distancePin;
  state_t state;
  state_t nextState;
  Servo servo;
  uint32_t previousUpdate;
  uint32_t previousLog;
  uint32_t previousChange;
  uint8_t index;
  float distance;
  float obstacleDistance;

private:
  boolean updateObstacle();

public:
  Eyes(byte detectorPin, byte servoPin, Navigator &navigator);
  void begin();
  void service();
};

#endif
