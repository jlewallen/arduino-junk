
void setup() {
  Serial.begin(9600);

  pinMode(3, OUTPUT);
}

byte speed = 0;

void loop() {
  if (Serial.available()) {
    switch (Serial.read()) {
    case 'q':
      speed = constrain(speed - 10, 0, 255);
      Serial.println(speed);
      break;
    case 'w':
      speed = constrain(speed + 10, 0, 255);
      Serial.println(speed);
      break;
    case '.':
      if (speed == 0) {
        speed = 200;
      }
      else {
        speed = 0;
      }
      break;
    }
  }
  analogWrite(3, speed);

	delay(100);

  analogWrite(3, 0);
  delay(2);
  long started = micros();
  long accum = 0;
  long samples = 0;
  while (micros() - started < 2000) {
    accum += analogRead(0);
    samples++;
  }
  Serial.print(accum / samples);
  Serial.print(" ");
}
