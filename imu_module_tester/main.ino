#include <Arduino.h>
#include <Wire.h>
#include <printf.h>
#include <WireHelpers.h>
#include "../imu_module/ImuModule.h"

#define TO_DEG(x)    ((x) * 57.2957795131)  // *180/pi

void configure(imu_configuration_t *configuration) {
  imu_command_t configure;
  configure.opcode = IMU_OPCODE_CONFIGURE;
  Wire.beginTransmission(IMU_MODULE_ADDRESS);
  Wire.write((uint8_t *)&configure, sizeof(imu_command_t));
  Wire.write((uint8_t *)configuration, sizeof(imu_configuration_t));
  Wire.endTransmission();
}

int main() {
  init();

  Wire.begin(0xee);
  Serial.begin(9600);
  printf_begin();

  delay(500);

  Serial.println("Ready...");

  imu_configuration_t configuration;
  configuration.hz = 0;

  for (;;) {
    if (Serial.available()) {
      switch (Serial.read()) {
      case 'c':
        Serial.println("Sending 100Hz");
        configuration.hz = 100;
        configure(&configuration);
        break;
      case 'v':
        Serial.println("Sending 50Hz");
        configuration.hz = 50;
        configure(&configuration);
        break;
      case 'x':
        Serial.println("Sending 20Hz");
        configuration.hz = 20;
        configure(&configuration);
        break;
      case 'z':
        Serial.println("Stopping...");
        configuration.hz = 0;
        configure(&configuration);
        break;
      case 'q': {
        imu_command_t sending;
        sending.opcode = IMU_OPCODE_READ;
        sending.arg = IMU_REG_ORIENTATION;
        Wire.beginTransmission(IMU_MODULE_ADDRESS);
        Wire.write((uint8_t *)&sending, sizeof(imu_command_t));
        Wire.requestFrom(IMU_MODULE_ADDRESS, sizeof(imu_orientation_t));
        imu_orientation_t orientation;
        if (wireReadBlock(&orientation, sizeof(imu_orientation_t), 1000)) {
          printf("%f ", TO_DEG(orientation.heading));
          printf("%f ", TO_DEG(orientation.yaw));
          printf("%f ", TO_DEG(orientation.pitch));
          printf("%f ", TO_DEG(orientation.roll));
          printf("\n\r");
        }
        Wire.endTransmission();
        break;
      }
      case 'w': {
        imu_command_t sending;
        sending.opcode = IMU_OPCODE_READ;
        sending.arg = IMU_REG_GYRO;
        Wire.beginTransmission(IMU_MODULE_ADDRESS);
        Wire.write((uint8_t *)&sending, sizeof(imu_command_t));
        Wire.requestFrom(IMU_MODULE_ADDRESS, sizeof(imu_vector_t));
        imu_vector_t vector;
        if (wireReadBlock(&vector, sizeof(imu_vector_t), 1000)) {
          printf("Gyro = ");
          printf("%f ", TO_DEG(vector.x));
          printf("%f ", TO_DEG(vector.y));
          printf("%f ", TO_DEG(vector.z));
          printf("\n\r");
        }
        Wire.endTransmission();
        break;
      }
      case 'e': {
        imu_command_t sending;
        sending.opcode = IMU_OPCODE_READ;
        sending.arg = IMU_REG_ACCELEROMETER;
        Wire.beginTransmission(IMU_MODULE_ADDRESS);
        Wire.write((uint8_t *)&sending, sizeof(imu_command_t));
        Wire.requestFrom(IMU_MODULE_ADDRESS, sizeof(imu_vector_t));
        imu_vector_t vector;
        if (wireReadBlock(&vector, sizeof(imu_vector_t), 1000)) {
          printf("Accelerometer = ");
          printf("%f ", TO_DEG(vector.x));
          printf("%f ", TO_DEG(vector.y));
          printf("%f ", TO_DEG(vector.z));
          printf("\n\r");
        }
        Wire.endTransmission();
        break;
      }
      }
    }
  }

  return 0;
}
