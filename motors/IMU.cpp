#include "IMU.h"

const int16_t IMU::sensorSigns[9] = { 1, 1, 1, -1, -1, -1, 1, 1, 1 }; // x,y,z - gyro, accelerometer, magnetometer

IMU::IMU() : dt(0.02), heading(0), timer(0), debuggingTimer(0), timerOld(0), counter(0) {
}

void IMU::initialize() {
  printf("IMU: Initializing...\n\r");
  
  ourGyro.writeReg(L3G4200D_CTRL_REG1, 0x0F); // normal power mode, all axes enabled, 100 Hz
  ourGyro.writeReg(L3G4200D_CTRL_REG4, 0x20); // 2000 dps full scale

  ourCompass.init();
  ourCompass.writeMagReg(LSM303_MR_REG_M, 0x00);    // continuous conversion mode
  ourCompass.writeAccReg(LSM303_CTRL_REG1_A, 0x27); // normal power mode, all axes enabled, 50 Hz
  ourCompass.writeAccReg(LSM303_CTRL_REG4_A, 0x30); // 8 g full scale
}

void IMU::calibrate() {
  printf("IMU: Calibrating...\n\r");

  memset(dataOffset, 0, sizeof(dataOffset));

  delay(100);

  for (byte i = 0; i < 32; ++i) {
    readGyro();
    readAccelerometer();
    for (byte y = 0; y < 6; y++)
      dataOffset[y] += data[y];
    printf(".");
    delay(30);
  }

  printf("\n\r");
  for (byte y = 0; y < 6; y++)
    dataOffset[y] /= 32;
  
  dataOffset[5] -= GRAVITY * sensorSigns[5];
  delay(500);
}

void IMU::readGyro() {
  ourGyro.read();
  data[0] = ourGyro.g.x;
  data[1] = ourGyro.g.y;
  data[2] = ourGyro.g.z;
  gyro[0] = sensorSigns[0] * (data[0] - dataOffset[0]);
  gyro[1] = sensorSigns[1] * (data[1] - dataOffset[1]);
  gyro[2] = sensorSigns[2] * (data[2] - dataOffset[2]);
}

void IMU::readAccelerometer() {
  ourCompass.readAcc();
  data[3] = ourCompass.a.x;
  data[4] = ourCompass.a.y;
  data[5] = ourCompass.a.z;
  accelerometer[0] = sensorSigns[3] * (data[3] - dataOffset[3]);
  accelerometer[1] = sensorSigns[4] * (data[4] - dataOffset[4]);
  accelerometer[2] = sensorSigns[5] * (data[5] - dataOffset[5]);
}

void IMU::readCompass() {
  ourCompass.readMag();
  compass[0] = sensorSigns[6] * ourCompass.m.x;
  compass[1] = sensorSigns[7] * ourCompass.m.y;
  compass[2] = sensorSigns[8] * ourCompass.m.z;
}

void IMU::calculateHeading() {
  float cosRoll = cos(roll);
  float sinRoll = sin(roll);
  float cosPitch = cos(pitch);
  float sinPitch = sin(pitch);
  
  // adjust for LSM303 compass axis offsets/sensitivity differences by scaling to +/-0.5 range
  magnetometerCorrected[0] = (float)(compass[0] - sensorSigns[6] * M_X_MIN) / (M_X_MAX - M_X_MIN) - sensorSigns[6] * 0.5;
  magnetometerCorrected[1] = (float)(compass[1] - sensorSigns[7] * M_Y_MIN) / (M_Y_MAX - M_Y_MIN) - sensorSigns[7] * 0.5;
  magnetometerCorrected[2] = (float)(compass[2] - sensorSigns[8] * M_Z_MIN) / (M_Z_MAX - M_Z_MIN) - sensorSigns[8] * 0.5;
  
  float magneticX = magnetometerCorrected[0] * cosPitch + magnetometerCorrected[1] * sinRoll * sinPitch + magnetometerCorrected[2] * cosRoll * sinPitch;
  float magneticY = magnetometerCorrected[1] * cosRoll - magnetometerCorrected[2] * sinRoll;
  heading = atan2(-magneticY, magneticX);
}

void IMU::normalize() {
  Vector3<float> temporary[3];
  
  float error = -dcm[0].dp(dcm[1]) * 0.5;
  temporary[0] = dcm[1].scale(error);
  temporary[1] = dcm[0].scale(error);
  temporary[0] = temporary[0].add(dcm[0]);
  temporary[1] = temporary[1].add(dcm[1]);
  temporary[2] = temporary[0].cp(temporary[1]);
  
  float renormX = 0.5 * (3 - temporary[0].dp(temporary[0])); //eq.21
  dcm[0] = temporary[0].scale(renormX); // eq.21
  float renormY = 0.5 * (3 - temporary[1].dp(temporary[1])); //eq.21
  dcm[1] = temporary[1].scale(renormY); // eq.21
  float renormZ = 0.5 * (3 - temporary[2].dp(temporary[2])); //eq.21
  dcm[2] = temporary[2].scale(renormZ); // eq.21
}

