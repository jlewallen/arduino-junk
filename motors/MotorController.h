#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>
#include "Servicable.h"

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
  void forward(byte speed);
  void backward(byte speed);
  void debug(const char *name);
  void begin();
  void direction(boolean f);
  void speed(byte s);
  void speed(boolean e, byte s);
  byte speed();
  void start();
  void toggle();
  void stop();
  void disable();
  void enable();
};

class PlatformMotionController : public Servicable {
private:
  MotorController *l;
  MotorController *r;

public:
  PlatformMotionController(MotorController &l, MotorController &r);
  MotorController *getLeft();
  MotorController *getRight();
  boolean hasBothMotorsEnabled();
  boolean isMovingBackward();
  boolean isMovingForward();
  boolean isMoving();
  byte getSpeed();
  void direction(boolean left, boolean right);
  void turn(boolean direction, byte s);
  void adjust(boolean direction, byte s);
  void stop();
  void pause();
  void resume();
  void service();
  void begin();
  void debug();
};

#endif
