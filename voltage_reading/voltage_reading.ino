/**
 *
 */
#include <Arduino.h>
#include <Serviceable.h>
#include <printf.h>

class VoltageReader : public Serviceable {
private:
  byte pin;
  uint32_t previous;
  uint32_t accumulator;
  uint32_t samples;
  uint32_t delta;

public:
  VoltageReader(byte pin) : pin(pin), previous(0), delta(1000) {
    pinMode(pin, INPUT);
    accumulator = 0;
    samples = 0;
  }

  void begin() {
  }

  void service() {
    accumulator += analogRead(pin);
    samples++;
    uint32_t now = millis();
    if (now - previous > delta) {
      uint32_t reading = accumulator / samples;
      printf("%lu %d: %lu ", millis(), pin, voltage(reading));
      printf("%lu\n\r", reading);
      previous = now;
      accumulator = 0;
      samples = 0;
    }
  }

  uint32_t voltage(uint32_t reading) {
    return reading * 819 / 604;
  }
};

static VoltageReader reader1(0);
static VoltageReader reader2(1);

void setup() {
  Serial.begin(19200);
  printf_begin();
  reader1.begin();
  reader2.begin();
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
}

static uint32_t previous = 0;
static uint32_t counter = 0;

void loop() {
  // reader1.service();
  // reader2.service();

  if (millis() - previous > 2000) {
    previous = millis();
    analogWrite(3, counter);
    counter = constrain((counter + 32) % 256, 32, 255);
    analogWrite(5, counter);
    Serial.println(counter);
  }

  delay(50);
}
