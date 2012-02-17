#include "Navigator.h"
#include "MotorController.h"
#include "ESC.h"
#include "IMU.h"
#include <PID_v1.h>

Navigator::Navigator(PlatformMotionController &platform, ESC &esc, IMU &imu)
  : platform(&platform), esc(&esc), imu(&imu), state(Stopped), nextState(Starting), enteredAt(0) {
    difference = 0;
    actualHeading = 0;
    desiredHeading = 0;
    pid = new PID(&actualHeading, &difference, &desiredHeading, 2, 5, 1, DIRECT);
    pid->SetMode(AUTOMATIC);
}

boolean Navigator::isMoving() {
  return state == Searching || state == Avoiding;
}

void Navigator::begin() {
}

void Navigator::search() {
  nextState = Searching;
}

void Navigator::stop() {
  nextState = Stopped;
}

void Navigator::service() {
  boolean entered = nextState != state;
  if (entered) {
    enteredAt = millis();
  }
  state = nextState;

  switch (state) {
  case Starting:
    break;
  case Stopped:
    if (entered) {
      printf("Nav: Stopping\n\r");
      platform->stop();
    }
    break;
  case Searching:
    if (entered) {
      printf("Nav: Searching\n\r");
      esc->setTargetSpeeds(150, 150);
      platform->direction(true, true);
      platform->adjust(true, 200);
      platform->resume();
      desiredHeading = imu->getHeading();
    }

    if (millis() - debugged > 500) {
      actualHeading = imu->getHeading();
      pid->Compute();
      printf("Nav: %f %f %f\n\r", desiredHeading, actualHeading, difference);
      // esc->setTargetSpeeds(150 + difference, 150 - difference);
      debugged = millis();
    }

    break;
  case Avoiding:
    if (entered) {
      printf("Nav: Avoiding\n\r");
      platform->direction(true, false);
      platform->resume();
      esc->setTargetSpeeds(70, 100);
    }
    break;
  case Clearing:
    if (entered) {
      printf("Nav: Avoiding\n\r");
      platform->direction(true, false);
      platform->resume();
      esc->setTargetSpeeds(70, 100);
    }
    if (millis() - enteredAt > 1000) {
      nextState = Searching;
    }
    break;
  }
}

void Navigator::turn() {
}

