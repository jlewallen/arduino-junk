/**
 *
 */
#include <Arduino.h>
#include <printf.h>
#include <Servo.h>
#include <Servicable.h>
#include "MotorController.h"
#include "ESC.h"
#include "IMU.h"
#include "Eyes.h"
#include "Navigator.h"

#define SWITCH_1_PIN 2
#define SWITCH_2_PIN 3

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

class SerialController : public Servicable {
private:
  PlatformMotionController *platform;
  ESC *esc;
  Navigator *navigator;
  Eyes *eyes;

public:
  SerialController(PlatformMotionController &platform, ESC &esc, Navigator &navigator, Eyes &eyes) : platform(&platform), esc(&esc), navigator(&navigator), eyes(&eyes) {
  }

  void begin() {
    Serial.begin(57600);
    printf_begin();
  }

  void ready() {
    Serial.println("Ready...");
  }

  void service() {
    if (Serial.available() > 0) {
      switch (Serial.read()) {
      case 'a':
        printf("Nudge Left\n\r");
        platform->execute(&Left, 600);
        break;
      case 's':
        printf("Nudge Right\n\r");
        platform->execute(&Right, 600);
        break;
      case 'g':
        printf("Nudge Forward\n\r");
        platform->execute(&Forward, 500);
        break;
      case 'h':
        printf("Nudge Backward\n\r");
        platform->execute(&Backward, 1000);
        break;
      case 'z':
        eyes->lookNext();
        break;
      case 'c':
        eyes->lookForward();
        break;
      case 'q':
        printf("Forward\n\r");
        navigator->search();
        break;
      case 'w':
        printf("Backward\n\r");
        break;
      case '.':
        printf("Stop\n\r");
        navigator->stop();
        break;
      }
    }
  }
};

class ButtonsController : public Servicable {
private:
  PlatformMotionController *platform;
  ESC *esc;
  Navigator *navigator;
  Latch button1;
  Latch button2;

public:
  ButtonsController(PlatformMotionController &platform, ESC &esc, Navigator &navigator) : platform(&platform), esc(&esc), navigator(&navigator), button1(SWITCH_1_PIN), button2(SWITCH_2_PIN) {
  }

  void begin() {
  }

  void service() {
    if (button1.goneLow()) {
      if (navigator->isMoving()) {
        printf("Switch #1, stop\n\r");
        navigator->stop();
      }
      else {
        printf("Switch #1, search\n\r");
        navigator->search();
      }
    }

    if (button2.goneLow()) {
      if (navigator->isMoving()) {
        printf("Switch #2, stop\n\r");
      }
      else {
        printf("Switch #2, nothing\n\r");
      }
    }
  }
};

static MotorController rightMotor(11, 4, 5);
static MotorController leftMotor(6, 7, 8);
static IMU imu;
static PlatformMotionController platform(leftMotor, rightMotor);
static ESC esc(platform, 0, 1);
static Eyes eyes(12, 9);
static Navigator navigator(platform, esc, imu, eyes);
static SerialController serialController(platform, esc, navigator, eyes);
static ButtonsController buttonsController(platform, esc, navigator);
static uint32_t previousPlatformDebug;

void setup() {
  serialController.begin();
  buttonsController.begin();
  esc.begin();
  eyes.begin();
  platform.begin();
  serialController.ready();
  imu.begin();
  previousPlatformDebug = millis();
}

void loop() {
  imu.service();
  serialController.service();
  buttonsController.service();
  platform.service();
  esc.service();
  eyes.service();
  navigator.service();

  // #define SERIAL_DEBUG
  #if defined(SERIAL_DEBUG)
  if (millis() - previousPlatformDebug > 1000) {
    platform.debug();
    previousPlatformDebug = millis();
  }
  #endif
}

