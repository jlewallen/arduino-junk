#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <Arduino.h>
#include "Servicable.h"

class ESC;
class Eyes;
class PlatformMotionController;

class Navigator : public Servicable {
private:
  typedef enum {
    Starting,
    Stopped,
    Searching,
    Avoiding,
    Clearing
  } state_t;

  PlatformMotionController *platform;
  ESC *esc;
  Eyes *eyes;
  state_t state;
  state_t nextState;
  uint32_t enteredAt;

public:
  Navigator(PlatformMotionController &platform, ESC &esc, Eyes &eyes);
  boolean isMoving();
  void begin();
  void search();
  void stop();
  void service();
};

#endif
