#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>
#include "Servicable.h"
#include "Debuggable.h"

typedef struct {
  boolean ignore;
  boolean enabled;
  boolean direction;
  byte speed;
} motor_command_t;

typedef struct platform_command {
  motor_command_t l;
  motor_command_t r;
  uint16_t duration;
  platform_command* n;
} platform_command_t;

extern platform_command_t Left;
extern platform_command_t Right;
extern platform_command_t Forward;
extern platform_command_t Backward;
extern platform_command_t Stop;

class MotorController {
private:
  boolean enabled;
  boolean forwardDirection;
  byte speedPin;
  byte pin1;
  byte pin2;
  byte value;

public:
  MotorController(byte speedPin, byte pin1, byte pin2);
  boolean isMovingBackward();
  boolean isMovingForward();
  boolean isEnabled();
  void fill(motor_command_t *command);
  void execute(motor_command_t *command);
  void debug(const char *name);
  void begin();
};

class PlatformMotionController : public Servicable {
private:
  platform_command_t active;
  uint32_t commandStartedAt;
  MotorController *l;
  MotorController *r;

public:
  PlatformMotionController(MotorController &l, MotorController &r);
  boolean hasBothMotorsEnabled();
  boolean isMovingBackward();
  boolean isMovingForward();
  boolean isMoving();
  boolean isExecuting();
  void begin();
  void fill(platform_command_t *command);
  void execute(platform_command_t *command);
  void execute(platform_command_t *command, int16_t duration);
  void stop();
  void service();
  void debug();
};

#endif
