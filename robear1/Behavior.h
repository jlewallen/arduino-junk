#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include <Servicable.h>
#include "FrontInfrared.h"
#include "VirtualBumper.h"
#include "Bumper.h"

namespace behaviors {

typedef struct {
  int8_t enabled;
  int8_t velocity;
  int8_t rotation;
} behavior_command_t;

class Behavior : public Servicable {
protected:
  behavior_command_t command;

public:
  virtual ~Behavior() {
  }
  behavior_command_t *getCommand() {
    return &command;
  }
};

class BumperBehavior : public Behavior {
private:
  typedef enum {
    Inactive,
    Reverse,
    Left,
    Right,
    Stopped,
    Forward,
  } state_t;
  
  ObstructionSensor &sensor;
  state_t state;
  state_t afterReverse;
  uint32_t changedAt;

public:
  BumperBehavior(ObstructionSensor &sensor) : sensor(sensor) {
    state = Inactive;
    afterReverse = Inactive;
    changedAt = 0;
  }

  void begin() {
    sensor.begin();
  }

  void service() {
    sensor.service();
    /*
    command.enabled = false;
    command.rotation = 0;
    command.velocity = 0;
    */

serviceAgain:
    switch (state) {
    case Inactive:
      if (sensor.any()) {
        afterReverse = Inactive;
        if (sensor.isCenterOrBothBlocked()) {
          afterReverse = Left;
        }
        else if (sensor.isLeftBlocked()) {
          afterReverse = Right;
        }
        else if (sensor.isRightBlocked()) {
          afterReverse = Left;
        }
        if (afterReverse != Inactive) {
          state = Reverse;
          changedAt = millis();
          printf("Inactive -> Reverse\n\r");
          goto serviceAgain;
        }
      }
      else {
        command.enabled = false;
      }
      break;
    case Reverse:
      if (millis() - changedAt > 2500) {
        state = afterReverse;
        printf("Reverse -> %d\n\r", state);
        changedAt = millis();
        goto serviceAgain;
      }
      else {
        command.enabled = true;
        command.rotation = 0;
        command.velocity = -5;
      }
      break;
    case Left:
      if (millis() - changedAt > 1500) {
        state = Stopped;
        printf("Left -> Forward\n\r");
        changedAt = millis();
        goto serviceAgain;
      }
      command.enabled = true;
      command.rotation = -6;
      command.velocity = 0;
      break;
    case Right:
      if (millis() - changedAt > 1500) {
        state = Stopped;
        printf("Right -> Forward\n\r");
        changedAt = millis();
        goto serviceAgain;
      }
      command.enabled = true;
      command.rotation = 6;
      command.velocity = 0;
      break;
    case Stopped:
      if (millis() - changedAt > 1000) {
        changedAt = millis();
        state = Inactive;
        printf("Stopped -> Inactive\n\r");
        goto serviceAgain;
      }
      command.enabled = true;
      command.rotation = 0;
      command.velocity = 0;
      break;
    case Forward: // Skipping for now...
      if (millis() - changedAt > 500) {
        changedAt = millis();
        state = Inactive;
        printf("Forward -> Inactive\n\r");
        goto serviceAgain;
      }
      command.enabled = true;
      command.rotation = 0;
      command.velocity = 8;
      break;
    }
  }
};

class UserBehavior : public Behavior {
public:
  UserBehavior() {
    command.enabled = true;
    command.rotation = 0;
    command.velocity = 0;
  }

  void begin() {
  }

  void service() {
  }

  void toggle() {
    if (command.velocity > 0) {
      command.velocity = 0;
    }
    else {
      command.velocity = 5;
    }
  }
};

}

#endif
