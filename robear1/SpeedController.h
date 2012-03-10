#ifndef SPEED_H
#define SPEED_H

#define SC_K_PRO(e)            ((e) * 2)
#define SC_K_DRV(e)            ((e) / 1)
#define SC_MAX                 (150L * 256L)
#define SC_SCALE_VELOCITY(v)   (v)

class SpeedController : Servicable {
private:
  encoding::Encoders *encoders;
  MotionController *motion;
  int32_t leftDesired;
  int32_t leftAccumulator;
  int32_t leftLast;
  int32_t rightDesired;
  int32_t rightAccumulator;
  int32_t rightLast;

public:
  SpeedController(encoding::Encoders &encoders, MotionController &motion) : 
    encoders(&encoders), motion(&motion) {
    leftDesired = 0;
    leftAccumulator = 0;
    leftLast = 0;
    rightDesired = 0;
    rightAccumulator = 0;
    rightLast = 0;
  }

  void control(int32_t left, int32_t right) {
    leftDesired = left;
    rightDesired = right;
  }

  void begin() {
  }

  void service() {
    int32_t lf = 0, rt = 0;
    int32_t la = 0, ra = 0;

    if (leftDesired != 0) {
      lf = (SC_SCALE_VELOCITY(leftDesired) - SC_SCALE_VELOCITY(encoders->getLeftVelocity())) * 256;
      la = SC_K_PRO(lf) + SC_K_DRV(lf - leftLast);
      leftAccumulator += la;
      leftLast = lf; 
      if (leftAccumulator > SC_MAX) leftAccumulator = SC_MAX; 
      else if (leftAccumulator < -SC_MAX) leftAccumulator = -SC_MAX; 
    }
    else {
      leftAccumulator = 0;
      leftLast = 0; 
    }

    if (rightDesired != 0) {
      rt = (SC_SCALE_VELOCITY(rightDesired) - SC_SCALE_VELOCITY(encoders->getRightVelocity())) * 256;
      ra = SC_K_PRO(rt) + SC_K_DRV(rt - rightLast);
      rightAccumulator += ra;
      rightLast = rt; 
      if (rightAccumulator > SC_MAX) rightAccumulator = SC_MAX; 
      else if (rightAccumulator < -SC_MAX) rightAccumulator = -SC_MAX; 
    }
    else {
      rightAccumulator = 0;
      rightLast = 0; 
    }

    int16_t leftSpeed = leftAccumulator / 256;
    int16_t rightSpeed = rightAccumulator / 256;

    #if defined(DEBUG_ESC)
    printf("d(%ld %ld) (%ld %ld) a(%ld %ld) last(%ld %ld) vel(%d %d) -> pwm(%d %d)\n\r",
           leftDesired, rightDesired,
           lf, rt,
           la, ra,
           leftLast, rightLast,
           encoders->getLeftVelocity(), encoders->getRightVelocity(),
           leftSpeed, rightSpeed);
    #endif

    motion->control(leftSpeed, rightSpeed);
  }

};

#endif
