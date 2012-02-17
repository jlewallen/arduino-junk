#include <math.h>
#include "Eyes.h"
#include "Navigator.h"

#define fabs(x)                   __builtin_fabs(x)
#define UPDATE_TIME               100
#define LATCH_TIME                100
#define DISTANCE_HYSTERESIS       2
#define DISTANCE_PROXIMITY        10
#define DISTANCE_FROM_VOLTAGE(v)  4192.936 * pow((v), -0.935) - 3.937;

static uint8_t states[] = {
   82,  82, 82, 82,
   50,  50,
   82,  82, 82, 82,
  110, 110
};

Eyes::Eyes(byte detectorPin, byte servoPin, Navigator &navigator)
  : navigator(&navigator), detectorPin(detectorPin), servoPin(servoPin), distancePin(2), state(Starting), previousUpdate(0), previousChange(0), index(0) {
}

void Eyes::begin() {
  servo.attach(servoPin);
  pinMode(detectorPin, INPUT);
}

void Eyes::service() {
  uint32_t now = millis();
  if (now - previousUpdate < UPDATE_TIME) {
    return;
  }
  previousUpdate = now;

  distance = DISTANCE_FROM_VOLTAGE(analogRead(distancePin));
  boolean entered = state != nextState;
  if (entered) {
    previousChange = now;
    previousLog = now;
    state = nextState;
  }

  switch (state) {
  case Starting:
    nextState = Scanning;
    break;
  case Scanning:
    if (entered) printf("Eyes: Scanning\n\r");
    if (distance < DISTANCE_PROXIMITY) {
      obstacleDistance = -1;
      nextState = Found;
    }
    break;
  case Found: {
    if (entered) printf("Eyes: Found\n\r");
    boolean blocked = updateObstacle();
    if (!blocked) {
      nextState = Scanning;
    }
    else if (now - previousChange >= LATCH_TIME) {
      nextState = Alert;
    }
    break;
  }
  case Alert: {
    if (entered) {
      navigator->stop();
      printf("Eyes: Alert\n\r");
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
    if (entered) printf("Eyes: Lost\n\r");
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
  /*
  uint32_t now = millis();
  if (now - previousLog > 100) {
    printf("Eyes: %f %f %f\n\r", obstacleDistance, distance, difference);
    previousLog = now;
  }
  */
  return difference < DISTANCE_HYSTERESIS && distance <= DISTANCE_PROXIMITY + DISTANCE_HYSTERESIS;
}

