#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <Arduino.h>
#include "Servicable.h"
#include "MotorController.h"

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
    Looking,
    Waiting
  } state_t;

  PlatformMotionController *platform;
  ESC *esc;
  PID *pid;
  IMU *imu;
  Eyes *eyes;
  state_t state;
  state_t nextState;
  state_t nextStateAfterPause;
  uint32_t enteredAt;
  double difference;
  double desiredHeading;
  double actualHeading;
  uint32_t lastCorrectedHeading;
  uint16_t waitingFor;
  platform_command_t command;

private:
  void pause(state_t after, uint16_t time);
  void pauseWhileMoving(state_t after);

public:
  Navigator(PlatformMotionController &platform, ESC &esc, IMU &imu, Eyes &eyes);
  boolean isMoving();
  void begin();
  void search();
  void stop();
  void service();
};

#endif
