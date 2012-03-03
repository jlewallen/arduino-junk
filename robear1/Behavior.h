#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include <Servicable.h>
#include "FrontInfrared.h"

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
  behavior_command_t *getCommand() {
    return &command;
  }
};

class Bumper : public Behavior {
private:
  typedef enum {
    Inactive,
    Reverse,
    Left,
    Right,
    Forward,
  } state_t;
  
  Obstructions obstructions;
  state_t state;
  state_t afterReverse;
  uint32_t changedAt;

public:
  Bumper() {
    state = Inactive;
    afterReverse = Inactive;
    changedAt = 0;
  }

  void begin() {
    obstructions.begin();
  }

  void service() {
    obstructions.service();

serviceAgain:
    switch (state) {
    case Inactive:
      if (obstructions.any()) {
        afterReverse = Inactive;
        if (obstructions.isCenterOrBothBlocked()) {
          afterReverse = Left;
        }
        else if (obstructions.isLeftBlocked()) {
          afterReverse = Right;
        }
        else if (obstructions.isRightBlocked()) {
          afterReverse = Left;
        }
        if (afterReverse != Inactive) {
          state = Reverse;
          changedAt = millis();
          goto serviceAgain;
        }
      }
      else {
        command.enabled = false;
      }
      break;
    case Reverse:
      if (millis() - changedAt > 1000) {
        state = afterReverse;
        printf("Reverse -> %d\n\r", state);
        changedAt = millis();
        goto serviceAgain;
      }
      else {
        command.enabled = true;
        command.rotation = 0;
        command.velocity = -8;
      }
      break;
    case Left:
      if (millis() - changedAt > 1500) {
        state = Forward;
        printf("Left -> Forward\n\r");
        changedAt = millis();
        goto serviceAgain;
      }
      command.enabled = true;
      command.rotation = -6;
      command.velocity = -6;
      break;
    case Right:
      if (millis() - changedAt > 1500) {
        state = Forward;
        printf("Right -> Forward\n\r");
        changedAt = millis();
        goto serviceAgain;
      }
      command.enabled = true;
      command.rotation = 6;
      command.velocity = -6;
      break;
    case Forward:
      if (millis() - changedAt > 1000) {
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

class User : public Behavior {
public:
  User() {
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
