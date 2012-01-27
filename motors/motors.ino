/**
 *
 */
#include <printf.h>

// #define SERIAL_DEBUG

#define DEFAULT_SPEED 200
#define SWITCH_1_PIN  9
#define SWITCH_2_PIN 10

class Servicable {
public:
  virtual ~Servicable () { }
  virtual void service() = 0;
};

class Latch : public Servicable {
private:
  byte pin;
  boolean value;

public:
  Latch(byte p) : pin(p) {
    pinMode(pin, INPUT);
  }

  void service() {
    tick();
  }

  boolean tick() {
    boolean newValue = digitalRead(pin);
    if (newValue != value) {
      value = newValue;
      return true;
    }
    return false;
  }

  boolean goneLow() {
    return tick() && value == LOW;
  }
};

class MotorController {
private:
  boolean enabled;
  boolean forwardDirection;
  byte speedPin;
  byte pin1;
  byte pin2;
  byte value;

public:
  MotorController(byte speedPin, byte pin1, byte pin2) :
    enabled(false),
    forwardDirection(false),
    speedPin(speedPin),
    pin1(pin1),
    pin2(pin2),
    value(0) {
  }

  boolean isMovingBackward() {
    return !isMovingForward();
  }

  boolean isMovingForward() {
    return forwardDirection;
  }

  boolean isEnabled() {
    return enabled;
  }

  void forward(byte speed) {
    direction(true);
  }

  void backward(byte speed) {
    direction(false);
  }

  void debug(const char *name) {
    printf("%s %s %s %d\n\r", name, enabled ? "on" : "off", forwardDirection ? "fwd" : "back", value);
  }

  void begin() {
    stop();
    pinMode(speedPin, OUTPUT);
    pinMode(pin1, OUTPUT); 
    pinMode(pin2, OUTPUT); 
  }

  void direction(boolean f) {
    forwardDirection = f;
    if (forwardDirection) {
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
    }
    else {
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
    }
  }

  void speed(boolean e, byte s) {
    enabled = e;
    value = s;
    if (enabled) {
      analogWrite(speedPin, value);
    }
    else {
      analogWrite(speedPin, 0);
    }
  }

  byte speed() {
    return value;
  }

  void start() {
    speed(true, value);
  }

  void toggle() {
    speed(!enabled, value);
  }

  void stop() {
    speed(false, value);
  }

  void disable() {
  }

  void enable() {
  }
};

class PlatformMotionController : public Servicable {
private:
  MotorController *l;
  MotorController *r;

public:
  PlatformMotionController(MotorController &l, MotorController &r) : l(&l), r(&r) {
  }

  boolean isMovingBackward() {
    return !isMovingForward();
  }

  boolean isMovingForward() {
    return l->isEnabled() && l->isMovingForward() && r->isEnabled() && r->isMovingForward();
  }

  byte getSpeed() {
    if (l->speed() == r->speed()) {
      return l->speed();
    }
    return 0;
  }

  void turn(boolean direction, byte s) {
    l->stop();
    r->stop();

    l->direction(direction);
    r->direction(!direction);

    l->speed(false, s);
    r->speed(false, s);

    l->start();
    r->start();
  }

  void adjust(boolean direction, byte s, byte diff) {
    l->stop();
    r->stop();

    l->direction(direction);
    r->direction(direction);

    l->speed(false, s);
    r->speed(false, s);

    l->start();
    r->start();
  }

  void stop() {
    l->stop();
    r->stop();
  }

  void pause() {
    l->stop();
    r->stop();
  }

  void resume() {
    l->start();
    r->start();
  }

  void service() {
  }

  void begin() {
    l->begin();
    r->begin();
  }

  void debug() {
    l->debug("L");
    r->debug("R");
  }
};

class ESC : public Servicable {
private:
  enum State {
    Running,
    Draining,
    Measuring
  };

  typedef struct {
    byte pin;
    long accumulator;
  } reading_t;

  PlatformMotionController *platform;
  long previous;
  State state;
  long samples;
  reading_t motor1;
  reading_t motor2;

public:
  ESC(PlatformMotionController &platform, byte p1, byte p2) : platform(&platform), previous(0), state(Running) {
    motor1.pin = p1;
    motor2.pin = p2;
  }

  void begin() {
  }

  void service() {
    long now = micros();
    switch (state) {
    case Running:
      if (!platform->isMovingForward()) {
        return;
      }
      if (now - previous > 100000) {
        platform->pause();
        state = Draining;
        previous = now;
      }
      break;
    case Draining:
      if (now - previous > 2000) {
        state = Measuring;
        motor1.accumulator = 0;
        motor2.accumulator = 0;
        samples = 0;
        previous = now;
      }
      break;
    case Measuring:
      motor1.accumulator += analogRead(motor1.pin);
      motor2.accumulator += analogRead(motor2.pin);
      samples++;
      if (now - previous > 2000) {
        Serial.print(motor1.accumulator / samples);
        Serial.print(" ");
        Serial.print(motor2.accumulator / samples);
        Serial.print(" ");
        Serial.print(motor1.accumulator / samples - motor2.accumulator / samples);
        Serial.println("");
        platform->resume();
        state = Running;
        previous = now;
      }
      break;
    }
  }
};

