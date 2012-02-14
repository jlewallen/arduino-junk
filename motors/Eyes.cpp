#include "Eyes.h"

static uint8_t states[] = {
   82,  82, 82, 82,
   50,  50,
   82,  82, 82, 82,
  110, 110
};

Eyes::Eyes(byte detectorPin, byte servoPin)
  : detectorPin(detectorPin), servoPin(servoPin), previous(0), index(0) {
}

void Eyes::begin() {
  servo.attach(servoPin);
  pinMode(detectorPin, INPUT);
}

void Eyes::service() {
  uint32_t now = millis();
  uint32_t diff = now - previous;
  if (diff > 250 && !hasDetectedObstacle()) {
    // index++;
    index = index % (sizeof(states) / sizeof(uint8_t));
    servo.write(states[index]);
    previous = now;
  }
}

boolean Eyes::hasDetectedObstacle() {
  return digitalRead(detectorPin) == LOW;
}
