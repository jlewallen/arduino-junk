/**
 * 13 - SRCLK
 * 11 - SER
 *  8 - RCLK
 */
#include <SPI.h>
const int ShiftPWM_latchPin = 8;
const bool ShiftPWM_invertOutputs = 0;
#include <ShiftPWM.h>

unsigned char maxBrightness = 255;
unsigned char pwmFrequency = 75;
uint8_t numberOfRegisters = 1;

void setup() {
  pinMode(ShiftPWM_latchPin, OUTPUT);  
  SPI.setBitOrder(LSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4); 
  SPI.begin(); 

  ShiftPWM.SetAmountOfRegisters(numberOfRegisters);
  ShiftPWM.Start(pwmFrequency, maxBrightness);
}

byte levels[] = {
  0,
  0,
  8,
  128,
  8,
  0,
  0,
  0
};
byte temp[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

void loop() {
  for (int16_t index = 0; index < numberOfRegisters * 8; index++) {
    ShiftPWM.SetOne(index, temp[index] = levels[index]);
  }
  for (int16_t index = 0; index < numberOfRegisters * 8; index++) {
    levels[index] = temp[(index + 1) % (numberOfRegisters * 8)];
  }
  delay(95);
}
