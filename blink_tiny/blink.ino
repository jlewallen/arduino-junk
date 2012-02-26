uint32_t timer0;
uint32_t timer1;
uint32_t timer2;
uint32_t d0;

class Blinker {
private:
  uint32_t previous;
  uint32_t interval;
  uint8_t pin;
  uint8_t enabled;

public:
  Blinker(uint32_t interval, uint8_t pin) :
    previous(0), interval(interval), pin(pin) {
  }

  void begin() {
    pinMode(pin, OUTPUT);
  }

  void service() {
    uint32_t now = millis();
    if (now - previous > interval) {
      enabled = !enabled;
      digitalWrite(pin, enabled);
      previous = now;
    }
  }
};

static Blinker b0( 500, 0);
static Blinker b1( 250, 1);
static Blinker b2(1000, 3);

void setup() {
  b0.begin();
  b1.begin();
  b2.begin();
}

void loop() {
  b0.service();
  b1.service();
  b2.service();
}
