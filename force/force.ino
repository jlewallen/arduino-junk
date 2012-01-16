#define PIN_LED    11
#define PIN_SENSOR 0

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SENSOR, INPUT);
}

void loop() {
	long sensor = analogRead(PIN_SENSOR);
	long mapped = map(sensor, 0, 1024, 0, 255);
	analogWrite(PIN_LED, mapped);
	delay(10);
}
