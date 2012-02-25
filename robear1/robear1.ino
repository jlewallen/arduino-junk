/**
 *
 */
#include <Arduino.h>
#include <printf.h>
#include <Servo.h>
#include <Servicable.h>
#include <Debuggable.h>

volatile int16_t leftValue = false;
volatile int16_t rightValue = false;
volatile uint32_t left = 0;
volatile uint32_t right = 0;

static void leftChange() {
  int16_t value = digitalRead(3);
  if (value != leftValue) {
    left++;
    leftValue = value;
  }
}

static void rightChange() {
  int16_t value = digitalRead(2);
  if (value != rightValue) {
    right++;
    rightValue = value;
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
    left = right = 0;
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
    leftCounter = left;
    rightCounter = left;
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

class MotionController : public Servicable {
private:
  uint8_t speed1;
  uint8_t speed2;
  uint8_t motor1;
  uint8_t motor2;

public:
  MotionController() {
    speed1 = 5;
    speed2 = 6;
    motor1 = 4;
    motor2 = 7;
  }

  void begin() {
    pinMode(speed1, OUTPUT);  
    pinMode(speed2, OUTPUT);  
    pinMode(motor1, OUTPUT);  
    pinMode(motor2, OUTPUT);  
  }

  void service() {
  }

  void stop(void)
  {
    analogWrite(speed1, 0);
    analogWrite(speed2, 0);    
    digitalWrite(motor1, LOW);   
    digitalWrite(motor2, LOW);      
  }   
  
  void backward(char a, char b)
  {
    analogWrite(speed1, a);
    digitalWrite(motor1, LOW);    
    analogWrite(speed2, b);    
    digitalWrite(motor2, LOW);
  }  
  
  void forward(char a, char b)
  {
    analogWrite(speed1, a);
    digitalWrite(motor1, HIGH);   
    analogWrite(speed2, b);    
    digitalWrite(motor2, HIGH);
  }
  
  void turnLeft(char a, char b)
  {
    stop();
    digitalWrite(motor1, LOW);    
    digitalWrite(motor2, HIGH);
    analogWrite(speed1, a);
    analogWrite(speed2, b);    
  }
  
  void turnRight(char a, char b)
  {
    stop();
    digitalWrite(motor1, HIGH);    
    digitalWrite(motor2, LOW);
    analogWrite(speed1, a);
    analogWrite(speed2, b);    
  }
};

class DebugController : public Servicable {
private:
public:
  void begin() {
  }

  void service() {
  }
};

class SerialController : public Servicable {
private:
  Head *head;
  MotionController *motion;

public:
  SerialController(Head &head, MotionController &motion) : head(&head), motion(&motion) {
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
        motion->forward(100, 100);
        delay(500);
        motion->stop();
        break;
      case 's':
        motion->backward(100, 100);
        delay(500);
        motion->stop();
        break;
      case 'a':
        motion->turnLeft(125, 125);
        delay(500);
        motion->stop();
        break;       
      case 'd':
        motion->turnRight(125, 125);
        delay(500);
        motion->stop();
        break;          
      case '.':
        motion->stop();
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
      }     
      /*
      case 'a':
        printf("Nudge Left\n\r");
        // platform->execute(&Left, 600);
        break;
      case 's':
        printf("Nudge Right\n\r");
        // platform->execute(&Right, 600);
        break;
      case 'g':
        printf("Nudge Forward\n\r");
        // platform->execute(&Forward, 500);
        break;
      case 'h':
        printf("Nudge Backward\n\r");
        // platform->execute(&Backward, 1000);
        break;
      case 'z':
        // eyes->lookNext();
        break;
      case 'c':
        // eyes->lookForward();
        break;
      case 'q':
        printf("Forward\n\r");
        // navigator->search();
        break;
      case 'w':
        printf("Backward\n\r");
        break;
      case '.':
        printf("Stop\n\r");
        // navigator->stop();
        break;
      }
      */
    }
  }
};

int16_t main(void) {
	init();

  MotionController motionController;
  ButtonsController buttonsController;
  Encoders encoders(1, 0);
  DigitalMaxSonar sonar(11);
  Head head;
  SerialController serialController(head, motionController);

  motionController.begin();
  buttonsController.begin();
  encoders.begin();
  sonar.begin();
  head.begin();
  serialController.begin();
  serialController.ready();

	for (;;) {
    serialController.service();
    encoders.service();
    buttonsController.service();
    sonar.service();
    head.service();
    motionController.service();
	}
	return 0;
}
