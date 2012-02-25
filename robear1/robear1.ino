/**
 *
 */
#include <Arduino.h>
#include <printf.h>
#include <Servo.h>
#include <Servicable.h>
#include <Debuggable.h>

volatile int16_t encodersLeftState = false;
volatile int16_t encodersRightState = false;
volatile uint32_t encodersLeftCounter = 0;
volatile uint32_t encodersRightCounter = 0;

static void leftChange() {
  int16_t value = digitalRead(3);
  if (value != encodersLeftState) {
    encodersLeftCounter++;
    encodersLeftState = value;
  }
}

static void rightChange() {
  int16_t value = digitalRead(2);
  if (value != encodersRightState) {
    encodersRightCounter++;
    encodersRightState = value;
  }
}

class Encoders : public Servicable {
private:
  uint32_t previous;
  uint8_t leftIn;
  uint8_t rightIn;
  uint32_t leftCounter;
  uint32_t rightCounter;

public:
  Encoders(uint8_t left, uint8_t right) : leftIn(left), rightIn(right) {
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
    attachInterrupt(leftIn, leftChange, CHANGE);
    attachInterrupt(rightIn, rightChange, CHANGE);
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
  }

  void service() {
    noInterrupts();
    leftCounter = encodersLeftCounter;
    rightCounter = encodersRightCounter;
    interrupts();
  }
};

uint16_t keyRanges[5] = { 30, 150, 360, 535, 760 };

class ButtonsController : public Servicable {
private:
  uint32_t previous;
  int8_t pressedPreviously;

  int8_t analogToButton(uint16_t value) {
    for (int8_t k = 0; k < 5; ++k) {
      if (value < keyRanges[k]) {  
        return k;  
      }
    }
    return -1;
  }

public:
  ButtonsController() : previous(0), pressedPreviously(0) {
  }

  void begin() {
    pinMode(13, OUTPUT);
  }

  void service() {
    int8_t pressed = analogToButton(analogRead(7));
    if (pressed != pressedPreviously) {
      if (pressed == -1) {
        button(pressed);
      }
      pressedPreviously = pressed;
    }
  }

  void button(uint8_t button) {
    switch (button) {
    case 0:
      break;
    case 1:
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

motion_command_t stop = {
  0, 0, false, false, 0
};

motion_command_t forward = {
  100, 100, true, true, 0
};

motion_command_t backward = {
  100, 100, false, false, 0
};

motion_command_t left = {
  100, 100, false, true, 0
};

motion_command_t right = {
  100, 100, true, false, 0
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
        execute(&stop);
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

class SerialController : public Servicable {
private:
  Head *head;
  MotionController *motion;
  DebugController *debug;

public:
  SerialController(Head &head, MotionController &motion, DebugController &debug) : head(&head), motion(&motion), debug(&debug) {
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
        motion->execute(&forward, 500);
        break;
      case 's':
        motion->execute(&backward, 500);
        break;
      case 'a':
        motion->execute(&left, 500);
        break;       
      case 'd':
        motion->execute(&right, 500);
        break;          
      case '.':
        motion->execute(&stop);
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
      }     
    }
  }
};

class Navigator : public Servicable {
private:
  MotionController *motion;
  Encoders *encoders;
  MaxSonar *sonar;
  Head *head;

public:
  Navigator(MotionController &motion, Encoders &encoders, MaxSonar &sonar, Head &head) :
    motion(&motion), encoders(&encoders), sonar(&sonar), head(&head) {
  }

  void begin() {
  }

  void service() {
  }
  
  void search() {
  }
};

int16_t main(void) {
	init();

  MotionController motionController;
  ButtonsController buttonsController;
  Encoders encoders(1, 0);
  DigitalMaxSonar sonar(11);
  Head head;
  DebugController debug(head, encoders, sonar);
  SerialController serialController(head, motionController, debug);

  motionController.begin();
  buttonsController.begin();
  encoders.begin();
  sonar.begin();
  head.begin();
  serialController.begin();
  debug.begin();
  serialController.ready();

	for (;;) {
    serialController.service();
    encoders.service();
    buttonsController.service();
    sonar.service();
    head.service();
    motionController.service();
    debug.service();
	}
	return 0;
}
