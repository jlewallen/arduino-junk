#include "MotorController.h"

platform_command_t Right = {
  { false, true, true, 200 },
  { false, true, false, 200 },
  0, NULL
};

platform_command_t Left = {
  { false, true, false, 200 },
  { false, true, true, 200 },
  0, NULL
};

platform_command_t Forward = {
  { false, true, true, 200 },
  { false, true, true, 200 },
  0, NULL
};

platform_command_t Backward = {
  { false, true, false, 200 },
  { false, true, false, 200 },
  0, NULL
};

platform_command_t Stop = {
  { false, false, true, 0 },
  { false, false, true, 0 },
  0, NULL
};

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

void MotorController::debug(const char *name) {
  DPRINTF("%s %s %s %d\n\r", name, enabled ? "on" : "off", forwardDirection ? "fwd" : "back", value);
}

void MotorController::begin() {
  pinMode(speedPin, OUTPUT);
  pinMode(pin1, OUTPUT); 
  pinMode(pin2, OUTPUT); 
}

void MotorController::fill(motor_command_t *command) {
  command->speed = value;
  command->enabled = enabled;
  command->direction = forwardDirection;
  command->ignore = false;
}

void MotorController::execute(motor_command_t *command) {
  if (command->ignore) {
    return;
  }
  forwardDirection = command->direction;
  if (forwardDirection) {
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
  }
  else {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
  }
  enabled = command->enabled;
  value = command->speed;
  if (enabled) {
    analogWrite(speedPin, value);
  }
  else {
    analogWrite(speedPin, 0);
  }
}

PlatformMotionController::PlatformMotionController(MotorController &l, MotorController &r) : l(&l), r(&r) {
}

void PlatformMotionController::fill(platform_command_t *command) {
  command->duration = 0;
  l->fill(&command->l);
  r->fill(&command->r);
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

boolean PlatformMotionController::isExecuting() {
  return active.duration > 0;
}

void PlatformMotionController::stop() {
  execute(&Stop);
}

void PlatformMotionController::service() {
  if (active.duration > 0) {
    if (millis() - commandStartedAt > active.duration) {
      active.duration = 0;
      stop();
    }
  }
}

void PlatformMotionController::begin() {
  l->begin();
  r->begin();
}

void PlatformMotionController::debug() {
  l->debug("L");
  r->debug("R");
}

void PlatformMotionController::execute(platform_command_t *command) {
  execute(command, command->duration);
}

void PlatformMotionController::execute(platform_command_t *command, int16_t duration) {
  commandStartedAt = millis();
  memcpy(&active, command, sizeof(platform_command_t));
  active.duration = duration;
  l->execute(&command->l);
  r->execute(&command->r);
}
