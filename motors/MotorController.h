#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

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
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
    }
    else {
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
    }
  }

  void speed(byte s) {
    speed(enabled, s);
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

  MotorController *getLeft() {
    return l;
  }

  MotorController *getRight() {
    return r;
  }

  boolean hasBothMotorsEnabled() {
    return l->isEnabled() && r->isEnabled();
  }

  boolean isMovingBackward() {
    return hasBothMotorsEnabled() && l->isMovingBackward() && r->isMovingBackward();
  }

  boolean isMovingForward() {
    return hasBothMotorsEnabled() && l->isMovingForward() && r->isMovingForward();
  }

  boolean isMoving() {
    return hasBothMotorsEnabled();
  }

  byte getSpeed() {
    if (l->speed() == r->speed()) {
      return l->speed();
    }
    return 0;
  }

  void direction(boolean left, boolean right) {
    l->direction(left);
    r->direction(right);
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

  void adjust(boolean direction, byte s) {
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

#endif
