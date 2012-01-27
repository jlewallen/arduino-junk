#include <printf.h>

#define SERIAL_DEBUG

void setup() {
  Serial.begin(9600);
  printf_begin();

  Serial.println("Ready...");
}

void loop() {
  boolean shortRange = digitalRead(7);
  float longRangeSensorValue = analogRead(0);
  float inches = 4192.936 * pow(longRangeSensorValue, -0.935) - 3.937;
  //cm = 10650.08 * pow(sensorValue,-0.935) - 10;
  delay(100);
  Serial.print("Sensor: ");
  Serial.print(longRangeSensorValue);
  Serial.print(" Inches: ");
  Serial.print(inches);
  if (!shortRange) {
    Serial.print(" ****");
  }
  Serial.println("");
  #if defined(SERIAL_DEBUG)
  #endif
}

