#ifndef ESC_H
#define ESC_H

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
  ESC(PlatformMotionController &platform, byte leftPin, byte rightPin) : platform(&platform), previous(0), state(Running) {
    leftMotor.pin = leftPin;
    rightMotor.pin = rightPin;
    setTargetSpeeds(200, 200);
  }

  void begin() {
  }

  void setTargetSpeeds(uint16_t left, uint16_t right) {
    leftTarget = left;
    rightTarget = right;
  }

  void service() {
    uint32_t now = micros();
    switch (state) {
    case Running:
      if (!platform->isMoving()) {
        return;
      }
      if (now - previous > 100000) {
        platform->pause();
        state = Draining;
        previous = now;
      }
      break;
    case Draining:
      if (now - previous > 2000) {
        state = Measuring;
        leftMotor.accumulator = 0;
        rightMotor.accumulator = 0;
        samples = 0;
        previous = now;
      }
      break;
    case Measuring:
      leftMotor.accumulator += analogRead(leftMotor.pin);
      rightMotor.accumulator += analogRead(rightMotor.pin);
      samples++;
      if (now - previous > 2000) {
        adjust();
        previous = now;
      }
      break;
    }
  }

  void adjust() {
    uint16_t leftRate = (int16_t)(leftMotor.accumulator / samples);
    uint16_t rightRate = (int16_t)(rightMotor.accumulator / samples);

    int16_t leftDifference = leftTarget - leftRate;
    int16_t leftSpeed = platform->getLeft()->speed();
    byte newLeftSpeed = constrain(leftSpeed + leftDifference / 2, 0, 255);
    platform->getLeft()->speed(newLeftSpeed);

    int16_t rightDifference = rightTarget - rightRate;
    int16_t rightSpeed = platform->getRight()->speed();
    byte newRightSpeed = constrain(rightSpeed + rightDifference / 2, 0, 255);
    platform->getRight()->speed(newRightSpeed);

    printf("%3d %3d %3d %3d, %3d %3d %3d %3d, %2d\r\n",
      leftRate, leftDifference, leftSpeed, newLeftSpeed,
      rightRate, rightDifference, rightSpeed, newRightSpeed,
      newRightSpeed - newLeftSpeed
    );

    platform->resume();
    state = Running;
  }
};

#endif
