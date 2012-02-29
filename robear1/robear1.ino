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

volatile int16_t encodersLeftState = false;
volatile int16_t encodersRightState = false;
volatile uint32_t encodersLeftCounter = 0;
volatile uint32_t encodersRightCounter = 0;

#define ENCODER_0_PIN        2
#define ENCODER_1_PIN        3
#define ENCODER_0_INTERRUPT  0
#define ENCODER_1_INTERRUPT  1

static void leftChange() {
  int16_t value = digitalRead(ENCODER_1_PIN);
  if (value != encodersLeftState) {
    encodersLeftCounter++;
    encodersLeftState = value;
  }
}

static void rightChange() {
  int16_t value = digitalRead(ENCODER_0_PIN);
  if (value != encodersRightState) {
    encodersRightCounter++;
    encodersRightState = value;
  }
}

class Encoders : public Servicable {
private:
  uint32_t previous;
  uint32_t leftCounter;
  uint32_t rightCounter;

public:
  Encoders() {
  }

public:
  uint32_t getLeft() {
    return leftCounter;
  }

  uint32_t getRight() {
    return rightCounter;
  }

  void clear() {
    noInterrupts();
    leftCounter = rightCounter = 0;
    encodersLeftCounter = 0;
    encodersRightCounter = 0;
    interrupts();
  }

  void begin() {
    attachInterrupt(ENCODER_0_INTERRUPT, rightChange, CHANGE);
    attachInterrupt(ENCODER_1_INTERRUPT, leftChange, CHANGE);
    pinMode(ENCODER_0_PIN, INPUT);
    pinMode(ENCODER_1_PIN, INPUT);
    digitalWrite(ENCODER_0_PIN, HIGH);
    digitalWrite(ENCODER_1_PIN, HIGH);
  }

  void service() {
    noInterrupts();
    leftCounter = encodersLeftCounter;
    rightCounter = encodersRightCounter;
    interrupts();
  }
};

class MaxSonar : public Servicable {
protected:
  uint8_t pin;
  uint32_t accumulator;
  uint32_t samples;
  uint32_t previous;
  float distance;

public:
  MaxSonar(uint8_t pin) : pin(pin) {
    accumulator = 0;
    samples = 0;
  }

  float getDistance() {
    return distance;
  }

  virtual void begin() = 0;
  virtual void service() = 0;
};

class AnalogMaxSonar : public MaxSonar {
public:
  AnalogMaxSonar(uint8_t pin) : MaxSonar(pin) {
  }

  void begin() {
  }

  void service() {
    if (millis() - previous > 10) {
      accumulator += analogRead(pin);
      samples++;
      previous = millis();
    }

    if (samples == 10) {
      float average = accumulator / (float)samples;
      distance = map(average, 0, 1024, 0, 700);
      accumulator = 0;
      samples = 0;
    }
  }
};

class DigitalMaxSonar : public MaxSonar {
private:
  boolean previousValue;

public:
  DigitalMaxSonar(uint8_t pin) : MaxSonar(pin) {
  }

  void begin() {
    pinMode(pin, INPUT);
  }

  void service() {
    boolean value = digitalRead(pin);
    if (value != previousValue) {
      if (value) {
        previous = micros();
      }
      else {
        accumulator += (micros() - previous) / 58;
        samples++;
      }
      previousValue = value;
    }

    if (samples == 5) {
      float average = accumulator / (float)samples;
      distance = average;
      accumulator = 0;
      samples = 0;
    }
  }
};

#define PITCH_MIN  15
#define PITCH_MAX 170
#define YAW_MIN    20
#define YAW_MAX   170

class Head : public Servicable {
private:
  uint32_t previousUpdate;
  Servo yawServo;
  Servo pitchServo;
  uint8_t pitch;
  uint8_t yaw;

public:
  Head() : previousUpdate(0) {
    lookStraight();
  }

  void begin() {
    yawServo.attach(9);
    pitchServo.attach(10);
  }

  void service() {
    pitchServo.write(pitch);
    yawServo.write(yaw);
  }

  void lookUp() {
    pitch = constrain(pitch - 10, PITCH_MIN, PITCH_MAX);
    DPRINTF("Pitch: %d\n\r", pitch);
  }

  void lookDown() {
    pitch = constrain(pitch + 10, PITCH_MIN, PITCH_MAX);
    DPRINTF("Pitch: %d\n\r", pitch);
  }

  void lookLeft() {
    yaw = constrain(yaw + 10, YAW_MIN, YAW_MAX);
    DPRINTF("Yaw: %d\n\r", pitch);
  }

  void lookRight() {
    yaw = constrain(yaw - 10, YAW_MIN, YAW_MAX);
    DPRINTF("Yaw: %d\n\r", pitch);
  }

  void lookStraight() {
    pitch = 56;
    yaw = 100;
  }
};

typedef struct {
  uint8_t speed1;
  uint8_t speed2;
  uint8_t motor1 : 1;
  uint8_t motor2 : 1;
  uint32_t duration;
} motion_command_t;

motion_command_t stopCommand = {
  0, 0, false, false, 0
};

motion_command_t forwardCommand = {
  100, 100, true, true, 0
};

motion_command_t backwardCommand = {
  100, 100, false, false, 0
};

