
#define RELAY_1 8
#define RELAY_2 9

class Relays {
public:
  void setup(byte pin) {
    set(pin, false);
    pinMode(pin, OUTPUT);
  }

  void set(byte pin, boolean on) {
    digitalWrite(pin, on ? LOW : HIGH);
  }
};

Relays relays;

void setup() {
  Serial.begin(9600);

  relays.setup(RELAY_1);
  relays.setup(RELAY_2);
}

void loop() {
  relays.set(RELAY_1, true);
  relays.set(RELAY_2, false);
  delay(5000);
  relays.set(RELAY_1, false);
  relays.set(RELAY_2, true);
  delay(5000);
}
