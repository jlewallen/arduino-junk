#include <Arduino.h>
#include <SPI.h>

#define EEPROM_WREN  6
#define EEPROM_WRDI  4
#define EEPROM_RDSR  5
#define EEPROM_WRSR  1
#define EEPROM_READ  3
#define EEPROM_WRITE 2 

#define EEPROM_TWC   5

typedef struct {
  uint8_t ss;
  uint8_t page_size;
} eeprom_module_t;

void eeprom_initialize(eeprom_module_t *ee, uint8_t ss, uint8_t page_size, uint8_t configure_spi) {
  ee->ss = ss;
  ee->page_size = page_size;

  digitalWrite(ee->ss, HIGH);
  pinMode(ee->ss, OUTPUT);

  if (configure_spi) {
    SPI.setDataMode(SPI_MODE3);
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.begin();
  }
}

void eeprom_write(eeprom_module_t *ee, uint16_t address, uint8_t *ptr, size_t sz) {
  while (sz > 0) {
    digitalWrite(ee->ss, LOW);
    SPI.transfer(EEPROM_WREN);    
    digitalWrite(ee->ss, HIGH);
    delayMicroseconds(100);

    digitalWrite(ee->ss, LOW);
    SPI.transfer(EEPROM_WRITE);
    SPI.transfer((uint8_t)(address >> 8));
    SPI.transfer((uint8_t)(address));
    uint8_t writing = min(ee->page_size, sz);
    for (uint8_t i = 0; i < writing; i++) {
      SPI.transfer(*ptr++);
    }
    digitalWrite(ee->ss, HIGH);
    delay(EEPROM_TWC);
    address += writing;
    sz -= writing;
  }
}

void eeprom_read(eeprom_module_t *ee, uint16_t address, uint8_t *ptr, size_t sz) {
  digitalWrite(ee->ss, LOW);
  SPI.transfer(EEPROM_READ);
  SPI.transfer((uint8_t)(address >> 8));
  SPI.transfer((uint8_t)(address));
  while (sz > 0) {
    *ptr++ = SPI.transfer(0xFF);
    sz--;
  }
  digitalWrite(ee->ss, HIGH);
}

void setup() {
  eeprom_module_t ee;
  uint8_t buffer[1024];

  for (size_t i = 0; i < sizeof(buffer); ++i) {
    buffer[i] = i;
  }

  Serial.begin(9600);
  Serial.println("Hello");

  eeprom_initialize(&ee, 10, 64, true);

  {
    delay(10);
    Serial.println("Writing...");
    uint32_t started = micros();
    eeprom_write(&ee, 0x00, buffer, sizeof(buffer));
    Serial.println(micros() - started);
    Serial.println("DONE");
  }

  {
    delay(10);
    Serial.println("Reading...");
    uint32_t started = micros();
    eeprom_read(&ee, 0x00, buffer, sizeof(buffer));
    Serial.println(micros() - started);
    Serial.println("DONE");
    for (size_t i = 0; i < sizeof(buffer); ++i) {
      Serial.print(buffer[i]);
      Serial.print(" ");
    }
  }

  Serial.println();
}

void loop() {

}
