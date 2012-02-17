#include "MotorController.h"

MotorController::MotorController(byte speedPin, byte pin1, byte pin2) :
  enabled(false),
  forwardDirection(false),
  speedPin(speedPin),
  pin1(pin1),
  pin2(pin2),
  value(0) {
}

boolean MotorController::isMovingBackward() {
  return !isMovingForward();
}

boolean MotorController::isMovingForward() {
  return forwardDirection;
}

boolean MotorController::isEnabled() {
  return enabled;
}

void MotorController::forward(byte speed) {
  direction(true);
}

void MotorController::backward(byte speed) {
  direction(false);
}

void MotorController::debug(const char *name) {
  printf("%s %s %s %d\n\r", name, enabled ? "on" : "off", forwardDirection ? "fwd" : "back", value);
}

void MotorController::begin() {
  stop();
  pinMode(speedPin, OUTPUT);
  pinMode(pin1, OUTPUT); 
  pinMode(pin2, OUTPUT); 
}

void MotorController::direction(boolean f) {
  forwardDirection = f;
  if (forwardDirection) {
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
  }
  else {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
  }
}

void MotorController::speed(byte s) {
  speed(enabled, s);
}

void MotorController::speed(boolean e, byte s) {
  enabled = e;
  value = s;
  if (enabled) {
    analogWrite(speedPin, value);
  }
  else {
    analogWrite(speedPin, 0);
  }
}

byte MotorController::speed() {
  return value;
}

void MotorController::start() {
  speed(true, value);
}

void MotorController::toggle() {
  speed(!enabled, value);
}

void MotorController::stop() {
  speed(false, value);
}

void MotorController::disable() {
}

void MotorController::enable() {
}

PlatformMotionController::PlatformMotionController(MotorController &l, MotorController &r) : l(&l), r(&r) {
}

MotorController *PlatformMotionController::getLeft() {
  return l;
}

MotorController *PlatformMotionController::getRight() {
  return r;
}

boolean PlatformMotionController::hasBothMotorsEnabled() {
  return l->isEnabled() && r->isEnabled();
}

boolean PlatformMotionController::isMovingBackward() {
  return hasBothMotorsEnabled() && l->isMovingBackward() && r->isMovingBackward();
}

boolean PlatformMotionController::isMovingForward() {
  return hasBothMotorsEnabled() && l->isMovingForward() && r->isMovingForward();
}

boolean PlatformMotionController::isMoving() {
  return hasBothMotorsEnabled();
}

byte PlatformMotionController::getSpeed() {
  if (l->speed() == r->speed()) {
    return l->speed();
  }
  return 0;
}

void PlatformMotionController::direction(boolean left, boolean right) {
  l->direction(left);
  r->direction(right);
}

void PlatformMotionController::turn(boolean direction, byte s) {
  l->stop();
  r->stop();

  l->direction(direction);
  r->direction(!direction);

  l->speed(false, s);
  r->speed(false, s);

  l->start();
  r->start();
}

void PlatformMotionController::adjust(boolean direction, byte s) {
  l->stop();
  r->stop();

  l->direction(direction);
  r->direction(direction);

  l->speed(false, s);
  r->speed(false, s);

  l->start();
  r->start();
}

void PlatformMotionController::stop() {
  l->stop();
  r->stop();
}

void PlatformMotionController::pause() {
  l->stop();
  r->stop();
}

void PlatformMotionController::resume() {
  l->start();
  r->start();
}

void PlatformMotionController::service() {
}

void PlatformMotionController::begin() {
  l->begin();
  r->begin();
}

void PlatformMotionController::debug() {
  l->debug("L");
  r->debug("R");
}
