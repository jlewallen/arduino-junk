#ifndef SimpleInterval_h
#define SimpleInterval_h

#include <Arduino.h>

class SimpleInterval {
  private:
    long interval;
    long previous;

  public:
    SimpleInterval(long interval) : interval(interval) {
      previous = millis();
    }

    boolean tick() {
      long now = millis();
      if (now - previous > interval) {
        previous = now;
        return true;
      }
      return false;
    }
};

#endif
