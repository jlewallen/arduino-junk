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
#include "../blink_module/Blink.h"

#include "Motion.h"
#include "Encoding.h"
#include "SpeedController.h"
#include "Sonar.h"
#include "Head.h"
#include "Behavior.h"
#include "Display.h"
#include "FrontInfrared.h"

uint16_t keyRanges[5] = { 30, 150, 360, 535, 760 };

class ButtonsController : public Servicable {
private:
  uint32_t previous;
  int8_t pressedPreviously;
  MotionController *motion;
  SpeedController *speed;
  behaviors::UserBehavior *userBehavior;

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
  ButtonsController(MotionController &motion, SpeedController &speed, behaviors::UserBehavior &userBehavior) :
    previous(0), pressedPreviously(0), motion(&motion), speed(&speed), userBehavior(&userBehavior) {
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
      userBehavior->toggle();
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
  MotionController *motion;
  behaviors::UserBehavior *userBehavior;

public:
  SerialController(MotionController &motion, behaviors::UserBehavior &userBehavior) :
    motion(&motion), userBehavior(&userBehavior) {
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
        break;          
      case 'j':
        break;
      case 'k':
        break;
      case 'm':
        break;
      case 'n':
        break;
      case 'v':
        break;
      case '1':
        break;
      case '0':
        userBehavior->toggle();
        break;
      }     
    }
  }
};

class VirtualAndPhysicalBumperSensor : public ObstructionSensor {
private:
  VirtualBumperSensor virtualSensor;
  BumperSensor physicalSensor;

public:
  void begin() {
    physicalSensor.begin();
    virtualSensor.begin();
  }

  void service() {
    physicalSensor.service();
    virtualSensor.service();
  }

  uint8_t isCenterOrBothBlocked() {
    return physicalSensor.isCenterOrBothBlocked() || virtualSensor.isBlocked();
  }
  
  uint8_t isLeftBlocked() {
    return physicalSensor.isLeftBlocked();
  }
  
  uint8_t isRightBlocked() {
    return physicalSensor.isRightBlocked();
  }
};

int16_t main(void) {
	init();

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  Wire.begin();

  VirtualAndPhysicalBumperSensor obstructionSensor;
  behaviors::UserBehavior userBehavior;
  behaviors::BumperBehavior bumperBehavior(obstructionSensor);
  behaviors::LocalMinimumBehavior localMinimumBehavior(bumperBehavior);
  Display display;
  MotionController motionController;
  encoding::Encoders encoders(motionController);
  SpeedController speedController(encoders, motionController);
  ButtonsController buttons(motionController, speedController, userBehavior);
  SerialController serial(motionController, userBehavior);

  serial.begin();
  bumperBehavior.begin();
  userBehavior.begin();
  localMinimumBehavior.begin();
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

    if (now - sensorHz > 5) {
      sensorHz = now;
      buttons.service();
      serial.service();
      userBehavior.service();
      bumperBehavior.service();
      localMinimumBehavior.service();
    }

    if (now - motionHz > (1000 / 5)) {
      motionHz = now;

      behaviors::behavior_command_t *selected = NULL;
      if (userBehavior.getCommand()->enabled) {
        selected = userBehavior.getCommand();
      }
      if (localMinimumBehavior.getCommand()->enabled) {
        selected = localMinimumBehavior.getCommand();
      }
      if (bumperBehavior.getCommand()->enabled) {
        selected = bumperBehavior.getCommand();
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
