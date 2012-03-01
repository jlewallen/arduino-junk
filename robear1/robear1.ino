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
#include "PanTilt.h"
#include "Behavior.h"
#include "Display.h"

class Obstructions : public Servicable {
private:
  uint8_t pin;
  uint8_t blocked;

public:
  Obstructions(uint8_t pin) : pin(pin), blocked(false) {
  }

  void begin() {
    pinMode(pin, INPUT);
  }

  uint8_t isBlocked() {
    return blocked;
  }

  void service() {
    uint8_t blockedNow = !digitalRead(pin);
    if (blockedNow != blocked) {
      blocked = blockedNow;
    }
  }
};

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

class Navigator : public Servicable, public StateMachine {
private:
  typedef enum {
    Stopped,
    Waiting,
    Searching,
    Obstructed,
    Avoiding,
    Turning,
    Stalled
  } state_t;

  MotionController *motion;
  encoding::Encoders *encoders;
  MaxSonar *sonar;
  Head *head;
  Obstructions *obstructions;
  Display *display;

public:
  Navigator(MotionController &motion, encoding::Encoders &encoders, MaxSonar &sonar, Head &head, Obstructions &obstructions, Display &display) :
    StateMachine(Stopped),
    motion(&motion), encoders(&encoders), sonar(&sonar), head(&head), obstructions(&obstructions), display(&display) {
  }

  void begin() {
  }

  void service() {
    StateMachine::service();
  }
  
  void search() {
    transitionAfter(Searching, 1000);
  }

  void stop() {
    transition(Stopped);
  }

  void entered(uint16_t state) {
    switch (state) {
    case Stopped:
      DPRINTF("Stopped\n\r");
      display->stopped();
      motion->execute(&stopCommand);
      break;
    case Stalled:
      DPRINTF("Stalled\n\r");
      display->stalled();
      motion->execute(&stopCommand);
      break;
    case Searching:
      DPRINTF("Searching\n\r");
      display->searching();
      motion->execute(&forwardCommand);
      break;
    case Obstructed:
      DPRINTF("Obstructed\n\r");
      display->obstructed();
      motion->execute(&stopCommand);
      transitionAfter(Avoiding, 750);
      break;
    case Avoiding:
      DPRINTF("Avoiding\n\r");
      display->avoiding();
      motion->execute(&backwardCommand);
      break;
    }
  }

  void service(uint16_t state, uint32_t enteredAt) {
    switch (state) {
    case Stopped:
      break;
    case Searching:
      if (obstructions->isBlocked()) {
        transition(Obstructed);
      }
      break;
    case Obstructed:
      break;
    case Avoiding:
      if (!obstructions->isBlocked()) {
        transition(Stopped);
      }
      break;
    case Turning:
      break;
    }
  }
};

uint16_t keyRanges[5] = { 30, 150, 360, 535, 760 };

class ButtonsController : public Servicable {
private:
  uint32_t previous;
  int8_t pressedPreviously;
  Navigator *navigator;
  SpeedController *speed;

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
  ButtonsController(Navigator &navigator, SpeedController &speed) :
    previous(0), pressedPreviously(0), navigator(&navigator), speed(&speed) {
  }

  void begin() {
    pinMode(13, OUTPUT);
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
      navigator->search();
      break;
    case 1:
      navigator->stop();
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
  Navigator *navigator;

public:
  SerialController(Head &head, MotionController &motion, DebugController &debug, Navigator &navigator) :
    head(&head), motion(&motion), debug(&debug), navigator(&navigator) {
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
        navigator->search();
        break;
      case '0':
        navigator->stop();
        break;
      }     
    }
  }
};

int16_t main(void) {
	init();

  Display display;
  MotionController motionController;
  encoding::Encoders encoders(motionController);
  DigitalMaxSonar sonar(11);
  Head head;
  Obstructions obstructions(12);
  DebugController debug(head, encoders, sonar);
  Navigator navigator(motionController, encoders, sonar, head, obstructions, display);
  SpeedController speedController(encoders, motionController);
  SerialController serialController(head, motionController, debug, navigator);
  ButtonsController buttonsController(navigator, speedController);

  Wire.begin();

  display.begin();
  motionController.begin();
  buttonsController.begin();
  encoders.begin();
  sonar.begin();
  head.begin();
  serialController.begin();
  obstructions.begin();
  debug.begin();
  navigator.begin();
  speedController.begin();
  serialController.ready();
  display.ready();

  uint32_t previous = 0;

	for (;;) {
    if (millis() - previous > 100) {
      previous = millis();

      serialController.service();
      encoders.service();
      buttonsController.service();
      sonar.service();
      head.service();
      obstructions.service();
      navigator.service();
      motionController.service();
      speedController.service();
      debug.service();
    }
	}
	return 0;
}
