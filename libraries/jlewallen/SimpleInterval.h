#ifndef SimpleInterval_h
#define SimpleInterval_h

#include <Arduino.h>

class SimpleInterval {
  private:
    int interval;
    int previous;

  public:
    SimpleInterval(int interval) : interval(interval) {
      previous = millis();
    }

    boolean tick() {
      int now = millis();
      if (now - previous > interval) {
        previous = now;
        return true;
      }
      return false;
    }
};

#endif
