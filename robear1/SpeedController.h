#ifndef SPEED_H
#define SPEED_H

#define SC_K_PRO(e)            ((e) * 4)
#define SC_K_DRV(e)            ((e) / 2)
#define SC_MAX                 (100 * 256)
#define SC_SCALE_VELOCITY(v)   (v)

class SpeedController : Servicable {
private:
  encoding::Encoders *encoders;
  MotionController *motion;
  int16_t leftDesired;
  int32_t leftAccumulator;
  int32_t leftLast;

  int16_t rightDesired;
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

  void control(int16_t left, int16_t right) {
    leftDesired = left;
    rightDesired = right;
  }

  void begin() {
  }

  void service() {
    int32_t lf = 0, rt = 0;

    if (leftDesired != 0) {
      lf = (SC_SCALE_VELOCITY(leftDesired) - SC_SCALE_VELOCITY(encoders->getLeftVelocity())) * 256; 
      leftAccumulator += SC_K_PRO(lf) + SC_K_DRV(lf - leftLast);
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
      rightAccumulator += SC_K_PRO(rt) + SC_K_DRV(rt - rightLast);
      rightLast = rt; 
      if (rightAccumulator > SC_MAX) rightAccumulator = SC_MAX; 
      else if (rightAccumulator < -SC_MAX) rightAccumulator = -SC_MAX; 
    }
    else {
      rightAccumulator = 0;
      rightLast = 0; 
    }

    printf("d(%d %d) (%lu %lu) last(%lu %lu) vel(%lu %lu) -> pwm(%lu %lu)\n\r",
           leftDesired, rightDesired,
           lf, rt,
           leftLast, rightLast,
           encoders->getLeftVelocity(), encoders->getRightVelocity(),
           leftAccumulator / 256, rightAccumulator / 256
           );

    motion->control(leftAccumulator / 256, rightAccumulator / 256);
  }

};

#endif
