#ifndef SONAR_H
#define SONAR_H

#define PIN 11

class MaxSonar : public Servicable {
protected:
  uint32_t accumulator;
  uint32_t samples;
  uint32_t previous;
  float distance;

public:
  MaxSonar() {
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
  AnalogMaxSonar() {
  }

  void begin() {
  }

  void service() {
    if (millis() - previous > 10) {
      accumulator += analogRead(PIN);
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
  DigitalMaxSonar() {
  }

  void begin() {
    pinMode(PIN, INPUT);
  }

  void service() {
    boolean value = digitalRead(PIN);
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
