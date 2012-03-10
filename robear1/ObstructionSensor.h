#ifndef OBSTRUCTION_SENSOR_H
#define OBSTRUCTION_SENSOR_H

class ObstructionSensor : public Servicable {
public:
  virtual ~ObstructionSensor() {
  }
  virtual uint8_t isCenterOrBothBlocked() = 0;
  virtual uint8_t isLeftBlocked() = 0;
  virtual uint8_t isRightBlocked() = 0;

  uint8_t any() {
    return isCenterOrBothBlocked() || isLeftBlocked() || isRightBlocked();
  }
};

#endif
