class MotorController {
private:
  boolean enabled;
  byte speedPin;
  byte pin1;
  byte pin2;
  byte speed;

public:
  MotorController(byte speedPin, byte pin1, byte pin2) : enabled(false), speedPin(speedPin), pin1(pin1), pin2(pin2), speed(0) {
  }

  void begin() {
    stop();
    pinMode(speedPin, OUTPUT);
    pinMode(pin1, OUTPUT); 
    pinMode(pin2, OUTPUT); 
  }

  void forward() {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
  }

  void backward() {
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
  }

  void setSpeed(byte s) {
    speed = s;
    if (enabled) {
      analogWrite(speedPin, speed);
    }
    else {
      analogWrite(speedPin, 0);
    }
  }

  byte getSpeed() {
    return speed;
  }

  void start() {
    enabled = true;
    setSpeed(speed);
  }

  void toggle() {
    enabled = !enabled;
    setSpeed(speed);
  }

  void stop() {
    enabled = false;
    setSpeed(speed);
  }
};

static MotorController motor1(3, 4, 5);
static MotorController motor2(6, 7, 8);
static byte speed = 0;

void adjustSpeed(byte s) {
  motor1.forward();
  motor1.setSpeed(s);
  motor1.start();

  motor2.forward();
  motor2.setSpeed(s);
  motor2.start();
}

void setup() {
  Serial.begin(9600);

  motor1.begin();
  motor2.begin();

  speed = 100;
  adjustSpeed(speed);

  Serial.println("Ready...");
}

void loop() {
  if (Serial.available() > 0) {
    switch (Serial.read()) {
    case 'f':
      speed += 10;
      Serial.println(speed);
      adjustSpeed(speed);
      break;
    case 's':
      speed -= 10;
      Serial.println(speed);
      adjustSpeed(speed);
      break;
    case '.':
      motor1.toggle();
      motor2.toggle();
      break;
    }
  }
  delay(50);
}

