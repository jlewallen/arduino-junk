#include <Arduino.h>

void setup() {
	Serial.println("Setup");
}

void loop() {
	Serial.println("Tick");
	delay(1000);
}

int main(void) {
	init();

	Serial.begin(9600);
	Serial.println("Hello");

	setup();

	for (;;) {
		loop();
	}
	return 0;
}
