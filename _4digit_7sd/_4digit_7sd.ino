#include <ShiftRegisters.h>
#include <SevenSegmentDisplay.h>
#include <SimpleInterval.h>

#define DISPLAY_BRIGHTNESS 5000
#define DIGIT_1 2
#define DIGIT_2 3
#define DIGIT_3 4

static ShiftRegisters shiftRegisters(8, 9, 10, 8);
static unsigned long previous = 0;
static int number = 0;

void setup() {
  randomSeed(analogRead(0));
  shiftRegisters.begin();
  pinMode(DIGIT_1, OUTPUT);
  pinMode(DIGIT_2, OUTPUT);
  pinMode(DIGIT_3, OUTPUT);
  previous = millis();
}

void loop() {
  unsigned long beginTime = millis();
  if (millis() - previous > 1000) {
    number = random(0, 999);
    previous = millis();
  }

  int displaying = number;
  digitalWrite(DIGIT_1, LOW);
  digitalWrite(DIGIT_2, LOW);
  digitalWrite(DIGIT_3, HIGH);
  shiftRegisters.write(SevenSegmentDisplay::forDigit(displaying % 10));
  displaying /= 10;
  delayMicroseconds(DISPLAY_BRIGHTNESS);
   
  digitalWrite(DIGIT_1, LOW);
  digitalWrite(DIGIT_2, HIGH);
  digitalWrite(DIGIT_3, LOW);
  shiftRegisters.write(SevenSegmentDisplay::forDigit(displaying % 10));
  displaying /= 10;
  delayMicroseconds(DISPLAY_BRIGHTNESS);

  digitalWrite(DIGIT_1, HIGH);
  digitalWrite(DIGIT_2, LOW);
  digitalWrite(DIGIT_3, LOW);
  shiftRegisters.write(SevenSegmentDisplay::forDigit(displaying % 10));
  displaying /= 10;
  delayMicroseconds(DISPLAY_BRIGHTNESS);
}
