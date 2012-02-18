#include <math.h>
#include "Eyes.h"
#include "Navigator.h"

#define fabs(x)                   __builtin_fabs(x)
#define UPDATE_TIME               100
#define LATCH_TIME                100
#define DISTANCE_HYSTERESIS       2
#define DISTANCE_PROXIMITY        10
#define DISTANCE_FROM_VOLTAGE(v)  4192.936 * pow((v), -0.935) - 3.937;

uint8_t Eyes::eyeDirections[] = {
  FORWARD,
  RIGHT,
  LEFT
};

Eyes::Eyes(byte detectorPin, byte servoPin)
  : detectorPin(detectorPin), servoPin(servoPin), distancePin(2), state(Starting), previousUpdate(0), previousChange(0), index(0) {
}

void Eyes::begin() {
  servo.attach(servoPin);
  pinMode(detectorPin, INPUT);
  nextState = Scanning;
  lookForward();
}

boolean Eyes::didChangedState() {
  return changedState;
}

Eyes::state_t Eyes::getState() {
  return state;
}

uint8_t Eyes::getLookDirection() {
  return index;
}

void Eyes::lookForward() {
  servo.write(eyeDirections[index = 0]);
  printf("Eyes: Forward\n\r");
}

void Eyes::lookNext() { 
  index = ((index + 1) % sizeof(eyeDirections));
  servo.write(eyeDirections[index]);
  printf("Eyes: %d\n\r", index);
}

void Eyes::service() {
  changedState = false;

  uint32_t now = millis();
  if (now - previousUpdate < UPDATE_TIME) {
    return;
  }
  previousUpdate = now;

  distance = DISTANCE_FROM_VOLTAGE(analogRead(distancePin));
  changedState = state != nextState;
  if (changedState) {
    previousChange = now;
    state = nextState;
  }

  switch (state) {
  case Starting:
    nextState = Scanning;
    break;
  case Scanning:
    if (changedState) DPRINTF("Eyes: Scanning\n\r");
    if (distance < DISTANCE_PROXIMITY) {
      obstacleDistance = -1;
      nextState = Found;
    }
    break;
  case Found: {
    boolean blocked = updateObstacle();
    if (changedState) DPRINTF("Eyes: Found %f\n\r", obstacleDistance);
    if (!blocked) {
      nextState = Scanning;
    }
    else if (now - previousChange >= LATCH_TIME) {
      nextState = Alert;
    }
    break;
  }
  case Alert: {
    if (changedState) {
      DPRINTF("Eyes: Alert %f\n\r", obstacleDistance);
    }
    boolean blocked = updateObstacle();
    if (!blocked) {
      nextState = Lost;
    }
    else if (now - previousChange >= LATCH_TIME) {
      nextState = Alert;
    }
    break;
  }
  case Lost: {
    if (changedState) DPRINTF("Eyes: Lost\n\r");
    boolean blocked = updateObstacle();
    if (blocked) {
      nextState = Alert;
    }
    else if (now - previousChange >= LATCH_TIME) {
      nextState = Scanning;
    }
    break;
  }
  }
}

boolean Eyes::updateObstacle() {
  if (obstacleDistance < 0 || obstacleDistance > distance) {
    obstacleDistance = distance;
  }
  double difference = fabs(distance - obstacleDistance);
  return difference < DISTANCE_HYSTERESIS && distance <= DISTANCE_PROXIMITY + DISTANCE_HYSTERESIS;
}

