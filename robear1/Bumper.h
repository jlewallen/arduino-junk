#ifndef BUMPER_H
#define BUMPER_H

#define BUMPER_PIN_RIGHT  12
#define BUMPER_PIN_LEFT   11

class BumperSensor : public Servicable {
private:
public:
  BumperSensor() {
  }

  void begin() {
    pinMode(BUMPER_PIN_LEFT, INPUT);
    pinMode(BUMPER_PIN_RIGHT, INPUT);
  }

  uint8_t isCenterOrBothBlocked() {
    return !digitalRead(BUMPER_PIN_LEFT) && !digitalRead(BUMPER_PIN_RIGHT);
  }

  uint8_t isLeftBlocked() {
    return !digitalRead(BUMPER_PIN_LEFT);
  }

  uint8_t isRightBlocked() {
    return !digitalRead(BUMPER_PIN_RIGHT);
  }

  uint8_t any() {
    return isCenterOrBothBlocked() || isLeftBlocked() || isRightBlocked();
  }

  void service() {
    /*
    printf("LEFT = ");
    if (digitalRead(BUMPER_PIN_LEFT)) {
      printf("1");
    }
    else {
      printf("0");
    }
    printf(" RIGHT = ");
    if (digitalRead(BUMPER_PIN_RIGHT)) {
      printf("1");
    }
    else {
      printf("0");
    }
    printf("\n\r");
    */
  }
};

#endif
