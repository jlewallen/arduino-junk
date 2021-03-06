#ifndef ENCODING_H
#define ENCODING_H

namespace encoding {

volatile uint8_t leftState = false;
volatile uint8_t rightState = false;
volatile uint16_t leftCounter = 0;
volatile uint16_t rightCounter = 0;

#define ENCODER_0_PIN        2
#define ENCODER_1_PIN        3
#define ENCODER_0_INTERRUPT  0
#define ENCODER_1_INTERRUPT  1

static void leftChange() {
  uint8_t value = digitalRead(ENCODER_1_PIN);
  if (value != leftState) {
    leftCounter++;
    leftState = value;
  }
}

static void rightChange() {
  uint8_t value = digitalRead(ENCODER_0_PIN);
  if (value != rightState) {
    rightCounter++;
    rightState = value;
  }
}

class Encoders : public Servicable {
private:
  MotionController *motion;
  int16_t leftVelocity;
  int16_t rightVelocity;
  int32_t leftTotal;
  int32_t rightTotal;
  int32_t previous;

public:
  Encoders(MotionController &motion) : motion(&motion) {
    leftVelocity = 0;
    rightVelocity = 0;
    leftTotal = 0;
    rightTotal = 0;
    previous = 0;
  }

  int16_t getLeftVelocity() {
    return leftVelocity;
  }

  int16_t getRightVelocity() {
    return rightVelocity;
  }

  int32_t getLeftCounterTotal() {
    return leftTotal;
  }

  int32_t getRightCounterTotal() {
    return rightTotal;
  }

public:
  void begin() {
    attachInterrupt(ENCODER_0_INTERRUPT, rightChange, CHANGE);
    attachInterrupt(ENCODER_1_INTERRUPT, leftChange, CHANGE);
    pinMode(ENCODER_0_PIN, INPUT);
    pinMode(ENCODER_1_PIN, INPUT);
    digitalWrite(ENCODER_0_PIN, HIGH);
    digitalWrite(ENCODER_1_PIN, HIGH);
  }

  void service() {
    noInterrupts();
    leftVelocity = leftCounter * motion->getLeftSign();
    leftTotal += leftCounter;
    leftCounter = 0;
    rightVelocity = rightCounter * motion->getRightSign();
    rightTotal += rightCounter;
    rightCounter = 0;
    interrupts();
  }
};

}

#endif