motion_command_t leftCommand = {
  125, 100, false, true, 0
};

motion_command_t rightCommand = {
  125, 100, true, false, 0
};

class MotionController : public Servicable {
private:
  uint8_t speed1;
  uint8_t speed2;
  uint8_t motor1;
  uint8_t motor2;
  motion_command_t active;
  uint32_t startedAt;

public:
  MotionController() {
    speed1 = 5;
    speed2 = 6;
    motor1 = 4;
    motor2 = 7;
    startedAt = 0;
    memzero(&active, sizeof(motion_command_t));
  }

  void begin() {
    pinMode(speed1, OUTPUT);  
    pinMode(speed2, OUTPUT);  
    pinMode(motor1, OUTPUT);  
    pinMode(motor2, OUTPUT);  
  }

  void service() {
    if (active.duration > 0) {
      if (millis() - startedAt > active.duration) {
        active.duration = 0;
        execute(&stopCommand);
      }
    }
  }

  void execute(motion_command_t *c) {
    execute(c, 0);
  }

  void execute(motion_command_t *c, uint32_t duration) {
    memcpy(&active, c, sizeof(motion_command_t));
    analogWrite(speed1, active.speed1);
    analogWrite(speed2, active.speed1);
    digitalWrite(motor1, active.motor1);
    digitalWrite(motor2, active.motor2);
    active.duration = duration;
    startedAt = millis();
  }
};

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
  Encoders *encoders;
  MaxSonar *sonar;
  boolean enabled;
  uint32_t previous;

public:
  DebugController(Head &head, Encoders &encoders, MaxSonar &sonar) :
    head(&head), encoders(&encoders), sonar(&sonar), enabled(false), previous(0) {
  }

  void begin() {
  }

  void service() {
    #ifdef DEBUG
    if (!enabled) {
      return;
    }
    if (millis() - previous > 500) {
      printf("s=%7.3f l=%5lu r=%5lu\n\r", sonar->getDistance(), encoders->getLeft(), encoders->getRight());
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

#define LIGHT_BOARD_ADDRESS 1

class Display {
private:
public:
  void begin() {
  }

  void ready() {
    for (byte i = 0; i < 3; ++i) {
      send(0b00101111);
      delay(50);
      send(0);
      delay(100);
    }
  }

  void send(byte b) {
    Wire.beginTransmission(LIGHT_BOARD_ADDRESS);
    Wire.write(b);
    Wire.endTransmission();
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
  Encoders *encoders;
  MaxSonar *sonar;
  Head *head;
  Obstructions *obstructions;
  Display *display;

public:
  Navigator(MotionController &motion, Encoders &encoders, MaxSonar &sonar, Head &head, Obstructions &obstructions, Display &display) :
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
    stallClear();
    switch (state) {
    case Stopped:
      DPRINTF("Stopped\n\r");
      motion->execute(&stopCommand);
      break;
    case Stalled:
      DPRINTF("Stalled\n\r");
      motion->execute(&stopCommand);
      break;
    case Searching:
      DPRINTF("Searching\n\r");
      motion->execute(&forwardCommand);
      break;
    case Obstructed:
      DPRINTF("Obstructed\n\r");
      motion->execute(&stopCommand);
      transitionAfter(Avoiding, 750);
      break;
    case Avoiding:
      DPRINTF("Avoiding\n\r");
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
      checkForStall();
      break;
    case Obstructed:
      break;
    case Avoiding:
      if (!obstructions->isBlocked()) {
        transition(Stopped);
      }
      checkForStall();
      break;
    case Turning:
      checkForStall();
      break;
    }
  }

private:
  uint32_t previous;
  uint32_t previousLeft;
  uint32_t previousRight;

  void stallClear() {
    previousLeft = encoders->getLeft();
    previous = millis();
    previousRight = encoders->getRight();
  }

  void checkForStall() {
    if (millis() - previous > 1000) {
      float leftSpeed = (encoders->getLeft() - previousLeft) / 1000.0;
      float rightSpeed = (encoders->getRight() - previousRight) / 1000.0;
      DPRINTF("Speed: %f %f\n\r", leftSpeed, rightSpeed);
      stallClear();
      if (leftSpeed < 0.005 || rightSpeed < 0.005) {
        transition(Stalled);
      }
    }
  }
};

uint16_t keyRanges[5] = { 30, 150, 360, 535, 760 };

class ButtonsController : public Servicable {
private:
  uint32_t previous;
  int8_t pressedPreviously;
  Navigator *navigator;

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
  ButtonsController(Navigator &navigator) :
    previous(0), pressedPreviously(0), navigator(&navigator) {
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
  Encoders encoders;
  DigitalMaxSonar sonar(11);
  Head head;
  Obstructions obstructions(12);
  DebugController debug(head, encoders, sonar);
  Navigator navigator(motionController, encoders, sonar, head, obstructions, display);
  SerialController serialController(head, motionController, debug, navigator);
  ButtonsController buttonsController(navigator);

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
  serialController.ready();
  display.ready();

	for (;;) {
    serialController.service();
    encoders.service();
    buttonsController.service();
    sonar.service();
    head.service();
    obstructions.service();
    navigator.service();
    motionController.service();
    debug.service();
	}
	return 0;
}
