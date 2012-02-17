#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <Arduino.h>
#include "Servicable.h"

class ESC;
class Eyes;
class PlatformMotionController;
class PID;
class IMU;

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
  PID *pid;
  IMU *imu;
  state_t state;
  state_t nextState;
  uint32_t enteredAt;

  double difference;
  double desiredHeading;
  double actualHeading;
  uint32_t debugged;

public:
  Navigator(PlatformMotionController &platform, ESC &esc, IMU &imu);
  boolean isMoving();
  void begin();
  void search();
  void stop();
  void service();
  void turn();
};

#endif
