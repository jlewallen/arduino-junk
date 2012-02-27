#include <Arduino.h>
extern "C" {
#include "usiTwiSlave.h"
}

#define I2C_SLAVE_ADDR  0x01

void blink(uint8_t pin, int8_t times, uint32_t onTime, uint32_t offTime) {
  noInterrupts();
  while (times > 0) {
    digitalWrite(pin, HIGH);
    delay(onTime);
    digitalWrite(pin, LOW);
    delay(offTime);
    times--;
  }
  interrupts();
}

static boolean enabled = false;

void setup() {
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  
  usiTwiSlaveInit(I2C_SLAVE_ADDR);

  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
}

void loop() {
  if (usiTwiDataInReceiveBuffer()) {
    digitalWrite(0, HIGH);
    byte received = usiTwiReceiveByte();
    digitalWrite(0, LOW);
    if (enabled) {
      enabled = LOW;
    }
    else {
      enabled = HIGH;
    }
    for (byte i = 0; i < received; ++i) {
      digitalWrite(0, HIGH);
      delay(50);
      digitalWrite(0, LOW);
      delay(50);
    }
    usiTwiTransmitByte(received + 1);
  }
  digitalWrite(1, HIGH);
  delay(100);
  digitalWrite(1, LOW);
}
