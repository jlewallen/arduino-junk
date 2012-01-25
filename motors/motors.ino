#include <printf.h>

class MotorController {
private:
  boolean enabled;
  boolean direction;
  byte speedPin;
  byte pin1;
  byte pin2;
  byte speed;

public:
  MotorController(byte speedPin, byte pin1, byte pin2) :
    enabled(false), direction(false),
    speedPin(speedPin),
    pin1(pin1),
    pin2(pin2),
    speed(0) {
  }

  void debug(const char *name) {
    printf("%s %s %s %d\n\r", name, enabled ? "on" : "off", direction ? "fwd" : "back", speed);
  }

  void begin() {
    stop();
    pinMode(speedPin, OUTPUT);
    pinMode(pin1, OUTPUT); 
    pinMode(pin2, OUTPUT); 
  }

  void forward() {
    setDirection(true);
  }

  void backward() {
    setDirection(false);
  }

  void setDirection(boolean d) {
    direction = d;
    if (direction) {
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
    }
    else {
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
    }
  }

  void setSpeed(boolean e, byte s) {
    enabled = e;
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
    setSpeed(true, speed);
  }

  void toggleDirection() {
    setDirection(!direction);
  }

  void toggle() {
    setSpeed(!enabled, speed);
  }

  void stop() {
    setSpeed(false, speed);
  }
};

static MotorController motor1(3, 4, 5);
static MotorController motor2(6, 7, 8);
static byte speed = 0;

void turn(boolean direction, byte s) {
  motor1.stop();
  motor2.stop();

  motor1.setDirection(direction);
  motor2.setDirection(!direction);

  motor1.setSpeed(false, s);
  motor2.setSpeed(false, s);

  motor1.start();
  motor2.start();
}

void adjust(boolean direction, byte s, byte diff) {
  motor1.stop();
  motor2.stop();

  motor1.setDirection(direction);
  motor2.setDirection(direction);

  motor1.setSpeed(false, s);
  motor2.setSpeed(false, s);

  motor1.start();
  motor2.start();
}

void setup() {
  Serial.begin(9600);
  printf_begin();

  motor1.begin();
  motor2.begin();

  Serial.println("Ready...");
}

void loop() {
  if (Serial.available() > 0) {
    switch (Serial.read()) {
    case 'e':
      turn(true, 100);
      break;
    case 'r':
      turn(false, 100);
      break;
    case 'q':
      printf("Forward 200\n\r");
      adjust(true, 200, 0);
      printf("Done\n\r");
      break;
    case 'w':
      printf("Backward 200\n\r");
      adjust(false, 200, 0);
      printf("Done\n\r");
      break;
    case 'f':
      // speed += 10;
      // Serial.println(speed);
      // adjust(true, speed);
      break;
    case 's':
      // speed -= 10;
      // Serial.println(speed);
      // adjust(speed);
      break;
    case ',':
      printf("Toggle Direction\n\r");
      motor1.toggleDirection();
      motor2.toggleDirection();
      break;
    case '.':
      printf("Stop\n\r");
      motor1.stop();
      motor2.stop();
      break;
    }
  }
  delay(50);
  motor1.debug("1");
  motor2.debug("2");
}

