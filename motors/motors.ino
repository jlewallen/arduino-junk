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

  void begin() {
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
    button1.begin();
    button2.begin();
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

int main(void) {
	init();

  MotorController rightMotor(11, 4, 5);
  MotorController leftMotor(6, 7, 8);
  IMU imu;
  PlatformMotionController platform(leftMotor, rightMotor);
  ESC esc(platform, 0, 1);
  Eyes eyes(12, 9);
  Navigator navigator(platform, esc, imu, eyes);
  SerialController serialController(platform, esc, navigator, eyes);
  ButtonsController buttonsController(platform, esc, navigator);
  uint32_t previousPlatformDebug;

  serialController.begin();
  buttonsController.begin();
  esc.begin();
  eyes.begin();
  platform.begin();
  serialController.ready();
  imu.begin();
  previousPlatformDebug = millis();

	for (;;) {
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
	return 0;
}
