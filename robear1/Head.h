#ifndef PAN_TILT_H
#define PAN_TILT_H

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

#endif
