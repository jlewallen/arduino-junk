#include <ShiftRegisters.h>
#include <SevenSegmentDisplay.h>

static ShiftRegisters shiftRegisters(8, 9, 10, 8);

void setup() {
  shiftRegisters.begin();
}               

void loop() {
  shiftRegisters.write(SevenSegmentDisplay::forDigit('0'));
  delay(500);
  shiftRegisters.write(SevenSegmentDisplay::forDigit('1'));
  delay(500);
  shiftRegisters.write(SevenSegmentDisplay::forDigit('2'));
  delay(500);
  shiftRegisters.write(SevenSegmentDisplay::forDigit('3'));
  delay(500);
  shiftRegisters.write(SevenSegmentDisplay::forDigit('4'));
  delay(500);
  shiftRegisters.write(SevenSegmentDisplay::forDigit('/'));
  delay(500);
  shiftRegisters.write(SevenSegmentDisplay::forDigit(':'));
  delay(500);
}
