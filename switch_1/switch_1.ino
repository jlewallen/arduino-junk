#define PIN_1 13
#define PIN_2 11
#define SWITCH_OPEN 3
#define SWITCH_CLOSE 2

static int8_t blinking_pin = PIN_1;
static int8_t blinking_on = false;
static long previous = 0;

void update_blinking_pin(int8_t pin) {
  if (pin != blinking_pin) {
    digitalWrite(blinking_pin, LOW);
    blinking_pin = pin;
    previous = 0;
  }
}

void changed() {
  if (digitalRead(SWITCH_CLOSE)) {
    update_blinking_pin(PIN_2);
  }
  if (digitalRead(SWITCH_OPEN)) {
    update_blinking_pin(PIN_1);
  }
}

void setup() {
  previous = millis();

  pinMode(PIN_1, OUTPUT);
  pinMode(PIN_2, OUTPUT);
  pinMode(SWITCH_OPEN, INPUT);
  pinMode(SWITCH_CLOSE, INPUT);

	attachInterrupt(0, changed, CHANGE);
	attachInterrupt(1, changed, CHANGE);

  for (short i = 0; i < 4; ++i) {
    digitalWrite(PIN_1, HIGH);
    digitalWrite(PIN_2, HIGH);
    delay(20);
    digitalWrite(PIN_1, LOW);
    digitalWrite(PIN_2, LOW);
    delay(50);
  }
}

void loop() {
  if (millis() - previous > 50) {
    previous = millis();
    blinking_on = !blinking_on;
    if (blinking_on) {
      digitalWrite(blinking_pin, HIGH);
    }
    else {
      digitalWrite(blinking_pin, LOW);
    }
  }
  delay(10);
}
