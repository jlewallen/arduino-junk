#ifndef VIRTUAL_BUMPER_H
#define VIRTUAL_BUMPER_H

#define VIRTUAL_BUMPER_NUMBER_OF_SAMPLES 10
#define VIRTUAL_BUMPER_SAMPLE_TIME       250

#include <LSM303.h>

class VirtualBumperSensor {
private:
  LSM303 sensor;
  uint32_t previous;
  int16_t samples[VIRTUAL_BUMPER_NUMBER_OF_SAMPLES][3];
  int16_t offset[3];
  float filtered[VIRTUAL_BUMPER_NUMBER_OF_SAMPLES][3];
  float rolling[3];
  uint8_t blocked;

public:
  VirtualBumperSensor() :
    previous(0), blocked(0) {
    rolling[0] = rolling[1] = rolling[2] = 0.0;
  }

  void begin() {
    sensor.init();
    sensor.writeMagReg(LSM303_MR_REG_M,    0b00000000); // continuous conversion mode
    sensor.writeAccReg(LSM303_CTRL_REG1_A, 0b00100111); // normal power mode, all axes enabled, 50Hz
    sensor.writeAccReg(LSM303_CTRL_REG4_A, 0b00110000); // 8g full scale

    calibrate();
  }

  void calibrate() {
    memzero(samples, sizeof(samples));
    memzero(offset, sizeof(offset));
    /*
    for (byte i = 0; i < 32; ++i) {
      sensor.readAcc();
      offset[0] += sensor.a.x;
      offset[1] += sensor.a.y;
      offset[2] += sensor.a.z;
      delay(30);
    }
    offset[0] /= 32;
    offset[1] /= 32;
    offset[2] /= 32;
    */
  }

  uint8_t isBlocked() {
    return blocked;
  }

  void service() {
    if (millis() - previous < (VIRTUAL_BUMPER_SAMPLE_TIME / VIRTUAL_BUMPER_NUMBER_OF_SAMPLES)) {
      return;
    }

    for (uint8_t i = VIRTUAL_BUMPER_NUMBER_OF_SAMPLES - 1; i > 0; --i) {
      samples[i][0] = samples[i - 1][0];
      samples[i][1] = samples[i - 1][1];
      samples[i][2] = samples[i - 1][2];
    }
    sensor.readAcc();
    samples[0][0] = sensor.a.x - offset[0];
    samples[0][1] = sensor.a.y - offset[1];
    samples[0][2] = sensor.a.z - offset[2];
    previous = millis();

    #define kAlpha 0.1f
    filtered[0][0] = 0.0f;
    filtered[0][1] = 0.0f;
    filtered[0][2] = 0.0f;
    for (uint8_t i = 1; i < VIRTUAL_BUMPER_NUMBER_OF_SAMPLES; ++i) {
      filtered[i][0] = kAlpha * (filtered[i-1][0] + samples[i][0] - samples[i-1][0]);
      filtered[i][1] = kAlpha * (filtered[i-1][1] + samples[i][1] - samples[i-1][1]);
      filtered[i][2] = kAlpha * (filtered[i-1][2] + samples[i][2] - samples[i-1][2]);
    }

    uint8_t spikes = 0;
    for (uint8_t i = 0; i < VIRTUAL_BUMPER_NUMBER_OF_SAMPLES; ++i) {
      spikes += ((filtered[i][0] > 5) || (filtered[i][1] > 5)) ? 1 : 0;
    }

    if (spikes >= 2) {
      #ifdef VIRTUAL_BUMPER_DEBUG
      printf("%2d ", spikes);
      for (uint8_t i = 0; i < VIRTUAL_BUMPER_NUMBER_OF_SAMPLES; ++i) {
        printf("(%3d, %3d) ", (int16_t)filtered[i][0], (int16_t)filtered[i][1]);
      }
      printf("\n\r");
      #endif
      blocked = true;
    }
    else {
      blocked = false;
    }
  }
};

#endif
