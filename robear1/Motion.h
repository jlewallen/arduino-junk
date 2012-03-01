#ifndef MOTION_H
#define MOTION_H

typedef struct {
  int16_t speed1;
  int16_t speed2;
  uint16_t duration;
} motion_command_t;

motion_command_t stopCommand = { 0, 0, 0 };
motion_command_t forwardCommand = { 90, 90, 0 };
motion_command_t backwardCommand = { -100, -100, 0 };
motion_command_t leftCommand = { -125, 100, 0 };
motion_command_t rightCommand = { 100, -125, 0 };

#define PIN_SPEED1  5
#define PIN_SPEED2  6
#define PIN_MOTOR1  4
#define PIN_MOTOR2  7
#define SIGN(v)     ((v) >= 0 ? 1 : -1)

class MotionController : public Servicable {
private:
  motion_command_t active;
  motion_command_t lastMotion;
  uint32_t startedAt;

public:
  MotionController() {
    startedAt = 0;
    memzero(&active, sizeof(motion_command_t));
    memzero(&lastMotion, sizeof(motion_command_t));
  }

  int8_t getLeftSign() {
    return SIGN(lastMotion.speed1);
  }

  int8_t getRightSign() {
    return SIGN(lastMotion.speed2);
  }

  void begin() {
    pinMode(PIN_SPEED1, OUTPUT);  
    pinMode(PIN_SPEED2, OUTPUT);  
    pinMode(PIN_MOTOR1, OUTPUT);  
    pinMode(PIN_MOTOR2, OUTPUT);  
  }

  void service() {
    if (active.duration > 0) {
      if (millis() - startedAt > active.duration) {
        active.duration = 0;
        execute(&stopCommand);
      }
    }
  }

  void control(int16_t speed1, int16_t speed2) {
    active.speed1 = speed1;
    active.speed2 = speed2;
    execute(&active);
  }

  void execute(motion_command_t *c) {
    execute(c, 0);
  }

  void execute(motion_command_t *c, uint32_t duration) {
    memcpy(&active, c, sizeof(motion_command_t));
    int16_t s1 = abs(active.speed1);
    int16_t s2 = abs(active.speed2);
    if (s1 > 0 || s2 > 0) {
      memcpy(&lastMotion, c, sizeof(motion_command_t));
    }
    analogWrite(PIN_SPEED1, 0);
    analogWrite(PIN_SPEED2, 0);
    digitalWrite(PIN_MOTOR1, active.speed1 > 0);
    digitalWrite(PIN_MOTOR2, active.speed2 > 0);
    analogWrite(PIN_SPEED1, s1);
    analogWrite(PIN_SPEED2, s2);
    active.duration = duration;
    startedAt = millis();
  }
};

#endif
