#define SENSOR_1 8

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR_1, INPUT);
}

void loop() {
  Serial.println(digitalRead(SENSOR_1));
  delay(500);
}

