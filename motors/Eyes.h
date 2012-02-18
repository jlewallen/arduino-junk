#ifndef EYES_H
#define EYES_H

#include <Arduino.h>
#include <Servo.h>
#include "Servicable.h"
#include "Debuggable.h"

#define FORWARD_INDEX             0
#define RIGHT_INDEX               1
#define LEFT_INDEX                2

#define OFFSET                    4
#define FORWARD                   (90 + OFFSET)
#define RIGHT                     (30 + OFFSET)
#define LEFT                      (130 + OFFSET)

class Eyes : public Servicable {
public:
  static uint8_t eyeDirections[];

  typedef enum {
    Starting,
    Scanning,
    Found,
    Lost,
    Alert
  } state_t;

private:
  byte detectorPin;
  byte servoPin;
  byte distancePin;
  state_t state;
  state_t nextState;
  boolean changedState;
  Servo servo;
  uint32_t previousUpdate;
  uint32_t previousChange;
  uint8_t looking;
  uint8_t index;
  float distance;
  float obstacleDistance;

private:
  boolean updateObstacle();

public:
  boolean didChangedState();
  state_t getState();
  uint8_t getLookDirection();

public:
  Eyes(byte detectorPin, byte servoPin);
  void begin();
  void service();
  void lookForward();
  void lookNext();
};

#endif
