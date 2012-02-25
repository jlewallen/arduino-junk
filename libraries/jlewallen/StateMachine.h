#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Debuggable.h>

#define INVALID_STATE     (65535)
#define WAITING_STATE     (INVALID_STATE - 1)

class StateMachine {
private:
  uint16_t state;
  uint16_t nextState;
  uint16_t afterWaitingState;
  uint32_t enteredAt;
  uint16_t duration;

public:
  StateMachine(uint16_t state) :
    state(INVALID_STATE), nextState(state), enteredAt(0) {
  }

  uint16_t getState() {
    return state;
  }

  void transition(uint16_t state) {
    nextState = state;
  }

  void transitionAfter(uint16_t state, uint16_t time) {
    transition(WAITING_STATE);
    afterWaitingState = state;
    duration = time;
    DPRINTF("Waiting %d...\n\r", time);
  }

  void service() {
    boolean justEntered = state != nextState;
    if (justEntered) {
      enteredAt = millis();
      state = nextState;
      entered(state);
    }
    else {
      switch (state) {
      case WAITING_STATE:
        if (millis() - enteredAt > duration) {
          nextState = afterWaitingState;
        }
        break;
      default:
        service(state, enteredAt);
        break;
      }
    }
  }

  virtual void entered(uint16_t state) = 0;

  virtual void service(uint16_t state, uint32_t enteredAt) = 0;
};

#endif
