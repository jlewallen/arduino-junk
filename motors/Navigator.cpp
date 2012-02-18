#include <PID_v1.h>
#include "Navigator.h"
#include "MotorController.h"
#include "ESC.h"
#include "IMU.h"
#include "Eyes.h"

uint8_t Eyes::eyeDirections[] = {
  FORWARD,
  RIGHT,
  LEFT
};

Navigator::Navigator(PlatformMotionController &platform, ESC &esc, IMU &imu, Eyes &eyes)
  : platform(&platform), esc(&esc), imu(&imu), eyes(&eyes), state(Stopped), nextState(Starting), enteredAt(0) {
    difference = 0;
    actualHeading = 0;
    desiredHeading = 0;
    pid = new PID(&actualHeading, &difference, &desiredHeading, 40, 0, 20, DIRECT);
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

  if (eyes->didChangedState()) {
    switch (eyes->getState()) {
    case Eyes::Starting: {
      break;
    }
    case Eyes::Scanning: {
      break;
    }
    case Eyes::Found: {
      break;
    }
    case Eyes::Lost: {
      break;
    }
    case Eyes::Alert: {
      eyes->lookNext();
      waitingFor = 2000;
      nextState = Waiting;
      break;
    }
    }
  }

  switch (state) {
  case Starting:
    break;
  case Stopped:
    if (entered) DPRINTF("Nav: Stopping\n\r");
    platform->stop();
    break;
  case Searching:
    if (entered) {
      DPRINTF("Nav: Searching\n\r");
      esc->setTargetSpeeds(150, 150);
      platform->direction(true, true);
      platform->resume();
      desiredHeading = imu->getHeading();
    }

    if (millis() - debugged > 500) {
      actualHeading = imu->getHeading();
      pid->Compute();
      DPRINTF("Nav: %f %f %f\n\r", desiredHeading, actualHeading, difference);
      esc->setTargetSpeeds(150 - difference, 150 + difference);
      debugged = millis();
    }

    break;
  case Avoiding:
    if (entered) {
      DPRINTF("Nav: Avoiding\n\r");
      platform->direction(true, false);
      platform->resume();
      esc->setTargetSpeeds(70, 100);
    }
    break;
  case Waiting:
    if (entered) DPRINTF("Nav: Waiting\n\r");
    if (millis() - enteredAt > waitingFor) {
      nextState = Looking;
    }
    break;
  case Looking:
    if (entered) DPRINTF("Nav: Looking\n\r");
    if (eyes->getState() == Eyes::Alert) {
      eyes->lookNext();
      waitingFor = 2000;
      nextState = Waiting;
    }
    else {
      if (entered) {
        DPRINTF("Go!\n\r");
      }
    }
    break;
  case Clearing:
    if (entered) {
      DPRINTF("Nav: Avoiding\n\r");
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

