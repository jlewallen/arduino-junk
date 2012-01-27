#include <printf.h>

#define SERIAL_DEBUG

class Latch {
private:
  byte pin;
  boolean value;

public:
  Latch(byte p) : pin(p) {
    pinMode(pin, INPUT);
  }

  boolean tick() {
    boolean newValue = digitalRead(pin);
    if (newValue != value) {
      value = newValue;
      return true;
    }
    return false;
  }
};

long one = 0;
long two = 0;

void encode1() {
  one++;
}

void encode0() {
  two++;
}

static Latch l1(7);
static Latch l2(8);

void setup() {
  Serial.begin(9600);
  printf_begin();

  attachInterrupt(0, encode0, CHANGE);
  attachInterrupt(1, encode1, CHANGE);

  Serial.println("Ready...");
}

void loop() {
  static long lastOne = 0;
  static long lastTwo = 0;
  if (lastOne != one || lastTwo != two) {
    printf("%d %d\n\r", one, two);
    lastOne = one;
    lastTwo = two;
  }
  delay(10);
}

