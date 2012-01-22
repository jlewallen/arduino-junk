class PassiveInfraredSensor {
public:
  enum State {
    NOTHING,
    ENTERING,
    MOTION,
    LEAVING
  };

private:
  uint16_t pause;
  boolean motion;
  byte pin;

public:
  PassiveInfraredSensor(byte pin) : pause(1000), motion(false), pin(pin) {
  }

  void begin() {
    digitalWrite(pin, LOW);
    pinMode(pin, INPUT);
  }

  void callibrate() {
    for (byte i = 0; i < 10; ++i) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println(" DONE");
  }

  void started() {
  }

  void stopped() {
  }

  State tick() {
    if (digitalRead(pin) == HIGH) {
      if (!motion) {
        motion = true;
        started();
        return ENTERING;
      }
      return MOTION;
    }
    else {
      if (motion) {
        motion = false;
        stopped();
        return LEAVING;
      }
      return NOTHING;
    }
  }
};

static PassiveInfraredSensor sensor(3);
static const byte ledPin = 9;

void setup() {
  Serial.begin(9600);

  digitalWrite(ledPin, LOW);
	pinMode(ledPin, OUTPUT);

  sensor.begin();
  digitalWrite(ledPin, HIGH);
  sensor.callibrate();
  digitalWrite(ledPin, LOW);
}

void loop() {
  switch (sensor.tick()) {
  case PassiveInfraredSensor::ENTERING:
    digitalWrite(ledPin, HIGH);
    Serial.println("motion started");
    break;
  case PassiveInfraredSensor::LEAVING:
    digitalWrite(ledPin, LOW);
    Serial.println("motion stopped");
    break;
  }
}
