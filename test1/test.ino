void setup() {
  pinMode(3, OUTPUT);
}

void loop() {
  analogWrite(3, 32);
  delay(10);
}
