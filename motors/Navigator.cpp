#include <PID_v1.h>
#include "Common.h"
#include "Navigator.h"
#include "MotorController.h"
#include "ESC.h"
#include "IMU.h"
#include "Eyes.h"

#define LOOK_DURATION 2000
#define TURN_DURATION 1200 

Navigator::Navigator(PlatformMotionController &platform, ESC &esc, IMU &imu, Eyes &eyes)
  : platform(&platform), esc(&esc), imu(&imu), eyes(&eyes), state(Stopped), nextState(Starting), enteredAt(0) {
    difference = 0;
    actualHeading = 0;
    desiredHeading = 0;
    pid = NULL;
    memcpy(&command, &Forward, sizeof(platform_command_t));
}

boolean Navigator::isMoving() {
  return state == Searching;
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

  if (platform->isMoving() && eyes->didChangedState()) {
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
      esc->disable();
      platform->stop();
      eyes->lookNext();
      pause(Looking, LOOK_DURATION);
      break;
    }
    }
  }

  switch (state) {
  case Starting:
    break;
  case Stopped:
    if (entered) {
      DPRINTF("Nav: Stopping\n\r");
      esc->disable();
      platform->stop();
    }
    break;
  case Searching:
    if (entered) {
      DPRINTF("Nav: Searching\n\r");
      esc->configure(150, 150);
      platform->execute(&command);
      actualHeading = desiredHeading = imu->getHeading();
      difference = 0;
      delete pid;
      pid = new PID(&actualHeading, &difference, &desiredHeading, 40, 0, 20, MANUAL);
      pid->SetMode(AUTOMATIC);
    }

    if (millis() - lastCorrectedHeading > 500) {
      actualHeading = imu->getHeading();
      pid->Compute();
      DPRINTF("Nav: %f %f %f\n\r", desiredHeading, actualHeading, difference);
      esc->configure(150 - difference, 150 + difference);
      lastCorrectedHeading = millis();
    }

    break;
  case Waiting:
    if (entered) DPRINTF("Nav: Waiting\n\r");
    if (waitingFor > 0) {
      if (millis() - enteredAt > waitingFor) {
        nextState = nextStateAfterPause;
      }
    }
    else if (!platform->isExecuting()) {
      nextState = nextStateAfterPause;
    }
    break;
  case Looking:
    if (entered) DPRINTF("Nav: Looking\n\r");
    if (eyes->getState() == Eyes::Alert) {
      eyes->lookNext();
      pause(Looking, LOOK_DURATION);
    }
    else {
      if (entered) {
        esc->disable();
        switch (eyes->getLookDirection()) {
        case FORWARD_INDEX:
          break;
        case LEFT_INDEX:
          platform->execute(&Left, TURN_DURATION);
          break;
        case RIGHT_INDEX:
          platform->execute(&Right, TURN_DURATION);
          break;
        }
        eyes->lookForward();
        pauseWhileMoving(Searching);
      }
    }
    break;
  }
}

void Navigator::pause(state_t after, uint16_t time) {
  waitingFor = time;
  nextState = Waiting;
  nextStateAfterPause = after;
}

void Navigator::pauseWhileMoving(state_t after) {
  pause(after, 0);
}
