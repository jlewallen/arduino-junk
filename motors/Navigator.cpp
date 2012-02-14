#include "Navigator.h"
#include "MotorController.h"
#include "ESC.h"
#include "Eyes.h"

Navigator::Navigator(PlatformMotionController &platform, ESC &esc, Eyes &eyes)
  : platform(&platform), esc(&esc), eyes(&eyes), state(Stopped), nextState(Starting), enteredAt(0) {
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
      printf("Nav: Stopping...\n\r");
      platform->stop();
    }
    break;
  case Searching:
    if (entered) {
      printf("Nav: Searching...\n\r");
      platform->direction(true, true);
      platform->resume();
      esc->setTargetSpeeds(150, 150);
    }
    if (eyes->hasDetectedObstacle()) {
      printf("Obstacle!\n\r");
      nextState = Avoiding;
    }
    break;
  case Avoiding:
    if (entered) {
      printf("Nav: Avoiding...\n\r");
      platform->direction(true, false);
      platform->resume();
      esc->setTargetSpeeds(70, 100);
    }
    if (!eyes->hasDetectedObstacle()) {
      printf("Clearing...\n\r");
      nextState = Clearing;
    }
    break;
  case Clearing:
    if (entered) {
      printf("Nav: Avoiding...\n\r");
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

