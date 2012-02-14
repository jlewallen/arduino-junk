class Generator {
private:
  byte pin;
  long previous;
  boolean value;
  long off;
  long on;

public:
  Generator(byte pin, long off, long on) : pin(pin), previous(0), value(false), off(off), on(on) {
  }

  void begin() {
    pinMode(pin, OUTPUT);
  }

  void service() {
    long now = millis();
    long interval = value ? on : off;
    if (now - previous > interval) {
      value = !value;
      digitalWrite(pin, value);
      previous = now;
    }
  }
};

static Generator signals[] = {
  Generator(2, 100, 1),
  Generator(3, 10, 4),
  Generator(4,  4, 2)
};

void setup() {
  for (uint8_t i = 0; i < 3; ++i) {
    signals[i].begin();
  }
}

void loop() {
  for (uint8_t i = 0; i < 3; ++i) {
    signals[i].service();
  }
}
