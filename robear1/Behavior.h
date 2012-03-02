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
    switch (state) {
    case Inactive:
      if (obstructions.isCenterOrBothBlocked()) {
        afterReverse = Left;
        state = Reverse;
        changedAt = millis();
      }
      else if (obstructions.isLeftBlocked()) {
        afterReverse = Left;
        state = Reverse;
        changedAt = millis();
      }
      else if (obstructions.isRightBlocked()) {
        afterReverse = Right;
        state = Reverse;
        changedAt = millis();
      }
      else {
        command.enabled = false;
      }
      break;
    case Reverse:
      if (changedAt - millis() > 1000) {
        changedAt = millis();
        state = afterReverse;
      }
      else {
        command.enabled = true;
        command.rotation = 0;
        command.velocity = -6;
      }
      break;
    case Left:
      if (changedAt - millis() > 1000) {
        changedAt = millis();
        state = Forward;
      }
      command.enabled = true;
      command.rotation = 10;
      command.velocity = -6;
      break;
    case Right:
      if (changedAt - millis() > 1000) {
        changedAt = millis();
        state = Forward;
      }
      command.enabled = true;
      command.rotation = -4;
      command.velocity = -4;
      break;
    case Forward:
      if (changedAt - millis() > 500) {
        changedAt = millis();
        state = Inactive;
      }
      command.enabled = true;
      command.rotation = 0;
      command.velocity = 4;
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
