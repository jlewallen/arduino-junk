#include <Arduino.h>
#include <Pins.h>

void setup() {
	pinMode(5, OUTPUT);
	digital_write<5>(HIGH);
}

void loop() {
}

int main(void) {
	init();
	setup();

	for (;;) {
		loop();
	}
	return 0;
}
