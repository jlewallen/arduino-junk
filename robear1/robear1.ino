/**
 *
 */
#include <Arduino.h>
#include <printf.h>
#include <Servo.h>
#include <Wire.h>
#include <Servicable.h>
#include <Debuggable.h>
#include <StateMachine.h>
#include "../blink_tiny/Blink.h"

#include "Motion.h"
#include "Encoding.h"
#include "SpeedController.h"
#include "Sonar.h"
#include "Head.h"
#include "Behavior.h"
#include "Display.h"
#include "FrontInfrared.h"

class DebugController : public Servicable {
private:
  Head *head;
  encoding::Encoders *encoders;
  MaxSonar *sonar;
  boolean enabled;
  uint32_t previous;

public:
  DebugController(Head &head, encoding::Encoders &encoders, MaxSonar &sonar) :
    head(&head), encoders(&encoders), sonar(&sonar), enabled(false), previous(0) {
  }

  void begin() {
  }

  void service() {
    #ifdef DEBUG
    if (!enabled) {
      return;
    }
    if (millis() - previous > 100) {
      printf("s=%7.3f l=%5d r=%5d l=%lu r=%lu\n\r",
             sonar->getDistance(),
             encoders->getLeftVelocity(),
             encoders->getRightVelocity(),
             encoders->getLeftCounterTotal(),
             encoders->getRightCounterTotal()
             );
      previous = millis();
    }
    #endif
  }

  void toggle() {
    #ifdef DEBUG
    enabled = !enabled;
    #endif
  }
};

uint16_t keyRanges[5] = { 30, 150, 360, 535, 760 };

class ButtonsController : public Servicable {
private:
  uint32_t previous;
  int8_t pressedPreviously;
  MotionController *motion;
  SpeedController *speed;
  behaviors::User *user;

private:
  int8_t analogToButton(uint16_t value) {
    for (int8_t k = 0; k < 5; ++k) {
      if (value < keyRanges[k]) {  
        return k;  
      }
    }
    return -1;
  }

public:
  ButtonsController(MotionController &motion, SpeedController &speed, behaviors::User &user) :
    previous(0), pressedPreviously(0), motion(&motion), speed(&speed), user(&user) {
  }

  void begin() {
  }

  void service() {
    int8_t pressed = analogToButton(analogRead(7));
    if (pressed != pressedPreviously) {
      if (pressed != -1) {
        button(pressed);
      }
      pressedPreviously = pressed;
    }
  }

  void button(uint8_t button) {
    switch (button) {
    case 0:
      user->toggle();
      break;
    case 1:
      motion->execute(&forwardCommand, 5000);
      break;
    case 2:
      speed->control(10, 10);
      break;
    case 3:
      break;
    case 4:
      break;
    }
  }
};

class SerialController : public Servicable {
private:
  Head *head;
  MotionController *motion;
  DebugController *debug;

public:
  SerialController() {
    head = NULL;
    motion = NULL;
    debug = NULL;
  }

  SerialController(Head &head, MotionController &motion, DebugController &debug) :
    head(&head), motion(&motion), debug(&debug) {
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
      case 'w':
        motion->execute(&forwardCommand, 500);
        break;
      case 's':
        motion->execute(&backwardCommand, 500);
        break;
      case 'a':
        motion->execute(&leftCommand, 500);
        break;       
      case 'd':
        motion->execute(&rightCommand, 500);
        break;          
      case '.':
        motion->execute(&stopCommand);
        head->lookStraight();
        break;          
      case 'j':
        head->lookLeft();
        break;
      case 'k':
        head->lookRight();
        break;
      case 'm':
        head->lookDown();
        break;
      case 'n':
        head->lookUp();
        break;
      case 'v':
        debug->toggle();
        break;
      case '1':
        break;
      case '0':
        break;
      }     
    }
  }
};

int16_t main(void) {
	init();

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  SerialController serial;
  serial.begin();

  behaviors::Bumper bumper;
  behaviors::User user;

  Display display;
  MotionController motionController;
  encoding::Encoders encoders(motionController);
  SpeedController speedController(encoders, motionController);
  ButtonsController buttons(motionController, speedController, user);

  Wire.begin();

  bumper.begin();
  user.begin();

  display.begin();
  motionController.begin();
  encoders.begin();
  speedController.begin();
  buttons.begin();
  display.ready();
  serial.ready();

  uint32_t sensorHz = 0;
  uint32_t motionHz = 0;

	for (;;) {
    uint32_t now = millis();
    if (now - sensorHz > (1000 / 20)) {
      sensorHz = now;
      buttons.service();
      serial.service();
    }
    if (now - motionHz > (1000 / 5)) {
      motionHz = now;

      bumper.service();
      user.service();

      behaviors::behavior_command_t *selected = NULL;
      if (user.getCommand()->enabled) {
        selected = user.getCommand();
      }
      if (bumper.getCommand()->enabled) {
        selected = bumper.getCommand();
      }
      if (selected != NULL) {
        int16_t left = selected->velocity + selected->rotation;
        int16_t right = selected->velocity - selected->rotation;
        speedController.control(left, right);
      }

      encoders.service();
      motionController.service();
      speedController.service();
    }
	}
	return 0;
}