class OpticalEncoders : public Servicable {
public:
  static long encoderOne;
  static long encoderTwo;

  static void encode0() {
    encoderOne++;
  }

  static void encode1() {
    encoderTwo++;
  }

  void begin() {
    encoderOne = 0;
    encoderTwo = 0;

    pinMode(0, INPUT);
    pinMode(1, INPUT);

    attachInterrupt(0, encode0, FALLING);
    attachInterrupt(1, encode1, FALLING);
  }

  void service() {
    static long lastOne = 0;
    static long lastTwo = 0;
    if (lastOne != encoderOne || lastTwo != encoderTwo) {
      if (lastOne != encoderOne) printf("1-%5d ", encoderOne - lastOne); else printf("1-      ");
      if (lastTwo != encoderTwo) printf("2-%5d ", encoderTwo - lastTwo); else printf("2-      ");
      printf("\n\r");
      lastOne = encoderOne;
      lastTwo = encoderTwo;
    }
  }
};

class SerialController : public Servicable {
private:
  PlatformMotionController *platform;
  ESC *esc;

public:
  SerialController(PlatformMotionController &platform, ESC &esc) : platform(&platform), esc(&esc) {
  }

  void begin() {
    Serial.begin(19200);
    printf_begin();
  }

  void ready() {
    Serial.println("Ready...");
  }

  void service() {
    if (Serial.available() > 0) {
      switch (Serial.read()) {
      case 'e':
        printf("Left\n\r");
        platform->turn(true, DEFAULT_SPEED);
        break;
      case 'r':
        printf("Right\n\r");
        platform->turn(false, DEFAULT_SPEED);
        break;
      case 'a':
        printf("Nudge Left\n\r");
        platform->turn(true, DEFAULT_SPEED);
        delay(600);
        platform->stop();
        break;
      case 's':
        printf("Nudge Right\n\r");
        platform->turn(false, DEFAULT_SPEED);
        delay(600);
        platform->stop();
        break;
      case 'z':
        printf("%d", platform->getSpeed() + 10);
        printf("Faster\n\r");
        platform->adjust(true, platform->getSpeed() + 10, 0);
        break;
      case 'x':
        printf("%d", platform->getSpeed() + 10);
        printf("Slower\n\r");
        platform->adjust(true, platform->getSpeed() - 10, 0);
        break;
      case 'q':
        printf("Forward\n\r");
        platform->adjust(true, DEFAULT_SPEED, 0);
        break;
      case 'w':
        printf("Backward\n\r");
        platform->adjust(false, DEFAULT_SPEED, 0);
        break;
      case '.':
        printf("Stop\n\r");
        platform->stop();
        break;
      }
    }
  }
};

class ButtonsController : public Servicable {
private:
  PlatformMotionController *platform;
  ESC *esc;
  Latch button1;
  Latch button2;

public:
  ButtonsController(PlatformMotionController &platform, ESC &esc) : platform(&platform), esc(&esc), button1(SWITCH_1_PIN), button2(SWITCH_2_PIN) {
  }

  void begin() {
  }

  void service() {
    if (digitalRead(12) == LOW) {
      if (platform->isMovingForward()) {
        printf("Object detected, stop\n\r");
        platform->stop();
      }
    }

    if (button1.goneLow()) {
      if (platform->isMovingForward()) {
        platform->stop();
      }
      else {
        printf("Forward\n\r");
        platform->adjust(true, DEFAULT_SPEED, 0);
      }
    }

    if (button2.goneLow()) {
      if (platform->isMovingBackward()) {
        platform->stop();
      }
      else {
        printf("Backward\n\r");
        platform->adjust(false, DEFAULT_SPEED, 0);
      }
    }
  }
};

static MotorController leftMotor(11, 4, 5);
static MotorController rightMotor(6, 7, 8);
static PlatformMotionController platform(leftMotor, rightMotor);
static ESC esc(platform, 0, 1);
static SerialController serialController(platform, esc);
static ButtonsController buttonsController(platform, esc);

void setup() {
  serialController.begin();
  buttonsController.begin();
  esc.begin();
  platform.begin();
  serialController.ready();
}

void loop() {
  serialController.service();
  buttonsController.service();
  platform.service();
  esc.service();
  #if defined(SERIAL_DEBUG)
  platform.debug();
  #endif
}

