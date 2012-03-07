#include <Arduino.h>
#include <Wire.h>
#include <printf.h>
#include <WireHelpers.h>
#include "../imu_module/ImuModule.h"

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
      case 'q':
        printf("Reading %d...\n\r", sizeof(imu_orientation_t));
        imu_command_t sending;
        sending.opcode = IMU_OPCODE_READ;
        Wire.beginTransmission(IMU_MODULE_ADDRESS);
        Wire.write((uint8_t *)&sending, sizeof(imu_command_t));
        Wire.requestFrom(IMU_MODULE_ADDRESS, sizeof(imu_orientation_t));
        imu_orientation_t orientation;
        if (wireReadBlock(&orientation, sizeof(imu_orientation_t), 1000)) {
          printf("%f ", orientation.heading);
          printf("%f ", orientation.yaw);
          printf("%f ", orientation.pitch);
          printf("%f ", orientation.roll);
          printf("\n\r");
        }
        Wire.endTransmission();
        break;
      }
    }
  }

  return 0;
}
