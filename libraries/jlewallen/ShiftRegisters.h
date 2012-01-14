#ifndef ShiftRegisters_h
#define ShiftRegisters_h

#include <Arduino.h>

class ShiftRegisters {
  private:
    int serPin;
    int rclkPin;
    int srclkPin;
    int width;
    
  public:
    ShiftRegisters(int serPin, int rclkPin, int srclkPin, int width)
     : serPin(serPin), rclkPin(rclkPin), srclkPin(srclkPin), width(width) {
    }
    
  public:
    void begin() {
      pinMode(serPin, OUTPUT);
      pinMode(rclkPin, OUTPUT);
      pinMode(srclkPin, OUTPUT);
    }
    
    void write(const boolean *values) {
      digitalWrite(rclkPin, LOW);
      for(int i = width - 1; i >= 0; i--) {
        digitalWrite(srclkPin, LOW);
        digitalWrite(serPin, values[i]);
        digitalWrite(srclkPin, HIGH);
      }
      digitalWrite(rclkPin, HIGH);
    }
};

#endif
