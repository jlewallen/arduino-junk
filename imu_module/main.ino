#include <Arduino.h>
#include <IMU.h>

#define LED_PIN 13

static uint32_t previous = 0;
static IMU imu;

class Indicator {
private:
  uint32_t toggleAt;

public:
  void begin() {
    toggleAt = 0;
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
  }

  void service() {
    if (millis() > toggleAt) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      toggleAt = millis() + 250;
    }
  }
};

int main(void) {
	init();

  Indicator indicator;
  IMU imu;

  indicator.begin();
  imu.begin();

	for (;;) {
    imu.service();
    indicator.service();
	}
	return 0;
}