void IMU::driftCorrection() {
  // Compensation the Roll, Pitch and Yaw drift. 
  Vector3<float> scaledOmegaP;
  Vector3<float> scaledOmegaI;

  // Calculate the magnitude of the accelerometer vector
  float accelMagnitude = accelVector.magnitude() / GRAVITY;

  // Dynamic weighting of accelerometer info (reliability filter)
  // Weight for accelerometer info (<0.5G = 0.0, 1G = 1.0 , >1.5G = 0.0)
  float accelWeight = constrain(1 - 2 * abs(1 - accelMagnitude), 0, 1);

  errorRollPitch = accelVector.cp(dcm[2]);
  omegaP = errorRollPitch.scale(Kp_ROLLPITCH * accelWeight);
  scaledOmegaI = errorRollPitch.scale(Ki_ROLLPITCH * accelWeight);
  omegaI = omegaI.add(scaledOmegaI);
  
  float magHeadingX = cos(heading);
  float magHeadingY = sin(heading);
  float errorCourse = (dcm[0][0] * magHeadingY) - (dcm[1][0] * magHeadingX); // calculating YAW error
  errorYaw = dcm[2].scale(errorCourse);
 
  scaledOmegaP = errorYaw.scale(Kp_YAW);
  omegaP = omegaP.add(scaledOmegaP);
  
  scaledOmegaI = errorYaw.scale(Ki_YAW);
  omegaI = omegaI.add(scaledOmegaI);
}

void IMU::matrixUpdate() {
  gyroVector.set(GYRO_SCALED_X(gyro[0]), /* gyro x roll */
                 GYRO_SCALED_Y(gyro[1]), /* gyro y pitch */
                 GYRO_SCALED_Z(gyro[2])  /* gyro Z yaw */
                 );
  accelVector.set(accelerometer[0], accelerometer[1], accelerometer[2]);
  omega = gyroVector.add(omegaI);
  omegaVector = omega.add(omegaP); // adding integrator term

  #define OUTPUTMODE 1
  #if OUTPUTMODE == 1         
  update[0][0] = 0;
  update[0][1] = -dt * omegaVector[2]; // -z
  update[0][2] =  dt * omegaVector[1]; // y
  update[1][0] =  dt * omegaVector[2]; // z
  update[1][1] = 0;
  update[1][2] = -dt * omegaVector[0]; // -x
  update[2][0] = -dt * omegaVector[1]; // -y
  update[2][1] =  dt * omegaVector[0]; // x
  update[2][2] = 0;
  #else
  // No drift correction...
  update[0][0] = 0;
  update[0][1] = -dt * gyroVector[2]; // -z
  update[0][2] =  dt * gyroVector[1]; // y
  update[1][0] =  dt * gyroVector[2]; // z
  update[1][1] = 0;
  update[1][2] = -dt * gyroVector[0]; // -x
  update[2][0] = -dt * gyroVector[1]; // -y
  update[2][1] =  dt * gyroVector[0]; // x
  update[2][2] = 0;
  #endif

  temporary = dcm.multiply(update);
  dcm = dcm.add(temporary);
}

void IMU::eulerAngles() {
  pitch = -asin(dcm[2][0]);
  roll = atan2(dcm[2][1], dcm[2][2]);
  yaw = atan2(dcm[1][0], dcm[0][0]);
}

void IMU::print() {
  if (millis() - debuggingTimer < 200) {
    return;
  }
  debuggingTimer  = millis();
  printf("IMU:");
  printf("%10lu ", timer);
  printf("% 6.3f ", TO_DEG(roll));
  printf("% 6.3f ", TO_DEG(pitch));
  printf("% 6.3f ", TO_DEG(yaw));
  printf("G % 6d ", data[0]);
  printf("% 6d ", data[1]);
  printf("% 6d ", data[2]);  
  printf("A % 6d ", data[3]);
  printf("% 6d ", data[4]);
  printf("% 6d ", data[5]);
  printf("M % 6.5f ", magnetometerCorrected[0]);
  printf("% 6.5f ", magnetometerCorrected[1]);
  printf("% 6.5f ", magnetometerCorrected[2]);
  printf("% 6.5f ", TO_DEG(heading));
  printf("\n\r");
}

void IMU::begin() {
  Wire.begin();

  printf("IMU: Pausing...\n\r");
  delay(1500);

  initialize();
  calibrate();

  printf("IMU: Ready\n\r");

  timer = millis();
}

void IMU::service() {
  // 50Mhz
  if ((millis() - timer) >= 20) {
    counter++;
    timerOld = timer;
    timer = millis();
    if (timer > timerOld) {
      // real time of loop run. (gyro integration time)
      dt = (timer - timerOld) / 1000.0;
    }
    else {
      dt = 0;
    }
    
    readGyro();
    readAccelerometer();
    if (counter > 5) {
      readCompass();
      calculateHeading();
      counter = 0;
    }
    
    matrixUpdate(); 
    normalize();
    driftCorrection();
    eulerAngles();
    print();
  }
}

