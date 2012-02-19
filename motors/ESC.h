#ifndef ESC_H
#define ESC_H

#include <Arduino.h>
#include "Servicable.h"
#include "Debuggable.h"
#include "MotorController.h"

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
    uint16_t target;
  } motor_t;

  PlatformMotionController *platform;
  uint32_t previous;
  State state;
  uint16_t samples;
  motor_t motorA;
  motor_t motorB;
  platform_command_t command;

public:
  ESC(PlatformMotionController &platform, byte pinA, byte pinB);
  void configure(uint16_t a, uint16_t b);
  void begin();
  void service();
  void adjust();
  void disable() {
    configure(0, 0);
  }
};

#endif
