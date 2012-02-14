/**
 * IMU class shamelessly stolen from https://github.com/pololu/MinIMU-9-Arduino-AHRS
 */
#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Wire.h>
#include <L3G4200D.h>
#include <LSM303.h>
#include "MathLibrary.h"
#include "Servicable.h"

#define GRAVITY      256  // 1G in the raw data coming from the accelerometer 
#define TO_RAD(x)    ((x) * 0.01745329252)  // *pi/180
#define TO_DEG(x)    ((x) * 57.2957795131)  // *180/pi

// L3G4200D gyro: 2000 dps full scale 70 mdps/digit; 1 dps = 0.07
#define GYRO_GAIN_X 0.07
#define GYRO_GAIN_Y 0.07
#define GYRO_GAIN_Z 0.07
#define GYRO_SCALED_X(x) ((x) * TO_RAD(GYRO_GAIN_X)) // return the scaled ADC raw data of the gyro in radians for second
#define GYRO_SCALED_Y(x) ((x) * TO_RAD(GYRO_GAIN_Y)) // return the scaled ADC raw data of the gyro in radians for second
#define GYRO_SCALED_Z(x) ((x) * TO_RAD(GYRO_GAIN_Z)) // return the scaled ADC raw data of the gyro in radians for second

// M min X: -490 Y: -685 Z: -387 M max X: 461 Y: 266 Z: 562
// LSM303 magnetometer calibration constants; use the Calibrate example from
// the Pololu LSM303 library to find the right values for your board
#define M_X_MIN     -490
#define M_Y_MIN     -685
#define M_Z_MIN     -387
#define M_X_MAX      461
#define M_Y_MAX      266
#define M_Z_MAX      562
#define Kp_ROLLPITCH 0.02
#define Ki_ROLLPITCH 0.00002
#define Kp_YAW       1.2
#define Ki_YAW       0.00002

class IMU : public Servicable {
private:
  static const int16_t sensorSigns[9];

  L3G4200D ourGyro;
  LSM303 ourCompass;
  int16_t data[6];
  int16_t dataOffset[6];
  int16_t gyro[3];
  int16_t accelerometer[3];
  int16_t compass[3];

  Vector3<float> magnetometerCorrected;
  Vector3<float> accelVector; // Store the acceleration in a vector
  Vector3<float> gyroVector; // Store the gyros turn rate in a vector
  Vector3<float> omegaVector; // Corrected gyroVector data
  Vector3<float> omegaP; // Omega Proportional correction
  Vector3<float> omegaI; // Omega Integrator
  Vector3<float> omega;
  Matrix3x3<float> dcm;
  Matrix3x3<float> update;
  Matrix3x3<float> temporary;
  Vector3<float> errorRollPitch; 
  Vector3<float> errorYaw;
  // euler angles
  float roll;
  float pitch;
  float yaw;

  // integration time (DCM algorithm), we will run the integration loop at 50Hz if possible
  float dt;
  float heading;
  uint32_t timer;
  uint32_t debuggingTimer;
  uint32_t timerOld;
  uint8_t counter;

public:
  float getHeading() { return heading; }
  float getYaw() { return yaw; }
  float getPitch() { return pitch; }
  float getRoll() { return roll; }

public:
  IMU();

private:
  void initialize();
  void calibrate();
  void readGyro();
  void readAccelerometer();
  void readCompass();

private:
  void calculateHeading();
  void normalize();
  void driftCorrection();
  void matrixUpdate();
  void eulerAngles();
  void print();

public:
  void begin();
  void service();
};

#endif
