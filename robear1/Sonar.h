#ifndef SONAR_H
#define SONAR_H

class MaxSonar : public Servicable {
protected:
  uint8_t pin;
  uint32_t accumulator;
  uint32_t samples;
  uint32_t previous;
  float distance;

public:
  MaxSonar(uint8_t pin) : pin(pin) {
    accumulator = 0;
    samples = 0;
  }

  float getDistance() {
    return distance;
  }

  virtual void begin() = 0;
  virtual void service() = 0;
};

class AnalogMaxSonar : public MaxSonar {
public:
  AnalogMaxSonar(uint8_t pin) : MaxSonar(pin) {
  }

  void begin() {
  }

  void service() {
    if (millis() - previous > 10) {
      accumulator += analogRead(pin);
      samples++;
      previous = millis();
    }

    if (samples == 10) {
      float average = accumulator / (float)samples;
      distance = map(average, 0, 1024, 0, 700);
      accumulator = 0;
      samples = 0;
    }
  }
};

class DigitalMaxSonar : public MaxSonar {
private:
  boolean previousValue;

public:
  DigitalMaxSonar(uint8_t pin) : MaxSonar(pin) {
  }

  void begin() {
    pinMode(pin, INPUT);
  }

  void service() {
    boolean value = digitalRead(pin);
    if (value != previousValue) {
      if (value) {
        previous = micros();
      }
      else {
        accumulator += (micros() - previous) / 58;
        samples++;
      }
      previousValue = value;
    }

    if (samples == 5) {
      float average = accumulator / (float)samples;
      distance = average;
      accumulator = 0;
      samples = 0;
    }
  }
};

#endif
