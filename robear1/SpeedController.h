#ifndef SPEED_H
#define SPEED_H

#define SC_K_PRO 3         // proportional 
#define SC_K_DRV 3         // derivative
#define SC_MAX   100 * 256 // maximum

class SpeedController : Servicable {
private:
  encoding::Encoders *encoders;
  MotionController *motion;
  int16_t leftDesired;
  int16_t leftAccumulator;
  int16_t leftLast;

  int16_t rightDesired;
  int16_t rightAccumulator;
  int16_t rightLast;

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
    int16_t lf, rt;

    lf = (leftDesired - encoders->getLeftVelocity()) * 256; 
    leftAccumulator +=  ((lf / SC_K_PRO) + ((lf - leftLast) / SC_K_DRV));
    leftLast = lf; 
    
    if (leftAccumulator > SC_MAX) leftAccumulator = SC_MAX; 
    else if (leftAccumulator < -SC_MAX) leftAccumulator = -SC_MAX; 

    rt = (rightDesired - encoders->getRightVelocity()) * 256; 
    rightAccumulator += ((rt / SC_K_PRO) + ((rt - rightLast) / SC_K_DRV)); 
    rightLast = rt; 
    
    if (rightAccumulator > SC_MAX) rightAccumulator = SC_MAX; 
    else if (rightAccumulator < -SC_MAX) rightAccumulator = -SC_MAX; 

    // motion->control(leftAccumulator / 256, rightAccumulator / 256);
  }

};

#endif
