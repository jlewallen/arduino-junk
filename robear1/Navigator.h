#ifndef NAVIGATOR_H
#define NAVIGATOR_H

class Navigator : public Servicable, public StateMachine {
private:
  typedef enum {
    Stopped,
    Waiting,
    Searching,
    Obstructed,
    Avoiding,
    Turning,
    Stalled
  } state_t;

  MotionController *motion;
  encoding::Encoders *encoders;
  MaxSonar *sonar;
  Head *head;
  Obstructions *obstructions;
  Display *display;

public:
  Navigator(MotionController &motion, encoding::Encoders &encoders, MaxSonar &sonar, Head &head, Obstructions &obstructions, Display &display) :
    StateMachine(Stopped),
    motion(&motion), encoders(&encoders), sonar(&sonar), head(&head), obstructions(&obstructions), display(&display) {
  }

  void begin() {
  }

  void service() {
    StateMachine::service();
  }
  
  void search() {
    transitionAfter(Searching, 1000);
  }

  void stop() {
    transition(Stopped);
  }

  void entered(uint16_t state) {
    switch (state) {
    case Stopped:
      DPRINTF("Stopped\n\r");
      display->stopped();
      motion->execute(&stopCommand);
      break;
    case Stalled:
      DPRINTF("Stalled\n\r");
      display->stalled();
      motion->execute(&stopCommand);
      break;
    case Searching:
      DPRINTF("Searching\n\r");
      display->searching();
      motion->execute(&forwardCommand);
      break;
    case Obstructed:
      DPRINTF("Obstructed\n\r");
      display->obstructed();
      motion->execute(&stopCommand);
      transitionAfter(Avoiding, 750);
      break;
    case Avoiding:
      DPRINTF("Avoiding\n\r");
      display->avoiding();
      motion->execute(&backwardCommand);
      break;
    }
  }

  void service(uint16_t state, uint32_t enteredAt) {
    switch (state) {
    case Stopped:
      break;
    case Searching:
      if (obstructions->isBlocked()) {
        transition(Obstructed);
      }
      break;
    case Obstructed:
      break;
    case Avoiding:
      if (!obstructions->isBlocked()) {
        transition(Stopped);
      }
      break;
    case Turning:
      break;
    }
  }
};

#endif
