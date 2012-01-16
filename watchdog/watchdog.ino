#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/atomic.h>

#define LED_PIN 13

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void system_sleep() {
  cbi(ADCSRA, ADEN);                   // switch Analog to Digitalconverter OFF
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // System sleeps here
	sleep_disable();                     // System continues execution here when watchdog timed out
  sbi(ADCSRA, ADEN);                   // switch Analog to Digitalconverter ON
}

void power_down() {
  // cbi(SMCR, SE);      // sleep enable, power down mode
  // cbi(SMCR, SM0);     // power down mode
  // sbi(SMCR, SM1);     // power down mode
  // cbi(SMCR, SM2);     // power down mode
	system_sleep();
}

//****************************************************************
// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
// 6=1 sec, 7=2 sec, 8=4 sec, 9=8sec
void watchdog(int mode) {
  byte value = 0;
  if (mode > 9)
		mode = 9;
  value = mode & 7;
  if (mode > 7)
		value |= (1 << 5);
  value |= (1 << WDCE);

  MCUSR &= ~(1 << WDRF);
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = value;
  WDTCSR |= _BV(WDIE);
}

ISR(WDT_vect)
{
	// Serial.println("Watchdog!");
}

void setup() {
  Serial.begin(9600);
	Serial.println("Started...");
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(13, HIGH);

  Serial.println("Sleeping");
  watchdog(9);
	if (true) {
		power_down();
    pinMode(LED_PIN, OUTPUT);
	}
	else {
		delay(1000);
	}
  watchdog(-1);
  Serial.println("Awake");

  digitalWrite(13, LOW);
	delay(1000);
}
