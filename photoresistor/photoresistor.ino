#include <SimpleInterval.h>
#include <MemoryFree.h>

int lightPin = 0;
int ledPin = 9;

static SimpleInterval second(1000);

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int level = analogRead(lightPin);
  level = 255 - map(level, 0, 1024, 0, 255);
  analogWrite(ledPin, level);
  if (second.tick()) {
    Serial.println(level);
    Serial.println(memoryFree());
  }
}
