#include <Arduino.h>
#include <IMU.h>
#include <WireHelpers.h>
#include "ImuModule.h"
#include "printf.h"

#define LED_PIN 13

class Indicator {
private:
  uint32_t toggleAt;
  uint16_t toggles;

public:
  void begin() {
    toggleAt = 0;
    toggles = 0;
    pinMode(LED_PIN, OUTPUT);
    blink(3);
  }

  void blink(uint16_t blinks) {
    toggles = blinks * 2;
    toggleAt = 0;
    digitalWrite(LED_PIN, LOW);
  }

  void service(uint8_t hz) {
    if (toggles > 0) {
      if (millis() > toggleAt) {
        toggles--;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        toggleAt = millis() + 100;
      }
      return;
    }
    if (hz == 0) {
      digitalWrite(LED_PIN, LOW);
      return;
    }
    if (millis() > toggleAt) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      toggleAt = millis() + hz * 10;
    }
  }

  void initializing() {
    digitalWrite(LED_PIN, HIGH);
  }

  void ready() {
    digitalWrite(LED_PIN, LOW);
  }

  void off() {
    digitalWrite(LED_PIN, LOW);
  }
};

class Module {
private:
  static Module *singleton;
  imu_configuration_t configuration;
  imu_command_t pending;
  Indicator indicator;
  IMU *imu;

public:
  static void onWireReceived(int16_t howMany) {
    singleton->received(howMany);
  }

  static void onWireRequested() {
    singleton->reply();
  }

  void reply() {
    indicator.blink(2);

    switch (pending.arg) {
    case IMU_REG_ORIENTATION: {
      imu_orientation_t orientation;
      memzero(&orientation, sizeof(imu_orientation_t));
      if (imu != NULL) {
        orientation.heading = imu->getHeading();
        orientation.yaw = imu->getYaw();
        orientation.pitch = imu->getPitch();
        orientation.roll = imu->getRoll();
      }
      Wire.write((uint8_t *)&orientation, sizeof(imu_orientation_t));
      break;
    }
    case IMU_REG_GYRO: {
      imu_vector_t vector;
      memzero(&vector, sizeof(imu_vector_t));
      if (imu != NULL) {
        vector.x = imu->getGyroVector()[0];
        vector.y = imu->getGyroVector()[1];
        vector.z = imu->getGyroVector()[2];
      }
      Wire.write((uint8_t *)&vector, sizeof(imu_vector_t));
      break;
    }
    case IMU_REG_ACCELEROMETER: {
      imu_vector_t vector;
      memzero(&vector, sizeof(imu_vector_t));
      if (imu != NULL) {
        vector.x = imu->getAccelerometerVector()[0];
        vector.y = imu->getAccelerometerVector()[1];
        vector.z = imu->getAccelerometerVector()[2];
      }
      Wire.write((uint8_t *)&vector, sizeof(imu_vector_t));
      break;
    }
    }
  }

  void received(int16_t howMany) {
    if (Wire.available()) {
      if (!wireReadBlock(&pending, sizeof(imu_command_t), 100)) {
        return;
      }
      switch (pending.opcode) {
      case IMU_OPCODE_CONFIGURE:
        imu_configuration_t reading;
        if (!wireReadBlock(&reading, sizeof(imu_configuration_t), 100)) {
          return;
        }
        memcpy(&configuration, &reading, sizeof(imu_configuration_t));
        if (imu != NULL) {
          delete imu;
          imu = NULL;
        }
        break;
      }
    }
  }

  void begin() {
    memzero(&configuration, sizeof(imu_configuration_t));
    singleton = this;
    imu = NULL;
    indicator.begin();

    Wire.begin(IMU_MODULE_ADDRESS);
    Wire.onReceive(onWireReceived);
    Wire.onRequest(onWireRequested);
  }

  void service() {
    if (configuration.hz > 0) {
      if (imu == NULL) {
        indicator.initializing();
        imu = new IMU(configuration.hz);
        imu->begin();
        indicator.ready();
      }
      imu->service();

      #ifdef DEBUG
      if (Serial.available()) {
        if (Serial.read() == '.') {
          imu->print();
        }
      }
      #endif
    }
    indicator.service(configuration.hz);
  }

};

Module *Module::singleton = NULL;

int main(void) {
	init();

  #ifdef DEBUG
  Serial.begin(9600);
  printf_begin();
  printf("IMU Module Ready\n\r");
  #endif

  Module module;
  module.begin();
	for (;;) {
    module.service();
	}
	return 0;
}
