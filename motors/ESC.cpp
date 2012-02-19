#include "ESC.h"
#include "MotorController.h"

ESC::ESC(PlatformMotionController &platform, byte pinA, byte pinB) : platform(&platform), previous(0), state(Running) {
  memzero(&motorA, sizeof(motor_t));
  memzero(&motorB, sizeof(motor_t));
  motorA.pin = pinA;
  motorB.pin = pinB;
}

void ESC::begin() {
}

void ESC::configure(uint16_t a, uint16_t b) {
  motorA.target = a;
  motorB.target = b;
}

void ESC::service() {
  if (motorA.target == 0 && motorB.target == 0) {
    return;
  }

  uint32_t now = micros();
  switch (state) {
  case Running:
    if (!platform->isMoving()) {
      return;
    }
    if (now - previous > 100000) {
      platform->fill(&command);
      platform->stop();
      state = Draining;
      previous = now;
    }
    break;
  case Draining:
    if (now - previous > 2000) {
      state = Measuring;
      motorA.accumulator = 0;
      motorB.accumulator = 0;
      samples = 0;
      previous = now;
    }
    break;
  case Measuring:
    motorA.accumulator += analogRead(motorA.pin);
    motorB.accumulator += analogRead(motorB.pin);
    samples++;
    if (now - previous > 2000) {
      adjust();
      previous = now;
    }
    break;
  }
}

void ESC::adjust() {
  uint16_t leftRate = (int16_t)(motorA.accumulator / samples);
  int16_t leftDifference = motorA.target - leftRate;
  byte newLeftSpeed = constrain(command.l.speed + leftDifference / 2, 0, 255);

  uint16_t rightRate = (int16_t)(motorB.accumulator / samples);
  int16_t rightDifference = motorB.target - rightRate;
  byte newRightSpeed = constrain(command.r.speed + rightDifference / 2, 0, 255);

  command.l.speed = newLeftSpeed;
  command.r.speed = newRightSpeed;
  platform->execute(&command);

  #ifdef ESC_LOGGING
  DPRINTF("%3d %3d %3d, %3d %3d %3d, %2d\r\n",
    leftRate, leftDifference, newLeftSpeed,
    rightRate, rightDifference, newRightSpeed,
    newRightSpeed - newLeftSpeed
  );
  #endif

  platform->execute(&command);
  state = Running;
}

