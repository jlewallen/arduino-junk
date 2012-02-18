#ifndef ESC_H
#define ESC_H

#include <Arduino.h>
#include "Servicable.h"
#include "Debuggable.h"

class PlatformMotionController;

class ESC : public Servicable {
private:
  enum State {
    Running,
    Draining,
    Measuring
  };

  typedef struct {
    byte pin;
    int16_t accumulator;
  } reading_t;

  PlatformMotionController *platform;
  uint32_t previous;
  State state;
  uint16_t samples;
  reading_t leftMotor;
  uint16_t leftTarget;
  reading_t rightMotor;
  uint16_t rightTarget;

public:
  ESC(PlatformMotionController &platform, byte leftPin, byte rightPin);
  void setTargetSpeeds(uint16_t left, uint16_t right);
  void begin();
  void service();
  void adjust();
};

#endif
